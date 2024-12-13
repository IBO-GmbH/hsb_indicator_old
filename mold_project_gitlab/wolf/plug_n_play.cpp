#include "plug_n_play.hpp"

using namespace logging;

wolf::plug_n_play::plug_n_play(const wolf::config& program_options,
                               wolf::plug_n_play::readers& readers)
    : m_logger("plug_n_play"),
      m_program_options(program_options),
      m_scd30_reader(readers.scd30_reader_),
      m_sgpc3_reader(readers.sgpc3_reader_),
      m_sht25_reader(readers.sht25_reader_),
      m_shtc1_reader(readers.shtc1_reader_),
      m_ee895_reader(readers.ee895_reader_),
      m_sht85_reader(readers.sht85_reader_),
      m_tmp117_reader(readers.tmp117_reader_)

{}

void wolf::plug_n_play::check_sensors() const {
  bool scd30_set = check_scd30();
  bool sgpc3_set = check_sgpc3();
  bool sht25_set = check_sht25();
  bool shtc1_set = check_shtc1();
  bool ee895_set = check_ee895();
  bool sht85_set = check_sht85();
  bool tmp117_set = check_tmp117();
  if (!sht25_set && !shtc1_set && !scd30_set && !sgpc3_set && !ee895_set &&
      !sht85_set && !tmp117_set) {
    signal_none();
    return;
  }
  if (sgpc3_set) signal_sgpc3();
  // sensors should not be able to be the second sensor of themselves
  if (ee895_set)
    signal_ee895(get_second_sensor(shtc1_set, sht25_set, scd30_set, false,
                                   sht85_set, tmp117_set));
  if (scd30_set)
    signal_scd30(get_second_sensor(shtc1_set, sht25_set, false, ee895_set,
                                   sht85_set, tmp117_set));
  if (sht85_set)
    signal_sht85(get_second_sensor(shtc1_set, sht25_set, scd30_set, ee895_set,
                                   false, tmp117_set));
  if (tmp117_set)
    signal_tmp117(get_second_sensor(shtc1_set, sht25_set, scd30_set, ee895_set,
                                    sht85_set, false));
  // shtc1 replaced sht25, so it should not be able to use them at the same
  // time, but if they are, shtc1 has priority
  if (shtc1_set) {
    signal_shtc1(get_second_sensor(false, false, scd30_set, ee895_set,
                                   sht85_set, tmp117_set));
    return;
  }
  if (sht25_set)
    signal_sht25(get_second_sensor(shtc1_set, false, scd30_set, ee895_set,
                                   sht85_set, tmp117_set));
}

bool wolf::plug_n_play::check_sht25() const {
  if (!m_sht25_reader.open(m_program_options.sht25_.device,
                           m_program_options.sht25_.i2c_slave)) {
    LOGGING_LOG(m_logger, severity::normal) << "could not open sht25 ";
    return false;
  }
  return true;
}

bool wolf::plug_n_play::check_shtc1() const {
  if (!m_shtc1_reader.open(m_program_options.shtc1_.device,
                           m_program_options.shtc1_.i2c_slave)) {
    LOGGING_LOG(m_logger, severity::normal) << "could not open shtc1";
    return false;
  }
  return true;
}

bool wolf::plug_n_play::check_scd30() const {
  if (!m_scd30_reader.open(m_program_options.scd30_.device,
                           m_program_options.scd30_.i2c_slave)) {
    LOGGING_LOG(m_logger, severity::normal) << "could not open scd30";
    return false;
  }
  return true;
}

bool wolf::plug_n_play::check_sgpc3() const {
  if (!m_sgpc3_reader.open(m_program_options.sgpc3_.device,
                           m_program_options.sgpc3_.i2c_slave)) {
    LOGGING_LOG(m_logger, severity::normal) << "could not open sgpc3";
    return false;
  }
  return true;
}

bool wolf::plug_n_play::check_ee895() const {
  if (!m_ee895_reader.open(m_program_options.ee895_.device,
                           m_program_options.ee895_.i2c_slave)) {
    LOGGING_LOG(m_logger, severity::normal) << "could not open ee895";
    return false;
  }
  return true;
}

bool wolf::plug_n_play::check_sht85() const {
  if (!m_sht85_reader.open(m_program_options.sht85_.device,
                           m_program_options.sht85_.i2c_slave)) {
    LOGGING_LOG(m_logger, severity::normal) << "could not open sht85 ";
    return false;
  }
  return true;
}

bool wolf::plug_n_play::check_tmp117() const {
  if (!m_tmp117_reader.open(m_program_options.tmp117_.device,
                            m_program_options.tmp117_.i2c_slave)) {
    LOGGING_LOG(m_logger, severity::normal) << "could not open tmp117";
    return false;
  }
  return true;
}

std::optional<wolf::sensor_type> wolf::plug_n_play::get_second_sensor(
    const bool shtc1_set, const bool sht25_set, const bool scd30_set,
    const bool ee895_set, const bool sht85_set, const bool tmp117_set) const {
  if (shtc1_set) return sensor_type::shtc1;
  if (sht25_set) return sensor_type::sht25;
  if (scd30_set) return sensor_type::scd30;
  if (ee895_set) return sensor_type::ee895;
  if (sht85_set) return sensor_type::sht85;
  if (tmp117_set) return sensor_type::tmp117;
  return {};
}
