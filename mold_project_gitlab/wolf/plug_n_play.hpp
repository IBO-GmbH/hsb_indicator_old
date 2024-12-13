#ifndef WOLF_PLUG_N_PLAY_HPP
#define WOLF_PLUG_N_PLAY_HPP

#include <boost/signals2.hpp>
#include "ee895_reader.hpp"
#include "log/logger.hpp"
#include "scd30_reader.hpp"
#include "sensor_type.hpp"
#include "sgpc3_reader.hpp"
#include "sht25_reader.hpp"
#include "sht85_reader.hpp"
#include "shtc1_reader.hpp"
#include "tmp117_reader.hpp"
#include "wolf/program_options_parser.hpp"

namespace wolf {

class plug_n_play {
 public:
  struct readers {
    scd30_reader& scd30_reader_;
    sgpc3_reader& sgpc3_reader_;
    sht25_reader& sht25_reader_;
    shtc1_reader& shtc1_reader_;
    ee895_reader& ee895_reader_;
    sht85_reader& sht85_reader_;
    tmp117_reader& tmp117_reader_;
  };

  plug_n_play(const config& program_options, readers& readers);
  virtual void check_sensors() const;

  virtual ~plug_n_play() = default;

  // priorities: shtc1/sht25 > scd30 temp & hum
  //             shtc1/sht25 & scd30 > ee895 temp
  //             ee895 > scd30 co2
  //             tmp117 > all other temp
  //             all other temp & hum > sht85
  boost::signals2::signal<void(const std::optional<sensor_type>)> signal_scd30;
  boost::signals2::signal<void()> signal_sgpc3;
  boost::signals2::signal<void(const std::optional<sensor_type>)> signal_sht25;
  boost::signals2::signal<void(const std::optional<sensor_type>)> signal_shtc1;
  boost::signals2::signal<void(const std::optional<sensor_type>)> signal_ee895;
  boost::signals2::signal<void(const std::optional<sensor_type>)> signal_sht85;
  boost::signals2::signal<void(const std::optional<sensor_type>)> signal_tmp117;
  boost::signals2::signal<void()> signal_none;

 private:
  bool check_sht25() const;
  bool check_shtc1() const;
  bool check_scd30() const;
  bool check_sgpc3() const;
  bool check_ee895() const;
  bool check_sht85() const;
  bool check_tmp117() const;
  std::optional<sensor_type> get_second_sensor(
      const bool shtc1_set, const bool sht25_set, const bool scd30_set,
      const bool ee895_set, const bool sht85_set, const bool tmp117_set) const;

 private:
  mutable logging::logger m_logger;
  const config m_program_options;
  scd30_reader& m_scd30_reader;
  sgpc3_reader& m_sgpc3_reader;
  sht25_reader& m_sht25_reader;
  shtc1_reader& m_shtc1_reader;
  ee895_reader& m_ee895_reader;
  sht85_reader& m_sht85_reader;
  tmp117_reader& m_tmp117_reader;
};
}  // namespace wolf

#endif  // WOLF_PLUG_N_PLAY_HPP
