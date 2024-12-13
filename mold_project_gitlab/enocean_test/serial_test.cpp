#include <boost/asio/signal_set.hpp>
#include <boost/program_options.hpp>
#include <iostream>
#include "log/log.hpp"
#include "log/logger.hpp"
#include "wolf/enocean_link.hpp"
#include "wolf/esp3_parser.hpp"
#include "wolf/sensor_id_enocean.hpp"

struct config {
  std::string device;
  std::uint32_t baud_rate;
};

boost::optional<config> parse_options(const int argc,
                                      const char* const argv[]) {
  using namespace boost::program_options;

  config result;
  options_description generic("Generic options");
  generic.add_options()("version,v", "print version string");
  generic.add_options()("help,h", "produce help message");
  options_description general("General");
  const auto device_value = value<std::string>(&result.device)
                                ->default_value("/dev/ttySC0")
                                ->required();
  general.add_options()("device", device_value, "serial device to open");
  const auto baud_rate_value =
      value<std::uint32_t>(&result.baud_rate)->default_value(57600)->required();
  general.add_options()("baud-rate", baud_rate_value,
                        "baud rate to open the serial device with");
  options_description cmdline_options;
  cmdline_options.add(generic).add(general);
  variables_map added_options;
  try {
    store(parse_command_line(argc, argv, cmdline_options), added_options);
  } catch (const std::exception& exception) {
    std::cerr << exception.what() << std::endl;
    return {};
  }
  notify(added_options);
  if (added_options.count("help")) {
    std::cout << cmdline_options << std::endl;
    return {};
  }
  return boost::optional<config>{result};
}

int main(int argc, char* argv[]) {
  logging::initialize();
  logging::logger main_logger("main");
  const auto maybe_config = parse_options(argc, argv);
  if (!maybe_config) return 0;
  const config config_ = maybe_config.get();

  LOGGING_LOG(main_logger, logging::severity::normal) << "starting up";

  boost::asio::io_service service;
  wolf::serial_interface reader{service};
  wolf::esp3_parser interpreter{};
  wolf::enocean_link parser{};

  if (!reader.open(config_.device, config_.baud_rate)) {
    LOGGING_LOG(main_logger, logging::severity::error)
        << "could not initialse serial reader";
    return 1;
  }
  reader.signal_read.connect(
      [&](const wolf::serial_interface::read_buffer& data,
          uint32_t read_count) { interpreter.handle_data(data, read_count); });
  interpreter.signal_learned.connect(
      [&](const wolf::types::id_esp3& id, const std::uint8_t& r_org,
          const std::uint8_t& func, const std::uint8_t& type) {
        parser.register_device(id, r_org, func, type, false);
      });
  interpreter.signal_data_4bs.connect(
      [&](const wolf::types::id_esp3& id, const wolf::types::data& data,
          const int /*rssi*/) { parser.parse_telegram(id, data, false); });

  boost::asio::signal_set signals_to_listen_to(service, SIGINT, SIGTERM);
  signals_to_listen_to.async_wait(
      [&](const boost::system::error_code& error, int /*signal_number*/) {
        if (error) {
          LOGGING_LOG(main_logger, logging::severity::error)
              << "signal_handler called with error:" << error.message();
          return;
        }
        LOGGING_LOG(main_logger, logging::severity::normal) << "shutting down";
        reader.close();
      });

  reader.read_async();
  service.run();

  return 0;
}
