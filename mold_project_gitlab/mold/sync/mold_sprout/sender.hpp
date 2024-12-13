#ifndef MOLD_SYNC_MOLD_SPROUT_SENDER_HPP
#define MOLD_SYNC_MOLD_SPROUT_SENDER_HPP

#include "mold/sprout_value.hpp"
#include "wolf/network/bond_sender.hpp"
#include "wolf/network/response/simple.hpp"

namespace mold::sync::mold_sprout {

class sender {
 public:
  virtual ~sender() = default;

  struct mold_sprout_value {
    using time_point = std::chrono::system_clock::time_point;
    wolf::types::uuid_array room_id;
    sprout_value value;
  };
  using callback_type = wolf::network::response::simple::callback;
  virtual void send(const mold_sprout_value& to_send,
                    const callback_type& callback) = 0;
};

class sender_implementation : public sender {
 public:
  sender_implementation(wolf::network::bond_sender& sender,
                        wolf::network::response::simple& response);
  void send(const mold_sprout_value& to_send,
            const callback_type& callback) override;

 private:
  Clima::MoldSprout cast_to_network(mold_sprout_value to_send);

  logging::logger m_logger;
  wolf::network::bond_sender& m_sender;
  wolf::network::response::simple& m_reponse;
};

}  // namespace mold::sync::mold_sprout

#endif
