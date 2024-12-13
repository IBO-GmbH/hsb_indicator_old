#ifndef WOLF_TABLE_SYNC_HELPER_HPP
#define WOLF_TABLE_SYNC_HELPER_HPP

#include <soci.h>
#include "log/logger.hpp"
#include "wolf/database_cache.hpp"

namespace wolf {

template <class container_type, typename id_type>
class table_sync_helper : public database_cache {
 public:
  using id = id_type;
  using column = std::pair<std::string, std::string>;
  using fields = std::vector<column>;
  table_sync_helper(logging::logger& logger, wolf::database& database_,
                    const std::string& table_name, const fields& fields);

  void add(const container_type& to_add);
  void update(const container_type& to_update);
  void remove(const id& id);

  container_type get(const id& id);
  using get_results = std::vector<container_type>;
  get_results get_all();

 private:
  logging::logger& m_logger;
  const std::string m_table_name;
  const fields m_fields;
};

template <class container_type, typename id_type>
table_sync_helper<container_type, id_type>::table_sync_helper(
    logging::logger& logger, database& database_, const std::string& table_name,
    const table_sync_helper::fields& fields)
    : database_cache(database_),
      m_logger(logger),
      m_table_name(table_name),
      m_fields(fields) {
  std::stringstream create_statement;
  create_statement << "create table if not exists " << table_name << " (";
  // creates a ", " too much :( but soon!
  // http://en.cppreference.com/w/cpp/experimental/ostream_joiner
  for (std::size_t index = 0; index < fields.size(); ++index) {
    const auto& field = fields[index];

    create_statement << field.first << " " << field.second;
    if (index == 0) {
      create_statement << " primary key";
    }
    if (index < fields.size() - 1) {
      create_statement << ", ";
    }
  }
  create_statement << ")";

  const std::string create = create_statement.str();
  LOGGING_LOG(m_logger, logging::severity::verbose)
      << "creating table:\"" << create << "\"";
  m_session << create;
}

template <class container_type, typename id_type>
void table_sync_helper<container_type, id_type>::add(
    const container_type& to_add) {
  std::stringstream insert_statement;
  insert_statement << "insert into " << m_table_name << "(";
  for (std::size_t index = 0; index < m_fields.size(); ++index) {
    const auto& field = m_fields[index];
    insert_statement << field.first;
    if (index < m_fields.size() - 1) {
      insert_statement << ", ";
    }
  }
  insert_statement << ") values(";
  for (std::size_t index = 0; index < m_fields.size(); ++index) {
    const auto& field = m_fields[index];
    insert_statement << ":" << field.first;
    if (index < m_fields.size() - 1) {
      insert_statement << ", ";
    }
  }
  insert_statement << ")";
  m_session << insert_statement.str(), soci::use(to_add);
}

template <class container_type, typename id_type>
void table_sync_helper<container_type, id_type>::update(
    const container_type& to_update) {
  std::stringstream update_statement;
  update_statement << "update " << m_table_name << " set ";
  for (std::size_t index = 0; index < m_fields.size(); ++index) {
    const auto& field = m_fields[index];
    update_statement << field.first << " = :" << field.first;
    if (index < m_fields.size() - 1) {
      update_statement << ", ";
    }
  }
  update_statement << " where " << m_fields[0].first
                   << " = :" << m_fields[0].first;
  m_session << update_statement.str(), soci::use(to_update);
  ensure_a_row_changed();
}

template <class container_type, typename id_type>
void table_sync_helper<container_type, id_type>::remove(const id& id) {
  m_session << "delete from " << m_table_name << " where " << m_fields[0].first
            << " = :" << m_fields[0].first,
      soci::use(id);
  ensure_a_row_changed();
}

template <class container_type, typename id_type>
typename table_sync_helper<container_type, id_type>::get_results
table_sync_helper<container_type, id_type>::get_all() {
  soci::rowset<container_type> result_sql =
      (m_session.prepare << "select * from " << m_table_name);
  get_results result;
  std::copy(result_sql.begin(), result_sql.end(), std::back_inserter(result));
  return result;
}

}  // namespace wolf

#endif  // WOLF_TABLE_SYNC_HELPER_HPP
