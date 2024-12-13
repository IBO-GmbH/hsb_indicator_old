#include "database_transaction_helper.hpp"

using namespace wolf;

database_transaction_helper::database_transaction_helper(database& database_)
    : m_database(database_) {}

std::unique_ptr<database_transaction_helper::transaction>
database_transaction_helper::begin() {
  if (m_already_began) return {};
  m_already_began = true;
  std::unique_ptr<transaction> result(new transaction());
  result->transaction.reset(new soci::transaction(m_database.get_session()));
  result->lock_guard = [this] { m_already_began = false; };
  return result;
}

void database_transaction_helper::commit(
    std::unique_ptr<transaction>&& to_commit) {
  if (!to_commit) return;
  auto moved = std::move(to_commit);
  moved->transaction->commit();
}

database_transaction_helper::transaction::~transaction() { lock_guard(); }
