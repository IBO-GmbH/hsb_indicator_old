#include "ForeignSensorForm.hpp"
#include "ui_ForeignSensorForm.h"

#include "DataExchange.hpp"

using namespace mold;

struct ForeignSensorForm::ListItem : public QListWidgetItem {
  ListItem(const std::vector<wolf::sensor_profile> &profiles,
           const QString &text, QListWidget *view)
      : QListWidgetItem(text, view), profiles(profiles) {}
  ~ListItem() = default;

  const std::vector<wolf::sensor_profile> profiles;
};

ForeignSensorForm::ForeignSensorForm(const WolfServices &services,
                                     const DataExchange &dataExchange,
                                     QWidget *parent)
    : QWidget(parent),
      ui(new Ui::ForeignSensorForm),
      m_logger("ForeignSensorForm"),
      m_services(services),
      m_dataExchange(dataExchange) {
  ui->setupUi(this);

  connect(ui->back, &QPushButton::clicked, this,
          &ForeignSensorForm::handleBack);
  createListEntries();
}

std::vector<wolf::sensor_profile> ForeignSensorForm::get_choosen_profiles()
    const {
  const auto selection = ui->list->selectedItems();
  if (selection.empty()) return {};
  auto item = static_cast<const ListItem *>(selection.first());
  return item->profiles;
}

void ForeignSensorForm::createListEntries() {
  using sensors_type = DataExchange::foreign_sensors_type;
  sensors_type sensors = m_dataExchange.get_foreign_sensors();
  std::sort(sensors.begin(), sensors.end(),
            [](const wolf::foreign_sensor &first,
               const wolf::foreign_sensor &second) {
              return first.profile.id.get_owner() <
                     second.profile.id.get_owner();
            });
  for (sensors_type::const_iterator sensor = sensors.cbegin();
       sensor != sensors.cend();) {
    const QString description = QString::fromStdString(sensor->description);
    std::vector<wolf::sensor_profile> profiles;
    const auto last_sensor =
        std::upper_bound(sensor, sensors.cend(), sensor->profile.id.get_owner(),
                         [](const auto &first, const auto &second) {
                           return first < second.profile.id.get_owner();
                         });
    std::transform(sensor, last_sensor, std::back_inserter(profiles),
                   [](const auto &item) { return item.profile; });
    new ListItem(profiles, description, ui->list);
    sensor = last_sensor;
  }
}

ForeignSensorForm::~ForeignSensorForm() = default;

void ForeignSensorForm::handleBack() { emit back(); }
