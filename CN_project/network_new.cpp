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

// Forward declarations
class Network;
class RoutingProtocol;
class RoutingTable;
class EndDevice;
class Hub;
class Router;
class FlowControlProtocol;
class AccessControlProtocol;
class ApplicationLayer;

class Network {
private:
    string network_ip;
    int next_subnet;

public:
    Network(string ip) {
        network_ip = ip;
        next_subnet = 0;
    }

    string assignIPAddress() {
        string subnet = to_string(next_subnet++);
        return network_ip + "." + subnet;
    }
};

class RoutingTable {
private:
    unordered_map<string, string> staticRoutes; // Destination IP -> Next hop IP
    unordered_map<string, string> dynamicRoutes; // Destination IP -> Next hop IP

public:
    void addStaticRoute(const string& destinationIP, const string& nextHopIP) {
        staticRoutes[destinationIP] = nextHopIP;
    }

    void addDynamicRoute(const string& destinationIP, const string& nextHopIP) {
        dynamicRoutes[destinationIP] = nextHopIP;
    }

    string getNextHop(const string& destinationIP) {
        if (staticRoutes.count(destinationIP) > 0) {
            return staticRoutes[destinationIP];
        }
        if (dynamicRoutes.count(destinationIP) > 0) {
            return dynamicRoutes[destinationIP];
        }
        return "";
    }

    void printRoutingTable() {
        cout << "Routing Table: " << endl;
        cout << "Destination IP\tNext Hop IP" << endl;
        for (const auto& route : staticRoutes) {
            cout << route.first << "\t\t" << route.second << " (Static)" << endl;
        }
        for (const auto& route : dynamicRoutes) {
            cout << route.first << "\t\t" << route.second << " (Dynamic)" << endl;
        }
    }
};

class RoutingProtocol {
public:
    virtual void updateRoutingTable(const unordered_map<string, string>& routingTable) = 0;
};

class RIP : public RoutingProtocol {
private:
    RoutingTable* routingTable; // Pointer to the routing table

public:
    RIP(RoutingTable* rt) {
        routingTable = rt;
    }

    void updateRoutingTable(const unordered_map<string, string>& routingTable) override {
        for (const auto& route : routingTable) {
            this->routingTable->addDynamicRoute(route.first, route.second);
        }
    }
};

class OSPF : public RoutingProtocol {
private:
    RoutingTable* routingTable; // Pointer to the routing table

public:
    OSPF(RoutingTable* rt) {
        routingTable = rt;
    }

    void updateRoutingTable(const unordered_map<string, string>& routingTable) override {
        this->routingTable->addDynamicRoute(routingTable.begin()->first, routingTable.begin()->second);
    }
};

class EndDevice{
    
private:
    int device_id;
    string device_name;
    string ip_address;
    string mac_address;
protected:
    string subnet_mask;
    Network* network;
    int connecting_device_id;
    string connecting_device_name;
    

public:
    EndDevice(int con_id, string con_name,  int id, string name, Network* net, string mac, string mask) {
        
        connecting_device_id = con_id;
        connecting_device_name = con_name;
        device_id = id;
        device_name = name;
        network = net;
        ip_address = network->assignIPAddress();
        mac_address = mac;
        subnet_mask = mask;
    }

    int getconnectingDeviceID(){
        return connecting_device_id;
    }

    string getconnectingDeviceName(){
        return connecting_device_name;
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

    string getSubnetMask(){
        return subnet_mask;
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
    Hub(int id, string name, int device_id, string device_name, string ip_address, string mac_address, Network* net)
        : EndDevice(connecting_device_id, connecting_device_name, device_id, device_name, net, mac_address, subnet_mask) {
        hub_id = id;
        hub_name = name;
        network = net;
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
        hub->connectDevice(this);
        connect();
    }

    void disconnectHub(Hub* hub) {
        auto it = find(connected_hubs.begin(), connected_hubs.end(), hub);
        if (it != connected_hubs.end()) {
            connected_hubs.erase(it);
            hub->disconnectDevice(this);
            disconnect();
        }
    }

    void printConnectedHubs(){
        cout << "Connected Hubs for " << getDeviceName() << ": " << endl;
        for (const auto& hub : connected_hubs) {
            cout << hub->getHubName() << endl;
        }
    }
};

class Router : public EndDevice {
private:
    int router_id;
    string router_name;
    vector<EndDevice*> connected_devices;
    vector<Hub*> connected_hubs;
    RoutingTable routingTable; // Add an instance of the RoutingTable class
    Network* network;
    vector<Router*> connected_routers;
    RoutingProtocol* routingProtocol;
    

    public:
    string subnetMask;

    Router(int id, string name, int device_id, string device_name, string ip_address, string mac_address, string subnetMask )
        : EndDevice(connecting_device_id, connecting_device_name, device_id, device_name, network, mac_address, subnet_mask){
        
        router_id = id;
        router_name = name; 
    }

    int getRouterId() {
        return router_id;
    }

    string getRouterName() {
        return router_name;
    }

    vector<EndDevice*> getConnectedDevices() {
        return connected_devices;
    }

    vector<Hub*> getConnectedHubs() {
        return connected_hubs;
    }

    void addStaticRoute(const string& destinationIP, const string& nextHopIP) {
        routingTable.addStaticRoute(destinationIP, nextHopIP);
    }

    void addDynamicRoute(const string& destinationIP, const string& nextHopIP) {
        routingTable.addDynamicRoute(destinationIP, nextHopIP);
    }

    string getNextHop(const string& destinationIP) {
        return routingTable.getNextHop(destinationIP);
    }

    void connectDevice(EndDevice* device) {
        connected_devices.push_back(device);
        device->connect();
    }

    void disconnectDevice(EndDevice* device) {
        connected_devices.erase(remove(connected_devices.begin(), connected_devices.end(), device), connected_devices.end());
        device->disconnect();
    }

    void printConnectedDevices() {
        cout << "Connected Devices to " << router_name << ": " << endl;
        for (const auto& device : connected_devices) {
            cout << "ID: " << device->getDeviceId() << ", Name: " << device->getDeviceName() << ", IP: " << device->getIpAddress() << endl;
        }
    }


    void connectHub(Hub* hub) {
        connected_hubs.push_back(hub);
    }

    void disconnectHub(Hub* hub) {
        auto it = find(connected_hubs.begin(), connected_hubs.end(), hub);
    if (it != connected_hubs.end()) {
        connected_hubs.erase(it);
        hub->disconnectDevice(this);
        disconnect();
    }
    }

     RoutingTable& getRoutingTable() { // Add this method to retrieve the routing table
        return routingTable;
    }

     void setRoutingProtocol(RoutingProtocol* protocol) {
        routingProtocol = protocol;
    }

    void updateRoutingTable() {
        // Simulating dynamic routing updates
        unordered_map<string, string> dynamicRoutes;

        // Generate random dynamic routes for testing
        dynamicRoutes["192.168.0.0"] = "192.168.1.1";
        dynamicRoutes["10.0.0.0"] = "10.0.0.1";

        routingProtocol->updateRoutingTable(dynamicRoutes);
    }

    void printRoutingTable() {
        routingTable.printRoutingTable();
    }


    void performStaticRouting(const string& destinationIP) {
        string nextHopIP = routingTable.getNextHop(destinationIP);
        if (!nextHopIP.empty()) {
            cout << "Performing static routing for destination IP: " << destinationIP << endl;
            cout << "Next hop IP: " << nextHopIP << endl;
        } else {
            cout << "No static route found for destination IP: " << destinationIP << endl;
        }
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

class ApplicationLayer {
public:
    virtual void use() = 0;
};

class HTTP : public ApplicationLayer {
private:
    int id;
    int port;

public:
    HTTP(int id) : id(id), port(80) {}

    void use() override {
        std::cout << "Using HTTP Application" << std::endl;
    }
};

class SSH : public ApplicationLayer {
private:
    int id;
    int port;

public:
    SSH(int id) : id(id), port(22) {}

    void use() override {
        std::cout << "Using SSH Application" << std::endl;
    }
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

class Switch : public FlowControlProtocol, public AccessControlProtocol{
private:
     RoutingTable routingTable; // Add an instance of the RoutingTable class
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

     string resolveMACAddress(const string& ip_address) {
        for (const auto& device : connected_devices) {
            if (device.second->getIpAddress() == ip_address) {
                return device.second->getMacAddress();
            }
        }
        return "";  // MAC address not found
    }

    void performStaticRouting(const string& destinationIP) {
        string nextHopIP = routingTable.getNextHop(destinationIP);
        if (!nextHopIP.empty()) {
            cout << "Performing static routing for destination IP: " << destinationIP << endl;
            cout << "Next hop IP: " << nextHopIP << endl;
        } else {
            cout << "No static route found for destination IP: " << destinationIP << endl;
        }
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
    RoutingTable routingTable;

    // Create instances of Switch and Router
    Switch switch_obj(flow_control_protocol, access_control_protocol);
    Router router1(1, "Router 1", 1, "Device 1", "192.168.0.1", "00:00:00:00:00:01", "255.255.255.0");
    Router router2(2, "Router 2", 6, "Device 2", "192.168.1.10", "00:00:00:00:00:02", "255.255.255.0");
    Router router3(3, "Router 3", 11, "Device 3", "192.168.0.10", "00:00:00:00:00:03", "255.255.255.0");

    // Add static routes to the routing table of the router
    router1.getRoutingTable().addStaticRoute("192.168.0.0", "192.168.0.1");
    router1.getRoutingTable().addStaticRoute("192.168.1.0",  "192.168.1.1");

    // Perform static routing
    switch_obj.performStaticRouting("192.168.0.100");  // Switch does not have a routing table, so it cannot perform static routing
    router1.performStaticRouting("192.168.0.100");  // Router performs static routing for the destination IP

    Network network("192.168.0");
    EndDevice device1(1, "router 1", 1, "Device 1", &network, "00:00:00:00:00:01", "255.255.255.0");
    EndDevice device2(1,"router 1", 2, "Device 2", &network, "00:00:00:00:00:02", "255.255.255.0");
    EndDevice device3(1,"router 1", 3, "Device 3", &network, "00:00:00:00:00:03", "255.255.255.0");
    EndDevice device4(1, "router 1", 4, "Device 4", &network, "00:00:00:00:00:04", "255.255.255.0");
    EndDevice device5(1,"router 1", 5, "Device 5", &network, "00:00:00:00:00:05", "255.255.255.0");

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
    Hub hub1(1, "Hub 1", 3, "Hub 1 Device", "192.168.1.3", "00:00:00:00:00:06", &network);
    Hub hub2(2, "Hub 2", 4, "Hub 2 Device", "192.168.1.4", "00:00:00:00:00:07", &network);

    EndDevice device6(2,"router 2", 6, "Device 6", &network, "00:00:00:00:00:08", "255.255.255.0");
    EndDevice device7(2, "router 2", 7, "Device 7", &network, "00:00:00:00:00:09", "255.255.255.0");
    EndDevice device8(2, "router 2", 8, "Device 8", &network, "00:00:00:00:00:10", "255.255.255.0");
    EndDevice device9(2, "router 2", 9, "Device 9", &network, "00:00:00:00:00:11", "255.255.255.0");
    EndDevice device10(2, "router 2", 10, "Device 10", &network, "00:00:00:00:00:12", "255.255.255.0");

    hub1.connectDevice(&device6);
    hub1.connectDevice(&device7);
    hub1.connectDevice(&device8);
    hub1.connectDevice(&device9);
    hub1.connectDevice(&device10);

    EndDevice device11(3, "router 3", 11, "Device 11", &network, "00:00:00:00:00:13", "255.255.255.0");
    EndDevice device12(3, "router 3", 12, "Device 12", &network, "00:00:00:00:00:14", "255.255.255.0");
    EndDevice device13(3, "router 3", 13, "Device 13", &network, "00:00:00:00:00:15", "255.255.255.0");
    EndDevice device14(3, "router 3", 14, "Device 14", &network, "00:00:00:00:00:16", "255.255.255.0");
    EndDevice device15(3, "router 3", 15, "Device 15", &network, "00:00:00:00:00:17", "255.255.255.0");

    hub2.connectDevice(&device11);
    hub2.connectDevice(&device12);
    hub2.connectDevice(&device13);
    hub2.connectDevice(&device14);
    hub2.connectDevice(&device15);

// Connect two hubs using a switch
switch_obj.connectDevice(&hub1, "port6", "00:00:00:00:00:06");
switch_obj.connectDevice(&hub2, "port7", "00:00:00:00:00:07");

    // Connect devices to the routers
    router1.connectDevice(&device1);
    router1.connectDevice(&device2);
    router2.connectDevice(&device6);
    router2.connectDevice(&device7);

    // Connect hubs to the routers
    router1.connectHub(&hub1);
    router2.connectHub(&hub2);

    // Access connected devices and hubs of the routers
    vector<EndDevice*> router1ConnectedDevices = router1.getConnectedDevices();
    vector<Hub*> router1ConnectedHubs = router1.getConnectedHubs();
    vector<EndDevice*> router2ConnectedDevices = router2.getConnectedDevices();
    vector<Hub*> router2ConnectedHubs = router2.getConnectedHubs();

    // Print the connected devices and hubs of router 1
    cout << "Connected devices of Router 1:" << endl;
    for (const auto& device : router1ConnectedDevices) {
        cout << "Device ID: " << device->getDeviceId() << ", Device Name: " << device->getDeviceName() << endl;
    }

    cout << "Connected hubs of Router 1:" << endl;
    for (const auto& hub : router1ConnectedHubs) {
        cout << "Hub ID: " << hub->getHubId() << ", Hub Name: " << hub->getHubName() << endl;
    }

    // Print the connected devices and hubs of router 2
    cout << "\nConnected devices of Router 2:" << endl;
    for (const auto& device : router2ConnectedDevices) {
        cout << "Device ID: " << device->getDeviceId() << ", Device Name: " << device->getDeviceName() << endl;
    }

    cout << "Connected hubs of Router 2:" << endl;
    for (const auto& hub : router2ConnectedHubs) {
        cout << "Hub ID: " << hub->getHubId() << ", Hub Name: " << hub->getHubName() << endl;
    }

    RIP ripProtocol(&routingTable);
    OSPF ospfProtocol(&routingTable);

    router1.setRoutingProtocol(&ripProtocol); // Set the routing protocol to RIP

    // Update the routing table
    router1.updateRoutingTable();

    // Print the routing table
    routingTable.printRoutingTable();

    router1.setRoutingProtocol(&ospfProtocol); // Set the routing protocol to OSPF

    // Update the routing table
    router1.updateRoutingTable();

    // Print the routing table
    routingTable.printRoutingTable();

    // Disconnect a device from router 1
    router1.disconnectDevice(&device2);

    // Disconnect a hub from router 2
    router2.disconnectHub(&hub2);

    // Print the updated connected devices and hubs of router 1
    cout << "\nUpdated connected devices of Router 1:" << endl;
    for (const auto& device : router1ConnectedDevices) {
        cout << "Device ID: " << device->getDeviceId() << ", Device Name: " << device->getDeviceName() << endl;
    }

    cout << "Updated connected hubs of Router 1:" << endl;
    for (const auto& hub : router1ConnectedHubs) {
        cout << "Hub ID: " << hub->getHubId() << ", Hub Name: " << hub->getHubName() << endl;
    }

    // Print the updated connected devices and hubs of router 2
    cout << "\nUpdated connected devices of Router 2:" << endl;
    for (const auto& device : router2ConnectedDevices) {
        cout << "Device ID: " << device->getDeviceId() << ", Device Name: " << device->getDeviceName() << endl;
    }

    cout << "Updated connected hubs of Router 2:" << endl;
    for (const auto& hub : router2ConnectedHubs) {
        cout << "Hub ID: " << hub->getHubId() << ", Hub Name: " << hub->getHubName() << endl;
    }

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

ApplicationLayer* httpApp = new HTTP(1);
    httpApp->use();

    ApplicationLayer* sshApp = new SSH(2);
    sshApp->use();

    delete httpApp;
    delete sshApp;


// Report the total number of broadcast and collision domains in the given network
// There are two broadcast domains and two collision domains in the given network
std::cout << "Number of broadcast domains: 2\n";
std::cout << "Number of collision domains: 2\n";

return 0;
};