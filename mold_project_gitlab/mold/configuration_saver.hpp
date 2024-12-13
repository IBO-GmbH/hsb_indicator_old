#ifndef MOLD_CONFIGURATION_SAVER_HPP
#define MOLD_CONFIGURATION_SAVER_HPP

#include "configuration_handler.hpp"

namespace mold {

class configuration_saver {
 public:
  configuration_saver(configuration_handler& configuration_handler_);

  configuration save_and_update(configuration configuration_);

 private:
  logging::logger m_logger;
  configuration_handler& m_configuration_handler;
};

}  // namespace mold

#endif  // CONFIGURATION_SAVER_HPP
