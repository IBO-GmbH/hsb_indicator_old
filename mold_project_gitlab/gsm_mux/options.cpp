#include "options.hpp"
#include <boost/program_options.hpp>
#include <iostream>
#include "log/program_options.hpp"

options::options() = default;

bool options::parse(int argc, char* argv[]) {
  namespace po = boost::program_options;

  po::options_description general("Allowed options");
  auto& config_general = config_.general_;
  general.add_options()("help", "produce help message")(
      "pin", po::value<int>(&config_general.pin)->default_value(16)->required(),
      "set pin that en/disables the modem")(
      "device",
      po::value<std::string>(&config_general.device)
          ->default_value("/dev/ttyAMA0")
          ->required(),
      "the serial device to open.")(
      "online-check-url",
      po::value<std::string>(&config_general.online_check_url)
          ->default_value("probeview.com")
          ->required(),
      "the url used to check if online")(
      "run_pppd",
      po::value<bool>(&config_general.run_pppd)
          ->default_value(true)
          ->required(),
      "shall the program run pppd or not.")(
      "isp-name",
      po::value<std::string>(&config_general.isp_name)
          ->default_value("tma")
          ->required(),
      "the isp to connet to. it's the name of the file in /etc/ppp/peers/");
  const auto log = logging::program_options{config_.log}.create_description();

  po::options_description all;
  all.add(general);
  all.add(log);

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, all), vm);
  po::notify(vm);

  if (vm.count("help")) {
    std::cout << all << std::endl;
    return false;
  }
  return true;
}
