#include <iostream>
#include <unordered_map>
#include <vector>

#include "IPAddress.h"
#include "IPNetwork.h"

class Router {
private:
    int id;
    int num_interface;
    bool active;
    std::vector<std::pair<IPNetwork, std::string>> interface;
    std::vector<int> connected_to_interface;
    std::unordered_map<IPNetwork, std::vector<std::string>> router_table;
    std::unordered_map<IPAddress, std::string> stored_arp;

public:
    Router(int id, int num_interface, bool active = true)
        : id(id), num_interface(num_interface), active(active),
          interface(num_interface), connected_to_interface(num_interface) {}

    void make_active() {
        active = true;
    }

    void make_inactive() {
        active = false;
    }

    void init_rip() {
        for (int i = 0; i < interface.size(); ++i) {
            if (interface[i].first.isEmpty()) {
                continue;
            }
            router_table[interface[i].first.network()] = {0, interface[i].first.netmask(), i, "-"};
        }
    }

    int arp_res(std::unordered_map<std::string, std::vector<IPNetwork>> msg_data, Router& device) {
        if (msg_data["header_3"][1].network() == msg_data["header_3"][0].network()) {
            std::cout << "Router " << id << " rejected the ARP Request" << std::endl;
            return 0;
        }
        return 0;
    }

    std::unordered_map<std::string, std::vector<IPNetwork>> arp_request(std::unordered_map<std::string, std::vector<IPNetwork>> msg_data, int interface) {
        std::unordered_map<std::string, std::vector<IPNetwork>> ret;
        std::cout << "ARP Request Sent By Device " << id << std::endl;
        int i = connected_to_interface[interface];
        ret = i.arp_res(msg_data, *this);
        msg_data["header_2"].push_back(ret["address"]);
        std::cout << msg_data << std::endl;
        return msg_data;
    }

    std::vector<std::string> check_longest_mask(IPNetwork mask, std::vector<std::string> val) {
        std::vector<std::string> minimum = val;
        for (const auto& entry : router_table) {
            if (entry.first == mask) {
                if (entry.second[2] < val[2]) {
                    minimum = entry.second;
                }
            }
        }
        return minimum;
    }

    void reply(std::unordered_map<std::string, std::vector<IPNetwork>>& msg_data, Router& sender, bool ack_msg = false) {
        std::cout << "Message At Router " << id << std::endl;
        if (msg_data["header_3"][1].network() == msg_data["header_3"][0].network()) {
            std::cout << "Packet Rejected By Router " << id << std::endl;
            stored_arp[msg_data["header_3"][0]] = msg_data["header_2"][0];
            stored_arp[msg_data["header_3"][1]] = msg_data["header_2"][1];
            return;
        } else if (router_table.find(msg_data["header_3"][1].network()) == router_table.end()) {
            std::cout << "No Hop Available" << std::endl;
            return;
        } else {
            for (const auto& entry : router_table) {
                if (msg_data["header_3"][1].network() == entry.first) {
                    if (entry.second.back() == "-" && stored_arp.find(msg_data["header_3"][1]) == stored_arp.end()) {
                        msg_data = arp_request(msg_data, entry.second[2]);
                        stored_arp[msg_data["header_3"][1]] = msg_data["header_2"][1];
                        stored_arp[msg_data["header_3"][0]] = msg_data["header_2"][0];
                        return connected_to_interface[entry.second[2]].reply(msg_data, *this, ack_msg);
                    }
                    return connected_to_interface[entry.second.back()].reply(msg_data, *this, ack_msg);
                }
            }
        }
    }

    int find_interface(Router& device) {
        for (int i = 0; i < connected_to_interface.size(); ++i) {
            if (connected_to_interface[i] == &device) {
                return i;
            }
        }
        return -1;
    }

    void make_RIP_table(std::vector<Router>& list_of_routers) {
        for (const auto& router : list_of_routers) {
            if (&router == this) {
                continue;
            }
            for (const auto& entry : router.router_table) {
                if (router.find_interface(*this) != -1) {
                    if (router_table.find(entry.first) == router_table.end() ||
                        (router_table.find(entry.first) != router_table.end() && router_table[entry.first].back() == -1)) {
                        router_table[entry.first] = {entry.second[0] + 1, entry.second[1], entry.second[2], router.find_interface(*this)};
                    }
                } else {
                    if (router_table[entry.first][0] > entry.second[0] + 1) {
                        router_table[entry.first] = {entry.second[0] + 1, entry.second[1], entry.second[2], router.find_interface(*this)};
                    }
                }
            }
        }
    }

    void config_interface_ip(int int_id, IPNetwork ip) {
        interface[int_id] = {ip, "Random MAC Address"};
    }
};

int main() {
    Router r1(0, 3);
    Router r2(1, 3);
    Router r3(2, 3);
    std::vector<Router> lr {r1, r2, r3};

    r1.config_interface_ip(0, IPNetwork("10.0.0.1/8"));
    r1.config_interface_ip(1, IPNetwork("20.0.0.1/8"));
    r2.config_interface_ip(0, IPNetwork("40.0.0.1/8"));
    r2.config_interface_ip(1, IPNetwork("30.0.0.1/8"));
    r3.config_interface_ip(0, IPNetwork("20.0.0.2/8"));
    r3.config_interface_ip(1, IPNetwork("30.0.0.2/8"));

    r1.init_rip();
    r2.init_rip();
    r3.init_rip();

    std::cout << r1.router_table << std::endl;
    std::cout << r2.router_table << std::endl;
    std::cout << r3.router_table << std::endl;

    r1.make_RIP_table(lr);
    std::cout << r1.router_table << std::endl;

    r2.make_RIP_table(lr);
    std::cout << r2.router_table << std::endl;

    r3.make_RIP_table(lr);
    std::cout << r3.router_table << std::endl;

    return 0;
}
