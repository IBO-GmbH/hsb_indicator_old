#include "comfort_questionnaire_cache.hpp"
#include "wolf/thread_checker.hpp"
#include "wolf/timestamp_helper.hpp"

static constexpr auto table_name = "comfort_questionnaire_options";

using namespace logging;

mold::comfort_questionnaire_cache::comfort_questionnaire_cache(
    wolf::database &database)
    : m_logger{"comfort_questionnaire_cache"},
      m_session(database.get_session()) {
  create_tables();
}

std::optional<mold::questionnaire_options>
mold::comfort_questionnaire_cache::get_options() {
  LOGGING_LOG(m_logger, severity::verbose) << "get_options";
  wolf::thread_checker::throw_if_not_service(
      "comfort_questionnaire_cache::get_options");
  std::vector<std::string> options;
  load_options(options);
  if (std::all_of(options.cbegin(), options.cend(),
                  [](const std::string &option) { return option.empty(); }))
    return std::optional<questionnaire_options>();
  return convert_strings_to_options(options);
}

void mold::comfort_questionnaire_cache::set_options(
    const mold::questionnaire_options &options) {
  LOGGING_LOG(m_logger, severity::verbose) << "set_options: " << options;
  wolf::thread_checker::throw_if_not_service(
      "comfort_questionnaire_cache::set_options");
  set_option("enabled", (options.enabled ? "1" : "0"));
  set_option("number_of_questionnaires",
             std::to_string(options.number_of_questionnaires));
  for (unsigned int index = 0;
       index < questionnaire_options::max_number_of_questionnaires; ++index) {
    set_option(("hours_time_" + std::to_string(index + 1)),
               std::to_string(options.hours_time[index]));
    set_option(("minutes_time_" + std::to_string(index + 1)),
               std::to_string(options.minutes_time[index]));
  }
}

void mold::comfort_questionnaire_cache::create_tables() {
  LOGGING_LOG(m_logger, severity::verbose) << "create_tables_options";
  m_session << "create table if not exists " << table_name
            << "(key text primary key, "
               "value text not null)";
}

void mold::comfort_questionnaire_cache::set_option(const std::string &key,
                                                   const std::string &value) {
  LOGGING_LOG(m_logger, severity::verbose)
      << "set, key: " << key << ", value: " << value;
  m_session << "insert or replace into " << table_name
            << " (key, value)"
               " values (:key, :value)",
      soci::use(key), soci::use(value);
}

std::string mold::comfort_questionnaire_cache::get_option(
    const std::string &key) {
  LOGGING_LOG(m_logger, severity::verbose) << "get, key: " << key;
  std::string result;
  m_session << "select value from " << table_name << " where key = :key",
      soci::into(result), soci::use(key);
  return result;
}

void mold::comfort_questionnaire_cache::load_options(
    std::vector<std::string> &option_strings) {
  option_strings.push_back(get_option("enabled"));
  option_strings.push_back(get_option("number_of_questionnaires"));
  for (unsigned int index = 1;
       index <= questionnaire_options::max_number_of_questionnaires; ++index) {
    option_strings.push_back(get_option("hours_time_" + std::to_string(index)));
    option_strings.push_back(
        get_option("minutes_time_" + std::to_string(index)));
  }
}

mold::questionnaire_options
mold::comfort_questionnaire_cache::convert_strings_to_options(
    const std::vector<std::string> &options) {
  questionnaire_options loaded_options;
  loaded_options.enabled = (options[0] == "0" ? false : true);
  loaded_options.number_of_questionnaires = std::stoi(options[1]);
  for (unsigned int index = 0;
       index <
       static_cast<unsigned int>(loaded_options.number_of_questionnaires);
       ++index) {
    loaded_options.hours_time[index] = std::stoi(options[2 * index + 2]);
    loaded_options.minutes_time[index] = std::stoi(options[2 * index + 3]);
  }
  return loaded_options;
}
