#include "provider_handler.hpp"

using namespace logging;

provider_handler::provider_handler(boost::asio::io_service& service,
                                   boost::asio::serial_port& serial,
                                   online_checker& online_check)
    : checker(online_check),
      m_logger{"provider_handler"},
      read_write{serial, "provider_handler"},
      timer(service) {
  connection_signal_offline = checker.signal_offline.connect(
      [this](int failure_counter) { handle_offline(failure_counter); });
  connection_signal_refresh = list.signal_refreshed.connect([this] {
    handle_refreshed();
    // we do not start timer, because refreshing provider periodically causes
    // timeouts
    // start_timer();
  });
}

void provider_handler::start_timer() {
  timer.expires_from_now(std::chrono::minutes(1));
  timer.async_wait([this](const boost::system::error_code& error) {
    if (error == boost::asio::error::operation_aborted) return;
    if (error) throw boost::system::system_error(error);
    list.refresh();
  });
}

void provider_handler::run(
    boost::interprocess::managed_shared_memory& shared_memory) {
  provider_writer_.construt_memory(shared_memory);
  list.refresh();
}

void provider_handler::close() {
  boost::system::error_code ignore_error;
  timer.cancel(ignore_error);
}

void provider_handler::handle_offline(int failure_counter) {
    signal_failed();
}

void provider_handler::handle_refreshed() {
  const auto provider = get_current_provider();
  LOGGING_LOG(m_logger, severity::normal)
      << "provider_handler, list got refreshed, current provider:" << provider;
  if (current_provider == provider) return;
  current_provider = provider;
  provider_writer_.write(provider);
  checker.run();
}

void provider_handler::handle_provider_changed() { checker.run(); }

std::string provider_handler::get_current_provider() const {
  const auto found =
      std::find_if(list.items.cbegin(), list.items.cend(),
                   [](const provider_list::item& provider) {
                     return provider.status == provider_list::stat::current;
                   });

  return (found == list.items.cend()) ? "" : found->name_short;
}
