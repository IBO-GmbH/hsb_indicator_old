#include <unity.h>
#include "sensor_value_reader.hpp"

#define SENSOR_ADDRESS 0x5E

unsigned int sensor_interval = 10;

unsigned long check_time;

unsigned int interval_1 = 20 * 1000;
unsigned int interval_2 = 42 * 1000;

bool done_1 = false;

unsigned int read_counter = 0;

sensor_value_reader value_reader;

// test if there were as much successfull reads as were expected
void test_read_counter(void)
{
    TEST_ASSERT_EQUAL(read_counter, 6);
}

// check if values were read from sensor (would be 0 if not)
void test_read_values(void)
{
    auto values = value_reader.get_sensor_values();
    for (unsigned int index = 0; index < values.size(); ++index)
    {
        TEST_ASSERT_TRUE(values[index] > 0.0);
        Serial.println(values[index], 2);
    }
    ++read_counter;
}

// change read interval after 20 seconds
void test_change_interval()
{
    // only do it once
    done_1 = true;
    value_reader.change_read_interval(5);
    TEST_ASSERT_TRUE(done_1);
}

void setup()
{
    // NOTE!!! Wait for >2 secs
    // if board doesn't support software reset via Serial.DTR/RTS
    delay(2000);

    UNITY_BEGIN(); // IMPORTANT LINE!

    value_reader.sensor_value_init();
    value_reader.change_read_interval(sensor_interval);
    check_time = millis();
}

void loop()
{
    auto current_time = millis();

    if(value_reader.handle_read(SENSOR_ADDRESS))
        RUN_TEST(test_read_values);

    if (((current_time - check_time) >= interval_1) && done_1 == false)
        RUN_TEST(test_change_interval);

    // stop test after 42 seconds
    if ((current_time - check_time) >= interval_2)
    {
        RUN_TEST(test_read_counter);
        UNITY_END();
    }
}