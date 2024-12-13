#include "command_handler.hpp"
#include <boost/process.hpp>

using namespace logging;
using namespace wolf;

command_handler::command_handler(network::command_parser& parser)
    : m_logger{"command_handler"} {
  m_connection_parsed =
      parser.signal_parsed.connect([this](auto& response, const auto script) {
        handle_command_with_catch(response, script);
      });
}

void command_handler::handle_command_with_catch(
    network::response::send_helper& response, const std::string_view script) {
  try {
    handle_command(response, script);
  } catch (const std::runtime_error& error) {
    try {
      std::stringstream error_message;
      error_message << "failed to execute script, error:" << error.what();
      LOGGING_LOG(m_logger, severity::error) << error_message.str();
      response.failure(error_message.str());
    } catch (...) {
    }
  }
}

void command_handler::handle_command(network::response::send_helper& response,
                                     const std::string_view script) {
  LOGGING_LOG(m_logger, severity::verbose)
      << "handle_command, script:" << script;
  std::string script_file_name = "command_script";
  {
    std::ofstream stream(script_file_name);
    if (!stream.is_open())
      throw std::runtime_error(
          "command_handler::handle_command: file not open");
    stream << script;
    stream.close();
  }

  boost::filesystem::path file(script_file_name);
  if (!boost::filesystem::exists(file)) {
    LOGGING_LOG(m_logger, severity::error)
        << "!boost::filesystem::exists(file)";
    response.failure("could not create script file!");
    return;
  }
  boost::filesystem::permissions(
      file, boost::filesystem::add_perms | boost::filesystem::owner_exe);

  const int exit_code = boost::process::system(file);
  if (exit_code == 0) {
    response.success();
    return;
  }
  std::stringstream error_message;
  error_message << "script, did not return result 0, result:" << exit_code
                << std::flush;
  LOGGING_LOG(m_logger, severity::error) << error_message.str();
  response.failure(error_message.str());
}
