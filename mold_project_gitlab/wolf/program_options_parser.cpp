#include "program_options_parser.hpp"
#include <boost/program_options.hpp>
#include <iostream>
#include "hardware_address.hpp"
#include "log/log.hpp"
#include "log/program_options.hpp"

using namespace wolf;

extern template boost::program_options::basic_parsed_options<char>
boost::program_options::parse_config_file(const char *filename,
                                          const options_description &,
                                          bool allow_unregistered = false);

static void ensure_argc_argv_are_valid(int argc, char *argv[]) {
  if (argc >= 1 && argv != nullptr) return;
  throw std::runtime_error("argc/argv are invalid");
}

bool program_options_parser::parse(int argc, char *argv[],
                                   const std::string &version,
                                   bool &end_program) {
  using namespace boost::program_options;

  ensure_argc_argv_are_valid(argc, argv);

  const std::string hardware_address_ = hardware_address::get_hostname_as_hex();
  std::string config_file;
  std::string mac_address_read;

  options_description generic("Generic options");
  generic.add_options()("version,v", "print version string")(
      "help,h", "produce help message")(
      "config,c", value<std::string>(&config_file), "config file to load");
  options_description general("General");
  general.add_options()("mac-address",
                        value<std::string>(&mac_address_read)
                            ->default_value(hardware_address_)
                            ->required(),
                        "mac address to use")(
      "sensor-activity-count",
      value<int>(&m_config.general_.sensor_activity_count)
          ->default_value(100)
          ->required(),
      "how many sensors shall be shown in activity list")(
      "sensor-values-grey-time",
      value<int>(&m_config.general_.sensor_values_grey_time)
          ->default_value(60)
          ->required(),
      "after how much time old sensor values should be printed red")(
      "send-sensor-rssi",
      value<bool>(&m_config.general_.send_sensor_rssi)
          ->default_value(true)
          ->required(),
      "set, if enocean sensor rssi should be sent to the server");
  logging::program_options logging{m_config.log_};
  const options_description logging_options = logging.create_description();
  options_description esp3("esp3");
  esp3.add_options()(
      "use-esp3",
      value<bool>(&m_config.use_esp3)->default_value(false)->required(),
      "use esp3")("esp3-device",
                  value<std::string>(&m_config.esp3_.device)
                      ->default_value("/dev/ttyAMA0")
                      ->required(),
                  "device to open and read")(
      "esp3-baud",
      value<unsigned int>(&m_config.esp3_.baud)
          ->default_value(57600)
          ->required(),
      "baud to open device with");
  options_description database = create_database();
  options_description sht25("sht25");
  sht25.add_options()(
      "use-sht25",
      value<bool>(&m_config.use_sht25)->default_value(false)->required(),
      "use sht25")(
      "sht25-read-interval",
      value<int>(&m_config.sht25_.read_interval)->default_value(10)->required(),
      "read interval for sht25 in seconds")(
      "sht25-device",
      value<std::string>(&m_config.sht25_.device)
          ->default_value("/dev/i2c-1")
          ->required(),
      "i2c device sht25 is connected to")(
      "sht25-i2c-slave",
      value<int>(&m_config.sht25_.i2c_slave)->default_value(0x40)->required(),
      "i2c slave id")(
      "sht25-temperature-filter-constant",
      value<float>(&m_config.sht25_.temperature_filter_constant)
          ->default_value(0.25f)
          ->required(),
      "minimum relative change for sht25 mean value for getting "
      "processed. Set to one to disable.")(
      "sht25-temperature-filter-threshold",
      value<float>(&m_config.sht25_.temperature_filter_threshold)
          ->default_value(1.0f)
          ->required(),
      "minimum absolute change for sht25 mean value for getting processed. Set "
      "to zero to disable.")(
      "sht25-temperature-filter-threshold-with-time",
      value<float>(&m_config.sht25_.temperature_filter_threshold_with_time)
          ->default_value(1.0f * 600.f)
          ->required(),
      "minimum change with time for sht25 mean value for getting processed.")(
      "sht25-humidity-filter-constant",
      value<float>(&m_config.sht25_.humidity_filter_constant)
          ->default_value(0.25f)
          ->required(),
      "minimum relative change for sht25 mean value for getting processed. Set "
      "to one to disable.")(
      "sht25-humidity-filter-threshold",
      value<float>(&m_config.sht25_.humidity_filter_threshold)
          ->default_value(2.0f)
          ->required(),
      "minimum absolute change for sht25 mean value for getting "
      "processed. Set to zero to disable.")(
      "sht25-humidity-filter-threshold-with-time",
      value<float>(&m_config.sht25_.humidity_filter_threshold_with_time)
          ->default_value(2.0f * 600.f)
          ->required(),
      "minimum change with time for sht25 mean value for getting processed.")(
      "sht25-filter-timeout",
      value<int>(&m_config.sht25_.filter_threshold_timeout)
          ->default_value(15 * 60)
          ->required(),
      "timeout in seconds when to ignore epsilon check.");
  options_description shtc1("shtc1");
  shtc1.add_options()(
      "shtc1-read-interval",
      value<int>(&m_config.shtc1_.read_interval)->default_value(10)->required(),
      "read interval for shtc1 in seconds")(
      "shtc1-device",
      value<std::string>(&m_config.shtc1_.device)
          ->default_value("/dev/i2c-1")
          ->required(),
      "i2c device shtc1 is connected to")(
      "shtc1-i2c-slave",
      value<int>(&m_config.shtc1_.i2c_slave)->default_value(0x70)->required(),
      "i2c slave id")(
      "shtc1-temperature-filter-constant",
      value<float>(&m_config.shtc1_.temperature_filter_constant)
          ->default_value(0.25f)
          ->required(),
      "minimum relative change for shtc1 mean value for getting "
      "processed. Set to one to disable.")(
      "shtc1-temperature-filter-threshold",
      value<float>(&m_config.shtc1_.temperature_filter_threshold)
          ->default_value(1.0f)
          ->required(),
      "minimum absolute change for shtc1 mean value for getting processed. Set "
      "to zero to disable.")(
      "shtc1-temperature-filter-threshold-with-time",
      value<float>(&m_config.shtc1_.temperature_filter_threshold_with_time)
          ->default_value(1.0f * 600.f)
          ->required(),
      "minimum change with time for shtc1 mean value for getting processed.")(
      "shtc1-humidity-filter-constant",
      value<float>(&m_config.shtc1_.humidity_filter_constant)
          ->default_value(0.25f)
          ->required(),
      "minimum relative change for shtc1 mean value for getting processed. Set "
      "to one to disable.")(
      "shtc1-humidity-filter-threshold",
      value<float>(&m_config.shtc1_.humidity_filter_threshold)
          ->default_value(2.0f)
          ->required(),
      "minimum absolute change for shtc1 mean value for getting "
      "processed. Set to zero to disable.")(
      "shtc1-humidity-filter-threshold-with-time",
      value<float>(&m_config.shtc1_.humidity_filter_threshold_with_time)
          ->default_value(2.0f * 600.f)
          ->required(),
      "minimum change with time for shtc1 mean value for getting processed.")(
      "shtc1-filter-timeout",
      value<int>(&m_config.shtc1_.filter_threshold_timeout)
          ->default_value(15 * 60)
          ->required(),
      "timeout in seconds when to ignore epsilon check.");
  options_description scd30("scd30");
  scd30.add_options()(
      "scd30-read-interval",
      value<int>(&m_config.scd30_.read_interval)->default_value(10)->required(),
      "read interval for scd30 in seconds")(
      "scd30-device",
      value<std::string>(&m_config.scd30_.device)
          ->default_value("/dev/i2c-1")
          ->required(),
      "i2c device scd30 is connected to")(
      "scd30-i2c-slave",
      value<int>(&m_config.scd30_.i2c_slave)->default_value(0x61)->required(),
      "i2c slave id")(
      "scd30-use-asc",
      value<bool>(&m_config.scd30_.use_asc)->default_value(false)->required(),
      "use automatic self calibration of scd30")(
      "scd30-temperature-filter-constant",
      value<float>(&m_config.scd30_.temperature_filter_constant)
          ->default_value(0.25f)
          ->required(),
      "minimum relative change for scd30 mean value for getting "
      "processed. Set to one to disable.")(
      "scd30-temperature-filter-threshold",
      value<float>(&m_config.scd30_.temperature_filter_threshold)
          ->default_value(1.0f)
          ->required(),
      "minimum absolute change for scd30 mean value for getting processed. Set "
      "to zero to disable.")(
      "scd30-temperature-filter-threshold-with-time",
      value<float>(&m_config.scd30_.temperature_filter_threshold_with_time)
          ->default_value(1.0f * 600.f)
          ->required(),
      "minimum change with time for scd30 mean value for getting processed.")(
      "scd30-humidity-filter-constant",
      value<float>(&m_config.scd30_.humidity_filter_constant)
          ->default_value(0.25f)
          ->required(),
      "minimum relative change for scd30 mean value for getting processed. Set "
      "to one to disable.")(
      "scd30-humidity-filter-threshold",
      value<float>(&m_config.scd30_.humidity_filter_threshold)
          ->default_value(2.0f)
          ->required(),
      "minimum absolute change for scd30 mean value for getting "
      "processed. Set to zero to disable.")(
      "scd30-humidity-filter-threshold-with-time",
      value<float>(&m_config.scd30_.humidity_filter_threshold_with_time)
          ->default_value(2.0f * 600.f)
          ->required(),
      "minimum change with time for scd30 mean value for getting processed.")(
      "scd30-co2-filter-constant",
      value<float>(&m_config.scd30_.co2_filter_constant)
          ->default_value(1.f)
          ->required(),
      "minimum relative change for scd30 mean value for getting processed. Set "
      "to one to disable.")(
      "scd30-co2-filter-threshold",
      value<float>(&m_config.scd30_.co2_filter_threshold)
          ->default_value(10.0f)
          ->required(),
      "minimum absolute change for scd30 mean value for getting "
      "processed. Set to zero to disable.")(
      "scd30-co2-filter-threshold-with-time",
      value<float>(&m_config.scd30_.co2_filter_threshold_with_time)
          ->default_value(50.0f * 600.f)
          ->required(),
      "minimum change with time for scd30 mean value for getting processed.")(
      "scd30-filter-timeout",
      value<int>(&m_config.scd30_.filter_threshold_timeout)
          ->default_value(15 * 60)
          ->required(),
      "timeout in seconds when to ignore epsilon check.")(
      "scd30-wait-time-co2",
      value<int>(&m_config.scd30_.wait_time_co2)->default_value(30)->required(),
      "time in seconds to wait until co2 reading starts");
  options_description sgpc3("sgpc3");
  sgpc3.add_options()("sgpc3-device",
                      value<std::string>(&m_config.sgpc3_.device)
                          ->default_value("/dev/i2c-1")
                          ->required(),
                      "i2c device sgpc3 is connected to")(
      "sgpc3-i2c-slave",
      value<int>(&m_config.sgpc3_.i2c_slave)->default_value(0x58)->required(),
      "i2c slave id")("sgpc3-voc-filter-constant",
                      value<float>(&m_config.sgpc3_.voc_filter_constant)
                          ->default_value(0.125f)
                          ->required(),
                      "minimum relative change for sgpc3 mean value for "
                      "getting processed. Set to one "
                      "to disable.")(
      "sgpc3-voc-filter-threshold",
      value<float>(&m_config.sgpc3_.voc_filter_threshold)
          ->default_value(5.0f)
          ->required(),
      "minimum absolute change for sgpc3 mean value for getting "
      "processed. Set to zero to disable.")(
      "sgpc3-voc-filter-threshold-with-time",
      value<float>(&m_config.sgpc3_.voc_filter_threshold_with_time)
          ->default_value(5.0f * 600.f)
          ->required(),
      "minimum change with time for sgpc3 mean value for getting processed.")(
      "sgpc3-filter-timeout",
      value<int>(&m_config.sgpc3_.filter_threshold_timeout)
          ->default_value(15 * 60)
          ->required(),
      "timeout in seconds when to ignore epsilon check.");
  options_description ee895("ee895");
  ee895.add_options()(
      "ee895-read-interval",
      value<int>(&m_config.ee895_.read_interval)->default_value(15)->required(),
      "read interval for ee895 in seconds")(
      "ee895-device",
      value<std::string>(&m_config.ee895_.device)
          ->default_value("/dev/i2c-1")
          ->required(),
      "i2c device ee895 is connected to")(
      "ee895-i2c-slave",
      value<int>(&m_config.ee895_.i2c_slave)->default_value(0x5e)->required(),
      "i2c slave id")("ee895-co2-filter-constant",
                      value<float>(&m_config.ee895_.co2_filter_constant)
                          ->default_value(1.f)
                          ->required(),
                      "minimum relative change for ee895 mean value for "
                      "getting processed. Set to one to disable.")(
      "ee895-co2-filter-threshold",
      value<float>(&m_config.ee895_.co2_filter_threshold)
          ->default_value(10.0f)
          ->required(),
      "minimum absolute change for ee895 mean value for getting "
      "processed. Set to zero to disable.")(
      "ee895-co2-filter-threshold-with-time",
      value<float>(&m_config.ee895_.co2_filter_threshold_with_time)
          ->default_value(50.0f * 600.f)
          ->required(),
      "minimum change with time for ee895 mean value for getting processed.")(
      "ee895-temperature-filter-constant",
      value<float>(&m_config.ee895_.temperature_filter_constant)
          ->default_value(0.25f)
          ->required(),
      "minimum relative change for ee895 mean value for getting "
      "processed. Set to one to disable.")(
      "ee895-temperature-filter-threshold",
      value<float>(&m_config.ee895_.temperature_filter_threshold)
          ->default_value(1.0f)
          ->required(),
      "minimum absolute change for ee895 mean value for getting processed. Set "
      "to zero to disable.")(
      "ee895-temperature-filter-threshold-with-time",
      value<float>(&m_config.ee895_.temperature_filter_threshold_with_time)
          ->default_value(1.0f * 600.f)
          ->required(),
      "minimum change with time for ee895 mean value for getting processed.")(
      "ee895-air_pressure-filter-constant",
      value<float>(&m_config.ee895_.air_pressure_filter_constant)
          ->default_value(0.25f)
          ->required(),
      "minimum relative change for ee895 mean value for getting processed. Set "
      "to one to disable.")(
      "ee895-air_pressure-filter-threshold",
      value<float>(&m_config.ee895_.air_pressure_filter_threshold)
          ->default_value(50.0f)
          ->required(),
      "minimum absolute change for ee895 mean value for getting "
      "processed. Set to zero to disable.")(
      "ee895-air_pressure-filter-threshold-with-time",
      value<float>(&m_config.ee895_.air_pressure_filter_threshold_with_time)
          ->default_value(50.0f * 600.f)
          ->required(),
      "minimum change with time for ee895 mean value for getting processed.")(
      "ee895-filter-timeout",
      value<int>(&m_config.ee895_.filter_threshold_timeout)
          ->default_value(15 * 60)
          ->required(),
      "timeout in seconds when to ignore epsilon check.");
  options_description sht85("sht85");
  sht85.add_options()(
      "sht85-read-interval",
      value<int>(&m_config.sht85_.read_interval)->default_value(10)->required(),
      "read interval for sht85 in seconds")(
      "sht85-device",
      value<std::string>(&m_config.sht85_.device)
          ->default_value("/dev/i2c-1")
          ->required(),
      "i2c device sht85 is connected to")(
      "sht85-i2c-slave",
      value<int>(&m_config.sht85_.i2c_slave)->default_value(0x44)->required(),
      "i2c slave id")(
      "sht85-temperature-filter-constant",
      value<float>(&m_config.sht85_.temperature_filter_constant)
          ->default_value(0.85f)
          ->required(),
      "minimum relative change for sht85 mean value for getting "
      "processed. Set to one to disable.")(
      "sht85-temperature-filter-threshold",
      value<float>(&m_config.sht85_.temperature_filter_threshold)
          ->default_value(1.0f)
          ->required(),
      "minimum absolute change for sht85 mean value for getting processed. Set "
      "to zero to disable.")(
      "sht85-temperature-filter-threshold-with-time",
      value<float>(&m_config.sht85_.temperature_filter_threshold_with_time)
          ->default_value(1.0f * 600.f)
          ->required(),
      "minimum change with time for sht85 mean value for getting processed.")(
      "sht85-humidity-filter-constant",
      value<float>(&m_config.sht85_.humidity_filter_constant)
          ->default_value(0.85f)
          ->required(),
      "minimum relative change for sht85 mean value for getting processed. Set "
      "to one to disable.")(
      "sht85-humidity-filter-threshold",
      value<float>(&m_config.sht85_.humidity_filter_threshold)
          ->default_value(2.0f)
          ->required(),
      "minimum absolute change for sht85 mean value for getting "
      "processed. Set to zero to disable.")(
      "sht85-humidity-filter-threshold-with-time",
      value<float>(&m_config.sht85_.humidity_filter_threshold_with_time)
          ->default_value(2.0f * 600.f)
          ->required(),
      "minimum change with time for sht85 mean value for getting processed.")(
      "sht85-filter-timeout",
      value<int>(&m_config.sht85_.filter_threshold_timeout)
          ->default_value(15 * 60)
          ->required(),
      "timeout in seconds when to ignore epsilon check.");
  options_description tmp117("tmp117");
  tmp117.add_options()("tmp117-read-interval",
                       value<int>(&m_config.tmp117_.read_interval)
                           ->default_value(10)
                           ->required(),
                       "read interval for tmp117 in seconds")(
      "tmp117-device",
      value<std::string>(&m_config.tmp117_.device)
          ->default_value("/dev/i2c-1")
          ->required(),
      "i2c device tmp117 is connected to")(
      "tmp117-i2c-slave",
      value<int>(&m_config.tmp117_.i2c_slave)->default_value(0x49)->required(),
      "i2c slave id")(
      "tmp117-temperature-filter-constant",
      value<float>(&m_config.tmp117_.temperature_filter_constant)
          ->default_value(0.85f)
          ->required(),
      "minimum relative change for tmp117 mean value for getting "
      "processed. Set to one to disable.")(
      "tmp117-temperature-filter-threshold",
      value<float>(&m_config.tmp117_.temperature_filter_threshold)
          ->default_value(1.0f)
          ->required(),
      "minimum absolute change for tmp117 mean value for getting processed. "
      "Set "
      "to zero to disable.")(
      "tmp117-temperature-filter-threshold-with-time",
      value<float>(&m_config.tmp117_.temperature_filter_threshold_with_time)
          ->default_value(1.0f * 600.f)
          ->required(),
      "minimum change with time for tmp117 mean value for getting processed.")(
      "tmp117-filter-timeout",
      value<int>(&m_config.tmp117_.filter_threshold_timeout)
          ->default_value(15 * 60)
          ->required(),
      "timeout in seconds when to ignore epsilon check.");
  options_description network("Network");
  network.add_options()(
      "use-network",
      value<bool>(&m_config.use_network)->default_value(false)->required(),
      "use network")("network-url",
                     value<std::string>(&m_config.network_.url)
                         ->default_value("ws://probeview.com:4711")
                         ->required(),
                     "websocket host to sync results to")(
      "network-reconnect",
      value<int>(&m_config.network_.reconnect_timeout)
          ->default_value(4)
          ->required(),
      "how long to wait before reconnecting in seconds")(
      "network-reconnect-double",
      value<bool>(&m_config.network_.reconnect_timeout_double)
          ->default_value(true)
          ->required(),
      "double the reconnect timeout (up to ten minutes)")(
      "network-ping-timer",
      value<int>(&m_config.network_.ping_timer)->default_value(0)->required(),
      "the interval a ping shall get send in seconds. 0 disables the ping. "
      "Use it to ensure the http connection doesn't get closed through "
      "firewall.")(
      "network-pong-timeout",
      value<int>(&m_config.network_.pong_timeout)->default_value(0)->required(),
      "the pong timeout. If a timeout occures, connection gets "
      "closed and reopened immediately. Set the value to 0 to "
      "disable the feature.")("network-gzip_compression",
                              value<bool>(&m_config.network_.gzip_compression)
                                  ->default_value(true)
                                  ->required(),
                              "use gzip compression")(
      "network-response-timeout",
      value<int>(&m_config.network_.response_timeout)
          ->default_value(30)
          ->required(),
      "how long to wait before a emitting missing response.");
  options_description tone("Tone");
  tone.add_options()(
      "use-tone",
      value<bool>(&m_config.use_tone)->default_value(false)->required(),
      "use tone output")(
      "tone-gpio",
      value<int>(&m_config.tone_.gpio)->default_value(17)->required(),
      "gpio port")(
      "tone-duration",
      value<int>(&m_config.tone_.duration)->default_value(200)->required(),
      "how long the tone shall last in milliseconds")(
      "tone-pause",
      value<int>(&m_config.tone_.pause)->default_value(100)->required(),
      "how long the pause between multiple tones shall last in milliseconds");
  options_description controller("Controller");
  controller.add_options()(
      "absolute-humidity-threshold",
      value<float>(&m_config.controller_.absolute_humidity_threshold)
          ->default_value(1000.f)
          ->required(),
      "upper threshold for absolute humidity control")(
      "relative-humidity-threshold",
      value<float>(&m_config.controller_.relative_humidity_threshold)
          ->default_value(100.1f)
          ->required(),
      "upper threshold for relative humidity control")(
      "co2-threshold",
      value<float>(&m_config.controller_.co2_threshold)
          ->default_value(20000.f)
          ->required(),
      "upper threshold for co2 control")(
      "sprout-value-threshold",
      value<float>(&m_config.controller_.sprout_value_threshold)
          ->default_value(100.1f)
          ->required(),
      "upper threshold for sprout_value control")(
      "voc-threshold",
      value<float>(&m_config.controller_.voc_threshold)
          ->default_value(5000.f)
          ->required(),
      "upper threshold for voc control")(
      "max-absolute-humidity-outdoor",
      value<float>(&m_config.controller_.max_absolute_humidity_outdoor)
          ->default_value(12.f)
          ->required(),
      "max value for absolute humidity outdoors")(
      "difference-absolute-humidity",
      value<float>(&m_config.controller_.difference_absolute_humidity)
          ->default_value(1.f)
          ->required(),
      "difference of absolute humidity indoors and outdoors")(
      "invalid-outdoor-temperature",
      value<float>(&m_config.controller_.invalid_outdoor_temperature)
          ->default_value(-100.f)
          ->required(),
      "invalid outdoor temperature if there is no outdoor sensor");
  options_description mqtt("MQTT");
  mqtt.add_options()("broker-ip",
                     value<std::string>(&m_config.mqtt_.broker_ip_address)
                         ->default_value("127.0.0.1")
                         ->required(),
                     "ip address of mqtt broker, usually the box itself")(
      "broker-port",
      value<std::uint16_t>(&m_config.mqtt_.broker_port)
          ->default_value(1883)
          ->required(),
      "port of mqtt broker, by default 1883");
  options_description cmdline_options;
  cmdline_options.add(generic);
  cmdline_options.add(general);
  cmdline_options.add(logging_options);
  cmdline_options.add(esp3);
  cmdline_options.add(database);
  cmdline_options.add(sht25);
  cmdline_options.add(shtc1);
  cmdline_options.add(scd30);
  cmdline_options.add(sgpc3);
  cmdline_options.add(ee895);
  cmdline_options.add(sht85);
  cmdline_options.add(tmp117);
  cmdline_options.add(network);
  cmdline_options.add(tone);
  cmdline_options.add(controller);
  cmdline_options.add(mqtt);
  program_options_parser::additional_options additional =
      create_additional_options();
  for (const options_description &work : additional) {
    cmdline_options.add(work);
  }
  variables_map added_options;
  try {
    parsed_options parsed = command_line_parser(argc, argv)
                                .options(cmdline_options)
                                .allow_unregistered()
                                .run();
    store(parsed, added_options);
    auto unregistered =
        collect_unrecognized(parsed.options, include_positional);
    for (const auto &option : unregistered)
      std::cerr << "could not parse: " << option << std::endl;
  } catch (const std::exception &exception) {
    std::cerr << exception.what() << std::endl;
    return false;
  }
  notify(added_options);

  if (added_options.count("help")) {
    std::cout << cmdline_options << std::endl;
    end_program = true;
    return true;
  }
  if (added_options.count("version")) {
    std::cout << version << std::endl;
    end_program = true;
    return true;
  }
  if (added_options.count("config")) {
    parsed_options parsed =
        parse_config_file<char>(config_file.c_str(), cmdline_options, true);
    store(parsed, added_options);
    notify(added_options);
    for (const auto &option : parsed.options) {
      if (option.unregistered)
        std::cerr << "could not parse: " << option.string_key << std::endl;
    }
  }
  // parse mac address
  {
    std::stringstream interpreter;
    interpreter << std::hex << added_options["mac-address"].as<std::string>();
    interpreter >> m_config.general_.mac_address;
  }
  // ensure network valid timeouts. >=0
  m_config.network_.ping_timer = std::max(0, m_config.network_.ping_timer);
  m_config.network_.pong_timeout = std::max(0, m_config.network_.pong_timeout);

  parse_additional_options(added_options);
  return true;
}

template <typename value_type>
static boost::program_options::typed_value<value_type, char> *create_value(
    value_type &to_set, value_type default_) {
  namespace po = boost::program_options;
  auto result = po::value<value_type>(&to_set);
  result->default_value(default_);
  result->required();
  return result;
}

const config &program_options_parser::get_config() const { return m_config; }

boost::program_options::options_description
program_options_parser::create_database() {
  namespace po = boost::program_options;

  po::options_description database("Database");

  const auto value_file =
      create_value<std::string>(m_config.database_.file, "database.db");
  database.add_options()("database-file", value_file, "database name");

  return database;
}

program_options_parser::additional_options
program_options_parser::create_additional_options() {
  return program_options_parser::additional_options();
}

void program_options_parser::parse_additional_options(
    const boost::program_options::variables_map &) {}
