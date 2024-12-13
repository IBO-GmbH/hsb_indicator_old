#ifndef MOLD_GUI_NUMBERFACTORY_HPP
#define MOLD_GUI_NUMBERFACTORY_HPP

#include <Includes/eoChannelEnums.h>
#include <string>

class QString;
class QWidget;

namespace mold {

class NumberBase;

class NumberFactory {
 public:
  NumberFactory() = delete;

  enum class unity { temperature, humidity, co2 };
  static NumberBase *createSpecial(const unity &unity_,
                                   QWidget *parent = nullptr);
  static NumberBase *createGeneric(const std::string &name,
                                   const std::string &unit,
                                   const CHANNEL_TYPE &channel,
                                   QWidget *parent = nullptr);
};
}  // namespace mold

#endif  // MOLD_GUI_NUMBERFACTORY_HPP
