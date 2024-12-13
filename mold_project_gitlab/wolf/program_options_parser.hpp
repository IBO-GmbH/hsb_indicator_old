#ifndef WOLF_PROGRAM_OPTIONS_PARSER_HPP
#define WOLF_PROGRAM_OPTIONS_PARSER_HPP

#include "log/config.hpp"
#include "log/logger.hpp"
#include "wolf/types.hpp"

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>

namespace wolf {

struct config {
  struct general {
    std::uint64_t mac_address;
    int sensor_activity_count;
    int sensor_values_grey_time;
    bool send_sensor_rssi;
  };
  general general_;

  logging::config::all log_;

  bool use_esp3;
  struct esp3 {
    std::string device;
    std::uint32_t baud;
  };
  esp3 esp3_;

  struct database {
    std::string file;
  };
  database database_;

  bool use_sht25;
  struct sht25 {
    int read_interval;
    std::string device;
    int i2c_slave;
    float temperature_filter_constant;
    float temperature_filter_threshold;
    float temperature_filter_threshold_with_time;
    float humidity_filter_constant;
    float humidity_filter_threshold;
    float humidity_filter_threshold_with_time;
    int filter_threshold_timeout;
  };
  sht25 sht25_;

  struct shtc1 {
    int read_interval;
    std::string device;
    int i2c_slave;
    float temperature_filter_constant;
    float temperature_filter_threshold;
    float temperature_filter_threshold_with_time;
    float humidity_filter_constant;
    float humidity_filter_threshold;
    float humidity_filter_threshold_with_time;
    int filter_threshold_timeout;
  };
  shtc1 shtc1_;

  struct scd30 {
    int read_interval;
    std::string device;
    int i2c_slave;
    bool use_asc;
    float temperature_filter_constant;
    float temperature_filter_threshold;
    float temperature_filter_threshold_with_time;
    float humidity_filter_constant;
    float humidity_filter_threshold;
    float humidity_filter_threshold_with_time;
    float co2_filter_constant;
    float co2_filter_threshold;
    float co2_filter_threshold_with_time;
    int filter_threshold_timeout;
    int wait_time_co2;
  };
  scd30 scd30_;

  struct sgpc3 {
    std::string device;
    int i2c_slave;
    float voc_filter_constant;
    float voc_filter_threshold;
    float voc_filter_threshold_with_time;
    int filter_threshold_timeout;
  };
  sgpc3 sgpc3_;

  struct ee895 {
    int read_interval;
    std::string device;
    int i2c_slave;
    float co2_filter_constant;
    float co2_filter_threshold;
    float co2_filter_threshold_with_time;
    float temperature_filter_constant;
    float temperature_filter_threshold;
    float temperature_filter_threshold_with_time;
    float air_pressure_filter_constant;
    float air_pressure_filter_threshold;
    float air_pressure_filter_threshold_with_time;
    int filter_threshold_timeout;
  };
  ee895 ee895_;

  struct sht85 {
    int read_interval;
    std::string device;
    int i2c_slave;
    float temperature_filter_constant;
    float temperature_filter_threshold;
    float temperature_filter_threshold_with_time;
    float humidity_filter_constant;
    float humidity_filter_threshold;
    float humidity_filter_threshold_with_time;
    int filter_threshold_timeout;
  };
  sht85 sht85_;

  struct tmp117 {
    int read_interval;
    std::string device;
    int i2c_slave;
    float temperature_filter_constant;
    float temperature_filter_threshold;
    float temperature_filter_threshold_with_time;
    int filter_threshold_timeout;
  };
  tmp117 tmp117_;

  bool use_network;
  struct network {
    std::string url;
    int reconnect_timeout;
    bool reconnect_timeout_double;
    int ping_timer;
    // websocketpp gives error:
    // [warning] Warning: a pong_timeout_handler is set but the transport in use
    // does not support timeouts.
    int pong_timeout{};
    bool gzip_compression;
    int response_timeout;
  };
  network network_;

  bool use_tone;
  struct tone {
    int gpio;
    int duration;
    int pause;
  };
  tone tone_;

  struct controller {
    float absolute_humidity_threshold;
    float relative_humidity_threshold;
    float co2_threshold;
    float sprout_value_threshold;
    float voc_threshold;
    float max_absolute_humidity_outdoor;
    float difference_absolute_humidity;
    float invalid_outdoor_temperature;
  };
  controller controller_;

  struct mqtt {
    std::string broker_ip_address;
    std::uint16_t broker_port;
  };
  mqtt mqtt_;
};

class program_options_parser : public boost::noncopyable {
 public:
  program_options_parser() = default;
  virtual ~program_options_parser() = default;

  bool parse(int argc, char* argv[], const std::string& version,
             bool& end_program);

  const config& get_config() const;

 private:
  boost::program_options::options_description create_database();

 protected:
  using additional_options =
      std::vector<boost::program_options::options_description>;
  virtual additional_options create_additional_options();
  virtual void parse_additional_options(
      const boost::program_options::variables_map& vm);

 private:
  config m_config;
};
}  // namespace wolf

#endif  // WOLF_PROGRAM_OPTIONS_PARSER_HPP
