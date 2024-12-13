#include "wolf/plug_n_play.hpp"
#include "gtest/gtest.h"

namespace plug_n_play_test {

static logging::logger test_logger("plug_n_play_test");
static bool scd30{false};
static bool sgpc3{false};
static bool sht25{false};
static bool shtc1{false};
static bool ee895{false};
static bool sht85{false};
static bool tmp117{false};

class scd30_reader_mock : public wolf::scd30_reader {
 public:
  bool open(const std::string& device, const int slave_id) override {
    if (!connected) return false;
    LOGGING_LOG(test_logger, logging::severity::normal)
        << "opened scd30 device: " << std::hex << device
        << ", slave_id: " << slave_id;
    return true;
  }

  void set_connected(const bool set) { connected = set; }

 private:
  bool connected{false};
};

class sgpc3_reader_mock : public wolf::sgpc3_reader {
 public:
  sgpc3_reader_mock(boost::asio::io_service& service)
      : wolf::sgpc3_reader(service) {}

  bool open(const std::string& device, const int slave_id) override {
    if (!connected) return false;
    LOGGING_LOG(test_logger, logging::severity::normal)
        << "opened sgpc3 device: " << std::hex << device
        << ", slave_id: " << slave_id;
    return true;
  }

  void set_connected(const bool set) { connected = set; }

 private:
  bool connected{false};
};

class sht25_reader_mock : public wolf::sht25_reader {
 public:
  sht25_reader_mock(boost::asio::io_service& service)
      : wolf::sht25_reader(service) {}

  bool open(const std::string& device, const int slave_id) override {
    if (!connected) return false;
    LOGGING_LOG(test_logger, logging::severity::normal)
        << "opened sht25 device: " << std::hex << device
        << ", slave_id: " << slave_id;
    return true;
  }

  void set_connected(const bool set) { connected = set; }

 private:
  bool connected{false};
};

class shtc1_reader_mock : public wolf::shtc1_reader {
 public:
  shtc1_reader_mock(boost::asio::io_service& service)
      : wolf::shtc1_reader(service) {}

  bool open(const std::string& device, const int slave_id) override {
    if (!connected) return false;
    LOGGING_LOG(test_logger, logging::severity::normal)
        << "opened shtc1 device: " << std::hex << device
        << ", slave_id: " << slave_id;
    return true;
  }

  void set_connected(const bool set) { connected = set; }

 private:
  bool connected{false};
};

class ee895_reader_mock : public wolf::ee895_reader {
 public:
  ee895_reader_mock(boost::asio::io_service& service)
      : wolf::ee895_reader(service) {}

  bool open(const std::string& device, const int slave_id) override {
    if (!connected) return false;
    LOGGING_LOG(test_logger, logging::severity::normal)
        << "opened ee895 device: " << std::hex << device
        << ", slave_id: " << slave_id;
    return true;
  }

  void set_connected(const bool set) { connected = set; }

 private:
  bool connected{false};
};

class sht85_reader_mock : public wolf::sht85_reader {
 public:
  sht85_reader_mock(boost::asio::io_service& service)
      : wolf::sht85_reader(service) {}

  bool open(const std::string& device, const int slave_id) override {
    if (!connected) return false;
    LOGGING_LOG(test_logger, logging::severity::normal)
        << "opened sht85 device: " << std::hex << device
        << ", slave_id: " << slave_id;
    return true;
  }

  void set_connected(const bool set) { connected = set; }

 private:
  bool connected{false};
};

class tmp117_reader_mock : public wolf::tmp117_reader {
 public:
  tmp117_reader_mock(boost::asio::io_service& service)
      : wolf::tmp117_reader(service) {}

  bool open(const std::string& device, const int slave_id) override {
    if (!connected) return false;
    LOGGING_LOG(test_logger, logging::severity::normal)
        << "opened tmp117 device: " << std::hex << device
        << ", slave_id: " << slave_id;
    return true;
  }

  void set_connected(const bool set) { connected = set; }

 private:
  bool connected{false};
};

static boost::asio::io_service service;
static scd30_reader_mock scd30_reader;
static sgpc3_reader_mock sgpc3_reader(service);
static sht25_reader_mock sht25_reader(service);
static shtc1_reader_mock shtc1_reader(service);
static ee895_reader_mock ee895_reader(service);
static sht85_reader_mock sht85_reader(service);
static tmp117_reader_mock tmp117_reader(service);
static wolf::plug_n_play::readers readers{
    scd30_reader, sgpc3_reader, sht25_reader, shtc1_reader,
    ee895_reader, sht85_reader, tmp117_reader};

void check_sensors(const bool scd30_check, const bool sgpc3_check,
                   const bool sht25_check, const bool shtc1_check,
                   const bool ee895_check, const bool sht85_check,
                   const bool tmp117_check) {
  EXPECT_EQ(scd30, scd30_check);
  EXPECT_EQ(sgpc3, sgpc3_check);
  EXPECT_EQ(sht25, sht25_check);
  EXPECT_EQ(shtc1, shtc1_check);
  EXPECT_EQ(ee895, ee895_check);
  EXPECT_EQ(sht85, sht85_check);
  EXPECT_EQ(tmp117, tmp117_check);
}

void reset_sensors() {
  scd30 = false;
  sgpc3 = false;
  sht25 = false;
  shtc1 = false;
  ee895 = false;
  sht85 = false;
  tmp117 = false;
  scd30_reader.set_connected(false);
  sgpc3_reader.set_connected(false);
  sht25_reader.set_connected(false);
  shtc1_reader.set_connected(false);
  ee895_reader.set_connected(false);
  sht85_reader.set_connected(false);
  tmp117_reader.set_connected(false);
}

auto scd30_opened =
    [](const std::optional<wolf::sensor_type>& /*second_sensor*/) {
      scd30 = true;
    };
auto sgpc3_opened = []() { sgpc3 = true; };
auto sht25_opened =
    [](const std::optional<wolf::sensor_type>& /*second_sensor*/) {
      sht25 = true;
    };
auto shtc1_opened =
    [](const std::optional<wolf::sensor_type>& /*second_sensor*/) {
      shtc1 = true;
    };
auto ee895_opened =
    [](const std::optional<wolf::sensor_type>& /*second_sensor*/) {
      ee895 = true;
    };
auto sht85_opened =
    [](const std::optional<wolf::sensor_type>& /*second_sensor*/) {
      sht85 = true;
    };
auto tmp117_opened =
    [](const std::optional<wolf::sensor_type>& /*second_sensor*/) {
      tmp117 = true;
    };
auto no_opened = []() { reset_sensors(); };

wolf::config make_config() {
  wolf::config config;
  config.sht25_.device = "/dev/i2c-1";
  config.sht25_.i2c_slave = 0x40;
  config.shtc1_.i2c_slave = 0x70;
  config.scd30_.i2c_slave = 0x61;
  config.sgpc3_.i2c_slave = 0x58;
  config.ee895_.i2c_slave = 0x5e;
  config.sht85_.i2c_slave = 0x44;
  config.tmp117_.i2c_slave = 0x49;
  return config;
}

void connect_functions(const std::shared_ptr<wolf::plug_n_play>& plug_n_play) {
  plug_n_play->signal_scd30.connect(scd30_opened);
  plug_n_play->signal_sgpc3.connect(sgpc3_opened);
  plug_n_play->signal_sht25.connect(sht25_opened);
  plug_n_play->signal_shtc1.connect(shtc1_opened);
  plug_n_play->signal_ee895.connect(ee895_opened);
  plug_n_play->signal_sht85.connect(sht85_opened);
  plug_n_play->signal_tmp117.connect(tmp117_opened);
  plug_n_play->signal_none.connect(no_opened);
}

static auto config = make_config();

std::shared_ptr<wolf::plug_n_play> initialize_pnp(
    const bool set_scd30, const bool set_sgpc3, const bool set_sht25,
    const bool set_shtc1, const bool set_ee895, const bool set_sht85,
    const bool set_tmp117) {
  auto plug_n_play = std::make_shared<wolf::plug_n_play>(config, readers);
  connect_functions(plug_n_play);
  if (set_scd30) scd30_reader.set_connected(true);
  if (set_sgpc3) sgpc3_reader.set_connected(true);
  if (set_sht25) sht25_reader.set_connected(true);
  if (set_shtc1) shtc1_reader.set_connected(true);
  if (set_ee895) ee895_reader.set_connected(true);
  if (set_sht85) sht85_reader.set_connected(true);
  if (set_tmp117) tmp117_reader.set_connected(true);
  return plug_n_play;
}

TEST(TestPlugNPlay, NoSensorsConnected) {
  auto plug_n_play =
      initialize_pnp(false, false, false, false, false, false, false);
  EXPECT_NO_THROW(plug_n_play->check_sensors());
  check_sensors(false, false, false, false, false, false, false);
  reset_sensors();
}

TEST(TestPlugNPlay, OnlySCD30Connected) {
  auto plug_n_play =
      initialize_pnp(true, false, false, false, false, false, false);
  EXPECT_NO_THROW(plug_n_play->check_sensors());
  check_sensors(true, false, false, false, false, false, false);
  reset_sensors();
}

TEST(TestPlugNPlay, OnlySGPC3Connected) {
  auto plug_n_play =
      initialize_pnp(false, true, false, false, false, false, false);
  EXPECT_NO_THROW(plug_n_play->check_sensors());
  check_sensors(false, true, false, false, false, false, false);
  reset_sensors();
}

TEST(TestPlugNPlay, OnlySHT25Connected) {
  auto plug_n_play =
      initialize_pnp(false, false, true, false, false, false, false);
  EXPECT_NO_THROW(plug_n_play->check_sensors());
  check_sensors(false, false, true, false, false, false, false);
  reset_sensors();
}

TEST(TestPlugNPlay, OnlySHTC1Connected) {
  auto plug_n_play =
      initialize_pnp(false, false, false, true, false, false, false);
  EXPECT_NO_THROW(plug_n_play->check_sensors());
  check_sensors(false, false, false, true, false, false, false);
  reset_sensors();
}

TEST(TestPlugNPlay, OnlyEE895Connected) {
  auto plug_n_play =
      initialize_pnp(false, false, false, false, true, false, false);
  EXPECT_NO_THROW(plug_n_play->check_sensors());
  check_sensors(false, false, false, false, true, false, false);
  reset_sensors();
}

TEST(TestPlugNPlay, OnlySHT85Connected) {
  auto plug_n_play =
      initialize_pnp(false, false, false, false, false, true, false);
  EXPECT_NO_THROW(plug_n_play->check_sensors());
  check_sensors(false, false, false, false, false, true, false);
  reset_sensors();
}

TEST(TestPlugNPlay, OnlyTMP117Connected) {
  auto plug_n_play =
      initialize_pnp(false, false, false, false, false, false, true);
  EXPECT_NO_THROW(plug_n_play->check_sensors());
  check_sensors(false, false, false, false, false, false, true);
  reset_sensors();
}

TEST(TestPlugNPlay, SCD30AndSGPC3Connected) {
  auto plug_n_play =
      initialize_pnp(true, true, false, false, false, false, false);
  EXPECT_NO_THROW(plug_n_play->check_sensors());
  check_sensors(true, true, false, false, false, false, false);
  reset_sensors();
}

TEST(TestPlugNPlay, SCD30AndSHT25Connected) {
  auto plug_n_play =
      initialize_pnp(true, false, true, false, false, false, false);
  EXPECT_NO_THROW(plug_n_play->check_sensors());
  check_sensors(true, false, true, false, false, false, false);
  reset_sensors();
}

TEST(TestPlugNPlay, SCD30AndSHTC1Connected) {
  auto plug_n_play =
      initialize_pnp(true, false, false, true, false, false, false);
  EXPECT_NO_THROW(plug_n_play->check_sensors());
  check_sensors(true, false, false, true, false, false, false);
  reset_sensors();
}

TEST(TestPlugNPlay, SCD30AndEE895Connected) {
  auto plug_n_play =
      initialize_pnp(true, false, false, false, true, false, false);
  EXPECT_NO_THROW(plug_n_play->check_sensors());
  check_sensors(true, false, false, false, true, false, false);
  reset_sensors();
}

TEST(TestPlugNPlay, SCD30AndSHT85Connected) {
  auto plug_n_play =
      initialize_pnp(true, false, false, false, false, true, false);
  EXPECT_NO_THROW(plug_n_play->check_sensors());
  check_sensors(true, false, false, false, false, true, false);
  reset_sensors();
}

TEST(TestPlugNPlay, SCD30AndTMP117Connected) {
  auto plug_n_play =
      initialize_pnp(true, false, false, false, false, false, true);
  EXPECT_NO_THROW(plug_n_play->check_sensors());
  check_sensors(true, false, false, false, false, false, true);
  reset_sensors();
}

TEST(TestPlugNPlay, SHT25AndSGPC3Connected) {
  auto plug_n_play =
      initialize_pnp(false, true, true, false, false, false, false);
  EXPECT_NO_THROW(plug_n_play->check_sensors());
  check_sensors(false, true, true, false, false, false, false);
  reset_sensors();
}

TEST(TestPlugNPlay, SHT25AndSHTC1Connected) {
  auto plug_n_play =
      initialize_pnp(false, false, true, true, false, false, false);
  EXPECT_NO_THROW(plug_n_play->check_sensors());
  check_sensors(false, false, false, true, false, false, false);
  reset_sensors();
}

TEST(TestPlugNPlay, SHT25AndSHT85Connected) {
  auto plug_n_play =
      initialize_pnp(false, false, true, false, false, true, false);
  EXPECT_NO_THROW(plug_n_play->check_sensors());
  check_sensors(false, false, true, false, false, true, false);
  reset_sensors();
}

TEST(TestPlugNPlay, SHT25AndTMP117Connected) {
  auto plug_n_play =
      initialize_pnp(false, false, true, false, false, false, true);
  EXPECT_NO_THROW(plug_n_play->check_sensors());
  check_sensors(false, false, true, false, false, false, true);
  reset_sensors();
}

TEST(TestPlugNPlay, EE895AndSGPC3Connected) {
  auto plug_n_play =
      initialize_pnp(false, true, false, false, true, false, false);
  EXPECT_NO_THROW(plug_n_play->check_sensors());
  check_sensors(false, true, false, false, true, false, false);
  reset_sensors();
}

TEST(TestPlugNPlay, EE895AndSHT25Connected) {
  auto plug_n_play =
      initialize_pnp(false, false, true, false, true, false, false);
  EXPECT_NO_THROW(plug_n_play->check_sensors());
  check_sensors(false, false, true, false, true, false, false);
  reset_sensors();
}

TEST(TestPlugNPlay, EE895AndSHTC1Connected) {
  auto plug_n_play =
      initialize_pnp(false, false, false, true, true, false, false);
  EXPECT_NO_THROW(plug_n_play->check_sensors());
  check_sensors(false, false, false, true, true, false, false);
  reset_sensors();
}

TEST(TestPlugNPlay, EE895AndSHT85Connected) {
  auto plug_n_play =
      initialize_pnp(false, false, false, false, true, true, false);
  EXPECT_NO_THROW(plug_n_play->check_sensors());
  check_sensors(false, false, false, false, true, true, false);
  reset_sensors();
}

TEST(TestPlugNPlay, EE895AndTMP117Connected) {
  auto plug_n_play =
      initialize_pnp(false, false, false, false, true, false, true);
  EXPECT_NO_THROW(plug_n_play->check_sensors());
  check_sensors(false, false, false, false, true, false, true);
  reset_sensors();
}

TEST(TestPlugNPlay, SHTC1AndSGPC3Connected) {
  auto plug_n_play =
      initialize_pnp(false, true, false, true, false, false, false);
  EXPECT_NO_THROW(plug_n_play->check_sensors());
  check_sensors(false, true, false, true, false, false, false);
  reset_sensors();
}

TEST(TestPlugNPlay, SHTC1AndSHT85Connected) {
  auto plug_n_play =
      initialize_pnp(false, false, true, false, false, true, false);
  EXPECT_NO_THROW(plug_n_play->check_sensors());
  check_sensors(false, false, true, false, false, true, false);
  reset_sensors();
}

TEST(TestPlugNPlay, SHTC1AndTMP117Connected) {
  auto plug_n_play =
      initialize_pnp(false, false, true, false, false, false, true);
  EXPECT_NO_THROW(plug_n_play->check_sensors());
  check_sensors(false, false, true, false, false, false, true);
  reset_sensors();
}

TEST(TestPlugNPlay, SHT85AndTMP117Connected) {
  auto plug_n_play =
      initialize_pnp(false, false, false, false, false, true, true);
  EXPECT_NO_THROW(plug_n_play->check_sensors());
  check_sensors(false, false, false, false, false, true, true);
  reset_sensors();
}

TEST(TestPlugNPlay, SHT85AndSGPC3Connected) {
  auto plug_n_play =
      initialize_pnp(false, true, false, false, false, true, false);
  EXPECT_NO_THROW(plug_n_play->check_sensors());
  check_sensors(false, true, false, false, false, true, false);
  reset_sensors();
}

TEST(TestPlugNPlay, TMP117AndSGPC3Connected) {
  auto plug_n_play =
      initialize_pnp(false, true, false, false, false, false, true);
  EXPECT_NO_THROW(plug_n_play->check_sensors());
  check_sensors(false, true, false, false, false, false, true);
  reset_sensors();
}
}  // namespace plug_n_play_test
