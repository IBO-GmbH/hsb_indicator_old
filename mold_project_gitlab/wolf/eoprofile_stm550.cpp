#include "eoprofile_stm550.hpp"
#include <algorithm>
#include <cmath>
#include "log/logger.hpp"
#include "types.hpp"

eoProfile_stm550::eoProfile_stm550(uint8_t rorg_, uint8_t func_,
                                   uint8_t type_) {
  rorg = rorg_;
  func = func_;
  type = type_;
}

eoReturn eoProfile_stm550::Parse(const eoMessage &eo_msg_) {
  if (eo_msg_.RORG != rorg) return EO_ERROR;
  eo_msg_.copyTo(eo_msg);
  return EO_OK;
}

eoReturn eoProfile_stm550::GetValue(CHANNEL_TYPE type_, float &value) {
  uint32_t raw_value;
  uint32_t range_max;
  switch (type_) {
    case S_TEMP:
      GetRawValue(eo_msg, raw_value, m_offset_temperature, m_size_temperature);
      range_max = get_max_range(m_size_temperature);
      value = ScaleFromRAW(raw_value, 0, range_max, m_min_temperature,
                           m_max_temperature);
      break;
    case S_LUMINANCE:
      GetRawValue(eo_msg, raw_value, m_offset_light, m_size_light);
      range_max = get_max_range(m_size_light);
      value = ScaleFromRAW(raw_value, 0, range_max, m_min_light, m_max_light);
      break;
    case S_ACCEL:
      value = calculate_acceleration();
      break;
    default:
      return EO_ERROR;
  }
  return EO_OK;
}

eoReturn eoProfile_stm550::GetValue(CHANNEL_TYPE type_, uint8_t &value) {
  uint32_t raw_value;
  uint32_t range_max;
  switch (type_) {
    case S_RELHUM:
      GetRawValue(eo_msg, raw_value, m_offset_humidity, m_size_humidity);
      range_max = get_max_range(m_size_humidity);
      value = static_cast<uint8_t>(ScaleFromRAW(
          raw_value, 0, range_max, m_min_humidity, m_max_humidity));
      break;
    case E_WINDOWHANDLE:
      GetRawValue(eo_msg, raw_value, m_offset_magnet, m_size_magnet);
      range_max = get_max_range(m_size_magnet);
      value = static_cast<uint8_t>(
          ScaleFromRAW(raw_value, 0, range_max, m_min_magnet, m_max_magnet));
      break;
    default:
      return EO_ERROR;
  }
  return EO_OK;
}

eoChannelInfo *eoProfile_stm550::GetChannel(uint8_t channelNumber) {
  switch (channelNumber) {
    case 0:
      return create_info_temperature();
    case 1:
      return create_info_humidity();
    case 2:
      return create_info_light();
    case 3:
      return create_info_acceleration();
    case 4:
      return create_info_magnet();
    default:
      return nullptr;
  }
}

uint8_t eoProfile_stm550::GetChannelCount() const { return m_channel_count; }

bool eoProfile_stm550::operator==(const eoProfile &othProfile) const {
  return (this->rorg == othProfile.rorg && this->func == othProfile.func &&
          this->type == othProfile.type);
}

bool eoProfile_stm550::operator!=(const eoProfile &othProfile) const {
  return !(*this == othProfile);
}

float eoProfile_stm550::ScaleFromRAW(uint32_t rawValue, uint32_t rangeMin,
                                     uint32_t rangeMax, float scaleMin,
                                     float scaleMax) {
  return static_cast<float>(rawValue) /
             static_cast<float>(rangeMax - rangeMin) * (scaleMax - scaleMin) +
         scaleMin;
}

eoReturn eoProfile_stm550::GetRawValue(const eoMessage &msg_, uint32_t &value,
                                       uint16_t offset, uint8_t size) {
  std::vector<uint8_t> data;
  for (int count = 0; count < msg_.GetDataLength(); ++count)
    data.push_back(msg_.data[count]);
  uint8_t byte_offset =
      static_cast<uint8_t>(std::floor(static_cast<float>(offset) / 8.f));
  uint8_t byte_size =
      static_cast<uint8_t>(std::ceil(static_cast<float>(offset + size) / 8.f)) -
      byte_offset;
  uint8_t bit_offset = offset % 8;
  uint8_t bit_size = (size + bit_offset) % 8;
  std::vector<uint8_t> to_convert;
  std::copy_n(data.begin() + byte_offset, byte_size,
              std::back_inserter(to_convert));
  if (to_convert.size() == 0) return EO_ERROR;
  // set all bits used by other values to 0
  to_convert.front() = static_cast<uint8_t>(to_convert.front() << bit_offset);
  to_convert.front() = to_convert.front() >> bit_offset;
  // shift bits of last byte completely to the right
  to_convert.back() = to_convert.back() >> (8 - bit_size);
  uint32_t temp_value = 0;
  for (std::size_t count = 0; count < to_convert.size(); ++count) {
    if (count != (to_convert.size() - 1))
      temp_value = temp_value << (8 * count);
    else
      temp_value = temp_value << bit_size;
    temp_value |= to_convert[count];
  }
  value = temp_value;
  return EO_OK;
}

eoChannelInfo *eoProfile_stm550::create_info_temperature() {
  eoChannelInfo *info = new eoChannelInfo;
  info->type = S_TEMP;
  info->signalType = VAL_CURR;
  info->min = m_min_temperature;
  info->max = m_max_temperature;
  return info;
}

eoChannelInfo *eoProfile_stm550::create_info_humidity() {
  eoChannelInfo *info = new eoChannelInfo;
  info->type = S_RELHUM;
  info->signalType = VAL_CURR;
  info->min = m_min_humidity;
  info->max = m_max_humidity;
  return info;
}

eoChannelInfo *eoProfile_stm550::create_info_light() {
  eoChannelInfo *info = new eoChannelInfo;
  info->type = S_LUMINANCE;
  info->signalType = VAL_CURR;
  info->min = m_min_light;
  info->max = m_max_light;
  return info;
}

eoChannelInfo *eoProfile_stm550::create_info_magnet() {
  eoChannelInfo *info = new eoChannelInfo;
  info->type = E_WINDOWHANDLE;
  info->signalType = VAL_CURR;
  info->min = m_min_magnet;
  info->max = m_max_magnet;
  return info;
}

eoChannelInfo *eoProfile_stm550::create_info_acceleration() {
  eoChannelInfo *info = new eoChannelInfo;
  info->type = S_ACCEL;
  info->signalType = VAL_CURR;
  info->min = m_min_acceleration;
  info->max = m_max_acceleration_vector;
  return info;
}

uint32_t eoProfile_stm550::get_max_range(const uint8_t &bitsize) {
  return static_cast<uint32_t>(std::pow(2, bitsize)) - 1;
}

float eoProfile_stm550::calculate_acceleration() {
  uint32_t raw_value_x;
  uint32_t raw_value_y;
  uint32_t raw_value_z;
  uint32_t range_max = get_max_range(m_size_acceleration);
  GetRawValue(eo_msg, raw_value_x, m_offset_acceleration_x,
              m_size_acceleration);
  GetRawValue(eo_msg, raw_value_y, m_offset_acceleration_y,
              m_size_acceleration);
  GetRawValue(eo_msg, raw_value_z, m_offset_acceleration_z,
              m_size_acceleration);
  const auto value_x = ScaleFromRAW(raw_value_x, 0, range_max,
                                    m_min_acceleration, m_max_acceleration);
  const auto value_y = ScaleFromRAW(raw_value_y, 0, range_max,
                                    m_min_acceleration, m_max_acceleration);
  const auto value_z = ScaleFromRAW(raw_value_z, 0, range_max,
                                    m_min_acceleration, m_max_acceleration);
  return static_cast<float>(std::sqrt(
      std::pow(value_x, 2) + std::pow(value_y, 2) + std::pow(value_z, 2)));
}
