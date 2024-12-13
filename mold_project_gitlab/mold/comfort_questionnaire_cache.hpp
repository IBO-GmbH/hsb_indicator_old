#ifndef MOLD_COMFORT_QUESTIONNAIRE_CACHE_HPP
#define MOLD_COMFORT_QUESTIONNAIRE_CACHE_HPP

#include "comfort_questionnaire.hpp"
#include "wolf/database.hpp"

namespace mold {

class comfort_questionnaire_cache {
 public:
  comfort_questionnaire_cache(wolf::database& database);
  std::optional<questionnaire_options> get_options();
  void set_options(const questionnaire_options& options);

 private:
  void create_tables();
  void set_option(const std::string& key, const std::string& value);
  std::string get_option(const std::string& key);
  void load_options(std::vector<std::string>& option_strings);
  static questionnaire_options convert_strings_to_options(
      const std::vector<std::string>& options);

 private:
  logging::logger m_logger;
  soci::session& m_session;
};
}  // namespace mold

#endif  // MOLD_COMFORT_QUESTIONNAIRE_CACHE_HPP
