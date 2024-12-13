#ifndef MOLD_GUI_GSMINFORMATIONREADER_HPP
#define MOLD_GUI_GSMINFORMATIONREADER_HPP

#include <QObject>
#include "log/logger.hpp"
#include "mold/gui/WolfServices.hpp"

namespace mold {

// TODO shall be a wolf class and use boost!
class GsmInformationReader : public QObject {
  Q_OBJECT
 public:
  using optionalString = std::optional<QString>;
  using optionalInt = std::optional<std::int32_t>;

  explicit GsmInformationReader(const WolfServices& services,
                                QObject* parent = nullptr);

  const optionalInt& getLastRssi() const;
  const optionalString& getLastProvider() const;

 signals:
  void signalRssi(optionalInt rssi);
  void signalProvider(optionalString provider);

 private:
  void setRssi(const optionalInt& rssi);
  void setProvider(const optionalString& provider);

 private:
  logging::logger m_logger;

  boost::signals2::scoped_connection m_connection_rssi;
  boost::signals2::scoped_connection m_connection_provider;

  optionalInt m_last_rssi;
  optionalString m_last_provider;
};

}  // namespace mold

#endif  // MOLD_GUI_GSMINFORMATIONREADER_HPP
