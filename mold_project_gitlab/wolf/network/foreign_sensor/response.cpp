#include "response.hpp"
#include <experimental/iterator>
#include "wolf/network/bond_deserializer.hpp"
#include "wolf/network/sensor_profile_helper.hpp"
#include "wolf/network/sensor_value_type_helper.hpp"

using namespace wolf::network::foreign_sensor;

response::response(wolf::network::response::message &message_)
    : m_logger{"network::foreign_sensor::response"}, m_message(message_) {}

void response::register_work_uuid(const wolf::types::uuid_array &work_id,
                                  const response::callback &to_call) {
  m_message.register_work_uuid(work_id, [this, to_call](const auto &message) {
    handle(message, to_call);
  });
}

static wolf::foreign_sensor cast_network_entry(
    const Clima::ForeignSensorListResultEntry &entry) {
  wolf::foreign_sensor result;
  wolf::network::sensor_profile_helper id_helper;
  result.profile = id_helper.cast_to_wolf(entry.Profile);
  result.description = entry.Description;
  return result;
}

void response::handle(const std::optional<Clima::Message> &message,
                      const response::callback &to_call) {
  if (!message.has_value()) {
    LOGGING_LOG(m_logger, logging::severity::warning) << "Response timed out!";
    to_call({});
    return;
  }
  bond_deserializer deserializer;
  const auto response =
      deserializer.parse_message<Clima::Response>(message->Entry);
  if (response.Code != Clima::Enums::ResponseCode::Success) {
    LOGGING_LOG(m_logger, logging::severity::error)
        << "handle got an error with an error: response.Message:"
        << response.Message;
    to_call({});
    return;
  }
  const auto response_foreign_sensor_list =
      deserializer.parse_message<Clima::ForeignSensorListResult>(
          message->Entry);
  data result;
  std::transform(response_foreign_sensor_list.Entries.cbegin(),
                 response_foreign_sensor_list.Entries.cend(),
                 std::back_inserter(result), cast_network_entry);

  {
    std::stringstream debug;
    std::copy(result.cbegin(), result.cend(),
              std::experimental::make_ostream_joiner(debug, ", "));
    LOGGING_LOG(m_logger, logging::severity::verbose)
        << "handle, result:[" << debug.str() << "]";
  }
  to_call(result);
}
