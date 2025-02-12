#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl/lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _objects_t {
    lv_obj_t *home;
    lv_obj_t *home_co2;
    lv_obj_t *home_air_pressure;
    lv_obj_t *home_heating_valve_settings;
    lv_obj_t *heating_valve_settings_learn_valve;
    lv_obj_t *settings;
    lv_obj_t *set_pin;
    lv_obj_t *settings_rooms;
    lv_obj_t *rooms_room;
    lv_obj_t *room_sensor_offset;
    lv_obj_t *room_control;
    lv_obj_t *control_ventilation;
    lv_obj_t *ventilation_actors;
    lv_obj_t *ventilation_limits;
    lv_obj_t *control_dehumidifier;
    lv_obj_t *dehumidifier_actors;
    lv_obj_t *control_heating;
    lv_obj_t *heating_actors;
    lv_obj_t *control_air_quality;
    lv_obj_t *air_quality_actors;
    lv_obj_t *rooms_sensor;
    lv_obj_t *outdoor_sensor;
    lv_obj_t *wifi;
    lv_obj_t *wifi_connect;
    lv_obj_t *language;
    lv_obj_t *options;
    lv_obj_t *options_display;
    lv_obj_t *display_options_questionnaire;
    lv_obj_t *options_building_value;
    lv_obj_t *building_value_per_room;
    lv_obj_t *per_room_room_type;
    lv_obj_t *options_reset_options;
    lv_obj_t *reset_options_values;
    lv_obj_t *reset_options_config;
    lv_obj_t *options_shutdown;
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
    lv_obj_t *obj24;
    lv_obj_t *obj25;
    lv_obj_t *obj26;
    lv_obj_t *obj27;
    lv_obj_t *obj28;
    lv_obj_t *obj29;
    lv_obj_t *obj30;
    lv_obj_t *obj31;
    lv_obj_t *obj32;
    lv_obj_t *obj33;
    lv_obj_t *obj34;
    lv_obj_t *obj35;
    lv_obj_t *obj36;
    lv_obj_t *obj37;
    lv_obj_t *obj38;
    lv_obj_t *obj39;
    lv_obj_t *obj40;
    lv_obj_t *obj41;
    lv_obj_t *obj42;
    lv_obj_t *obj43;
    lv_obj_t *obj44;
    lv_obj_t *obj45;
    lv_obj_t *obj46;
    lv_obj_t *obj47;
    lv_obj_t *obj48;
    lv_obj_t *obj49;
    lv_obj_t *obj50;
    lv_obj_t *obj51;
    lv_obj_t *obj52;
    lv_obj_t *obj53;
    lv_obj_t *obj54;
    lv_obj_t *obj55;
    lv_obj_t *obj56;
    lv_obj_t *obj57;
    lv_obj_t *obj58;
    lv_obj_t *obj59;
    lv_obj_t *obj60;
    lv_obj_t *obj61;
    lv_obj_t *obj62;
    lv_obj_t *obj63;
    lv_obj_t *obj64;
    lv_obj_t *obj65;
    lv_obj_t *obj66;
    lv_obj_t *obj67;
    lv_obj_t *obj68;
    lv_obj_t *obj69;
    lv_obj_t *obj70;
    lv_obj_t *obj71;
    lv_obj_t *obj72;
    lv_obj_t *obj73;
    lv_obj_t *obj74;
    lv_obj_t *obj75;
    lv_obj_t *obj76;
    lv_obj_t *obj77;
    lv_obj_t *obj78;
    lv_obj_t *obj79;
    lv_obj_t *obj80;
    lv_obj_t *obj81;
    lv_obj_t *obj82;
    lv_obj_t *obj83;
    lv_obj_t *obj84;
    lv_obj_t *obj85;
    lv_obj_t *obj86;
    lv_obj_t *obj87;
    lv_obj_t *obj88;
    lv_obj_t *obj89;
    lv_obj_t *obj90;
    lv_obj_t *obj91;
    lv_obj_t *obj92;
    lv_obj_t *obj93;
    lv_obj_t *obj94;
    lv_obj_t *obj95;
    lv_obj_t *obj96;
    lv_obj_t *obj97;
    lv_obj_t *obj98;
    lv_obj_t *obj99;
    lv_obj_t *obj100;
    lv_obj_t *obj101;
    lv_obj_t *obj102;
    lv_obj_t *obj103;
    lv_obj_t *obj104;
    lv_obj_t *obj105;
    lv_obj_t *obj106;
    lv_obj_t *container_top_bar;
    lv_obj_t *container_bottom_bar;
    lv_obj_t *obj107;
    lv_obj_t *obj108;
    lv_obj_t *obj109;
    lv_obj_t *obj110;
    lv_obj_t *obj111;
    lv_obj_t *pin_text_area;
    lv_obj_t *obj112;
    lv_obj_t *obj113;
    lv_obj_t *obj114;
    lv_obj_t *obj115;
    lv_obj_t *obj116;
    lv_obj_t *obj117;
    lv_obj_t *obj118;
    lv_obj_t *obj119;
    lv_obj_t *obj120;
    lv_obj_t *wifi_password;
    lv_obj_t *obj121;
    lv_obj_t *obj122;
    lv_obj_t *obj123;
    lv_obj_t *obj124;
    lv_obj_t *obj125;
    lv_obj_t *obj126;
    lv_obj_t *obj127;
} objects_t;

extern objects_t objects;

enum ScreensEnum {
    SCREEN_ID_HOME = 1,
    SCREEN_ID_HOME_CO2 = 2,
    SCREEN_ID_HOME_AIR_PRESSURE = 3,
    SCREEN_ID_HOME_HEATING_VALVE_SETTINGS = 4,
    SCREEN_ID_HEATING_VALVE_SETTINGS_LEARN_VALVE = 5,
    SCREEN_ID_SETTINGS = 6,
    SCREEN_ID_SET_PIN = 7,
    SCREEN_ID_SETTINGS_ROOMS = 8,
    SCREEN_ID_ROOMS_ROOM = 9,
    SCREEN_ID_ROOM_SENSOR_OFFSET = 10,
    SCREEN_ID_ROOM_CONTROL = 11,
    SCREEN_ID_CONTROL_VENTILATION = 12,
    SCREEN_ID_VENTILATION_ACTORS = 13,
    SCREEN_ID_VENTILATION_LIMITS = 14,
    SCREEN_ID_CONTROL_DEHUMIDIFIER = 15,
    SCREEN_ID_DEHUMIDIFIER_ACTORS = 16,
    SCREEN_ID_CONTROL_HEATING = 17,
    SCREEN_ID_HEATING_ACTORS = 18,
    SCREEN_ID_CONTROL_AIR_QUALITY = 19,
    SCREEN_ID_AIR_QUALITY_ACTORS = 20,
    SCREEN_ID_ROOMS_SENSOR = 21,
    SCREEN_ID_OUTDOOR_SENSOR = 22,
    SCREEN_ID_WIFI = 23,
    SCREEN_ID_WIFI_CONNECT = 24,
    SCREEN_ID_LANGUAGE = 25,
    SCREEN_ID_OPTIONS = 26,
    SCREEN_ID_OPTIONS_DISPLAY = 27,
    SCREEN_ID_DISPLAY_OPTIONS_QUESTIONNAIRE = 28,
    SCREEN_ID_OPTIONS_BUILDING_VALUE = 29,
    SCREEN_ID_BUILDING_VALUE_PER_ROOM = 30,
    SCREEN_ID_PER_ROOM_ROOM_TYPE = 31,
    SCREEN_ID_OPTIONS_RESET_OPTIONS = 32,
    SCREEN_ID_RESET_OPTIONS_VALUES = 33,
    SCREEN_ID_RESET_OPTIONS_CONFIG = 34,
    SCREEN_ID_OPTIONS_SHUTDOWN = 35,
    SCREEN_ID_SENSOR_ACTIVITY = 36,
};

void create_screen_home();
void tick_screen_home();

void create_screen_home_co2();
void tick_screen_home_co2();

void create_screen_home_air_pressure();
void tick_screen_home_air_pressure();

void create_screen_home_heating_valve_settings();
void tick_screen_home_heating_valve_settings();

void create_screen_heating_valve_settings_learn_valve();
void tick_screen_heating_valve_settings_learn_valve();

void create_screen_settings();
void tick_screen_settings();

void create_screen_set_pin();
void tick_screen_set_pin();

void create_screen_settings_rooms();
void tick_screen_settings_rooms();

void create_screen_rooms_room();
void tick_screen_rooms_room();

void create_screen_room_sensor_offset();
void tick_screen_room_sensor_offset();

void create_screen_room_control();
void tick_screen_room_control();

void create_screen_control_ventilation();
void tick_screen_control_ventilation();

void create_screen_ventilation_actors();
void tick_screen_ventilation_actors();

void create_screen_ventilation_limits();
void tick_screen_ventilation_limits();

void create_screen_control_dehumidifier();
void tick_screen_control_dehumidifier();

void create_screen_dehumidifier_actors();
void tick_screen_dehumidifier_actors();

void create_screen_control_heating();
void tick_screen_control_heating();

void create_screen_heating_actors();
void tick_screen_heating_actors();

void create_screen_control_air_quality();
void tick_screen_control_air_quality();

void create_screen_air_quality_actors();
void tick_screen_air_quality_actors();

void create_screen_rooms_sensor();
void tick_screen_rooms_sensor();

void create_screen_outdoor_sensor();
void tick_screen_outdoor_sensor();

void create_screen_wifi();
void tick_screen_wifi();

void create_screen_wifi_connect();
void tick_screen_wifi_connect();

void create_screen_language();
void tick_screen_language();

void create_screen_options();
void tick_screen_options();

void create_screen_options_display();
void tick_screen_options_display();

void create_screen_display_options_questionnaire();
void tick_screen_display_options_questionnaire();

void create_screen_options_building_value();
void tick_screen_options_building_value();

void create_screen_building_value_per_room();
void tick_screen_building_value_per_room();

void create_screen_per_room_room_type();
void tick_screen_per_room_room_type();

void create_screen_options_reset_options();
void tick_screen_options_reset_options();

void create_screen_reset_options_values();
void tick_screen_reset_options_values();

void create_screen_reset_options_config();
void tick_screen_reset_options_config();

void create_screen_options_shutdown();
void tick_screen_options_shutdown();

void create_screen_sensor_activity();
void tick_screen_sensor_activity();

void create_screens();
void tick_screen(int screen_index);


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/