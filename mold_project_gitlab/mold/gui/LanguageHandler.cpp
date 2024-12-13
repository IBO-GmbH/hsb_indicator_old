#include "LanguageHandler.hpp"
#include <QApplication>
#include "wolf/thread_checker.hpp"

using namespace logging;
using namespace mold;

const std::string LanguageHandler::m_stringEnglish = "en";
const std::string LanguageHandler::m_stringGerman = "de";

LanguageHandler::LanguageHandler(const WolfServices &services,
                                 DataExchange &dataExchange,
                                 wolf::key_value_cache &database,
                                 const std::string &defaultLanguage,
                                 const std::string &languageFileGerman)
    : QObject(nullptr),
      m_logger{"LanguageHandler"},
      m_services(services),
      m_dataExchange(dataExchange),
      m_database(database),
      m_languageFileGerman(languageFileGerman),
      m_languageString(getLanguageFromDatabase()) {
  m_on_completion = [this] {
    LOGGING_LOG(m_logger, severity::verbose)
        << "finished translating room names";
  };
  if (m_languageString.empty()) {
    LOGGING_LOG(m_logger, severity::normal)
        << "language from databse is not set - setting fallback language:"
        << defaultLanguage;
    setFromString(defaultLanguage);
    return;
  }
  setFromString(m_languageString);
}

wolf::languages LanguageHandler::getLanguage() { return m_language; }

void LanguageHandler::setGerman() {
  wolf::thread_checker::throw_if_not_ui("LanguageHandler::setGerman");
  LOGGING_LOG(m_logger, severity::normal)
      << "setting language to german, languageFileGerman:"
      << m_languageFileGerman;

  loadGerman();
  m_language = wolf::languages::german;
  setLanguageToDatabase(m_stringGerman);
}

void LanguageHandler::loadGerman() {
  const bool loadResult =
      m_translator.load(QString::fromStdString(m_languageFileGerman));
  if (!loadResult) {
    LOGGING_LOG(m_logger, severity::error)
        << "could not load german language file: '" << m_languageFileGerman
        << "'";
    throw std::runtime_error(
        "LanguageHandler::setGerman: could not load german language file!");
  }
  const bool installed = QApplication::installTranslator(&m_translator);
  if (!installed) {
    LOGGING_LOG(m_logger, severity::error)
        << "could not install german language file: '" << m_languageFileGerman
        << "'";
    throw std::runtime_error(
        "LanguageHandler::setGerman: could not install german language file!");
  }
}

void LanguageHandler::setEnglish() {
  wolf::thread_checker::throw_if_not_ui("LanguageHandler::setEnglish");
  LOGGING_LOG(m_logger, severity::normal) << "setting language to english";
  loadEnglish();
  m_language = wolf::languages::english;
  setLanguageToDatabase(m_stringEnglish);
}

void LanguageHandler::loadEnglish() {
  QApplication::removeTranslator(&m_translator);
}

void LanguageHandler::setFromString(const std::string &language) {
  if (language == m_stringEnglish) {
    setEnglish();
    m_services.room_name_language_changer->async_set(wolf::languages::english,
                                                     m_on_completion);
    return;
  }
  if (language == m_stringGerman) {
    setGerman();
    m_services.room_name_language_changer->async_set(wolf::languages::german,
                                                     m_on_completion);
    return;
  }
  LOGGING_LOG(m_logger, severity::warning)
      << "could not set languange from string:" << language;
  loadEnglish();
  BOOST_ASSERT_MSG(
      false,
      "LanguageHandler::setFromString: could not set languange from string!");
}

void LanguageHandler::setLanguageToDatabase(const std::string &language) {
  wolf::thread_checker::throw_if_not_ui(
      "LanguageHandler::setLanguageToDatabase");
  if (m_languageString == language) return;
  m_services.service->post([this, language]() {
    try {
      m_database.set_language(language);
    } catch (const std::exception &error) {
      LOGGING_LOG(m_logger, severity::error)
          << "database->set_language failed with a fatal error. what():"
          << error.what();
    }
  });
}

std::string LanguageHandler::getLanguageFromDatabase() const {
  wolf::thread_checker::throw_if_not_ui(
      "LanguageHandler::getLanguageFromDatabase");
  std::string language;
  std::mutex mutex;
  std::condition_variable condition_variable;
  std::unique_lock<std::mutex> lock(mutex);

  m_services.service->post([this, &language, &mutex, &condition_variable]() {
    {
      std::unique_lock<std::mutex> lock(mutex);
      language = m_database.get_language();
    }
    condition_variable.notify_one();
  });

  condition_variable.wait(lock);

  return language;
}
