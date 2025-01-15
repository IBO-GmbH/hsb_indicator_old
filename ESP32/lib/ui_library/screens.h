#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl/lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _objects_t {
    lv_obj_t *start;
    lv_obj_t *start_co2;
    lv_obj_t *start_air_pressure;
    lv_obj_t *settings;
    lv_obj_t *set_pin;
    lv_obj_t *rooms;
    lv_obj_t *outdoor_sensor;
    lv_obj_t *wifi;
    lv_obj_t *language;
    lv_obj_t *options;
    lv_obj_t *sensor_activity;
    lv_obj_t *obj0;
    lv_obj_t *obj1;
    lv_obj_t *obj2;
    lv_obj_t *obj3;
    lv_obj_t *obj4;
    lv_obj_t *obj5;
    lv_obj_t *obj6;
    lv_obj_t *obj7;
    lv_obj_t *obj8;
    lv_obj_t *obj9;
    lv_obj_t *obj10;
    lv_obj_t *obj11;
    lv_obj_t *obj12;
    lv_obj_t *obj13;
    lv_obj_t *obj14;
    lv_obj_t *obj15;
    lv_obj_t *obj16;
    lv_obj_t *obj17;
    lv_obj_t *obj18;
    lv_obj_t *obj19;
    lv_obj_t *obj20;
    lv_obj_t *obj21;
    lv_obj_t *obj22;
    lv_obj_t *obj23;
    lv_obj_t *container_top_bar;
    lv_obj_t *container_bottom_bar;
    lv_obj_t *obj24;
    lv_obj_t *obj25;
    lv_obj_t *pin_text_area;
    lv_obj_t *obj26;
} objects_t;

extern objects_t objects;

enum ScreensEnum {
    SCREEN_ID_START = 1,
    SCREEN_ID_START_CO2 = 2,
    SCREEN_ID_START_AIR_PRESSURE = 3,
    SCREEN_ID_SETTINGS = 4,
    SCREEN_ID_SET_PIN = 5,
    SCREEN_ID_ROOMS = 6,
    SCREEN_ID_OUTDOOR_SENSOR = 7,
    SCREEN_ID_WIFI = 8,
    SCREEN_ID_LANGUAGE = 9,
    SCREEN_ID_OPTIONS = 10,
    SCREEN_ID_SENSOR_ACTIVITY = 11,
};

void create_screen_start();
void tick_screen_start();

void create_screen_start_co2();
void tick_screen_start_co2();

void create_screen_start_air_pressure();
void tick_screen_start_air_pressure();

void create_screen_settings();
void tick_screen_settings();

void create_screen_set_pin();
void tick_screen_set_pin();

void create_screen_rooms();
void tick_screen_rooms();

void create_screen_outdoor_sensor();
void tick_screen_outdoor_sensor();

void create_screen_wifi();
void tick_screen_wifi();

void create_screen_language();
void tick_screen_language();

void create_screen_options();
void tick_screen_options();

void create_screen_sensor_activity();
void tick_screen_sensor_activity();

void create_screens();
void tick_screen(int screen_index);


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/