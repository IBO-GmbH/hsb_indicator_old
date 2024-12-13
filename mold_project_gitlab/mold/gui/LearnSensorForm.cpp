#include "LearnSensorForm.hpp"
#include <QTimer>
#include "NumberFactory.hpp"
#include "NumberTemperature.hpp"
#include "SensorValueTypeToText.hpp"
#include "mold/gui/DataExchange.hpp"
#include "ui_LearnSensorForm.h"
#include "wolf/enocean_channel_helper.hpp"
#include "wolf/room_handler.hpp"
#include "wolf/sensor_id_enocean.hpp"
#include "wolf/thread_checker.hpp"

using namespace mold;
using namespace logging;

LearnSensorForm::LearnSensorForm(WolfServices& services,
                                 DataExchange& dataExchange, QWidget* parent)
    : QWidget(parent),
      ui(new Ui::LearnSensorForm),
      m_logger{"LearnSensorForm"},
      m_services(services),
      m_dataExchange(dataExchange) {
  ui->setupUi(this);
  ui->sensorLearnSuccess->hide();
  createDefaultSeonsorValues();

  // signals
  m_connection_learn = connect(&m_dataExchange, &DataExchange::learned, this,
                               &LearnSensorForm::handleLearned);
  connect(&m_dataExchange, &DataExchange::data_4bs_or_vld, this,
          [this](const wolf::types::id_esp3& id, const wolf::types::data& data,
                 const int /*rssi*/) {
            wolf::thread_checker::throw_if_not_ui(
                "LearnSensorForm::LearnSensorForm");
            m_enocean.parse_telegram(id, data, false);
          });
  m_connection_enocean_data = m_enocean.signal_parsed.connect(
      [this](const wolf::types::id_esp3& /*id*/,
             const wolf::enocean_link::parse_results& data) {
        handleEnoceanParsed(data);
      });
  connect(ui->back, &QPushButton::clicked, this, &LearnSensorForm::handleBack);
  hideBackIfNoRooms();
}

LearnSensorForm::~LearnSensorForm() = default;

static bool has_value_a_value(const LearnSensorForm::value& check) {
  return check.last.is_initialized();
}

struct cast_internal_value_to_sensor_value {
  std::chrono::system_clock::time_point timestamp;

  wolf::sensor_value operator()(const LearnSensorForm::value& to_cast) {
    wolf::sensor_value result{to_cast.id, to_cast.last.get(), timestamp};
    return result;
  }
};

std::vector<wolf::sensor_value> LearnSensorForm::getValues() const {
  if (m_last_enocean_id == 0) return {};
  std::vector<LearnSensorForm::value> with_value;
  std::copy_if(m_values.cbegin(), m_values.cend(),
               std::back_inserter(with_value), has_value_a_value);
  std::vector<wolf::sensor_value> result;
  std::transform(with_value.cbegin(), with_value.cend(),
                 std::back_inserter(result),
                 cast_internal_value_to_sensor_value{m_values_timestamp});
  return result;
}

wolf::types::id_esp3 LearnSensorForm::getLastEnoceanId() const {
  return m_last_enocean_id;
}

std::array<std::uint8_t, 3> LearnSensorForm::getEep() const {
  return m_last_eep;
}

std::vector<wolf::sensor_id> LearnSensorForm::getLearnedIds() const {
  std::vector<wolf::sensor_id> result;
  for (const auto& value : m_values) result.push_back(value.id);
  return result;
}

void LearnSensorForm::createDefaultSeonsorValues() {
  NumberBase* const temperatureWidget =
      NumberFactory::createSpecial(NumberFactory::unity::temperature, this);
  m_values.push_back(
      createValue(S_TEMP, optional_subchannel(), temperatureWidget));
  NumberBase* const humidityWidget =
      NumberFactory::createSpecial(NumberFactory::unity::humidity, this);
  m_values.push_back(
      createValue(S_RELHUM, optional_subchannel(), humidityWidget));

  alignValueWidgets();
}

void LearnSensorForm::setSensorValuesToUi() {
  wolf::thread_checker::throw_if_not_ui("LearnSensorForm::setSensorValuesToUi");
  for (const auto& value_ : m_values) {
    if (!value_.last) continue;
    if (!value_.widget)
      throw std::runtime_error(
          "LearnSensorForm::setSensorValuesToUi: widget has no value");
    if (value_.displayable) value_.widget->setValue(value_.last.get());
  }

  // make a success blink
  ui->sensorLearnSuccess->hide();
  QTimer::singleShot(250, ui->sensorLearnSuccess, &QLabel::show);
}

void LearnSensorForm::handleLearned(const wolf::types::id_esp3 id,
                                    const std::uint8_t& org,
                                    const std::uint8_t& func,
                                    const std::uint8_t& type) {
  wolf::thread_checker::throw_if_not_ui("LearnSensorForm::handleLearned");
  if (wolf::enocean_channel_helper::eep_is_heating_valve({org, func, type})) {
    LOGGING_LOG(m_logger, severity::normal)
        << "skip heating valve in LearnSensorForm";
    return;
  }
  auto profiles = m_enocean.register_device(id, org, func, type, false);
  if (profiles.empty()) {
    LOGGING_LOG(m_logger, severity::warning)
        << "sensor profile has no profiles, id:" << id << " org:" << std::hex
        << static_cast<int>(org) << " func:" << static_cast<int>(func)
        << " type:" << static_cast<int>(type);
    return;
  }

  // remove default values - they have been decoration
  removeAllValues();
  m_last_enocean_id = id;
  m_last_eep = {org, func, type};
  for (const auto& profile : profiles) {
    LOGGING_LOG(m_logger, severity::verbose)
        << "learned profile, type:" << std::hex
        << static_cast<int>(profile.type) << " name:" << profile.name
        << " unit:" << profile.unit << " range from:" << profile.range_from
        << " range to:" << profile.range_to;

    // create ui elements
    const auto value_type = wolf::enocean_channel_helper::cast_to_wolf(
        profile.type, profile.subchannel, m_last_eep);
    const auto description = SensorValueTypeToText::description(value_type);
    auto description_string =
        description.has_value() ? description->toStdString() : profile.name;
    const auto unit = SensorValueTypeToText::unit(value_type);
    auto unit_string = unit.has_value() ? unit->toStdString() : profile.unit;
    NumberBase* numberWidget = createWidgetFromEnoceanType(
        profile.type, description_string, unit_string);
    if (!numberWidget) {
      LOGGING_LOG(m_logger, severity::warning) << "value is not used";
      continue;
    }
    if (isDisplayableValue(profile.type)) numberWidget->show();
    m_values.push_back(
        createValue(profile.type, profile.subchannel, numberWidget));
  }
  alignValueWidgets();
  ui->teachDescription->hide();
  ui->sensorLearnSuccess->show();

  disconnect(m_connection_learn);
  ui->back->setVisible(true);
}

void LearnSensorForm::handleEnoceanParsed(
    const wolf::enocean_link::parse_results& data) {
  for (const wolf::enocean_link::parse_result& item : data) {
    if (!isValidValue(item.type)) {
      LOGGING_LOG(m_logger, severity::warning)
          << "type is not a valid value: 0x" << std::hex << item.type << ", "
          << item.name;
      continue;
    }
    auto found = std::find_if(m_values.begin(), m_values.end(),
                              [&](const value& value_) {
                                return (value_.type == item.type &&
                                        value_.subchannel == item.subchannel);
                              });
    if (found == m_values.end()) {
      LOGGING_LOG(m_logger, severity::error)
          << "type not found in values container - "
             "must not happen, because EEP should tell us "
             "all available value types";
      throw std::runtime_error(
          "LearnSensorForm::handleEnoceanParsed: type not found in values "
          "container");
    }
    found->last = item.value;
  }
  m_values_timestamp = std::chrono::system_clock::now();
  setSensorValuesToUi();
}

void LearnSensorForm::handleBack() { emit back(); }

LearnSensorForm::value LearnSensorForm::createValue(
    const CHANNEL_TYPE& type, const optional_subchannel& subchannel,
    NumberBase* widget) const {
  const auto id = wolf::sensor_id_enocean::create(m_last_enocean_id, type,
                                                  subchannel, m_last_eep);
  return {id,         type,
          subchannel, boost::optional<float>(),
          widget,     isDisplayableValue(type)};
}

void LearnSensorForm::alignValueWidgets() {
  if (m_values.empty()) return;

  int displayable_values = 0;
  int width_all = std::accumulate(
      m_values.begin(), m_values.end(), 0,
      [&displayable_values](const auto& sum, const auto& value) {
        if (!value.displayable) return sum;
        ++displayable_values;
        return sum + value.widget->width();
      });
  int space = (width() - width_all) / (displayable_values + 1);
  space = std::max(space, 0);
  const int height_line =
      (height() + ui->settingsHead->height() - ui->teachDescription->height()) /
      2;

  int positionX = 0;
  for (std::size_t index = 0; index < m_values.size(); ++index) {
    const auto& value = m_values[index];
    if (!value.displayable) continue;
    const int width_ = value.widget->width();
    positionX += space;
    const int height_ = value.widget->height();
    const int positionY = height_line - height_ / 2;
    value.widget->move(positionX, positionY);
    positionX += width_;
  }
  if (m_values[0].widget->x() <= 10) {
    auto difference = 10 - m_values[0].widget->x();
    for (auto& value : m_values) {
      if (!value.displayable) continue;
      auto positionX = value.widget->x();
      positionX += difference;
      value.widget->move(positionX, value.widget->y());
    }
  }
}

void LearnSensorForm::removeAllValues() {
  for (const auto& value : m_values) {
    if (!value.displayable) continue;
    delete value.widget;
  }
  m_values.clear();
}

void LearnSensorForm::hideBackIfNoRooms() {
  auto rooms = m_dataExchange.get_rooms();
  if (rooms.empty() || (rooms.front().sensors.empty()))
    ui->back->setVisible(false);
}

bool LearnSensorForm::isValidGeneric(const CHANNEL_TYPE& type) const {
  return type == S_CONC || type == E_VOC || type == S_PARTICLES ||
         type == S_PERCENTAGE || type == S_CURRENT || type == S_PRESSURE ||
         type == S_LUMINANCE || type == E_WINDOWHANDLE || type == F_OCCUPIED ||
         type == S_ACCEL || type == S_VOLTAGE;
}

bool LearnSensorForm::isValidValue(const CHANNEL_TYPE& type) const {
  return type == S_TEMP || type == S_RELHUM || isValidGeneric(type);
}

bool LearnSensorForm::isDisplayableValue(const CHANNEL_TYPE& type) const {
  return type != E_WINDOWHANDLE && type != F_OCCUPIED && type != S_ACCEL;
}

NumberBase* LearnSensorForm::createWidgetFromEnoceanType(
    const CHANNEL_TYPE& channel, const std::string& name,
    const std::string& unit) {
  if (channel == S_TEMP)
    return NumberFactory::createSpecial(NumberFactory::unity::temperature,
                                        this);
  if (channel == S_RELHUM)
    return NumberFactory::createSpecial(NumberFactory::unity::humidity, this);
  if (!isValidGeneric(channel)) return nullptr;
  return NumberFactory::createGeneric(name, unit, channel, this);
}
