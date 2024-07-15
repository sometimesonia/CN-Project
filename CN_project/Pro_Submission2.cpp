#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <thread>
#include <windows.h>

using namespace std;

// Define the constants for the Go-Back-N protocol
const int WINDOW_SIZE = 10;
const int TIMEOUT = 1000;

// Define the HTTP and SSH port numbers
const int HTTP_PORT = 80;
const int SSH_PORT = 22;

class Network {
private:
    string network_ip;
    int next_subnet;
    unordered_map<string, string> arpCache; // ARP cache: IP Address -> MAC Address

public:
    Network(string ip) {
        network_ip = ip;
        next_subnet = 0;
    }

    string assignIPAddress() {
        string subnet = to_string(next_subnet++);
        return network_ip + "." + subnet;
    }

    string getNetworkIP() {
        return network_ip;
    }
    
    void addToARPCache(const string& ipAddress, const string& macAddress) {
        arpCache[ipAddress] = macAddress;
    }

    string getMacAddressFromARPCache(const string& ipAddress) {
        if (arpCache.count(ipAddress) > 0) {
            return arpCache[ipAddress];
        }
        return "";
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
        // Perform Bellman-Ford algorithm
        for (const auto& route : routingTable) {
            string destinationIP = route.first;
            string nextHopIP = route.second;

            // Check if the destination IP already exists in the routing table
            if (this->routingTable->getNextHop(destinationIP) != "") {
                // Check if the new path is better (has a shorter distance)
                if (this->routingTable->getNextHop(destinationIP) != nextHopIP) {
                    this->routingTable->addDynamicRoute(destinationIP, nextHopIP);
                }
            } else {
                // Add the new route to the routing table
                this->routingTable->addDynamicRoute(destinationIP, nextHopIP);
            }
        }
        this->routingTable->printRoutingTable();
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
        Sleep(1000); // Delay for 1 second (1000 milliseconds)
        this->routingTable->printRoutingTable(); // Print the routing table
    }
};

class TransportLayerProtocol {
public:
    virtual void send(const string& data, const string& sourceIP, const string& destinationIP, int sourcePort, int destinationPort, const string& destinationMac) = 0;
    virtual void receive(const string& data, const string& sourceIP, const string& destinationIP, int sourcePort, int destinationPort) = 0;
};

class GoBackNProtocol : public TransportLayerProtocol {
private:
    int windowSize;
    int Sf;
    int Sn;
    int timer; // Timer variable to track the timeout

public:
    GoBackNProtocol(int size) : windowSize(5), Sf(5), Sn(10), timer(1000) {}

     void send(const string& data, const string& sourceIP, const string& destinationIP, int sourcePort, int destinationPort, const string& destinationMac) override {
    // Implement Go-Back-N protocol for sending data
    cout << "Sending data using Go-Back-N protocol" << endl;
    cout << "Data: " << data << endl;
    cout << "Source IP: " << sourceIP << endl;
    cout << "Destination IP: " << destinationIP << endl;
    cout << "Source Port: " << sourcePort << endl;
    cout << "Destination Port: " << destinationPort << endl;
    cout << "Window Size: " << windowSize << endl;

    // Start the timer
    timer = 1000; // Setting the timer value to 1000 (example value)

    // Iterate over the packets to be sent
    for (int seqNum = Sf; seqNum < data.size(); seqNum++) {
        // Check if the window is not full
        if (Sn < Sf + windowSize) {
            cout << "Sending packet with sequence number " << seqNum << endl;
            Sn++;
        } else {
            cout << "Window is full. Waiting for acknowledgements..." << endl;

            // Wait for acknowledgements or timeout
            while (timer > 0) {
                // Simulate passage of time (decrease timer value)
                timer--;

                // Check for incoming acknowledgements
                // ...

                // Simulate delay before re-checking
                Sleep(100); // Delay for 100 milliseconds
            }

            // Timeout occurred, retransmit packets
            cout << "Timeout occurred. Retransmitting packets..." << endl;
            seqNum = Sf - 1; // Decrement seqNum to retransmit the packet
            Sn = Sf; // Reset the next sequence number to send
            timer = 1000; // Reset the timer value
        }
    }

    cout << endl;
}
    void receive(const string& data, const string& sourceIP, const string& destinationIP, int sourcePort, int destinationPort) override {
        // Implement Go-Back-N protocol for receiving data
        cout << "Receiving data using Go-Back-N protocol" << endl;
        cout << "Data: " << data << endl;
        cout << "Source IP: " << sourceIP << endl;
        cout << "Destination IP: " << destinationIP << endl;
        cout << "Source Port: " << sourcePort << endl;
        cout << "Destination Port: " << destinationPort << endl;
        cout << "Window Size: " << windowSize << endl;
        cout << endl;

        // Process received packets
        for (int i = 0; i < data.size(); i++) {
            if (i >= Sf && i < Sn) {
                cout << "Received packet with sequence number " << i << endl;
                // Send acknowledgement for the received packet
                sendAck(i);
            } else {
                cout << "Packet with sequence number " << i << " is outside the window. Discarding..." << endl;
            }
        }
    }

    void sendAck(int ackNum) {
        // Send acknowledgement for the received packet
        cout << "Sending acknowledgement for packet with sequence number " << ackNum << endl;
    }
};

class ApplicationLayerService {
public:
    virtual void start() = 0;
    virtual void stop() = 0;
};

class HTTPService : public ApplicationLayerService {
public:
    void start() override {
        cout << "HTTP service started" << endl;
    }

    void stop() override {
        cout << "HTTP service stopped" << endl;
    }
};

class SSHService : public ApplicationLayerService {
public:
    void start() override {
        cout << "SSH service started" << endl;
    }

    void stop() override {
        cout << "SSH service stopped" << endl;
    }
};

class EndDevice {
private:
    int device_id;
    string device_name;
    string ip_address;
    string mac_address;
    string subnet_mask;
    Network* network;
    TransportLayerProtocol* transportProtocol;
    vector<ApplicationLayerService*> services;
    unordered_map<string, string> arpCache; 

public:
    EndDevice(int id, string name, Network* net, string mac, string mask) {
        device_id = id;
        device_name = name;
        network = net;
        ip_address = network->assignIPAddress();
        mac_address = mac;
        subnet_mask = mask;
        transportProtocol = nullptr;
    }

    void addToARPCache(const string& ipAddress, const string& macAddress) {
        network->addToARPCache(ipAddress, macAddress);
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

    void setTransportProtocol(TransportLayerProtocol* protocol) {
        transportProtocol = protocol;
    }

    void addService(ApplicationLayerService* service) {
        services.push_back(service);
    }

    void startServices() {
        for (auto service : services) {
            service->start();
        }
    }

    void stopServices() {
        for (auto service : services) {
            service->stop();
        }
    }

    void printDeviceInfo() {
        cout << "Device Name: " << device_name << endl;
        cout << "Device ID: " << device_id << endl;
        cout << "IP Address: " << ip_address << endl;
        cout << "MAC Address: " << mac_address << endl;
        cout << "Subnet Mask: " << subnet_mask << endl;
        cout << "Network IP: " << network->getNetworkIP() << endl;
        cout << endl;
    }

    string resolveMacAddress(const string& ipAddress) {
        string macAddress = network->getMacAddressFromARPCache(ipAddress);
        if (macAddress.empty()) {
            // Perform ARP lookup to find the MAC address
            // Here, you can implement the logic to send ARP request and receive ARP response
            // For demonstration purposes, let's assume the MAC address is found
            macAddress = "12:34:56:78:90:AB";
            addToARPCache(ipAddress, macAddress);
        }
        return macAddress;
    }


    void sendData(const string& data, const string& destinationIP, int destinationPort) {
        if (transportProtocol != nullptr) {
            string destinationMac = resolveMacAddress(destinationIP);
            transportProtocol->send(data, ip_address, destinationIP, 0, destinationPort, destinationMac);
        }
    }

    void receiveData(const string& data, const string& sourceIP, int sourcePort) {
        if (transportProtocol != nullptr) {
            transportProtocol->receive(data, sourceIP, ip_address, sourcePort, 0);
        }
    }
};

int main() {
    Network network("192.168.0");
    RoutingTable routingTable;
    RIP rip(&routingTable);
    OSPF ospf(&routingTable);
    GoBackNProtocol goBackNProtocol(10);
    HTTPService httpService;
    SSHService sshService;

    EndDevice device1(1, "Device 1", &network, "00:11:22:33:44:55", "255.255.255.0");
    device1.printDeviceInfo();
    device1.setTransportProtocol(&goBackNProtocol);
    device1.addService(&httpService);
    device1.addService(&sshService);

    EndDevice device2(2, "Device 2", &network, "AA:BB:CC:DD:EE:FF", "255.255.255.0");
    device2.printDeviceInfo();
    device2.setTransportProtocol(&goBackNProtocol);
    device2.addService(&httpService);

    routingTable.addStaticRoute("192.168.0.1", "192.168.0.2");
    routingTable.addStaticRoute("192.168.0.10", "192.168.0.11");
    routingTable.printRoutingTable();

    unordered_map<string, string> dynamicRoutes = {
        {"192.168.0.3", "192.168.0.4"},
        {"192.168.0.20", "192.168.0.21"}
    };
    rip.updateRoutingTable(dynamicRoutes);

    device1.startServices();
    device2.startServices();

    device1.sendData("Hello from Device 1", device2.getIpAddress(), 12345);
    device2.receiveData("Hello from Device 1", device1.getIpAddress(), 12345);

    device1.stopServices();
    device2.stopServices();

    return 0;
}
