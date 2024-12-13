#ifndef MOLD_GUI_LEARNSENSORFORM_HPP
#define MOLD_GUI_LEARNSENSORFORM_HPP

#include <QWidget>
#include "mold/gui/DataExchange.hpp"
#include "wolf/enocean_link.hpp"
#include "wolf/sensor_profile.hpp"
#include "wolf/sensor_value.hpp"

namespace Ui {
class LearnSensorForm;
}

namespace mold {

class NumberBase;

class LearnSensorForm : public QWidget {
  Q_OBJECT

 public:
  using optional_subchannel = std::optional<wolf::enocean_subchannel_type>;

  struct value {
    // TODO use sensor_value?
    wolf::sensor_id id;
    CHANNEL_TYPE type;
    optional_subchannel subchannel;
    boost::optional<float> last;
    NumberBase *widget;
    bool displayable;
  };

  explicit LearnSensorForm(WolfServices &services, DataExchange &dataExchange,
                           QWidget *parent = nullptr);
  ~LearnSensorForm();
  std::vector<wolf::sensor_value> getValues() const;
  wolf::types::id_esp3 getLastEnoceanId() const;
  std::array<std::uint8_t, 3> getEep() const;
  std::vector<wolf::sensor_id> getLearnedIds() const;

 signals:
  void back();

 protected slots:

 private:
  using values = std::vector<value>;

  void createDefaultSeonsorValues();
  void setSensorValuesToUi();
  void handleLearned(const wolf::types::id_esp3 id, const std::uint8_t &org,
                     const std::uint8_t &func, const std::uint8_t &type);
  void handleEnoceanParsed(const wolf::enocean_link::parse_results &data);
  void handleBack();
  NumberBase *createWidgetFromEnoceanType(const CHANNEL_TYPE &channel,
                                          const std::string &name,
                                          const std::string &unit);
  value createValue(const CHANNEL_TYPE &type,
                    const optional_subchannel &subchannel,
                    NumberBase *widget) const;
  void alignValueWidgets();
  void removeAllValues();
  void hideBackIfNoRooms();
  bool isValidGeneric(const CHANNEL_TYPE &type) const;
  bool isValidValue(const CHANNEL_TYPE &type) const;
  bool isDisplayableValue(const CHANNEL_TYPE &type) const;

 private:
  std::unique_ptr<Ui::LearnSensorForm> ui;

  logging::logger m_logger;

  WolfServices &m_services;
  DataExchange &m_dataExchange;

  wolf::types::id_esp3 m_last_enocean_id{};
  std::array<std::uint8_t, 3> m_last_eep;
  wolf::enocean_link m_enocean;
  values m_values;
  std::chrono::system_clock::time_point m_values_timestamp;

  QMetaObject::Connection m_connection_learn;
  boost::signals2::scoped_connection
      m_connection_enocean_data;  // from service thread to ui thread
};
}  // namespace mold

#endif  // MOLD_GUI_LEARNSENSORFORM_HPP
