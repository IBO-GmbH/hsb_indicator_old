#ifndef WOLF_DATABASE_TRANSACTION_HELPER_HPP
#define WOLF_DATABASE_TRANSACTION_HELPER_HPP

#include <transaction.h>
#include "database.hpp"

namespace wolf {

class database_transaction_helper {
 public:
  database_transaction_helper(database& database_);

  struct transaction {
    ~transaction();

   private:
    friend class ::wolf::database_transaction_helper;

    std::unique_ptr<soci::transaction> transaction;
    std::function<void()> lock_guard;
  };
  std::unique_ptr<transaction> begin();
  void commit(std::unique_ptr<transaction>&& to_commit);

 protected:
  database& m_database;
  bool m_already_began;
};

}  // namespace wolf

#endif  // WOLF_DATABASE_TRANSACTION_HELPER_HPP
