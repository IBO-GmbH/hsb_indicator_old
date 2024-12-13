#ifndef MOLD_GUI_LANGUAGEHANDLER_HPP
#define MOLD_GUI_LANGUAGEHANDLER_HPP

#include <QObject>
#include <QTranslator>
#include "mold/gui/DataExchange.hpp"
#include "wolf/key_value_cache.hpp"
#include "wolf/languages.hpp"

namespace mold {
class options_cache;
}

namespace mold {

class LanguageHandler : public QObject, public boost::noncopyable {
  Q_OBJECT
 public:
  explicit LanguageHandler(const WolfServices& services,
                           DataExchange& dataExchange,
                           wolf::key_value_cache& database,
                           const std::string& defaultLanguage,
                           const std::string& languageFileGerman);
  wolf::languages getLanguage();

 public slots:
  void setGerman();
  void setEnglish();

 private:
  void loadGerman();
  void loadEnglish();

  void setFromString(const std::string& language);
  void setLanguageToDatabase(const std::string& language);
  std::string getLanguageFromDatabase() const;

  logging::logger m_logger;

  QTranslator m_translator;

  const WolfServices& m_services;
  DataExchange& m_dataExchange;
  wolf::key_value_cache& m_database;
  const std::string m_languageFileGerman;

  static const std::string m_stringEnglish;
  static const std::string m_stringGerman;

  const std::string m_languageString;
  wolf::languages m_language;

  using completion_callback = std::function<void()>;
  completion_callback m_on_completion;
};
}  // namespace mold

#endif  // MOLD_GUI_LANGUAGEHANDLER_HPP
