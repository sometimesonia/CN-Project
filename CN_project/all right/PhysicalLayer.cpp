#include <iostream>
#include <vector>
#include <algorithm>
#include <string>

using namespace std;

class Hub;

class EndDevice {
  private:
    int device_id;
    string device_name;
    string ip_address;
    Hub* hub_;

  public:
    EndDevice(int id, string name, string ip);

    int getDeviceId();

    string getDeviceName();

    string getIpAddress();

    void connect();

    void disconnect();

    void sendData(string data);

    void receiveData(string data);

    void connectHub(Hub* hub);
};

class Hub {
  private:
    int hub_id;
    string hub_name;
    vector<EndDevice*> connected_devices;
    vector<Hub*> hub_;

  public:
    Hub(int id, string name);

    int getHubId();

    string getHubName();

    vector<EndDevice*> getConnectedDevices();

    vector<Hub*> getConnectedHubs();

    void connectDevice(EndDevice* device);

    void disconnectDevice(EndDevice* device);

    void connectHub(Hub* hub);

    void disconnectHub(Hub* hub);

    void sendData(EndDevice* source_device, string data);

    void receiveData(EndDevice* source_device, string data);
};

// Implement the member functions for EndDevice class
EndDevice::EndDevice(int id, string name, string ip) {
  device_id = id;
  device_name = name;
  ip_address = ip;
}

int EndDevice::getDeviceId() {
  return device_id;
}

string EndDevice::getDeviceName() {
  return device_name;
}

string EndDevice::getIpAddress() {
  return ip_address;
}

void EndDevice::connect() {
  cout << device_name << " connected" << endl;
}

void EndDevice::disconnect() {
  cout << device_name << " disconnected" << endl;
}

void EndDevice::sendData(string data) {
  cout << device_name << " sent data: " << data << endl;
  hub_->sendData(this, data);
}

void EndDevice::receiveData(string data) {
  cout << device_name << " received data: " << data << endl;
}

void EndDevice::connectHub(Hub* hub) {
  hub_ = hub;
  hub_->connectDevice(this);
}

// Implement the member functions for Hub class
Hub::Hub(int id, string name) {
  hub_id = id;
  hub_name = name;
}

int Hub::getHubId() {
  return hub_id;
}

string Hub::getHubName() {
  return hub_name;
}

vector<EndDevice*> Hub::getConnectedDevices() {
  return connected_devices;
}

vector<Hub*> Hub::getConnectedHubs() {
  return hub_;
}

void Hub::connectDevice(EndDevice* device) {
  connected_devices.push_back(device);
  device->connect();
}

void Hub::disconnectDevice(EndDevice* device) {
  connected_devices.erase(remove(connected_devices.begin(), connected_devices.end(), device), connected_devices.end());
  device->disconnect();
}

void Hub::connectHub(Hub* hub) {
  hub_.push_back(hub);
}

void Hub::disconnectHub(Hub* hub) {
  hub_.erase(remove(hub_.begin(), hub_.end(), hub), hub_.end());
}

void Hub::sendData(EndDevice* source_device, string data) {
  cout << hub_name << " forwarding data from " << source_device->getDeviceName() << endl;
  for (auto device : connected_devices) {
    if (device != source_device) {
      device->receiveData(data);
    }
  }
  for (auto connected_hub : hub_) {
    connected_hub->sendData(source_device, data);
  }
}

void Hub::receiveData(EndDevice* source_device, string data) {
  cout << hub_name << " received data from " << source_device->getDeviceName() << endl;
  sendData(source_device, data);
}

int main() {
  // Your main function remains unchanged
  // ...
  return 0;
}
