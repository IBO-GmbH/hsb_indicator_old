#include "thread_checker.hpp"

using namespace wolf;

std::thread::id thread_checker::m_id_ui{};
std::thread::id thread_checker::m_id_service{};

void thread_checker::register_as_ui() { m_id_ui = std::this_thread::get_id(); }

void thread_checker::register_as_service() {
  m_id_service = std::this_thread::get_id();
}

bool thread_checker::is_ui() { return m_id_ui == std::this_thread::get_id(); }

bool thread_checker::is_service() {
  return m_id_service == std::this_thread::get_id();
}

void thread_checker::throw_if_not_service(
    const std::string &function_to_check) {
  if (!is_service())
    throw std::runtime_error(
        (function_to_check + ": wolf::thread_checker is_service false"));
}

void thread_checker::throw_if_not_ui(const std::string &function_to_check) {
  if (!is_ui())
    throw std::runtime_error(
        (function_to_check + ": wolf::thread_checker is_ui false"));
}
