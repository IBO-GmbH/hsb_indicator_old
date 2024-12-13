#include "database.hpp"
#include <iostream>

using namespace wolf;
using namespace logging;

database::soci_logger::soci_logger() : std::ostream(this), m_logger{"soci"} {}

int database::soci_logger::overflow(int c) {
  handle_character(static_cast<char>(c));
  return 0;
}

void database::soci_logger::handle_character(const char c) {
  if ('\n' == c) {
    m_line.flush();
    LOGGING_LOG(m_logger, logging::severity::verbose) << m_line.str();
    m_line = std::stringstream();
    return;
  }
  m_line << c;
}

database::database() : m_logger{"database"} {}

database::database(const std::string &file) : m_logger{"database"} {
  open(file);
}

void database::open(const std::string &database) {
  BOOST_ASSERT(!database.empty());
  LOGGING_LOG(m_logger, severity::normal)
      << "opening database:'" << database << "'";

  m_session.set_log_stream(&m_soci_logger);
  m_session.open("sqlite3", database);
  m_session << "PRAGMA journal_mode=WAL;";
}

void database::close() {
  m_session.close();
  m_session.set_log_stream(nullptr);
}

soci::session &database::get_session() { return m_session; }
