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
class EndDevice;
class Hub;
class Router;
class FlowControlProtocol;
class AccessControlProtocol;

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

class EndDevice {
private:
    int device_id;
    string device_name;
    string ip_address;
    string mac_address;
    string subnet_mask;
    Network* network;

public:
    EndDevice(int id, string name, Network* net, string mac, string mask) {
        device_id = id;
        device_name = name;
        network = net;
        ip_address = network->assignIPAddress();
        mac_address = mac;
        subnet_mask = mask;
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

    string getSubnetMask() {
        return subnet_mask;
    }

    void connect() {
        cout << device_name << " connected" << endl;
    }

    void disconnect() {
        cout << device_name << " disconnected" << endl;
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
        for (const auto& route : routingTable) {
            this->routingTable->addDynamicRoute(route.first, route.second);
        }
    }
};

class FlowControlProtocol {
public:
    virtual bool canSendPacket() = 0;
    virtual void receiveAck() = 0;
};

class GoBackN : public FlowControlProtocol {
public:
    bool canSendPacket() override {
        // Implementation for Go-Back-N flow control
        return true;
    }

    void receiveAck() override {
        // Implementation for Go-Back-N flow control
    }
};

class StopNWait : public FlowControlProtocol {
public:
    bool canSendPacket() override {
        // Implementation for Stop-and-Wait flow control
        return true;
    }

    void receiveAck() override {
        // Implementation for Stop-and-Wait flow control
    }
};

class SelectiveRepeat : public FlowControlProtocol {
public:
    bool canSendPacket() override {
        // Implementation for Selective Repeat flow control
        return true;
    }

    void receiveAck() override {
        // Implementation for Selective Repeat flow control
    }
};

class AccessControlProtocol {
public:
    virtual bool canSendPacket() = 0;
};

class PureAloha : public AccessControlProtocol {
public:
    bool canSendPacket() override {
        // Implementation for Pure Aloha access control
        return true;
    }
};

class SlottedAloha : public AccessControlProtocol {
public:
    bool canSendPacket() override {
        // Implementation for Slotted Aloha access control
        return true;
    }
};

class Hub {
private:
    string hub_name;
    Network* network;
    vector<EndDevice*> connected_devices;

public:
    Hub(string name, Network* net) {
        hub_name = name;
        network = net;
    }

    void connectDevice(EndDevice* device) {
        connected_devices.push_back(device);
        device->connect();
    }

    void disconnectDevice(EndDevice* device) {
        auto it = find(connected_devices.begin(), connected_devices.end(), device);
        if (it != connected_devices.end()) {
            connected_devices.erase(it);
            device->disconnect();
        }
    }

    void broadcastPacket(string packet) {
        for (const auto& device : connected_devices) {
            cout << "Broadcasting packet '" << packet << "' from " << hub_name << " to " << device->getDeviceName() << endl;
        }
    }
};

class Router {
private:
    string router_name;
    Network* connected_network;
    Network* network;
    RoutingTable* routingTable;
    RoutingProtocol* routingProtocol;

public:
    Router(string name, Network* net, Network* connectedNet, RoutingTable* rt, RoutingProtocol* rp) {
        router_name = name;
        network = net;
        connected_network = connectedNet;
        routingTable = rt;
        routingProtocol = rp;
    }

    void sendPacket(string destinationIP, string packet) {
        string nextHopIP = routingTable->getNextHop(destinationIP);
        if (nextHopIP != "") {
            cout << "Sending packet '" << packet << "' from " << router_name << " to " << nextHopIP << endl;
        } else {
            cout << "No route found for destination IP " << destinationIP << " in " << router_name << endl;
        }
    }

    void updateRoutingTable(const unordered_map<string, string>& routingTable) {
        routingProtocol->updateRoutingTable(routingTable);
    }
};

int main() {
    // Create a network
    Network network("192.168.0");
    
    // Create a routing table
    RoutingTable routingTable;
    
    // Create routing protocols
    RIP rip(&routingTable);
    OSPF ospf(&routingTable);
    
    // Create devices
    EndDevice device1(1, "Device 1", &network, "00:11:22:33:44:55", "255.255.255.0");
    EndDevice device2(2, "Device 2", &network, "11:22:33:44:55:66", "255.255.255.0");
    
    // Create a hub
    Hub hub("Hub", &network);
    
    // Connect devices to the hub
    hub.connectDevice(&device1);
    hub.connectDevice(&device2);
    
    // Disconnect device 1 from the hub
    hub.disconnectDevice(&device1);
    
    // Create routers
    Router router1("Router 1", &network, &network, &routingTable, &rip);
    Router router2("Router 2", &network, &network, &routingTable, &ospf);
    
    // Update the routing table of router 1
    unordered_map<string, string> newRoutes = {{"192.168.0.2", "192.168.0.3"}};
    router1.updateRoutingTable(newRoutes);
    
    // Send a packet from router 1 to router 2
    router1.sendPacket("192.168.0.2", "Hello!");
    
    return 0;
}
