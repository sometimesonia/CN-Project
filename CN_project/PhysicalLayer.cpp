#include <iostream>
#include <vector>
#include <algorithm>

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
      cout << device_name << " connected" << endl;
    }

    void disconnect() {
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

int main() {
  int num_devices;
  string topology;
  
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
}
