#ifndef WOLF_TONE_WRITER_HPP
#define WOLF_TONE_WRITER_HPP

#include <boost/noncopyable.hpp>

#include "log/logger.hpp"

namespace wolf {

class tone_writer : public boost::noncopyable {
 public:
  tone_writer(const int& gpio);

  bool open();
  void set_enabled(const bool& enabled);

 private:
  logging::logger m_logger;

  const int m_gpio;

  bool m_enabled;
};
}

#endif  // WOLF_TONE_WRITER_HPP
