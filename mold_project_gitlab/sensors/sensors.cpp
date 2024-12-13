#include <boost/asio/signal_set.hpp>
#include "ee895_test.hpp"
#include "log/log.hpp"
#include "scd30_test.hpp"
#include "sgpc3_test.hpp"
#include "sht25_test.hpp"
#include "sht85_test.hpp"
#include "shtc1_test.hpp"
#include "tmp117_test.hpp"
#include "wolf/plug_n_play.hpp"

using namespace logging;

int main(int argc, char* argv[]) {
  initialize();

  bool end_program{false};
  wolf::program_options_parser program_options;
  if (!program_options.parse(argc, argv, "sensor_test", end_program)) return 1;
  auto config = program_options.get_config();

  logger m_logger{"main"};
  LOGGING_LOG(m_logger, severity::verbose) << "starting up";

  boost::asio::io_service service;
  wolf::scd30_reader scd30_reader;
  wolf::sgpc3_reader sgpc3_reader(service);
  wolf::sht25_reader sht25_reader(service);
  wolf::shtc1_reader shtc1_reader(service);
  wolf::ee895_reader ee895_reader(service);
  wolf::sht85_reader sht85_reader(service);
  wolf::tmp117_reader tmp117_reader(service);

  wolf::plug_n_play::readers readers{scd30_reader, sgpc3_reader, sht25_reader,
                                     shtc1_reader, ee895_reader, sht85_reader,
                                     tmp117_reader};

  wolf::plug_n_play plug_n_play(config, readers);

  std::unique_ptr<sht25_test> sht25_test_;
  plug_n_play.signal_sht25.connect(
      [&sht25_test_, &service, &sht25_reader](const auto& /**/) {
        sht25_test_.reset(new sht25_test(service, sht25_reader));
      });
  std::unique_ptr<sgpc3_test> sgpc3_test_;
  plug_n_play.signal_sgpc3.connect([&sgpc3_test_, &service, &sgpc3_reader]() {
    sgpc3_test_.reset(new sgpc3_test(service, sgpc3_reader));
  });
  std::unique_ptr<scd30_test> scd30_test_;
  plug_n_play.signal_scd30.connect(
      [&scd30_test_, &service, &scd30_reader](const auto& /**/) {
        scd30_test_.reset(new scd30_test(service, scd30_reader));
      });
  std::unique_ptr<shtc1_test> shtc1_test_;
  plug_n_play.signal_shtc1.connect(
      [&shtc1_test_, &shtc1_reader](const auto& /**/) {
        shtc1_test_.reset(new shtc1_test(shtc1_reader));
      });
  std::unique_ptr<ee895_test> ee895_test_;
  plug_n_play.signal_ee895.connect(
      [&ee895_test_, &ee895_reader](const auto& /**/) {
        ee895_test_.reset(new ee895_test(ee895_reader));
      });
  std::unique_ptr<sht85_test> sht85_test_;
  plug_n_play.signal_sht85.connect(
      [&sht85_test_, &sht85_reader](const auto& /**/) {
        sht85_test_.reset(new sht85_test(sht85_reader));
      });
  std::unique_ptr<tmp117_test> tmp117_test_;
  plug_n_play.signal_tmp117.connect(
      [&tmp117_test_, &tmp117_reader](const auto& /**/) {
        tmp117_test_.reset(new tmp117_test(tmp117_reader));
      });

  plug_n_play.signal_none.connect([&m_logger]() {
    LOGGING_LOG(m_logger, severity::error)
        << "no onboard sensors, shutting down";
    return 1;
  });

  // set up program enviroment
  boost::asio::signal_set signals_to_listen_to(service, SIGINT, SIGTERM);
  auto signal_callback = [&](const boost::system::error_code& error,
                             int /*signal_number*/) {
    if (error) {
      LOGGING_LOG(m_logger, severity::error)
          << "signal_handler called with error:" << error.message();
      return;
    }
    LOGGING_LOG(m_logger, severity::normal) << "shutting down";
    sht25_test_.reset();
    sgpc3_test_.reset();
    scd30_test_.reset();
    shtc1_test_.reset();
    ee895_test_.reset();
    sht85_test_.reset();
    tmp117_test_.reset();
  };
  signals_to_listen_to.async_wait(signal_callback);

  plug_n_play.check_sensors();
  LOGGING_LOG(m_logger, severity::normal) << "starting asio io_service run()";
  service.run();
  LOGGING_LOG(m_logger, severity::normal) << "shutted down";

  return 0;
}
