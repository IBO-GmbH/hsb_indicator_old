add to `mold.cpp`

fake value
```cpp
boost::asio::steady_timer debug_timer{service};
debug_timer.expires_from_now(std::chrono::seconds(5));
debug_timer.async_wait([&](boost::system::error_code) {
  std::vector<sensor_value> values;
  auto sensor = sensor_id_enocean::extract_id(learned->get_all()[0].id);
  values.push_back({sensor_id_enocean::create(sensor, S_TEMP), 37.5f});
  values.push_back({sensor_id_enocean::create(sensor, S_RELHUM), 50.f});
  value_handler_->handle_list(values);
});
```

fake raw enocean values, inclusive learn (temp/hum)
```cpp
boost::asio::steady_timer debug_timer{service};
debug_timer.expires_from_now(std::chrono::seconds(5));
debug_timer.async_wait([&](boost::system::error_code) {
  serial_interface::read_buffer buffer;
  const types::data data = {0x55, 0x00, 0x0a, 0x07, 0x01, 0xeb, 0xa5, 0x10,
                            0x08, 0x02, 0x80, 0x01, 0x90, 0x9b, 0xdc, 0x00,
                            0x01, 0xff, 0xff, 0xff, 0xff, 0x28, 0x00, 0x9f,
                            0x55, 0x00, 0x0a, 0x07, 0x01, 0xeb, 0xa5, 0x00,
                            0x8e, 0x8f, 0x0a, 0x01, 0x90, 0x9b, 0xdc, 0x00,
                            0x01, 0xff, 0xff, 0xff, 0xff, 0x28, 0x00, 0xf7};
  assert(data.size() <= buffer.size());
  std::copy(data.cbegin(), data.cend(), buffer.begin());
  parser_serial->handle_data(buffer, data.size());
});
```
