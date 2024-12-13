#include "at_read_write.hpp"

at_read_write::at_read_write(boost::asio::serial_port& serial,
                             const std::string& info)
    : reader(serial, info), writer(serial, info) {}
