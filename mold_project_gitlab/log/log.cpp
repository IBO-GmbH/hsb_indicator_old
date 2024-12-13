#include "log.hpp"
#include <boost/log/attributes/mutable_constant.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>
#include "log/journal_sink.hpp"

using namespace logging;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
BOOST_LOG_ATTRIBUTE_KEYWORD(severity_keyword, "Severity", severity)
#pragma GCC diagnostic pop

static auto date_time_formatter =
    boost::log::expressions::stream
    << boost::log::expressions::format_date_time<boost::posix_time::ptime>(
           "TimeStamp", "%Y-%m-%d %H:%M:%S.%f");
static auto process_formatter =
    boost::log::expressions::stream << boost::log::expressions::attr<
        boost::log::attributes::current_process_id::value_type>("PID");
static auto thread_formatter =
    boost::log::expressions::stream << boost::log::expressions::attr<
        boost::log::attributes::current_thread_id::value_type>("TID");
static auto tag_formatter =
    boost::log::expressions::stream
    << boost::log::expressions::attr<std::string>("Tag");

static void start_color(const severity severity_,
                        boost::log::formatting_ostream& output) {
  switch (severity_) {
    case severity::error:
      output << "\033[41m";
      break;
    case severity::normal:
      output << "\033[32m";
      break;
    case severity::warning:
      output << "\033[31m";
      break;
    default:
      break;
  }
}

static void end_color(const severity severity_,
                      boost::log::formatting_ostream& output) {
  if (severity_ != severity::verbose) output << "\033[0m";
}

namespace {
struct console_formatter {
  const config::console config;
  void operator()(boost::log::record_view const& record,
                  boost::log::formatting_ostream& output) const {
    boost::log::value_ref<severity> severity_ =
        boost::log::extract<severity>("Severity", record);
    BOOST_ASSERT(severity_);
    severity severity_value = severity_.get();
    if (config.color) start_color(severity_value, output);
    if (config.timestamp) date_time_formatter(record, output) << " ";
    process_formatter(record, output) << " ";
    thread_formatter(record, output) << " ";
    output << severity_ << " [";
    tag_formatter(record, output);
    output << "] " << record[boost::log::expressions::smessage];
    if (config.color) end_color(severity_value, output);
  }
};
}  // namespace

static void initialize_file(const config::file& file_config_,
                            const bool flush_after_each_entry) {
  namespace attrs = boost::log::attributes;
  namespace expr = boost::log::expressions;

  if (file_config_.file.empty()) return;
  const boost::log::formatter formatter =
      expr::stream << expr::format_date_time<boost::posix_time::ptime>(
                          "TimeStamp", "%Y-%m-%d %H:%M:%S.%f")
                   << " "
                   << expr::attr<attrs::current_process_id::value_type>("PID")
                   << " "
                   << expr::attr<attrs::current_thread_id::value_type>("TID")
                   << " " << expr::attr<severity>("Severity") << " ["
                   << expr::attr<std::string>("Tag") << "] " << expr::message;
  using file_sink =
      boost::log::sinks::synchronous_sink<boost::log::sinks::text_file_backend>;
  const int file_size = file_config_.rotation_size * 1024 * 1024;  // byte to mb
  boost::shared_ptr<file_sink> sink = boost::log::add_file_log(
      boost::log::keywords::file_name = file_config_.file,
      boost::log::keywords::auto_flush = flush_after_each_entry,
      boost::log::keywords::rotation_size = file_size);
  sink->set_formatter(formatter);
  sink->locked_backend()->set_file_collector(
      boost::log::sinks::file::make_collector(
          boost::log::keywords::target = "logs",
          boost::log::keywords::max_size =
              file_config_.file_count * file_size));
}

static void initialize_console(const config::console& console_config_,
                               const bool flush_after_each_entry) {
  if (!console_config_.enabled) return;
  boost::log::add_console_log(
      std::cout, boost::log::keywords::auto_flush = flush_after_each_entry)
      ->set_formatter(console_formatter{console_config_});
}

static void initialize_journal(const config::journal& journal_config_) {
  using sink_type = boost::log::sinks::synchronous_sink<journal_sink>;
  if (!journal_config_.enabled) return;
  auto core = boost::log::core::get();
  const auto sink = boost::make_shared<sink_type>();
  core->add_sink(sink);
}

void logging::initialize(const config::all& config_) {
  namespace attrs = boost::log::attributes;

  boost::shared_ptr<boost::log::core> core = boost::log::core::get();
  core->add_global_attribute("TimeStamp", attrs::utc_clock());
  core->add_global_attribute("PID", attrs::current_process_id());
  core->add_global_attribute("TID", attrs::current_thread_id());
  core->add_global_attribute("File", attrs::mutable_constant<std::string>(""));
  core->add_global_attribute("Line", attrs::mutable_constant<int>(0));

  core->set_filter(severity_keyword <= config_.common_.severity);
  const auto flush_after_each_entry = config_.common_.flush;
  initialize_file(config_.file_, flush_after_each_entry);
  initialize_console(config_.console_, flush_after_each_entry);
  initialize_journal(config_.journal_);
}

void logging::initialize() {
  config::all config_;
  config_.console_.enabled = true;
  config_.journal_.enabled = false;
  initialize(config_);
}
