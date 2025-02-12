#ifndef EEZ_LVGL_UI_STYLES_H
#define EEZ_LVGL_UI_STYLES_H

#include <lvgl/lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

// Style: startTopBarLabel
lv_style_t *get_style_start_top_bar_label_MAIN_DEFAULT();
void add_style_start_top_bar_label(lv_obj_t *obj);
void remove_style_start_top_bar_label(lv_obj_t *obj);

// Style: settingsScreenButton
lv_style_t *get_style_settings_screen_button_MAIN_DEFAULT();
void add_style_settings_screen_button(lv_obj_t *obj);
void remove_style_settings_screen_button(lv_obj_t *obj);

// Style: settingsScreenButtonWithContent
lv_style_t *get_style_settings_screen_button_with_content_MAIN_DEFAULT();
void add_style_settings_screen_button_with_content(lv_obj_t *obj);
void remove_style_settings_screen_button_with_content(lv_obj_t *obj);

// Style: settingsTopBarContainer
lv_style_t *get_style_settings_top_bar_container_MAIN_DEFAULT();
void add_style_settings_top_bar_container(lv_obj_t *obj);
void remove_style_settings_top_bar_container(lv_obj_t *obj);

// Style: settingsTopBarBackButton
lv_style_t *get_style_settings_top_bar_back_button_MAIN_DEFAULT();
void add_style_settings_top_bar_back_button(lv_obj_t *obj);
void remove_style_settings_top_bar_back_button(lv_obj_t *obj);

// Style: DefaultScreen
lv_style_t *get_style_default_screen_MAIN_DEFAULT();
void add_style_default_screen(lv_obj_t *obj);
void remove_style_default_screen(lv_obj_t *obj);

// Style: settingsBarTopLabel
lv_style_t *get_style_settings_bar_top_label_MAIN_DEFAULT();
void add_style_settings_bar_top_label(lv_obj_t *obj);
void remove_style_settings_bar_top_label(lv_obj_t *obj);

// Style: startValueGreyLabel
lv_style_t *get_style_start_value_grey_label_MAIN_DEFAULT();
void add_style_start_value_grey_label(lv_obj_t *obj);
void remove_style_start_value_grey_label(lv_obj_t *obj);

// Style: startValueGreenLabel
lv_style_t *get_style_start_value_green_label_MAIN_DEFAULT();
void add_style_start_value_green_label(lv_obj_t *obj);
void remove_style_start_value_green_label(lv_obj_t *obj);

// Style: startValueThirdContainer
lv_style_t *get_style_start_value_third_container_MAIN_DEFAULT();
void add_style_start_value_third_container(lv_obj_t *obj);
void remove_style_start_value_third_container(lv_obj_t *obj);

// Style: startTopBarContainer
lv_style_t *get_style_start_top_bar_container_MAIN_DEFAULT();
void add_style_start_top_bar_container(lv_obj_t *obj);
void remove_style_start_top_bar_container(lv_obj_t *obj);

// Style: startBottomBarContainer
lv_style_t *get_style_start_bottom_bar_container_MAIN_DEFAULT();
void add_style_start_bottom_bar_container(lv_obj_t *obj);
void remove_style_start_bottom_bar_container(lv_obj_t *obj);

// Style: startTopBarButtonLeft
lv_style_t *get_style_start_top_bar_button_left_MAIN_DEFAULT();
void add_style_start_top_bar_button_left(lv_obj_t *obj);
void remove_style_start_top_bar_button_left(lv_obj_t *obj);

// Style: startTopBarButtonRight
lv_style_t *get_style_start_top_bar_button_right_MAIN_DEFAULT();
void add_style_start_top_bar_button_right(lv_obj_t *obj);
void remove_style_start_top_bar_button_right(lv_obj_t *obj);

// Style: startEvaluationBar
lv_style_t *get_style_start_evaluation_bar_MAIN_DEFAULT();
lv_style_t *get_style_start_evaluation_bar_INDICATOR_DEFAULT();
void add_style_start_evaluation_bar(lv_obj_t *obj);
void remove_style_start_evaluation_bar(lv_obj_t *obj);

// Style: startBottomBarImageContainer
lv_style_t *get_style_start_bottom_bar_image_container_MAIN_DEFAULT();
void add_style_start_bottom_bar_image_container(lv_obj_t *obj);
void remove_style_start_bottom_bar_image_container(lv_obj_t *obj);

// Style: startBottomBarDateTimeLabel
lv_style_t *get_style_start_bottom_bar_date_time_label_MAIN_DEFAULT();
void add_style_start_bottom_bar_date_time_label(lv_obj_t *obj);
void remove_style_start_bottom_bar_date_time_label(lv_obj_t *obj);

// Style: startBottomBarSettingsButton
lv_style_t *get_style_start_bottom_bar_settings_button_MAIN_DEFAULT();
void add_style_start_bottom_bar_settings_button(lv_obj_t *obj);
void remove_style_start_bottom_bar_settings_button(lv_obj_t *obj);

// Style: startValueBigGreenLabel
lv_style_t *get_style_start_value_big_green_label_MAIN_DEFAULT();
void add_style_start_value_big_green_label(lv_obj_t *obj);
void remove_style_start_value_big_green_label(lv_obj_t *obj);

// Style: startValueUnitLabel
lv_style_t *get_style_start_value_unit_label_MAIN_DEFAULT();
void add_style_start_value_unit_label(lv_obj_t *obj);
void remove_style_start_value_unit_label(lv_obj_t *obj);

// Style: settingsButtonTextLabel
lv_style_t *get_style_settings_button_text_label_MAIN_DEFAULT();
void add_style_settings_button_text_label(lv_obj_t *obj);
void remove_style_settings_button_text_label(lv_obj_t *obj);

// Style: optionsCheckBox
lv_style_t *get_style_options_check_box_MAIN_DEFAULT();
lv_style_t *get_style_options_check_box_INDICATOR_DEFAULT();
void add_style_options_check_box(lv_obj_t *obj);
void remove_style_options_check_box(lv_obj_t *obj);

// Style: defaultTab
lv_style_t *get_style_default_tab_MAIN_DEFAULT();
void add_style_default_tab(lv_obj_t *obj);
void remove_style_default_tab(lv_obj_t *obj);

// Style: regularTextLabel
lv_style_t *get_style_regular_text_label_MAIN_DEFAULT();
void add_style_regular_text_label(lv_obj_t *obj);
void remove_style_regular_text_label(lv_obj_t *obj);

// Style: whiteSettingsTextLabel
lv_style_t *get_style_white_settings_text_label_MAIN_DEFAULT();
void add_style_white_settings_text_label(lv_obj_t *obj);
void remove_style_white_settings_text_label(lv_obj_t *obj);

// Style: whiteInfoTextLabel
lv_style_t *get_style_white_info_text_label_MAIN_DEFAULT();
void add_style_white_info_text_label(lv_obj_t *obj);
void remove_style_white_info_text_label(lv_obj_t *obj);

// Style: blackInfoTextLabel
lv_style_t *get_style_black_info_text_label_MAIN_DEFAULT();
void add_style_black_info_text_label(lv_obj_t *obj);
void remove_style_black_info_text_label(lv_obj_t *obj);

// Style: regularTextSmallLabel
lv_style_t *get_style_regular_text_small_label_MAIN_DEFAULT();
void add_style_regular_text_small_label(lv_obj_t *obj);
void remove_style_regular_text_small_label(lv_obj_t *obj);



#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_STYLES_H*/