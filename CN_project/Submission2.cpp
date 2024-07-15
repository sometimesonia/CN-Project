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

    string getNetworkIP() {
        return network_ip;
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
            Sleep(1000); // Delay for 1 second (1000 milliseconds)
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
    virtual void send(const string& data, const string& sourceIP, const string& destinationIP, int sourcePort, int destinationPort) = 0;
    virtual void receive(const string& data, const string& sourceIP, const string& destinationIP, int sourcePort, int destinationPort) = 0;
};

class SlidingWindowProtocol : public TransportLayerProtocol {
private:
    int windowSize;

public:
    SlidingWindowProtocol(int size) : windowSize(size) {}

    void send(const string& data, const string& sourceIP, const string& destinationIP, int sourcePort, int destinationPort) override {
        // Implement sliding window protocol for sending data
        cout << "Sending data using sliding window protocol" << endl;
        cout << "Data: " << data << endl;
        cout << "Source IP: " << sourceIP << endl;
        cout << "Destination IP: " << destinationIP << endl;
        cout << "Source Port: " << sourcePort << endl;
        cout << "Destination Port: " << destinationPort << endl;
        cout << "Window Size: " << windowSize << endl;
        cout << endl;
    }

    void receive(const string& data, const string& sourceIP, const string& destinationIP, int sourcePort, int destinationPort) override {
        // Implement sliding window protocol for receiving data
        cout << "Receiving data using sliding window protocol" << endl;
        cout << "Data: " << data << endl;
        cout << "Source IP: " << sourceIP << endl;
        cout << "Destination IP: " << destinationIP << endl;
        cout << "Source Port: " << sourcePort << endl;
        cout << "Destination Port: " << destinationPort << endl;
        cout << "Window Size: " << windowSize << endl;
        cout << endl;
    }
};

class ApplicationLayerService {
public:
    virtual void start() = 0;
    virtual void stop() = 0;
};

class Service1 : public ApplicationLayerService {
public:
    void start() override {
        cout << "Service 1 started" << endl;
    }

    void stop() override {
        cout << "Service 1 stopped" << endl;
    }
};

class Service2 : public ApplicationLayerService {
public:
    void start() override {
        cout << "Service 2 started" << endl;
    }

    void stop() override {
        cout << "Service 2 stopped" << endl;
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

    void sendData(const string& data, const string& destinationIP, int destinationPort) {
        if (transportProtocol != nullptr) {
            transportProtocol->send(data, ip_address, destinationIP, 0, destinationPort);
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
    SlidingWindowProtocol slidingWindowProtocol(10);
    Service1 service1;
    Service2 service2;

    EndDevice device1(1, "Device 1", &network, "00:11:22:33:44:55", "255.255.255.0");
    device1.printDeviceInfo();
    device1.setTransportProtocol(&slidingWindowProtocol);
    device1.addService(&service1);
    device1.addService(&service2);

    EndDevice device2(2, "Device 2", &network, "AA:BB:CC:DD:EE:FF", "255.255.255.0");
    device2.printDeviceInfo();
    device2.setTransportProtocol(&slidingWindowProtocol);
    device2.addService(&service1);

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
