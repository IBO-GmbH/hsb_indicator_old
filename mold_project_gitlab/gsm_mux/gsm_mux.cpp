#include "gsm_mux.hpp"
#include <wiringPi.h>
#include <boost/asio.hpp>
#include <fstream>
#include <sys/sysmacros.h>

#include "log/log.hpp"
#include "options.hpp"
#include "provider_writer.hpp"
extern "C" {
#include "gsmmux.h"
}

int main(int argc, char* argv[]) {
  options options_;
  if (!options_.parse(argc, argv)) return 0;

  logging::initialize(options_.config_.log);
  logging::logger logger("gsm_mux");

  if (wiringPiSetup() != 0)
    throw std::runtime_error("could not wiringPiSetup()");
  const auto& config_general = options_.config_.general_;
  pinMode(config_general.pin, OUTPUT);
  LOGGING_LOG(logger, logging::severity::normal)
      << "successfully set up wiringPi";

  try {
    gsm_mux program(config_general);
    return program.run();
  } catch (const std::runtime_error& error) {
    LOGGING_LOG(logger, logging::severity::error)
        << "there was an error, going down! error:" << error.what();
    return 1;
  }
}

namespace {
struct wiring_high_low_writer {
  int pin;

  wiring_high_low_writer(int pin_) : pin(pin_) { digitalWrite(pin, HIGH); }
  ~wiring_high_low_writer() { digitalWrite(pin, LOW); }
};

struct hardware_switch {
  bool toogle_in_desctutor{true};
  const int pin;

  hardware_switch(const int pin) : pin(pin) { toggle(pin); }
  ~hardware_switch() {
    if (toogle_in_desctutor) toggle(pin);
  }

  static void toggle(const int pin) {
    static logging::logger logger("hardware_switch");
    LOGGING_LOG(logger, logging::severity::normal) << "toggling hardware";
    const wiring_high_low_writer writer(pin);
    sleep(1);
  }
};
}  // namespace

const std::string gsm_mux::shared_memory_name = "GsmInformation";

gsm_mux::gsm_mux(const config::general& config_)
    : m_logger{"gsm_mux"}, config_(config_) {
  LOGGING_LOG(m_logger, logging::severity::normal) << "gsm_mux starting";
  boost::interprocess::shared_memory_object::remove(shared_memory_name.data());
  shared_memory = boost::interprocess::managed_shared_memory(
      boost::interprocess::create_only, shared_memory_name.data(), 65536);
  pppd_checker.signal_problem_detected.connect(
      [this] { trigger_abnormal_exit(3); });
}

gsm_mux::~gsm_mux() {
  boost::interprocess::shared_memory_object::remove(shared_memory_name.data());
  pppd.stop();
}

int gsm_mux::run() {
  hardware_switch on_off_switch(config_.pin);
  open_serial();
  try {
    wait_for_ready();
  } catch (...) {
    LOGGING_LOG(m_logger, logging::severity::normal)
        << "maybe we switched it off instead of on, won't toggle again. "
           "will end program";
    on_off_switch.toogle_in_desctutor = false;
    throw std::runtime_error("gsm_mux::run: is not ready");
  }
  LOGGING_LOG(m_logger, logging::severity::normal)
      << "sccessfully enabled modem";

  iccid_reader_.read(shared_memory);
  enable_modem_cmux();
  sleep(1);
  set_line_discipline();
  setup_n_gsm();
  sleep(1);
  stats.open(shared_memory, "/dev/gsmtty2");
  stats.run();

  provider_serial.open("/dev/gsmtty3");
  provider_handler_.checker.signal_offline.connect(
      [this](int) { pppd.stop(); });
  provider_handler_.signal_failed.connect([this] { trigger_abnormal_exit(2); });

  if (config_.run_pppd) {
    // now it gets dirty:
    // run pppd not before first provider-list refresh.
    connection_provider_refresh =
        provider_handler_.list.signal_refreshed.connect([this] {
          pppd.run();
          connection_provider_refresh.disconnect();
        });
  }

  sleep(1);

  provider_handler_.run(shared_memory);

  wait_for_signal();
  service.run();
  send_disconnect();

  LOGGING_LOG(m_logger, logging::severity::normal)
      << "ending with " << exit_code;
  return exit_code;
}

void gsm_mux::wait_for_signal() {
  signals.async_wait([this](const boost::system::error_code& error,
                            int signal) {
    if (error) {
      LOGGING_LOG(m_logger, logging::severity::warning)
          << "signals.async_wait, error:" << error;
      return;
    }
    LOGGING_LOG(m_logger, logging::severity::normal) << "got signal:" << signal;
    stop();
  });
}

void gsm_mux::stop() {
  LOGGING_LOG(m_logger, logging::severity::normal) << "stop()";
  stats.close();
  provider_handler_.close();
  online_checker_.stop();
  pppd.stop();
}

void gsm_mux::open_serial() {
  LOGGING_LOG(m_logger, logging::severity::normal)
      << "opening serial, device:" << config_.device;
  serial.open(config_.device);
  const int baud_rate = 115200;
  LOGGING_LOG(m_logger, logging::severity::normal)
      << "opened serial, setting baud rate:" << baud_rate;
  serial.set_option(boost::asio::serial_port::baud_rate(baud_rate));
  LOGGING_LOG(m_logger, logging::severity::normal) << "set up baud rate";
}

void gsm_mux::setup_serial() {
  static constexpr int LINE_SPEED = B115200;
  termios attributes;
  if (tcgetattr(serial.native_handle(), &attributes) == -1)
    throw std::runtime_error("tcgetattr - Cannot get line attributes");

  // set the new attrbiutes
  attributes.c_iflag = 0;
  attributes.c_oflag = 0;
  attributes.c_cflag = CS8 | CREAD | CLOCAL;
  attributes.c_cflag |= CRTSCTS;
  attributes.c_lflag = 0;
  attributes.c_cc[VMIN] = 1;
  attributes.c_cc[VTIME] = 0;

  if (cfsetospeed(&attributes, LINE_SPEED) < 0)
    throw std::runtime_error("cfsetospeed - Cannot get out line speed");
  if (cfsetispeed(&attributes, LINE_SPEED) < 0)
    throw std::runtime_error("cfsetispeed - Cannot get in line speed");

  // write the attributes
  if (tcsetattr(serial.native_handle(), TCSANOW, &attributes) == -1)
    throw std::runtime_error("tcsetattr - Cannot set line attributes");
  LOGGING_LOG(m_logger, logging::severity::normal)
      << "successfully set up serial device";
}

void gsm_mux::wait_for_ready() {
  sleep(8);
  LOGGING_LOG(m_logger, logging::severity::normal) << "waiting to be ready";
  command.write_at_command_and_verify_response("AT", std::chrono::seconds(4));
  LOGGING_LOG(m_logger, logging::severity::normal)
      << "successfully waited for device to be ready";
}

void gsm_mux::enable_modem_cmux() {
  // http://www.quectel.com/uploadimage/downlad/m95_at_commands_manual_v1.3.pdf
  command.write_at_command_and_verify_response("AT");
  command.write_at_command_and_verify_response("AT+IFC=2,2");
  command.write_at_command_and_verify_response("AT+GMM");
  command.write_at_command_and_verify_response("AT");
  command.write_at_command_and_verify_response("AT+IPR=115200&w");
  command.write_at_command_and_verify_response(
      "AT+CMUX=0,0,5,127,10,3,30,10,2");
  LOGGING_LOG(m_logger, logging::severity::normal) << "enabled modem_cmux";
}

void gsm_mux::set_line_discipline() {
  LOGGING_LOG(m_logger, logging::severity::verbose) << "setting line discipline";
  static constexpr int N_GSM0710 = 21;
  int line_discipline = N_GSM0710;
  if (ioctl(serial.native_handle(), TIOCSETD, &line_discipline) < 0)
    throw std::runtime_error("ioctl TIOCSETD, is 'n_gsm' module registred?");
  LOGGING_LOG(m_logger, logging::severity::normal) << "set line_discipline";
}

void gsm_mux::setup_n_gsm() {
  static constexpr int MTU = 127;  // AT+CMUX has the MTU in it!
  LOGGING_LOG(m_logger, logging::severity::normal) << "setting up n_gsm";

  gsm_config old_config;
  if (ioctl(serial.native_handle(), GSMIOC_GETCONF, &old_config) < 0)
    throw std::runtime_error("ioctl GSMIOC_GETCONF");

  auto config = old_config;

  /* set and write new attributes */
  config.initiator = 1;
  config.encapsulation = 0;
  config.mru = MTU;
  config.mtu = MTU;
  config.t1 = 10;
  config.n2 = 3;
  config.t2 = 30;
  config.t3 = 10;

  if (0 == std::memcmp(&config, &old_config, sizeof(gsm_config))) {
    // the configuration hasn't changed, but we want to reinitialize.
    // so set a different configuration first.
    --old_config.mtu;
    if (ioctl(serial.native_handle(), GSMIOC_SETCONF, &old_config) < 0)
      LOGGING_LOG(m_logger, logging::severity::warning)
          << "couldn't reinitialize n_gsm";
  }

  if (ioctl(serial.native_handle(), GSMIOC_SETCONF, &config) < 0)
    throw std::runtime_error("ioctl GSMIOC_SETCONF");

  LOGGING_LOG(m_logger, logging::severity::normal) << "set up n_gsm";
}

void gsm_mux::enable_daemon() {
  if (daemon(1, 1) == 0) return;
  throw std::runtime_error("could not eanble daemon");
}

int gsm_mux::get_major() {
  std::ifstream devices("/proc/devices");
  std::string line;
  while (std::getline(devices, line)) {
    if (line.find("gsmtty") == std::string::npos) continue;
    std::stringstream line_stream(line);
    int result;
    line_stream >> result;
    LOGGING_LOG(m_logger, logging::severity::normal)
        << "found major:" << result;
    return result;
  }
  throw std::runtime_error("could not find major");
}

void gsm_mux::create_nodes(int major) {
  mode_t oldmask = umask(0);
  for (int minor = 1; minor < num_nodes + 1; ++minor) {
    const std::string node_name = base_name + std::to_string(minor);
    create_node(node_name, major, minor);
  }
  umask(oldmask);  // TODO use RAII
}

void gsm_mux::remove_nodes() {
  LOGGING_LOG(m_logger, logging::severity::normal) << "removing nodes";
  for (int minor = 1; minor < num_nodes + 1; ++minor) {
    const std::string node_name = base_name + std::to_string(minor);
    remove_node(node_name);
  }
}

void gsm_mux::send_disconnect() {
  std::array<char, 7> message = {
      {static_cast<char>(0xf9), 0x03, static_cast<char>(0xef), 0x03,
       static_cast<char>(0xc3), 0x16, static_cast<char>(0xf9)}};
  const std::size_t wrote =
      boost::asio::write(serial, boost::asio::buffer(message));
  LOGGING_LOG(m_logger, logging::severity::normal)
      << "wrote disconnect, wrote:" << wrote;
}

void gsm_mux::remove_node(std::string name) {
  LOGGING_LOG(m_logger, logging::severity::normal)
      << "removing device:" << name;
  auto result = unlink(name.c_str());
  if (result == 0) return;
  LOGGING_LOG(m_logger, logging::severity::warning)
      << "could not unlink! ignoring error";
}

void gsm_mux::create_node(std::string name, int major, int minor) {
  LOGGING_LOG(m_logger, logging::severity::normal)
      << "creating device:" << name << ", major:" << major
      << ", minor:" << minor;
  dev_t device = makedev(major, minor);
  const auto result = mknod(name.c_str(), S_IFCHR | 0666, device);
  if (result == 0) return;
  throw std::runtime_error("could not create device");
}

void gsm_mux::trigger_abnormal_exit(const int code) {
  LOGGING_LOG(m_logger, logging::severity::error)
      << "abnormal exit with code " << code;
  throw std::runtime_error("abnormal exit triggered");
}
