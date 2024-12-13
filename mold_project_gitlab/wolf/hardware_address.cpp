#include "hardware_address.hpp"

#include <ifaddrs.h>
#include <netdb.h>
#include <unistd.h>

#include <cstring>
#include <sstream>
#include <stdexcept>

using namespace wolf;

std::string hardware_address::get_hostname_as_hex() {
  char hostname[NI_MAXHOST];
  const int result = gethostname(hostname, NI_MAXHOST);
  if (result != 0) {
    std::stringstream error;
    error << "Failed to get hostname, err: " << std::strerror(errno);
    throw std::runtime_error(error.str());
  }
  std::istringstream is(hostname);
  std::uint64_t id;
  is >> std::hex >> id;
  return cast_to_hex_string(id);
}

std::string hardware_address::cast_to_hex_string(const uint64_t number) {
  std::ostringstream out;
  out << std::hex << number << std::flush;
  return out.str();
}

hardware_address::address_list hardware_address::get_ip_addresses() {
  struct ifaddrs *addrs, *tmp;
  address_list addresses{};
  char host[NI_MAXHOST];

  if (getifaddrs(&addrs) == -1) return addresses;

  tmp = addrs;

  while (tmp) {
    if (tmp->ifa_addr && tmp->ifa_addr->sa_family == AF_INET) {
      int check = getnameinfo(tmp->ifa_addr, sizeof(struct sockaddr_in), host,
                              NI_MAXHOST, nullptr, 0, NI_NUMERICHOST);
      if (check == 0) {
        std::string address(tmp->ifa_name);
        address += ": ";
        address += host;
        addresses.push_back(address);
      }
    }
    tmp = tmp->ifa_next;
  }
  freeifaddrs(addrs);
  return addresses;
}
