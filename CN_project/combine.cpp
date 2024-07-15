#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include<algorithm>
#include<iterator>

using namespace std;

class EndDevice {
  private:
    int device_id;
    string device_name;
    string ip_address;

  public:
    EndDevice(int id, string name, string ip) {
      device_id = id;
      device_name = name;
      ip_address = ip;
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

    void connect() {
      // additional logic for connecting the device
      cout << device_name << " connected" << endl;
    }

    void disconnect() {
      // additional logic for disconnecting the device
      cout << device_name << " disconnected" << endl;
    }
};

class Hub {
  private:
    int hub_id;
    string hub_name;
    vector<EndDevice*> connected_devices;
    vector<Hub*> connected_hubs;

  public:
    Hub(int id, string name) {
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
      // additional logic for connecting a device to the hub
    }

    void disconnectDevice(EndDevice* device) {
      connected_devices.erase(remove(connected_devices.begin(), connected_devices.end(), device), connected_devices.end());
      device->disconnect();
      // additional logic for disconnecting a device from the hub
    }

    void connectHub(Hub* hub) {
      connected_hubs.push_back(hub);
      // additional logic for connecting to another hub
    }

    void disconnectHub(Hub* hub) {
      connected_hubs.erase(remove(connected_hubs.begin(), connected_hubs.end(), hub), connected_hubs.end());
      // additional logic for disconnecting from another hub
    }
};


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
    int num_devices;
  string topology;

  // Get user input for network topology and number of devices
  cout << "Enter the network topology (star/mesh/bus): ";
  cin >> topology;
  cout << "Enter the number of end devices: ";
  cin >> num_devices;

  // Create end devices
  vector<EndDevice*> devices;
  for (int i = 1; i <= num_devices; i++) {
    string name = "Device " + to_string(i);
    string ip = "192.168.0." + to_string(i);
    EndDevice* device = new EndDevice(i, name, ip);
    devices.push_back(device);
  }

  // Create hubs
  vector<Hub*> hubs;
  if (topology == "star") {
    // Create a hub for each device and connect it to the hub at the center
    Hub* center_hub = new Hub(1, "Center Hub");
    hubs.push_back(center_hub);
    for (int i = 0; i < devices.size(); i++) {
      Hub* hub = new Hub(i + 2, "Hub " + to_string(i + 1));
      hubs.push_back(hub);
      center_hub->connectHub(hub);
      center_hub->connectDevice(devices[i]);
      hub->connectDevice(devices[i]);
    }
  } else if (topology == "mesh") {
    // Create a hub for each device and connect it to every other hub
    for (int i = 0; i < devices.size(); i++) {
      Hub* hub = new Hub(i + 1, "Hub " + to_string(i + 1));
      hubs.push_back(hub);
      for (int j = 0; j < hubs.size(); j++) {
        if (j != i) {
          hub->connectHub(hubs[j]);
        }
      }
      hub->connectDevice(devices[i]);
    }
  } else if (topology == "bus") {
    // Create a single hub and connect all devices to it
    Hub* hub = new Hub(1, "Bus Hub");
    hubs.push_back(hub);
    for (int i = 0; i < devices.size(); i++) {
      hub->connectDevice(devices[i]);
    }
  } else {
    cout << "Invalid topology. Please enter 'star', 'mesh', or 'bus'." << endl;
    return 0;
  }


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
    
    // Send and receive data
  for (int i = 0; i < devices.size(); i++) {
    EndDevice* device = devices[i];
    cout << "Sending data from " << device->getDeviceName() << "..." << endl;
    for (int j = 0; j < hubs.size(); j++) {
      vector<EndDevice*> connected_devices = hubs[j]->getConnectedDevices();
      for (int k = 0; k < connected_devices.size(); k++) {
        if (connected_devices[k] != device) {
          cout << "Received data at " << connected_devices[k]->getDeviceName() << endl;
        }
      }
    }
  }

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

