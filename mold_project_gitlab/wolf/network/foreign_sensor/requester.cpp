#include "requester.hpp"
#include "wolf/network/sensor_value_type_helper.hpp"

using namespace wolf::network::foreign_sensor;

requester::requester(wolf::network::bond_sender &sender, response &response_)
    : m_logger{"network::foreign_sensor::requester"},
      m_sender(sender),
      m_response(response_) {}

void requester::request(const response::callback &callback) {
  LOGGING_LOG(m_logger, logging::severity::normal) << "request";
  Clima::ForeignSensorListRequest request;
  request.SensorType = Clima::Enums::SensorType::None;
  const auto work_uuid = m_sender.send(
      request, Clima::Enums::MessageType::ForeignSensorListRequest);
  m_response.register_work_uuid(work_uuid, callback);
}
