#include "GsmInformationReader.hpp"

#include "PostToGUIThread.hpp"
#include "mold/gui/WolfServices.hpp"

using namespace logging;
using namespace mold;

GsmInformationReader::GsmInformationReader(const WolfServices &services,
                                           QObject *parent)
    : QObject(parent),
      m_logger("GsmInformationReader"),
      m_last_rssi(optionalInt()),
      m_last_provider(optionalString()) {
  if (services.gsm_information_reader == nullptr) return;

  m_connection_rssi = services.gsm_information_reader->signal_rssi.connect(
      [this](const auto &rssi) {
        postToGUIThread([this, rssi] { setRssi(rssi); });
      });
  m_connection_provider =
      services.gsm_information_reader->signal_provider.connect(
          [this](const auto &provider) {
            postToGUIThread([this, provider] {
              if (provider.has_value()) {
                setProvider(QString::fromStdString(provider.value()));
                return;
              }
              setProvider(optionalString());
            });
          });
}

const GsmInformationReader::optionalInt &GsmInformationReader::getLastRssi()
    const {
  return m_last_rssi;
}

const GsmInformationReader::optionalString &
GsmInformationReader::getLastProvider() const {
  return m_last_provider;
}

void GsmInformationReader::setRssi(const optionalInt &rssi) {
  if (rssi == m_last_rssi) return;
  m_last_rssi = rssi;
  emit signalRssi(m_last_rssi);
}

void GsmInformationReader::setProvider(const optionalString &provider) {
  if (provider == m_last_provider) return;
  m_last_provider = provider;
  emit signalProvider(m_last_provider);
}
