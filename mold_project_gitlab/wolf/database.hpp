#ifndef WOLF_DATABASE_HPP
#define WOLF_DATABASE_HPP

#include "log/logger.hpp"

#include <soci.h>

namespace wolf {

class database {
 public:
  database();
  database(const std::string &file);

  /**
   * @brief open throws on any error!
   * @param database
   */
  void open(const std::string &database);
  void close();

  soci::session &get_session();

 private:
  class soci_logger : public std::ostream, std::streambuf {
   public:
    soci_logger();

    int overflow(int c) override;

   private:
    void handle_character(const char c);

   private:
    logging::logger m_logger;
    std::stringstream m_line;
  };

 private:
  logging::logger m_logger;
  soci_logger m_soci_logger;

  soci::session m_session;
};  // namespace wolf
}  // namespace wolf

#endif  // WOLF_DATABASE_HPP
