#ifndef PROVIDER_LIST_HPP
#define PROVIDER_LIST_HPP

#include <boost/signals2/signal.hpp>
#include <ostream>
#include <string>
#include <vector>
#include "at_read_write.hpp"
#include "log/logger.hpp"

class provider_list {
 public:
  enum class stat : int { unknown = 0, available, current, forbidden };
  struct item {
    stat status;
    std::string name;
    std::string name_short;
    std::string operator_;
  };
  using items_type = std::vector<item>;
  std::vector<item> items;

  provider_list(at_read_write& read_write);
  void refresh();
  boost::signals2::signal<void()> signal_refreshed;

 private:
  void handle_at_read();
  void parse_cops(const std::string& parse);

  mutable logging::logger m_logger;
  at_read_write& read_write;
};

std::ostream& operator<<(std::ostream& out, const provider_list::stat print);
std::ostream& operator<<(std::ostream& out, const provider_list::item print);

#endif  // PROVIDER_LIST_HPP
