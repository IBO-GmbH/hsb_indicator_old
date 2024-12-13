#include "journal_sink.hpp"
#include <boost/log/attributes.hpp>
#include <boost/log/attributes/value_extraction.hpp>
#include <boost/log/expressions.hpp>
#include "log/severity.hpp"

#define SD_JOURNAL_SUPPRESS_LOCATION
extern "C" {
#include <systemd/sd-journal.h>
}

using namespace logging;

static int cast_severity_to_rfc3164_severity(const severity severity_) {
  // https://tools.ietf.org/html/rfc3164#page-9
  switch (severity_) {
    case severity::error:
      return LOG_ERR;
    case severity::warning:
      return LOG_WARNING;
    case severity::normal:
      return LOG_INFO;
    case severity::verbose:
      return LOG_DEBUG;
  }
  return LOG_INFO;
}

static void add_message(std::vector<std::string>& values,
                        const boost::log::record_view& record) {
  const auto message = record[boost::log::expressions::smessage];
  if (!message) return;

  const auto tag = record["Tag"].extract<std::string>();
  const auto severity_ = record["Severity"].extract<severity>();
  std::string log = "MESSAGE=";

  if (severity_) 
  {
    std::stringstream ss;
    ss << severity_.get();
    log += "[" + ss.str() + "]";
  }

  if (tag) 
      log += "[" + tag.get() + "]";

  log += " ";
  log += message.get();
  values.push_back(log);  
}

static void add_tid(std::vector<std::string>& values,
                    const boost::log::record_view& record) {
  namespace attrs = boost::log::attributes;
  const auto tid =
      record["TID"].extract<attrs::current_thread_id::value_type>();
  if (!tid) return;
  values.push_back("TID=" + std::to_string(tid.get().native_id()));
}

static void add_severity(std::vector<std::string>& values,
                         const boost::log::record_view& record) {
  const auto severity_ = record["Severity"].extract<severity>();
  if (!severity_) return;
  const auto severity_casted =
      cast_severity_to_rfc3164_severity(severity_.get());
  values.push_back("PRIORITY=" + std::to_string(severity_casted));
}

static void add_tag(std::vector<std::string>& values,
                    const boost::log::record_view& record) {
  const auto tag = record["Tag"].extract<std::string>();
  if (!tag) return;
  values.push_back("TAG=" + tag.get());
}

static void add_file(std::vector<std::string>& values,
                     const boost::log::record_view& record) {
  const auto tag = record["File"].extract<std::string>();
  if (!tag) return;
  values.push_back("CODE_FILE=" + tag.get());
}

static void add_line(std::vector<std::string>& values,
                     const boost::log::record_view& record) {
  const auto tag = record["Line"].extract<int>();
  if (!tag) return;
  values.push_back("CODE_LINE=" + std::to_string(tag.get()));
}

void journal_sink::consume(const boost::log::record_view& record) {
  std::vector<std::string> values;
  add_message(values, record);
  add_tid(values, record);
  add_severity(values, record);
  add_tag(values, record);
  add_file(values, record);
  add_line(values, record);
  std::vector<struct iovec> result;
  for (const auto& value : values)
    result.push_back(iovec{const_cast<char*>(value.c_str()), value.size()});
  sd_journal_sendv(result.data(), static_cast<int>(result.size()));
}
