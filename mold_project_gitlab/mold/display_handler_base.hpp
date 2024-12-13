#ifndef MOLD_DISPLAY_HANDLER_BASE_HPP
#define MOLD_DISPLAY_HANDLER_BASE_HPP

namespace mold {

class display_handler_base {
 public:
  virtual void set_on(const bool on) = 0;
  virtual bool display_is_locked() const = 0;
  virtual void enable_states_update(const bool enable) = 0;
  virtual void handle_states_update() = 0;
  virtual ~display_handler_base() = default;
};
}  // namespace mold

#endif  // MOLD_DISPLAY_HANDLER_BASE_HPP
