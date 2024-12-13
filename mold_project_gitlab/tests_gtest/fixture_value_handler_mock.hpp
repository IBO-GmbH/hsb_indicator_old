#ifndef FIXTURE_VALUES_HANDLER_MOCK_HPP
#define FIXTURE_VALUES_HANDLER_MOCK_HPP

#include "wolf/sensor_value_handler.hpp"

class fixture_value_handler_mock : public wolf::sensor_value_handler {
 public:
  float check_humidtiy;
  float check_temperature;
  float check_co2;
  float check_sprout_value;
  float check_voc;

  fixture_value_handler_mock(boost::asio::io_service& io_service);

  void save_value(const wolf::sensor_value& value) override;
  void set_check_value(const float value);

  boost::optional<float> get_value_float(const wolf::sensor_id& id) const override;
  void set_humidity(const float humidity);
  void set_temperature(const float temperature);
  void set_co2(const float co2);
  void set_sprout_value(const float sprout_value);
  void set_voc(const float voc);

 private:
  float check_value;
};

#endif  // FIXTURE_VALUES_HANDLER_MOCK_HPP
