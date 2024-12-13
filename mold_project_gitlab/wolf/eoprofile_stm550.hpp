#ifndef WOLF_EOPROFILE_STM550_HPP
#define WOLF_EOPROFILE_STM550_HPP

#include <eoProfile.h>

class eoProfile_stm550 : public eoProfile {
 public:
  eoProfile_stm550(uint8_t rorg_, uint8_t func_, uint8_t type_);

  eoReturn Parse(const eoMessage &eo_msg_) override;
  eoReturn GetValue(CHANNEL_TYPE type, float &value) override;
  eoReturn GetValue(CHANNEL_TYPE type, uint8_t &value) override;
  eoChannelInfo *GetChannel(uint8_t channelNumber) override;
  uint8_t GetChannelCount() const override;

  bool operator==(const eoProfile &othProfile) const override;
  bool operator!=(const eoProfile &othProfile) const override;

 private:
  float ScaleFromRAW(uint32_t rawValue, uint32_t rangeMin, uint32_t rangeMax,
                     float scaleMin, float scaleMax);
  static eoReturn GetRawValue(const eoMessage &msg, uint32_t &value,
                              uint16_t offset, uint8_t size);
  eoChannelInfo *create_info_temperature();
  eoChannelInfo *create_info_humidity();
  eoChannelInfo *create_info_light();
  eoChannelInfo *create_info_magnet();
  eoChannelInfo *create_info_acceleration();
  uint32_t get_max_range(const uint8_t &bitsize);
  float calculate_acceleration();

 private:
  static constexpr uint8_t m_message_byte_count = 9;
  eoMessage eo_msg{m_message_byte_count};
  static constexpr uint8_t m_channel_count = 5;

  static constexpr uint16_t m_offset_temperature = 0;
  static constexpr uint8_t m_size_temperature = 10;
  static constexpr uint16_t m_offset_humidity =
      m_offset_temperature + m_size_temperature;
  static constexpr uint8_t m_size_humidity = 8;
  static constexpr uint16_t m_offset_light =
      m_offset_humidity + m_size_humidity;
  static constexpr uint8_t m_size_light = 17;
  static constexpr uint16_t m_offset_acceleration_x =
      m_offset_light + m_size_light;
  static constexpr uint16_t m_size_acceleration = 10;
  static constexpr uint16_t m_offset_acceleration_y =
      m_offset_acceleration_x + m_size_acceleration;
  static constexpr uint16_t m_offset_acceleration_z =
      m_offset_acceleration_y + m_size_acceleration;
  static constexpr uint16_t m_size_acceleration_status = 2;
  static constexpr uint16_t m_offset_magnet = m_offset_acceleration_z +
                                              m_size_acceleration +
                                              m_size_acceleration_status;
  static constexpr uint8_t m_size_magnet = 1;

  static constexpr float m_min_temperature = -40.f;
  static constexpr float m_max_temperature = 60.f;
  static constexpr float m_min_humidity = 0.f;
  static constexpr float m_max_humidity = 100.f;
  static constexpr float m_min_light = 0.f;
  static constexpr float m_max_light = 100000.f;
  static constexpr float m_min_magnet = 0.f;
  static constexpr float m_max_magnet = 1.f;
  static constexpr float m_min_acceleration = -2.5f;
  static constexpr float m_max_acceleration = 2.5f;
  // sqrt(3*2,5²)
  static constexpr float m_max_acceleration_vector = 4.33f;
};

#endif  // WOLF_EOPROFILE_STM550_HPP
