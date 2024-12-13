#ifndef MOLD_GUI_LEARNHEATINGVALVE_HPP
#define MOLD_GUI_LEARNHEATINGVALVE_HPP

#include <QLabel>
#include <QWidget>
#include "WolfServices.hpp"
#include "wolf/enocean_link.hpp"

namespace Ui {
class LearnHeatingValve;
}

namespace mold {

class LearnHeatingValve : public QWidget {
  Q_OBJECT

 public:
  struct value {
    wolf::sensor_id id;
    CHANNEL_TYPE type;
    boost::optional<float> last;
    QLabel *value_label;
  };

  explicit LearnHeatingValve(WolfServices &services,
                             const heating_valve_settings &settings,
                             QWidget *parent = nullptr);
  ~LearnHeatingValve();
  std::vector<wolf::sensor_value> getValues() const;
  wolf::types::id_esp3 getLastEnoceanId() const;
  std::array<std::uint8_t, 3> getEep() const;
  std::vector<wolf::sensor_id> getLearnedIds() const;

 signals:
  void back();
  void removeValves();

 private slots:
  void handleRemoveValves();

 private:
  void handleLearned(const wolf::types::id_esp3 id, const std::uint8_t &org,
                     const std::uint8_t &func, const std::uint8_t &type);
  void handleEnoceanParsed(const wolf::enocean_link::parse_results &data);
  static bool isValidValue(const CHANNEL_TYPE &type);
  void setSensorValuesToUi();
  value createValue(const CHANNEL_TYPE &type) const;
  QLabel *getLabelForType(const CHANNEL_TYPE &type) const;
  static bool isInvalidType(const unsigned int &type);
  void setFormToInitialState();
  void sendData(const wolf::types::data &data_bytes) const;
  std::optional<float> getCurrentTemperature() const;

 private:
  std::unique_ptr<Ui::LearnHeatingValve> ui;

  logging::logger m_logger;

  WolfServices &m_services;
  wolf::enocean_link m_enocean;

  boost::signals2::scoped_connection
      m_connection_enocean_learn;  // from service thread to ui thread
  boost::signals2::scoped_connection
      m_connection_enocean_data_received;  // from service thread to ui thread

  wolf::types::id_esp3 m_last_enocean_id{};
  std::array<std::uint8_t, 3> m_last_eep;
  using values = std::vector<value>;
  values m_values;
  std::chrono::system_clock::time_point m_values_timestamp;
  heating_valve_settings m_settings;
};
}  // namespace mold

#endif  // MOLD_GUI_LEARNHEATINGVALVE_HPP
