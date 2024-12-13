#ifndef GSM_MUX_HPP
#define GSM_MUX_HPP

#include <boost/asio/io_service.hpp>
#include <boost/asio/serial_port.hpp>
#include <boost/asio/signal_set.hpp>
#include "command_handler.hpp"
#include "config.hpp"
#include "iccid_reader.hpp"
#include "log/logger.hpp"
#include "pppd_handler.hpp"
#include "provider_handler.hpp"
#include "stats_reader.hpp"

class gsm_mux {
 public:
  gsm_mux(const config::general& config_);
  ~gsm_mux();

  int run();

 private:
  void wait_for_signal();
  void stop();
  void open_serial();
  void setup_serial();
  void wait_for_ready();
  void enable_modem_cmux();
  void set_line_discipline();
  void setup_n_gsm();
  void enable_daemon();
  int get_major();
  void create_nodes(int major);
  void remove_nodes();
  void send_disconnect();
  void remove_node(std::string name);
  void create_node(std::string name, int major, int minor);
  [[noreturn]] void trigger_abnormal_exit(const int code);

 private:
  mutable logging::logger m_logger;
  const config::general config_;
  boost::asio::io_service service;
  boost::asio::signal_set signals{service, SIGINT, SIGTERM};
  boost::asio::serial_port serial{service};
  const std::string base_name = "/dev/ttyGSM";
  static constexpr int num_nodes = 4;
  command_handler command{serial, service};
  iccid_reader iccid_reader_{command};

  stats_reader stats{service};
  boost::asio::serial_port provider_serial{service};
  online_checker online_checker_{service, config_.online_check_url};
  provider_handler provider_handler_{service, provider_serial, online_checker_};
  pppd_output_checker pppd_checker{service};
  pppd_handler pppd{service, config_.isp_name, pppd_checker};
  boost::signals2::connection connection_provider_refresh;

  static const std::string shared_memory_name;

  boost::interprocess::managed_shared_memory shared_memory;
  int exit_code{};
};

#endif  // GSM_MUX_HPP
