#include <boost/asio/read_until.hpp>
#include <boost/process.hpp>
#include "gtest/gtest.h"
#include "log/logger.hpp"

static logging::logger logger("configuration_cache_test");
static const std::string test_lines = "first line\nsecond line\nthird line";
static const std::string test_line_out_and_err =
    "echo first line; echo second line 1>&2; echo third line";

TEST(BoostProcess, Echo) {
  boost::process::child child(boost::process::search_path("echo"), test_lines);
  child.join();
  EXPECT_EQ(child.exit_code(), 0);
}

namespace {
struct async_pipe_reader {
  boost::asio::io_context context;
  std::vector<char> buffer{4};
  boost::process::async_pipe async_pipe{context};
  std::function<void(const boost::system::error_code &error, std::size_t size)>
      callback;
  void read() {
    boost::asio::async_read(async_pipe, boost::asio::buffer(buffer), callback);
    // the following ressizes to the buffer
    // boost::asio::async_read(async_pipe, boost::asio::dynamic_buffer(buffer),
    // callback);
  }
};
}  // namespace

TEST(BoostProcess, AsyncRead) {
  async_pipe_reader reader;
  boost::process::child child(boost::process::search_path("echo"), test_lines,
                              boost::process::std_out > reader.async_pipe);
  std::string result;
  reader.callback = [&](const boost::system::error_code &error,
                        std::size_t size) {
    std::copy_n(reader.buffer.cbegin(), size, std::back_inserter(result));
    if (error.value() == boost::asio::error::misc_errors::eof) return;
    EXPECT_FALSE(error);
    reader.read();
  };
  reader.read();
  reader.context.run();
  child.join();
  EXPECT_EQ(child.exit_code(), 0);
  const std::string compare = test_lines + "\n";  // echo adds a newline
  EXPECT_EQ(result, compare);
}

namespace {
struct async_pipe_line_reader {
  boost::asio::io_context context;
  std::string buffer;
  boost::process::async_pipe async_pipe{context};
  std::function<void(const boost::system::error_code &error, std::size_t size)>
      callback;
  void read() {
    boost::asio::async_read_until(
        async_pipe, boost::asio::dynamic_buffer(buffer), '\n', callback);
  }
};

}  // namespace

static void read_lines_count(async_pipe_line_reader &reader, const int count) {
  int call_count{};
  reader.callback = [&](const boost::system::error_code &error,
                        std::size_t size) {
    const auto line = reader.buffer.substr(0, size);
    reader.buffer.erase(0, size);
    if (!line.empty()) {
      EXPECT_TRUE(line.find("line") != std::string::npos);
      ++call_count;
    }
    if (error.value() == boost::asio::error::misc_errors::eof) return;
    EXPECT_FALSE(error);
    reader.read();
  };
  reader.read();
  reader.context.run();
  EXPECT_EQ(call_count, count);
}

static void read_three_lines(async_pipe_line_reader &reader) {
  read_lines_count(reader, 3);
}

TEST(BoostProcess, AsyncReadByLine) {
  async_pipe_line_reader reader;
  boost::process::child child(boost::process::search_path("echo"), test_lines,
                              boost::process::std_out > reader.async_pipe);
  read_three_lines(reader);
  child.join();
  EXPECT_EQ(child.exit_code(), 0);
}

TEST(BoostProcess, AsyncReadByLineTwice) {
  async_pipe_line_reader reader;
  {
    boost::process::child child(boost::process::search_path("echo"), test_lines,
                                boost::process::std_out > reader.async_pipe);
    read_three_lines(reader);
    child.join();
  }
  reader.async_pipe = boost::process::async_pipe(reader.context);
  reader.context.reset();
  boost::process::child child(boost::process::search_path("echo"), test_lines,
                              boost::process::std_out > reader.async_pipe);
  read_three_lines(reader);
  child.join();
}

TEST(BoostProcess, AsyncReadByLineCoutCerr) {
  async_pipe_line_reader reader_out;
  async_pipe_line_reader reader_err;
  boost::process::child child(boost::process::search_path("bash"), "-c",
                              test_line_out_and_err,
                              boost::process::std_out > reader_out.async_pipe,
                              boost::process::std_err > reader_err.async_pipe);
  read_lines_count(reader_out, 2);
  read_lines_count(reader_err, 1);
}

TEST(BoostProcess, AsyncReadByLineCoutCerrRedirect) {
  async_pipe_line_reader reader;
  boost::process::child child(
      boost::process::search_path("bash"), "-c", test_line_out_and_err,
      (boost::process::std_out & boost::process::std_err) > reader.async_pipe);
  read_lines_count(reader, 3);
  child.join();
}

TEST(BoostProcess, CloseAfterFinished) {
  async_pipe_line_reader reader;
  boost::process::child child(boost::process::search_path("echo"), test_lines,
                              boost::process::std_out > reader.async_pipe);
  child.join();
  reader.async_pipe.close();
}
