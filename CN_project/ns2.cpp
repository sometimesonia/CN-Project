#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <thread>

using namespace std;

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
            std::this_thread::sleep_for(std::chrono::seconds(1)); // Delay for 1 second
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
};

#include <windows.h>

// Declare your classes and functions here

int main() {
    Network network("192.168.0");
    RoutingTable routingTable;
    RIP rip(&routingTable);
    OSPF ospf(&routingTable);
    EndDevice device1(1, "Device 1", &network, "00:11:22:33:44:55", "255.255.255.0");
    EndDevice device2(2, "Device 2", &network, "AA:BB:CC:DD:EE:FF", "255.255.255.0");

    // Perform your operations here
   // routingTable.addStaticRoute("192.168.0.1", "192.168.0.2");
   // routingTable.addStaticRoute("192.168.0.10", "192.168.0.11");
   // routingTable.printRoutingTable();

        unordered_map<string, string> dynamicRoutes = {
        {"192.168.0.3", "192.168.0.4"},
        {"192.168.0.20", "192.168.0.21"}
    };
    routingTable.addDynamicRoute(dynamicRoutes["192.168.0.3"], dynamicRoutes["192.168.0.4"]);
    routingTable.addDynamicRoute(dynamicRoutes["192.168.0.20"], dynamicRoutes["192.168.0.21"]);

    // Perform your operations here
    routingTable.printRoutingTable();

    return 0;
}


