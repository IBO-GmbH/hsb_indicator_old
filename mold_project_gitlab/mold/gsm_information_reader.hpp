#ifndef MOLD_GSM_INFORMATION_READER_HPP
#define MOLD_GSM_INFORMATION_READER_HPP

#include <boost/asio/io_service.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/signals2.hpp>
#include "mold/program_options_parser.hpp"

namespace mold {

class gsm_information_reader {
 public:
  using memory = boost::interprocess::managed_shared_memory;
  using char_allocator = boost::interprocess::allocator<
      char, boost::interprocess::managed_shared_memory::segment_manager>;
  using ipc_string =
      boost::interprocess::basic_string<char, std::char_traits<char>,
                                        char_allocator>;
  using optional_memory = std::optional<memory>;
  using duration = std::chrono::system_clock::duration;
  using optional_string = std::optional<std::string>;
  using optional_int = std::optional<std::int32_t>;

  gsm_information_reader(boost::asio::io_service &service,
                         const config &config_);
  ~gsm_information_reader();

  boost::signals2::signal<void(const optional_string &)> signal_iccid;
  boost::signals2::signal<void(const optional_int &)> signal_rssi;
  boost::signals2::signal<void(const optional_string &)> signal_provider;

 private:
  optional_memory open_shared_memory() const;
  void read_from_shared_memory();
  void read_iccid(memory &shared_memory) const;
  void read_rssi(memory &shared_memory) const;
  void read_provider(memory &shared_memory) const;
  void start_timer();
  void cancel_timer();
  void handle_wait(const boost::system::error_code &error);

 private:
  mutable logging::logger m_logger;
  const config &m_config;

  boost::asio::steady_timer m_timer;

  static const std::string shared_memory_name;
  const duration m_time_interval =
      std::chrono::seconds(m_config.gsm_.info_read_timeout);
};
}  // namespace mold

#endif  // MOLD_GSM_INFORMATION_READER_HPP
