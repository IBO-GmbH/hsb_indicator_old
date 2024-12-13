#ifndef MOLD_GUI_STATUSLISTHELPER_HPP
#define MOLD_GUI_STATUSLISTHELPER_HPP

#include "mold/configuration.hpp"
#include "mold/gui/DataExchange.hpp"
#include "wolf/room.hpp"

namespace mold {

class StatusListHelper {
 public:
  StatusListHelper(const DataExchange &dataExchange);

  enum struct StatusType {
    configuration,
    co2,
    generic,
    particles,
    current_clamp
  };
  struct Status {
    wolf::types::uuid_array room;
    StatusType type;
    std::vector<wolf::sensor_id> sensors;
  };
  using StatusList = std::vector<Status>;
  StatusList getStatusList() const;

  boost::optional<configuration> hasRoomMoldSetUp(
      const wolf::types::uuid_array &room) const;

 private:
  static void addConfigurationToListIfValid(
      StatusList &result, const wolf::types::uuid_array &room,
      const boost::optional<configuration> &config);
  static bool isConfigurationPageValid(
      const boost::optional<configuration> &config);
  static void addSensorToListIfValid(StatusList &result,
                                     const wolf::types::uuid_array &room,
                                     const wolf::sensor_id &sensor);
  static bool isSensorGenericOrCo2(const wolf::sensor_id &sensor_id);
  static void addMultipleSensors(StatusList &result,
                                 const wolf::types::uuid_array &room,
                                 const std::vector<wolf::sensor_id> &sensors,
                                 const StatusType &type);

 private:
  const DataExchange &dataExchange;
};
}  // namespace mold

#endif  // MOLD_GUI_STATUSLISTHELPER_HPP
