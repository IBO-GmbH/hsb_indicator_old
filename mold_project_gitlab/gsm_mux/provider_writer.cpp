#include "provider_writer.hpp"
#include <boost/filesystem/operations.hpp>
#include <fstream>

using namespace logging;

provider_writer::provider_writer() : m_logger{"provider_writer"} {}

void provider_writer::construt_memory(
    boost::interprocess::managed_shared_memory &shared_memory) {
  provider_memory.reset(shared_memory.construct<ipc_string>("provider")(
      "", shared_memory.get_segment_manager()));
  allocator.reset(new char_allocator(shared_memory.get_segment_manager()));
}

void provider_writer::write(const std::string &provider) {
  LOGGING_LOG(m_logger, severity::normal)
      << "provider_writer, writing provider: '" << provider << "'";
  ipc_string provider_(*allocator);
  provider_ = provider.c_str();
  *provider_memory = provider_;
}
