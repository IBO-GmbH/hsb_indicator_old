#ifndef WOLF_VALUE_HANDLER_HPP
#define WOLF_VALUE_HANDLER_HPP

#include <boost/asio/io_service.hpp>
#include <boost/signals2.hpp>
#include <chrono>
#include "database_transaction_helper.hpp"
#include "log/logger.hpp"
#include "profile_handler.hpp"
#include "sensor_value.hpp"
#include "sensor_value_offset_handler.hpp"

namespace wolf {

// TODO this class does way too much and violates the SRP
class sensor_value_handler {
 public:
  using time_point = std::chrono::system_clock::time_point;
  using values_type = std::vector<sensor_value>;

  sensor_value_handler(boost::asio::io_service& service,
                       sensor_value_offset_handler& offset_handler);
  sensor_value_handler(const sensor_value_handler&) = delete;
  virtual ~sensor_value_handler() = default;

  void handle_list(const values_type& values);
  void handle_single_value(const sensor_value& value);

  using entries_type = std::vector<sensor_value>;
  const entries_type& get_all() const;
  virtual boost::optional<float> get_value_float(const sensor_id& id) const;
  boost::optional<sensor_value> get_value(const sensor_id& id) const;

  boost::signals2::signal<void(const values_type&)> signal_values;

 protected:
  virtual void handle(const sensor_values& value);
  virtual void save_value(const sensor_value& value);

 private:
  entries_type::iterator find_sensor(const sensor_id& id);
  entries_type::const_iterator find_sensor(const sensor_id& id) const;
  void handle_offset_updated(const sensor_value_offset& old,
                             const sensor_value_offset& offset);

 private:
  logging::logger m_logger;

  boost::asio::io_service& m_service;
  sensor_value_offset_handler& m_offset_handler;
  entries_type m_values;
};
}  // namespace wolf

#endif  // WOLF_VALUE_HANDLER_HPP
