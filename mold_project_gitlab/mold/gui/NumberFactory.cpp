#include "NumberFactory.hpp"

#include "NumberGeneric.hpp"
#include "NumberHumidity.hpp"
#include "NumberTemperature.hpp"
#include "wolf/thread_checker.hpp"

using namespace mold;

NumberBase *NumberFactory::createSpecial(const NumberFactory::unity &unity_,
                                         QWidget *parent) {
  wolf::thread_checker::throw_if_not_ui("NumberFactory::createSpecial");
  if (unity_ == unity::temperature) return new NumberTemperature(parent);
  if (unity_ == unity::humidity) return new NumberHumidity(parent);
  throw std::runtime_error(
      "NumberFactory::createSpecial: unknown type of unity");
}

NumberBase *NumberFactory::createGeneric(const std::string &name,
                                         const std::string &unit,
                                         const CHANNEL_TYPE &channel,
                                         QWidget *parent) {
  wolf::thread_checker::throw_if_not_ui("NumberFactory::createGeneric");
  auto generic = new NumberGeneric(name, unit, parent);
  if (channel == S_PARTICLES || channel == S_CURRENT)
    generic->adjustSizeParticlesAndCurrent();
  if (channel == S_CONC) generic->adjustSizeCo2();
  return generic;
}
