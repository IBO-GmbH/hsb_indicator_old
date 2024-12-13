#ifndef WOLF_THREAD_CHECKER_HPP
#define WOLF_THREAD_CHECKER_HPP

#include <thread>

namespace wolf {

class thread_checker {
 public:
  thread_checker() = delete;
  thread_checker(const thread_checker &) = delete;

  static void register_as_ui();
  static void register_as_service();

  static bool is_ui();
  static bool is_service();

  static void throw_if_not_service(const std::string &function_to_check);
  static void throw_if_not_ui(const std::string &function_to_check);

 private:
  static std::thread::id m_id_ui;
  static std::thread::id m_id_service;
};
}

#endif  // WOLF_THREAD_CHECKER_HPP
