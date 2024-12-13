#ifndef WIFI_CONNECTOR_HPP
#define WIFI_CONNECTOR_HPP

#include <QProcess>
#include <boost/process.hpp>
#include <functional>
#include <future>
#include <sstream>
#include <string>

#include "log/logger.hpp"

namespace wifi {

class connector : QObject {
  Q_OBJECT
 public:
  using callback_success = std::function<void()>;
  using callback_failure = std::function<void()>;
  connector(const std::string &ssid, const std::string &password,
            const callback_success &on_success,
            const callback_failure &on_failure, QObject *parent = nullptr);
  ~connector();

 private:
  void connect_async(const std::string ssid, const std::string password);

  template <typename Arg, typename... Args>
  std::string to_string(Arg &&arg, Args &&... args) {
    std::stringstream ss;
    ss << std::forward<Arg>(arg);
    using expander = int[];
    (void)expander{0, (void(ss << ' ' << std::forward<Args>(args)), 0)...};
    return ss.str();
  }

  template <typename... Args>
  int execute(Args &&... args) {
    LOGGING_LOG(m_logger, logging::severity::verbose)
        << "executing " << to_string(args...);

    int exit_code = -1;
    try {
      exit_code = boost::process::system(args...);
    } catch (const std::runtime_error &re) {
      LOGGING_LOG(m_logger, logging::severity::warning)
          << "failed to execute " << to_string(args...)
          << " runtime error: " << re.what();
    } catch (const std::exception &ex) {
      LOGGING_LOG(m_logger, logging::severity::warning)
          << "failed to execute " << to_string(args...)
          << "error: " << ex.what();
    } catch (...) {
      LOGGING_LOG(m_logger, logging::severity::warning)
          << "failed to execute " << to_string(args...)
          << "unknown failure occurred";
    }

    if (exit_code != 0) {
      LOGGING_LOG(m_logger, logging::severity::warning)
          << "could not execute " << to_string(args...)
          << ", exit code = " << std::to_string(exit_code);
    } else {
      LOGGING_LOG(m_logger, logging::severity::verbose)
          << "successfully executed " << to_string(args...);
    }

    return exit_code;
  }

 private:
  logging::logger m_logger;

  const callback_success m_on_success;
  const callback_failure m_on_failure;

  std::future<void> m_fut;
};

}  // namespace wifi

#endif  // WIFI_CONNECTOR_HPP
