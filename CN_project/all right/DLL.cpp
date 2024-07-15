#include <iostream>
#include <string>
#include <unordered_map>
#include <queue>
#include <thread>
#include <chrono>

using namespace std;

class Layer2Device {
protected:
    string device_name;

public:
    Layer2Device(string name) : device_name(name) {}

    string getDeviceName() const { return device_name; }

    virtual void sendPacket(string dest_mac, string data) = 0;
    virtual void receivePacket(string src_mac, string data) = 0;
};

class Bridge : public Layer2Device {
private:
    unordered_map<string, Layer2Device*> forwarding_table;

public:
    Bridge(string name) : Layer2Device(name) {}

    void addForwardingTableEntry(string mac, Layer2Device* port) {
        forwarding_table[mac] = port;
    }

    void sendPacket(string dest_mac, string data) override {
        if (forwarding_table.find(dest_mac) != forwarding_table.end()) {
            cout << getDeviceName() << " forwarding packet to " << forwarding_table[dest_mac]->getDeviceName() << endl;
            forwarding_table[dest_mac]->receivePacket(getDeviceName(), data);
        } else {
            cout << getDeviceName() << " discarding packet for unknown MAC address " << dest_mac << endl;
        }
    }

    void receivePacket(string src_mac, string data) override {
        cout << getDeviceName() << " received packet from " << src_mac << ": " << data << endl;
    }
};

class Switch : public Layer2Device {
private:
    unordered_map<string, Layer2Device*> forwarding_table;

public:
    Switch(string name) : Layer2Device(name) {}

    void addForwardingTableEntry(string mac, Layer2Device* port) {
        forwarding_table[mac] = port;
    }

    void sendPacket(string dest_mac, string data) override {
        if (forwarding_table.find(dest_mac) != forwarding_table.end()) {
            cout << getDeviceName() << " forwarding packet to " << forwarding_table[dest_mac]->getDeviceName() << endl;
            forwarding_table[dest_mac]->receivePacket(getDeviceName(), data);
        } else {
            cout << getDeviceName() << " unknown destination MAC address " << dest_mac << endl;
            cout << getDeviceName() << " performing address learning for " << dest_mac << endl;
            forwarding_table[dest_mac] = this;
            for (auto& entry : forwarding_table) {
                if (entry.first != dest_mac && entry.second == this) {
                    entry.second = nullptr;
                }
            }
            sendPacket(dest_mac, data);
        }
    }

    void receivePacket(string src_mac, string data) override {
        cout << getDeviceName() << " received packet from " << src_mac << ": " << data << endl;
    }
};

// ... (rest of the code remains unchanged)
