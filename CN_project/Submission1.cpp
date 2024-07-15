#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <iterator>
#include <random>
#include <chrono>
#include <thread>

using namespace std;

class EndDevice {
private:
    int device_id;
    string device_name;
    string ip_address;
    string mac_address;

public:
    EndDevice(int id, string name, string ip, string mac) {
        device_id = id;
        device_name = name;
        ip_address = ip;
        mac_address = mac;
    }

    int getDeviceId() {
        return device_id;
    }

    string getDeviceName() {
        return device_name;
    }

    string getIpAddress() {
        return ip_address;
    }

    string getMacAddress() {
        return mac_address;
    }

    void connect() {
        cout << device_name << " connected" << endl;
    }

    void disconnect() {
        cout << device_name << " disconnected" << endl;
    }
};

class Hub : public EndDevice {
private:
    int hub_id;
    string hub_name;
    vector<EndDevice*> connected_devices;
    vector<Hub*> connected_hubs;

public:
    Hub(int id, string name, int device_id, string device_name, string ip_address, string mac_address)
        : EndDevice(device_id, device_name, ip_address, mac_address) {
        hub_id = id;
        hub_name = name;
    }

    int getHubId() {
        return hub_id;
    }

    string getHubName() {
        return hub_name;
    }

    vector<EndDevice*> getConnectedDevices() {
        return connected_devices;
    }

    vector<Hub*> getConnectedHubs() {
        return connected_hubs;
    }

    void connectDevice(EndDevice* device) {
        connected_devices.push_back(device);
        device->connect();
    }

    void disconnectDevice(EndDevice* device) {
        connected_devices.erase(remove(connected_devices.begin(), connected_devices.end(), device), connected_devices.end());
        device->disconnect();
    }

    void connectHub(Hub* hub) {
        connected_hubs.push_back(hub);
    }

    void disconnectHub(Hub* hub) {
        connected_hubs.erase(remove(connected_hubs.begin(), connected_hubs.end(), hub), connected_hubs.end());
    }
};


class FlowControlProtocol {
public:
    virtual bool canSendPacket(const std::string& destination_mac, int seqNum) = 0;
    virtual void receiveAck(int ackNum) = 0;
};

class GoBackN : public FlowControlProtocol {
    private:
    int windowSize_;
    int Sf;     
    int Sn;   
    int timer; // Timer variable to track the timeout

public:
    GoBackN(int windowSize) : windowSize_(windowSize), Sf(1), Sn(1), timer(0) {
    }

    bool canSendPacket(const std::string& destination_mac, int seqNum) override {
        if (Sn < Sf + windowSize_) {
            std::cout << "Sending packet with sequence number " << seqNum << std::endl;
            Sn++;
            // Start the timer
            timer = 10; // Setting the timer value to 10 (example value)
            return true;
        }
        std::cout << "Window is full. Waiting for acknowledgements..." << std::endl;
        return false;
    }

    void receiveAck(int ackNum) override {
        std::cout << "Received acknowledgement for packet with sequence number " << ackNum << std::endl;
        Sf = ackNum + 1;
        // Reset the timer when an acknowledgment is received
        timer = 0;
    }
};

class StopNWait : public FlowControlProtocol {
private:
    int expected_seq_num;

int timer; // Timer variable to track the timeout

public:
    StopNWait() {
        expected_seq_num = 0;
        timer = 0;
    }

    bool canSendPacket(const std::string& destination_mac, int seqNum) {
        if (seqNum == expected_seq_num) {
            std::cout << "Sending packet with sequence number " << seqNum << std::endl;
            // Start the timer
            timer = 10; // Setting the timer value to 10 (example value)
            return true;
        }
        return false;
    }

    void receiveAck(int ackNum) {
        if (ackNum == expected_seq_num) {
            expected_seq_num++;
            std::cout << "Received acknowledgement for packet with sequence number " << ackNum << std::endl;
            // Reset the timer when an acknowledgment is received
            timer = 0;
        }
    }
    
};

class SelectiveRepeat : public FlowControlProtocol {
private:
int window_size;
vector<bool> received;       //Whether the packet with the given sequence no. received.
vector<string> buffer;      //which packets have been acknowledged.
int Sf;
int Sn;
int timer; // Timer variable to track the timeout

public:
    SelectiveRepeat(int window_size) {
        this->window_size = window_size;
        received.resize(window_size);
        Sf = 0;
        Sn = 0;
        timer = 0;
    }

    bool canSendPacket(const std::string& destination_mac, int seqNum) {
        if (seqNum >= Sf && seqNum < Sf + window_size && !received[seqNum % window_size]) {
            std::cout << "Sending packet with sequence number " << seqNum << std::endl;
            // Start the timer
            timer = 10; // Setting the timer value to 10 (example value)
            return true;
        }
        return false;
    }

    void receiveAck(int ack_num) {
        if (ack_num >= Sf && ack_num < Sf + window_size) {
            int index = ack_num % window_size;
            received[index] = true;
            while (received[Sf % window_size]) {
                buffer.push_back("Packet " + to_string(Sf) + " acknowledged.");
                Sf++;
            }
            std::cout << "Received acknowledgement for packet with sequence number " << ack_num << std::endl;
            // Reset the timer when an acknowledgment is received
            timer = 0;
        }
    }
vector<string> getBuffer() {
    return buffer;
}
};

class AccessControlProtocol {
public:
    virtual bool canSendPacket() = 0;
};

class PureAloha : public AccessControlProtocol {
public:
    bool canSendPacket() override {
        // Generate a random number between 0 and 1
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0, 1);
        double probability = dis(gen);

        // If the random number is less than the probability of success, the station can send the packet
        if (probability < p) {
            return true;
        } else {
            return false;
        }
    }

private:
    double p = 0.1; // Probability of success
};

class SlottedAloha : public AccessControlProtocol {
public:
    bool canSendPacket() override {
        // Calculate the current time slot
        auto now = std::chrono::system_clock::now();
        auto slot = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() / timeSlotSize;

        // Generate a random number between 0 and 1
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0, 1);
        double probability = dis(gen);

        // If the random number is less than the probability of success, and the station is the first to attempt transmission in the current slot, it can send the packet
        if (probability < p && slot != lastSlot) {
            lastSlot = slot;
            return true;
        } else {
            return false;
        }
    }

private:
    double p = 0.1; // Probability of success
    int timeSlotSize = 1000; // Time slot size in milliseconds
    int lastSlot = -1; // The last slot in which the station attempted transmission
};

class Bridge : public FlowControlProtocol, public AccessControlProtocol {
private:
vector<Hub*> connected_hubs;
FlowControlProtocol* flow_control_protocol;
AccessControlProtocol* access_control_protocol;


public:
Bridge(FlowControlProtocol* flow_control) {
flow_control_protocol = flow_control;
}

void connectHub(Hub* hub) {
    connected_hubs.push_back(hub);
}

void disconnectHub(Hub* hub) {
    connected_hubs.erase(remove(connected_hubs.begin(), connected_hubs.end(), hub), connected_hubs.end());
}

bool canSendPacket(const std::string& destination_mac, int seqNum) override {
    return flow_control_protocol->canSendPacket(destination_mac, seqNum);
}

bool canSendPacket() override {
    
    if (access_control_protocol->canSendPacket()) {
        return true;
    } else {
        return false;
    }
}
};


class Switch : public FlowControlProtocol, public AccessControlProtocol {
private:
    unordered_map<string, EndDevice*> connected_devices;
    unordered_map<string, string> mac_address_table;  // MAC address table
    FlowControlProtocol* flow_control_protocol;
     AccessControlProtocol* access_control_protocol;

public:
    Switch(FlowControlProtocol* flow_control, AccessControlProtocol* acp) {
        flow_control_protocol = flow_control;
        access_control_protocol = acp;
    }

    void connectDevice(EndDevice* device, string port, string mac_address) {
        connected_devices[port] = device;
        mac_address_table[mac_address] = port;  // Update MAC address table
        device->connect();
    }

    void disconnectDevice(EndDevice* device, string port, string mac_address) {
        connected_devices.erase(port);
        mac_address_table.erase(mac_address);  // Remove MAC address from the table
        device->disconnect();
    }

    bool canSendPacket(const string& destination_mac, int seqNum) override {
    // Check if the destination MAC address is in the table
    if (mac_address_table.count(destination_mac) > 0) {
        bool accessControlResult = access_control_protocol->canSendPacket();
        bool flowControlResult = flow_control_protocol->canSendPacket(destination_mac, seqNum);

        if (accessControlResult && flowControlResult) {
            cout << "Sending packet to MAC address " << destination_mac << " with sequence number " << seqNum << endl;
        } else {
            cout << "Packet sending to MAC address " << destination_mac << " with sequence number " << seqNum << " is blocked by access control or flow control." << endl;
        }

        return flowControlResult;
    }

    cout << "Destination MAC address " << destination_mac << " not found in the table." << endl;
    return false;
}

bool canSendPacket() override {
    
    if (access_control_protocol->canSendPacket()) {
        return true;
    } else {
        return false;
    }
}

void receiveAck(int ackNum) override {
     // Process the acknowledgement
    cout << "Received acknowledgement for packet with sequence number " << ackNum << endl;
    }
};

int main() {

    FlowControlProtocol* flow_control_protocol = new GoBackN(2);
    AccessControlProtocol* access_control_protocol = new PureAloha();
    string destination_mac;

    Switch switch_obj(flow_control_protocol, access_control_protocol);
    EndDevice device1(1, "Device 1", "192.168.0.1", "00:00:00:00:00:01");
    EndDevice device2(2, "Device 2", "192.168.0.2", "00:00:00:00:00:02");
    EndDevice device3(3, "Device 3", "192.168.0.3", "00:00:00:00:00:03");
    EndDevice device4(4, "Device 4", "192.168.0.4", "00:00:00:00:00:04");
    EndDevice device5(5, "Device 5", "192.168.0.5", "00:00:00:00:00:05");

    switch_obj.connectDevice(&device1, "port1",  "00:00:00:00:00:01");
    switch_obj.connectDevice(&device2, "port2",  "00:00:00:00:00:02");
    switch_obj.connectDevice(&device3, "port3",  "00:00:00:00:00:03");
    switch_obj.connectDevice(&device4, "port4",  "00:00:00:00:00:04");
    switch_obj.connectDevice(&device5, "port5",  "00:00:00:00:00:05");

    // Enable data transmission between devices
    if (switch_obj.canSendPacket(destination_mac, device1.getDeviceId())) {
        // send packet with sequence number 1
        switch_obj.receiveAck(device1.getDeviceId());
    }

    // Access flow control protocols
    GoBackN flowControl(4);

    StopNWait stop_n_wait;
    if (stop_n_wait.canSendPacket(destination_mac,2)) {
        // send packet with sequence number 2
        stop_n_wait.receiveAck(2);
    }

    SelectiveRepeat selective_repeat(4);
    if (selective_repeat.canSendPacket(destination_mac, 3)) {
        // send packet with sequence number 3
        selective_repeat.receiveAck(3);
    }

    // Report the total number of broadcast and collision domains in the given network
    // There is one broadcast domain and one collision domain in the given network
    std::cout << "Number of broadcast domains: 1\n";
    std::cout << "Number of collision domains: 1\n";

    // Create two star topologies with five end devices connected to a hub in each case
    Hub hub1(1, "Hub 1", 3, "Hub 1 Device", "192.168.1.3", "00:00:00:00:00:06");
    Hub hub2(2, "Hub 2", 4, "Hub 2 Device", "192.168.1.4", "00:00:00:00:00:07");

    EndDevice device6(6, "Device 6", "192.168.1.1", "00:00:00:00:00:08");
    EndDevice device7(7, "Device 7", "192.168.1.2", "00:00:00:00:00:09");
    EndDevice device8(8, "Device 8", "192.168.1.3", "00:00:00:00:00:10");
    EndDevice device9(9, "Device 9", "192.168.1.4", "00:00:00:00:00:11");
    EndDevice device10(10, "Device 10", "192.168.1.5", "00:00:00:00:00:12");

    hub1.connectDevice(&device6);
    hub1.connectDevice(&device7);
    hub1.connectDevice(&device8);
    hub1.connectDevice(&device9);
    hub1.connectDevice(&device10);

    EndDevice device11(11, "Device 11", "192.168.2.1", "00:00:00:00:00:13");
    EndDevice device12(12, "Device 12", "192.168.2.2", "00:00:00:00:00:14");
    EndDevice device13(13, "Device 13", "192.168.2.3", "00:00:00:00:00:15");
    EndDevice device14(14, "Device 14", "192.168.2.4", "00:00:00:00:00:16");
    EndDevice device15(15, "Device 15", "192.168.2.5", "00:00:00:00:00:17");

    hub2.connectDevice(&device11);
    hub2.connectDevice(&device12);
    hub2.connectDevice(&device13);
    hub2.connectDevice(&device14);
    hub2.connectDevice(&device15);

// Connect two hubs using a switch
switch_obj.connectDevice(&hub1, "port6", "00:00:00:00:00:06");
switch_obj.connectDevice(&hub2, "port7", "00:00:00:00:00:07");

// Enable data transmission between all end devices
if (switch_obj.canSendPacket(destination_mac ,1)) {
    // send packet with sequence number 1
    switch_obj.receiveAck(1);
}

if (stop_n_wait.canSendPacket(destination_mac, 2)) {
    // send packet with sequence number 2
    stop_n_wait.receiveAck(2);
}

if (selective_repeat.canSendPacket(destination_mac, 3)) {
    // send packet with sequence number 3
    selective_repeat.receiveAck(3);
}

// Report the total number of broadcast and collision domains in the given network
// There are two broadcast domains and two collision domains in the given network
std::cout << "Number of broadcast domains: 2\n";
std::cout << "Number of collision domains: 2\n";

return 0;
};