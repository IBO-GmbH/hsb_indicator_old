#include "comfort_questionnaire_handler.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <ctime>
#include "wolf/sensor_id_comfort_questionnaire.hpp"

using namespace logging;

mold::comfort_questionnaire_handler::comfort_questionnaire_handler(
    mold::comfort_questionnaire_cache *const cache,
    wolf::sensor_value_handler &value_handler,
    boost::asio::io_service &io_service)
    : m_logger("comfort_questionnaire_handler"),
      m_cache(cache),
      m_value_handler(value_handler),
      m_cancel_last_questionnaire_timer(io_service),
      m_open_next_questionnaire_timer(io_service),
      m_options(default_options) {
  if (m_cache) m_options = m_cache->get_options().value_or(default_options);
  calculate_next_questionnaire_time(std::chrono::system_clock::now());
  create_sensors();
  set_cancel_timer();
}

void mold::comfort_questionnaire_handler::set_options(
    const mold::questionnaire_options &options) {
  if (m_cache) m_cache->set_options(options);
  m_options = options;
  if (m_options.enabled)
    add_sensors();
  else {
    remove_sensors();
    cancel_all_timers();
  }
  calculate_next_questionnaire_time(std::chrono::system_clock::now());
}

mold::questionnaire_options mold::comfort_questionnaire_handler::get_options() {
  if (m_cache) {
    auto loaded_options = m_cache->get_options();
    if (!loaded_options.has_value())
      LOGGING_LOG(m_logger, severity::normal) << "could not load options";
    else {
      LOGGING_LOG(m_logger, severity::verbose)
          << "loaded_options: " << loaded_options.value();
      m_options = loaded_options.value();
    }
  }
  return m_options;
}

void mold::comfort_questionnaire_handler::deactivate_questionnaire() {
  change_activated_status(false);
  signal_cancel_last_questionnaire();
  remove_sensors();
}

void mold::comfort_questionnaire_handler::close() { cancel_all_timers(); }

void mold::comfort_questionnaire_handler::delay_questionnaire() {
  boost::system::error_code error;
  LOGGING_LOG(m_logger, severity::verbose)
      << "delay questionnaire for 30 minutes, then ask again";
  m_open_next_questionnaire_timer.expires_from_now(
      boost::posix_time::minutes(30), error);
  start_next_questionnaire_timer(error);
}

void mold::comfort_questionnaire_handler::save_questionnaire(
    const mold::questionnaire &to_save) {
  LOGGING_LOG(m_logger, severity::verbose) << "save questionnaire: " << to_save;
  m_last_questionnaire = to_save;
  const auto values = convert_questionnaire_to_sensor_values(to_save);
  m_value_handler.handle_list(values);
  calculate_next_questionnaire_time(to_save.timestamp);
}

void mold::comfort_questionnaire_handler::discard_questionnaire() {
  LOGGING_LOG(m_logger, severity::verbose)
      << "discard questionnaire and start time for next one";
  calculate_next_questionnaire_time(std::chrono::system_clock::now());
}

void mold::comfort_questionnaire_handler::add_sensors_if_necessary() {
  if (m_options.enabled) add_sensors();
}

std::optional<mold::questionnaire>
mold::comfort_questionnaire_handler::get_last_questionnaire() {
  return m_last_questionnaire;
}

void mold::comfort_questionnaire_handler::change_activated_status(
    const bool status) {
  auto options = m_options;
  options.enabled = status;
  set_options(options);
}

void mold::comfort_questionnaire_handler::set_cancel_timer() {
  if (!m_options.enabled) return;
  std::tm *set_time = convert_timepoint_to_tm(std::chrono::system_clock::now());
  if (set_time->tm_hour >= 22 && set_time->tm_min > 0) ++set_time->tm_mday;
  set_time->tm_hour = 22;
  set_time->tm_min = 0;
  boost::system::error_code error;
  m_cancel_last_questionnaire_timer.expires_at(
      boost::posix_time::from_time_t(std::mktime(set_time)), error);
  start_cancel_timer(error,
                     "could not set expire time to "
                     "cancel_last_questionnaire_timer, message: ",
                     *set_time);
}

void mold::comfort_questionnaire_handler::restart_cancel_timer() {
  boost::system::error_code error;
  m_cancel_last_questionnaire_timer.expires_from_now(
      boost::posix_time::hours(24), error);
  const auto expire_time = m_cancel_last_questionnaire_timer.expires_at();
  start_cancel_timer(
      error, "could not restart cancel_last_questionnaire_timer, message: ",
      boost::posix_time::to_tm(expire_time));
}

void mold::comfort_questionnaire_handler::start_cancel_timer(
    const boost::system::error_code &error, const std::string &error_text,
    const tm &next_time) {
  LOGGING_LOG(m_logger, severity::verbose)
      << "set next cancel time to: "
      << std::put_time(&next_time, "%d-%m-%Y %X");
  if (error) {
    LOGGING_LOG(m_logger, severity::warning) << error_text << error.message();
    return;
  }
  m_cancel_last_questionnaire_timer.async_wait(
      [this](const boost::system::error_code &error) {
        handle_wait_cancel_timer(error);
      });
}

void mold::comfort_questionnaire_handler::handle_wait_cancel_timer(
    const boost::system::error_code &error) {
  if (error) {
    if (error.value() != boost::asio::error::operation_aborted)
      LOGGING_LOG(m_logger, severity::warning)
          << "cancel_last_questionnaire_timer timed out with error, message:"
          << error.message();
    return;
  }
  LOGGING_LOG(m_logger, severity::verbose)
      << "handel_wait_cancel_timer expired, discard last pending questionnaire "
         "and set next timer";
  set_next_questionnaire_timer(m_options.hours_time[0],
                               m_options.minutes_time[0], true);
  restart_cancel_timer();
  signal_cancel_last_questionnaire();
}

void mold::comfort_questionnaire_handler::set_next_questionnaire_timer(
    const int hours, const int minutes, const bool next_day) {
  LOGGING_LOG(m_logger, severity::verbose)
      << "set next questionnaire timer to: " << hours << ":"
      << std::setfill('0') << std::setw(2) << minutes
      << ", next day: " << next_day;
  std::tm *set_time = convert_timepoint_to_tm(std::chrono::system_clock::now());
  if (next_day) ++set_time->tm_mday;
  set_time->tm_hour = hours;
  set_time->tm_min = minutes;
  boost::system::error_code error;
  m_open_next_questionnaire_timer.expires_at(
      boost::posix_time::from_time_t(std::mktime(set_time)), error);
  start_next_questionnaire_timer(error);
}

void mold::comfort_questionnaire_handler::cancel_all_timers() {
  LOGGING_LOG(m_logger, severity::verbose) << "cancel_all_timers";
  boost::system::error_code error;
  m_cancel_last_questionnaire_timer.cancel(error);
  if (error)
    LOGGING_LOG(m_logger, severity::warning)
        << "could not cancel cancel_last_questionnaire_timer, message:"
        << error.message();
  m_open_next_questionnaire_timer.cancel(error);
  if (error)
    LOGGING_LOG(m_logger, severity::warning)
        << "could not cancel open_next_questionnaire_timer, message:"
        << error.message();
}

void mold::comfort_questionnaire_handler::calculate_next_questionnaire_time(
    const std::chrono::system_clock::time_point &time) {
  if (!m_options.enabled) return;
  std::tm *set_time = convert_timepoint_to_tm(time);
  LOGGING_LOG(m_logger, severity::verbose)
      << "calculate new time for: " << std::put_time(set_time, "%d-%m-%Y %X");
  int set_hours = m_options.hours_time[0];
  int set_minutes = m_options.minutes_time[0];
  bool next_day = true;
  for (unsigned int index = 0;
       index < static_cast<unsigned int>(m_options.number_of_questionnaires);
       ++index) {
    if ((m_options.hours_time[index] - set_time->tm_hour) < 0) continue;
    if ((m_options.hours_time[index] - set_time->tm_hour) == 0 &&
        (m_options.minutes_time[index] - set_time->tm_min) <= 0)
      continue;
    set_hours = m_options.hours_time[index];
    set_minutes = m_options.minutes_time[index];
    next_day = false;
    break;
  }
  set_next_questionnaire_timer(set_hours, set_minutes, next_day);
}

tm *mold::comfort_questionnaire_handler::convert_timepoint_to_tm(
    const std::chrono::system_clock::time_point &time) {
  std::time_t current_time = std::chrono::system_clock::to_time_t(time);
  return std::localtime(&current_time);
}

void mold::comfort_questionnaire_handler::handle_wait_next_questionnaire_timer(
    const boost::system::error_code &error) {
  if (error) {
    if (error.value() != boost::asio::error::operation_aborted)
      LOGGING_LOG(m_logger, severity::warning)
          << "open_next_questionnaire_timer timed out with error, message:"
          << error.message();
    return;
  }
  signal_open_next_questionnaire();
}

void mold::comfort_questionnaire_handler::start_next_questionnaire_timer(
    const boost::system::error_code &error) {
  if (error) {
    LOGGING_LOG(m_logger, severity::warning)
        << "could not set expire time to open_next_questionnaire_timer, "
           "message: "
        << error.message();
    return;
  }
  m_open_next_questionnaire_timer.async_wait(
      [this](const boost::system::error_code &error) {
        handle_wait_next_questionnaire_timer(error);
      });
}

void mold::comfort_questionnaire_handler::create_sensors() {
  questionnaire_sensors sensors;
  sensors.push_back(wolf::sensor_id_comfort_questionnaire::create(
      wolf::sensor_value_type::questionnaire_gender));
  sensors.push_back(wolf::sensor_id_comfort_questionnaire::create(
      wolf::sensor_value_type::questionnaire_age));
  sensors.push_back(wolf::sensor_id_comfort_questionnaire::create(
      wolf::sensor_value_type::questionnaire_weight));
  sensors.push_back(wolf::sensor_id_comfort_questionnaire::create(
      wolf::sensor_value_type::questionnaire_height));
  sensors.push_back(wolf::sensor_id_comfort_questionnaire::create(
      wolf::sensor_value_type::questionnaire_clothing));
  sensors.push_back(wolf::sensor_id_comfort_questionnaire::create(
      wolf::sensor_value_type::questionnaire_activity));
  sensors.push_back(wolf::sensor_id_comfort_questionnaire::create(
      wolf::sensor_value_type::questionnaire_temperature));
  sensors.push_back(wolf::sensor_id_comfort_questionnaire::create(
      wolf::sensor_value_type::questionnaire_thermic_comfort));
  sensors.push_back(wolf::sensor_id_comfort_questionnaire::create(
      wolf::sensor_value_type::questionnaire_heating));
  sensors.push_back(wolf::sensor_id_comfort_questionnaire::create(
      wolf::sensor_value_type::questionnaire_air_quality));
  sensors.push_back(wolf::sensor_id_comfort_questionnaire::create(
      wolf::sensor_value_type::questionnaire_humidity));
  m_sensors = sensors;
}

void mold::comfort_questionnaire_handler::add_sensors() {
  signal_change_questionnaire_sensors(true, m_sensors);
}

void mold::comfort_questionnaire_handler::remove_sensors() {
  signal_change_questionnaire_sensors(false, m_sensors);
}

mold::comfort_questionnaire_handler::sensor_values
mold::comfort_questionnaire_handler::convert_questionnaire_to_sensor_values(
    const mold::questionnaire &to_convert) {
  sensor_values values;
  values.push_back({m_sensors[0], static_cast<float>(to_convert.gender_),
                    to_convert.timestamp});
  values.push_back({m_sensors[1], static_cast<float>(to_convert.age_),
                    to_convert.timestamp});
  values.push_back({m_sensors[2], static_cast<float>(to_convert.weight_),
                    to_convert.timestamp});
  values.push_back({m_sensors[3], static_cast<float>(to_convert.height_),
                    to_convert.timestamp});
  values.push_back({m_sensors[4], static_cast<float>(to_convert.clothing_),
                    to_convert.timestamp});
  values.push_back({m_sensors[5], static_cast<float>(to_convert.activity_),
                    to_convert.timestamp});
  values.push_back({m_sensors[6], static_cast<float>(to_convert.temperature_),
                    to_convert.timestamp});
  values.push_back({m_sensors[7],
                    static_cast<float>(to_convert.thermic_comfort_),
                    to_convert.timestamp});
  values.push_back({m_sensors[8], static_cast<float>(to_convert.heating_),
                    to_convert.timestamp});
  values.push_back({m_sensors[9], static_cast<float>(to_convert.air_quality_),
                    to_convert.timestamp});
  values.push_back({m_sensors[10], static_cast<float>(to_convert.humidity_),
                    to_convert.timestamp});
  return values;
}
