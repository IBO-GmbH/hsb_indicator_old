#include "program_options.hpp"
#include <boost/program_options.hpp>

using namespace logging;
namespace bp = boost::program_options;

program_options::program_options(config::all& config) : m_config(config) {}

struct options_appender {
  bp::options_description& options;
  template <typename add_type>
  void operator()(const char* name, add_type& destination,
                  const char* description) {
    auto value = bp::value<add_type>(&destination);
    value->default_value(destination);
    value->required();
    options.add_options()(name, value, description);
  }
};

boost::program_options::options_description
program_options::create_description() {
  bp::options_description result("Logging");
  options_appender appender{result};
  appender(
      "log-file", m_config.file_.file,
      "log file to write to. eg: 'log-file=mold_%5N.log' disable logging to "
      "file by using 'log-file='");
  appender("log-append", m_config.file_.append,
           "enable or disable log append.");
  appender("log-console", m_config.console_.enabled,
           "enable or disable logging to console.");
  appender("log-console-colored", m_config.console_.color,
           "enable or disable color in the console-logging.");
  appender("log-console-timestamp", m_config.console_.timestamp,
           "enable or disable timestamp in the console-logging.");
  appender("log-journal", m_config.journal_.enabled,
           "enable or disable logging to journal.");
  appender("log-flush", m_config.common_.flush,
           "flush after each log entry. Set to false for productive "
           "enviroment.");
  appender("log-severity", m_config.common_.severity,
           "set the verbosity of the log. Possible values are \"verbose, "
           "normal, warning, error\"");
  appender("log-rotation-size", m_config.file_.rotation_size,
           "max log file size in megabyte");
  appender("log-file-count", m_config.file_.file_count,
           "max count of log files");
  return result;
}
