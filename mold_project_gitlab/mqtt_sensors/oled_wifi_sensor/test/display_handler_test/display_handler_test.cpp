#include <unity.h>
#include "display_handler.hpp"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1    // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C

unsigned long check_time;

unsigned int interval_1 = 10 * 1000;
unsigned int interval_2 = 20 * 1000;
unsigned int interval_3 = 30 * 1000;
unsigned int interval_4 = 35 * 1000;
unsigned int interval_5 = 45 * 1000;

bool done_1 = false;
bool done_2 = false;
bool done_3 = false;
bool done_4 = false;

display_handler display_handler_(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_RESET);

// void setUp(void) {
// // set stuff up here
// }

// void tearDown(void) {
// // clean stuff up here
// }

void test_init(void)
{
    TEST_ASSERT_TRUE(display_handler_.initialize_display(SCREEN_ADDRESS));
}

// these tests need to be done with the display, as the correct functionality can only
// be checked if by looking at what the display shows

// change values after 10 seconds
void test_change_values()
{
    // only do it once
    done_1 = true;
    display_handler_.set_sensor_values({20.5,1200,1000});
    TEST_ASSERT_TRUE(done_1);
}

// change rotation time and units after 20 seconds
void test_change_rotation_time()
{
    // only do it once
    done_2 = true;
    display_handler_.change_rotation_time(1);
    display_handler_.set_units({"F","ppm","bar"});
    TEST_ASSERT_TRUE(done_2);
}

// clear display after 30 seconds
void test_clear_display()
{
    // only do it once
    done_3 = true;
    display_handler_.empty_display();
    TEST_ASSERT_TRUE(done_3);
}

// start again after 35 seconds
void test_restart_rotation()
{
    // only do it once
    done_4 = true;
    display_handler_.start_rotation();
    TEST_ASSERT_TRUE(done_4);
}

void setup()
{
    // NOTE!!! Wait for >2 secs
    // if board doesn't support software reset via Serial.DTR/RTS
    delay(2000);

    UNITY_BEGIN(); // IMPORTANT LINE!

    RUN_TEST(test_init);

    display_handler_.empty_display();
    display_handler_.change_rotation_time(2);
    display_handler_.set_display_off_time(10);
    display_handler_.start_rotation();
    check_time = millis();
}

void loop()
{
    display_handler_.sensor_values_display_rotation();
    auto current_time = millis();

    if (((current_time - check_time) >= interval_1) && done_1 == false)
        RUN_TEST(test_change_values);
    if (((current_time - check_time) >= interval_2) && done_2 == false)
        RUN_TEST(test_change_rotation_time);
    if (((current_time - check_time) >= interval_3) && done_3 == false)
        RUN_TEST(test_clear_display);
    if (((current_time - check_time) >= interval_4) && done_4 == false)
        RUN_TEST(test_restart_rotation);

    // stop test after 45 seconds
    if ((current_time - check_time) >= interval_5)
    {
        UNITY_END();
    }
}