#include <iostream>
#include <string>
#include <unordered_map>

using namespace std;

class FlowControlProtocol {
public:
    virtual bool canSendPacket() = 0;
};

class Bridge {
private:
    string name;
    unordered_map<string, FlowControlProtocol*> forwarding_table;

public:
    Bridge(string name) {
        this->name = name;
    }

   void addForwardingTableEntry(string mac_address, FlowControlProtocol* protocol) {
    if (mac_address.size() != 17) { // check that MAC address has correct format
        throw invalid_argument("Invalid MAC address");
    }
    forwarding_table[mac_address] = protocol;
}


    void sendPacket(string dst_mac, string data) {
    if (forwarding_table.count(dst_mac) == 0) { // check that MAC address exists in forwarding table
        cout << "Packet dropped: MAC address not found in forwarding table" << endl;
        return;
    }
    FlowControlProtocol* protocol = forwarding_table[dst_mac];
    if (protocol->canSendPacket()) {
        cout << "Packet sent from " << name << " to " << dst_mac << " with data: " << data << endl;
    } else {
        cout << "Packet dropped due to flow control protocol overflow" << endl;
    }
}

};
class Switch : public FlowControlProtocol {
private:
    unordered_map<string, Bridge*> forwarding_table;

public:
    void addForwardingTableEntry(string mac_address, Bridge* bridge) {
        forwarding_table[mac_address] = bridge;
    }

    Bridge* getBridgeForMacAddress(string mac_address) {
        return forwarding_table[mac_address];
    }

    bool canSendPacket() {
        return true; // Or implement your own logic
    }
};

class TokenBucket : public FlowControlProtocol {
private:
    int max_tokens;
    int current_tokens;
    int fill_rate;

public:
    TokenBucket(int max_tokens, int fill_rate) {
        this->max_tokens = max_tokens;
        this->fill_rate = fill_rate;
        current_tokens = max_tokens;
    }

    bool canSendPacket() {
        if (current_tokens > 0) {
            current_tokens--;
            return true;
        } else {
            return false;
        }
    }

    void refill() {
        current_tokens += fill_rate;
        if (current_tokens > max_tokens) {
            current_tokens = max_tokens;
        }
    }
};

class LeakyBucket : public FlowControlProtocol {
private:
    int max_tokens;
    int current_tokens;
    int outflow_rate;

public:
    LeakyBucket(int max_tokens, int outflow_rate) {
        this->max_tokens = max_tokens;
        this->outflow_rate = outflow_rate;
        current_tokens = max_tokens;
    }

    bool canSendPacket() {
        if (current_tokens > 0) {
            current_tokens--;
            return true;
        } else {
            return false;
        }
    }

    void refill() {
        current_tokens += outflow_rate;
        if (current_tokens > max_tokens) {
            current_tokens = max_tokens;
        }
    }
};

class MACFilteringProtocol {
private:
    unordered_map<string, string> mac_table;

public:
    void addEntry(string mac_address, string vlan) {
        mac_table[mac_address] = vlan;
    }

    bool isAuthorized(string src_mac, string dst_mac) {
        return mac_table.find(src_mac) != mac_table.end() && mac_table.find(dst_mac) != mac_table.end() && mac_table[src_mac] == mac_table[dst_mac];
    }
};
int main() {
    Bridge bridge1("Bridge1");
    Bridge bridge2("Bridge2");
    Switch switch1;

    bridge1.addForwardingTableEntry("00:11:22:33:44:55", &switch1);
    bridge2.addForwardingTableEntry("aa:bb:cc:dd:ee:ff", &switch1);
    switch1.addForwardingTableEntry("00:11:22:33:44:55", &bridge1);
    switch1.addForwardingTableEntry("aa:bb:cc:dd:ee:ff", &bridge2);

    MACFilteringProtocol mac_filtering_protocol;
    mac_filtering_protocol.addEntry("00:11:22:33:44:55", "VLAN1");
    mac_filtering_protocol.addEntry("aa:bb:cc:dd:ee:ff", "VLAN2");

    TokenBucket token_bucket(10, 5);
    LeakyBucket leaky_bucket(10, 2);

    string src_mac = "00:11:22:33:44:55";
    string dst_mac = "aa:bb:cc:dd:ee:ff";
    string data = "Hello World!";

    Bridge* bridge = switch1.getBridgeForMacAddress(dst_mac);
if (bridge == nullptr) { // handle case where MAC address is not found
    cout << "Packet dropped: MAC address not found in forwarding table" << endl;
    return 0;
}
if (token_bucket.canSendPacket()) {
    bridge->sendPacket(dst_mac, data);
} else {
    cout << "Packet dropped due to Token Bucket overflow" << endl;
}
if (leaky_bucket.canSendPacket()) {
    bridge->sendPacket(dst_mac, data);
} else {
    cout << "Packet dropped due to Leaky Bucket overflow" << endl;
}
    return 0;
}

