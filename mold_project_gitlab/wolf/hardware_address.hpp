#ifndef WOLF_NETWORK_MAC_ADDRESS_HPP
#define WOLF_NETWORK_MAC_ADDRESS_HPP

#include <string>
#include <vector>

namespace wolf {

class hardware_address {
 public:
  using address_list = std::vector<std::string>;

  hardware_address() = delete;
  static std::string get_hostname_as_hex();
  static address_list get_ip_addresses();

 private:
  static std::string cast_to_hex_string(const std::uint64_t number);
};
}

#endif  // WOLF_NETWORK_MAC_ADDRESS_HPP
