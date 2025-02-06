#include "styles.h"
#include "images.h"
#include "fonts.h"

#include "screens.h"

//
// Style: startTopBarLabel
//

void init_style_start_top_bar_label_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_align(style, LV_TEXT_ALIGN_CENTER);
    lv_style_set_text_font(style, &lv_font_montserrat_36);
    lv_style_set_text_color(style, lv_color_hex(0xffffffff));
    lv_style_set_text_opa(style, 255);
};

lv_style_t *get_style_start_top_bar_label_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_start_top_bar_label_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_start_top_bar_label(lv_obj_t *obj) {
    lv_obj_add_style(obj, get_style_start_top_bar_label_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_start_top_bar_label(lv_obj_t *obj) {
    lv_obj_remove_style(obj, get_style_start_top_bar_label_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: settingsScreenButton
//

void init_style_settings_screen_button_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_opa(style, 255);
    lv_style_set_bg_color(style, lv_color_hex(0xffc7c7c7));
    lv_style_set_radius(style, 5);
};

lv_style_t *get_style_settings_screen_button_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_settings_screen_button_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_settings_screen_button(lv_obj_t *obj) {
    lv_obj_add_style(obj, get_style_settings_screen_button_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_settings_screen_button(lv_obj_t *obj) {
    lv_obj_remove_style(obj, get_style_settings_screen_button_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: settingsScreenButtonWithContent
//

void init_style_settings_screen_button_with_content_MAIN_DEFAULT(lv_style_t *style) {
    init_style_settings_screen_button_MAIN_DEFAULT(style);
    
    lv_style_set_layout(style, LV_LAYOUT_FLEX);
    lv_style_set_flex_main_place(style, LV_FLEX_ALIGN_CENTER);
    lv_style_set_flex_track_place(style, LV_FLEX_ALIGN_CENTER);
};

lv_style_t *get_style_settings_screen_button_with_content_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_settings_screen_button_with_content_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_settings_screen_button_with_content(lv_obj_t *obj) {
    lv_obj_add_style(obj, get_style_settings_screen_button_with_content_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_settings_screen_button_with_content(lv_obj_t *obj) {
    lv_obj_remove_style(obj, get_style_settings_screen_button_with_content_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: settingsTopBarContainer
//

void init_style_settings_top_bar_container_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_flex_flow(style, LV_FLEX_FLOW_ROW);
    lv_style_set_flex_main_place(style, LV_FLEX_ALIGN_SPACE_BETWEEN);
    lv_style_set_flex_cross_place(style, LV_FLEX_ALIGN_CENTER);
    lv_style_set_bg_color(style, lv_color_hex(0xff929292));
    lv_style_set_layout(style, LV_LAYOUT_FLEX);
    lv_style_set_bg_opa(style, 255);
};

lv_style_t *get_style_settings_top_bar_container_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_settings_top_bar_container_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_settings_top_bar_container(lv_obj_t *obj) {
    lv_obj_add_style(obj, get_style_settings_top_bar_container_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_settings_top_bar_container(lv_obj_t *obj) {
    lv_obj_remove_style(obj, get_style_settings_top_bar_container_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: settingsTopBarBackButton
//

void init_style_settings_top_bar_back_button_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_opa(style, 0);
    lv_style_set_bg_img_src(style, &img_arrow_left);
    lv_style_set_shadow_width(style, 0);
    lv_style_set_shadow_ofs_y(style, 0);
};

lv_style_t *get_style_settings_top_bar_back_button_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_settings_top_bar_back_button_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_settings_top_bar_back_button(lv_obj_t *obj) {
    lv_obj_add_style(obj, get_style_settings_top_bar_back_button_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_settings_top_bar_back_button(lv_obj_t *obj) {
    lv_obj_remove_style(obj, get_style_settings_top_bar_back_button_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: DefaultScreen
//

void init_style_default_screen_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(0xffffffff));
    lv_style_set_layout(style, LV_LAYOUT_FLEX);
    lv_style_set_flex_flow(style, LV_FLEX_FLOW_COLUMN);
    lv_style_set_flex_main_place(style, LV_FLEX_ALIGN_START);
    lv_style_set_pad_row(style, 0);
    lv_style_set_pad_column(style, 0);
};

lv_style_t *get_style_default_screen_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_default_screen_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_default_screen(lv_obj_t *obj) {
    lv_obj_add_style(obj, get_style_default_screen_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_default_screen(lv_obj_t *obj) {
    lv_obj_remove_style(obj, get_style_default_screen_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: settingsBarTopLabel
//

void init_style_settings_bar_top_label_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(0xffffffff));
    lv_style_set_text_opa(style, 255);
    lv_style_set_text_font(style, &lv_font_montserrat_32);
};

lv_style_t *get_style_settings_bar_top_label_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_settings_bar_top_label_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_settings_bar_top_label(lv_obj_t *obj) {
    lv_obj_add_style(obj, get_style_settings_bar_top_label_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_settings_bar_top_label(lv_obj_t *obj) {
    lv_obj_remove_style(obj, get_style_settings_bar_top_label_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: startValueGreyLabel
//

void init_style_start_value_grey_label_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_font(style, &lv_font_montserrat_30);
};

lv_style_t *get_style_start_value_grey_label_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_start_value_grey_label_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_start_value_grey_label(lv_obj_t *obj) {
    lv_obj_add_style(obj, get_style_start_value_grey_label_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_start_value_grey_label(lv_obj_t *obj) {
    lv_obj_remove_style(obj, get_style_start_value_grey_label_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: startValueGreenLabel
//

void init_style_start_value_green_label_MAIN_DEFAULT(lv_style_t *style) {
    init_style_start_value_grey_label_MAIN_DEFAULT(style);
    
    lv_style_set_text_color(style, lv_color_hex(0xff19d42d));
};

lv_style_t *get_style_start_value_green_label_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_start_value_green_label_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_start_value_green_label(lv_obj_t *obj) {
    lv_obj_add_style(obj, get_style_start_value_green_label_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_start_value_green_label(lv_obj_t *obj) {
    lv_obj_remove_style(obj, get_style_start_value_green_label_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: startValueThirdContainer
//

void init_style_start_value_third_container_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_layout(style, LV_LAYOUT_FLEX);
    lv_style_set_flex_flow(style, LV_FLEX_FLOW_COLUMN);
    lv_style_set_flex_track_place(style, LV_FLEX_ALIGN_SPACE_BETWEEN);
    lv_style_set_flex_main_place(style, LV_FLEX_ALIGN_CENTER);
    lv_style_set_flex_cross_place(style, LV_FLEX_ALIGN_CENTER);
    lv_style_set_pad_row(style, 30);
};

lv_style_t *get_style_start_value_third_container_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_start_value_third_container_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_start_value_third_container(lv_obj_t *obj) {
    lv_obj_add_style(obj, get_style_start_value_third_container_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_start_value_third_container(lv_obj_t *obj) {
    lv_obj_remove_style(obj, get_style_start_value_third_container_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: startTopBarContainer
//

void init_style_start_top_bar_container_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(0xff19d42d));
    lv_style_set_layout(style, LV_LAYOUT_FLEX);
    lv_style_set_flex_flow(style, LV_FLEX_FLOW_ROW);
    lv_style_set_flex_main_place(style, LV_FLEX_ALIGN_SPACE_BETWEEN);
    lv_style_set_flex_cross_place(style, LV_FLEX_ALIGN_CENTER);
    lv_style_set_flex_track_place(style, LV_FLEX_ALIGN_SPACE_BETWEEN);
    lv_style_set_bg_opa(style, 255);
};

lv_style_t *get_style_start_top_bar_container_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_start_top_bar_container_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_start_top_bar_container(lv_obj_t *obj) {
    lv_obj_add_style(obj, get_style_start_top_bar_container_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_start_top_bar_container(lv_obj_t *obj) {
    lv_obj_remove_style(obj, get_style_start_top_bar_container_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: startBottomBarContainer
//

void init_style_start_bottom_bar_container_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_align(style, LV_ALIGN_BOTTOM_LEFT);
    lv_style_set_bg_color(style, lv_color_hex(0xffd1d1d1));
    lv_style_set_layout(style, LV_LAYOUT_FLEX);
    lv_style_set_flex_flow(style, LV_FLEX_FLOW_ROW);
    lv_style_set_flex_main_place(style, LV_FLEX_ALIGN_SPACE_BETWEEN);
    lv_style_set_flex_cross_place(style, LV_FLEX_ALIGN_CENTER);
    lv_style_set_flex_track_place(style, LV_FLEX_ALIGN_SPACE_BETWEEN);
    lv_style_set_bg_opa(style, 255);
};

lv_style_t *get_style_start_bottom_bar_container_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_start_bottom_bar_container_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_start_bottom_bar_container(lv_obj_t *obj) {
    lv_obj_add_style(obj, get_style_start_bottom_bar_container_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_start_bottom_bar_container(lv_obj_t *obj) {
    lv_obj_remove_style(obj, get_style_start_bottom_bar_container_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: startTopBarButtonLeft
//

void init_style_start_top_bar_button_left_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_img_src(style, &img_arrow_left);
    lv_style_set_shadow_width(style, 0);
    lv_style_set_shadow_ofs_y(style, 0);
    lv_style_set_bg_opa(style, 0);
};

lv_style_t *get_style_start_top_bar_button_left_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_start_top_bar_button_left_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_start_top_bar_button_left(lv_obj_t *obj) {
    lv_obj_add_style(obj, get_style_start_top_bar_button_left_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_start_top_bar_button_left(lv_obj_t *obj) {
    lv_obj_remove_style(obj, get_style_start_top_bar_button_left_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: startTopBarButtonRight
//

void init_style_start_top_bar_button_right_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_align(style, LV_ALIGN_TOP_RIGHT);
    lv_style_set_bg_img_src(style, &img_arrow_right);
    lv_style_set_bg_opa(style, 0);
    lv_style_set_bg_color(style, lv_color_hex(0xff2196f3));
    lv_style_set_shadow_width(style, 0);
    lv_style_set_shadow_ofs_y(style, 0);
};

lv_style_t *get_style_start_top_bar_button_right_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_start_top_bar_button_right_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_start_top_bar_button_right(lv_obj_t *obj) {
    lv_obj_add_style(obj, get_style_start_top_bar_button_right_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_start_top_bar_button_right(lv_obj_t *obj) {
    lv_obj_remove_style(obj, get_style_start_top_bar_button_right_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: startEvaluationBar
//

void init_style_start_evaluation_bar_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(0xff016823));
    lv_style_set_bg_grad_dir(style, LV_GRAD_DIR_HOR);
    lv_style_set_bg_grad_color(style, lv_color_hex(0xffff0000));
    lv_style_set_radius(style, 0);
    lv_style_set_line_rounded(style, false);
    lv_style_set_line_color(style, lv_color_hex(0xff5a2424));
    lv_style_set_line_opa(style, 255);
    lv_style_set_bg_opa(style, 255);
};

lv_style_t *get_style_start_evaluation_bar_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_start_evaluation_bar_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_start_evaluation_bar_INDICATOR_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(0xff05ad43));
    lv_style_set_bg_grad_dir(style, LV_GRAD_DIR_HOR);
    lv_style_set_bg_grad_color(style, lv_color_hex(0xffff0000));
    lv_style_set_radius(style, 0);
};

lv_style_t *get_style_start_evaluation_bar_INDICATOR_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_start_evaluation_bar_INDICATOR_DEFAULT(style);
    }
    return style;
};

void add_style_start_evaluation_bar(lv_obj_t *obj) {
    lv_obj_add_style(obj, get_style_start_evaluation_bar_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_start_evaluation_bar_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
};

void remove_style_start_evaluation_bar(lv_obj_t *obj) {
    lv_obj_remove_style(obj, get_style_start_evaluation_bar_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_start_evaluation_bar_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
};

//
// Style: startBottomBarImageContainer
//

void init_style_start_bottom_bar_image_container_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_layout(style, LV_LAYOUT_FLEX);
    lv_style_set_flex_flow(style, LV_FLEX_FLOW_ROW);
    lv_style_set_flex_main_place(style, LV_FLEX_ALIGN_START);
    lv_style_set_flex_track_place(style, LV_FLEX_ALIGN_SPACE_BETWEEN);
    lv_style_set_pad_left(style, 7);
};

lv_style_t *get_style_start_bottom_bar_image_container_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_start_bottom_bar_image_container_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_start_bottom_bar_image_container(lv_obj_t *obj) {
    lv_obj_add_style(obj, get_style_start_bottom_bar_image_container_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_start_bottom_bar_image_container(lv_obj_t *obj) {
    lv_obj_remove_style(obj, get_style_start_bottom_bar_image_container_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: startBottomBarDateTimeLabel
//

void init_style_start_bottom_bar_date_time_label_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(0xff6a6a6a));
};

lv_style_t *get_style_start_bottom_bar_date_time_label_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_start_bottom_bar_date_time_label_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_start_bottom_bar_date_time_label(lv_obj_t *obj) {
    lv_obj_add_style(obj, get_style_start_bottom_bar_date_time_label_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_start_bottom_bar_date_time_label(lv_obj_t *obj) {
    lv_obj_remove_style(obj, get_style_start_bottom_bar_date_time_label_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: startBottomBarSettingsButton
//

void init_style_start_bottom_bar_settings_button_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_opa(style, 0);
    lv_style_set_bg_img_src(style, &img_settings_cog);
    lv_style_set_shadow_width(style, 0);
    lv_style_set_shadow_ofs_y(style, 0);
};

lv_style_t *get_style_start_bottom_bar_settings_button_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_start_bottom_bar_settings_button_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_start_bottom_bar_settings_button(lv_obj_t *obj) {
    lv_obj_add_style(obj, get_style_start_bottom_bar_settings_button_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_start_bottom_bar_settings_button(lv_obj_t *obj) {
    lv_obj_remove_style(obj, get_style_start_bottom_bar_settings_button_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: startValueBigGreenLabel
//

void init_style_start_value_big_green_label_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_font(style, &lv_font_montserrat_48);
    lv_style_set_text_opa(style, 255);
    lv_style_set_text_color(style, lv_color_hex(0xff19d42d));
};

lv_style_t *get_style_start_value_big_green_label_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_start_value_big_green_label_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_start_value_big_green_label(lv_obj_t *obj) {
    lv_obj_add_style(obj, get_style_start_value_big_green_label_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_start_value_big_green_label(lv_obj_t *obj) {
    lv_obj_remove_style(obj, get_style_start_value_big_green_label_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: startValueUnitLabel
//

void init_style_start_value_unit_label_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_font(style, &lv_font_montserrat_38);
};

lv_style_t *get_style_start_value_unit_label_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_start_value_unit_label_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_start_value_unit_label(lv_obj_t *obj) {
    lv_obj_add_style(obj, get_style_start_value_unit_label_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_start_value_unit_label(lv_obj_t *obj) {
    lv_obj_remove_style(obj, get_style_start_value_unit_label_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: settingsButtonText
//

void init_style_settings_button_text_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_align(style, LV_ALIGN_CENTER);
    lv_style_set_text_font(style, &lv_font_montserrat_26);
    lv_style_set_text_align(style, LV_TEXT_ALIGN_CENTER);
};

lv_style_t *get_style_settings_button_text_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_settings_button_text_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_settings_button_text(lv_obj_t *obj) {
    lv_obj_add_style(obj, get_style_settings_button_text_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_settings_button_text(lv_obj_t *obj) {
    lv_obj_remove_style(obj, get_style_settings_button_text_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: optionsCheckBox
//

void init_style_options_check_box_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_font(style, &lv_font_montserrat_26);
};

lv_style_t *get_style_options_check_box_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_options_check_box_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_options_check_box_INDICATOR_DEFAULT(lv_style_t *style) {
    lv_style_set_border_color(style, lv_color_hex(0xff9e9e9e));
};

lv_style_t *get_style_options_check_box_INDICATOR_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_options_check_box_INDICATOR_DEFAULT(style);
    }
    return style;
};

void add_style_options_check_box(lv_obj_t *obj) {
    lv_obj_add_style(obj, get_style_options_check_box_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_options_check_box_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
};

void remove_style_options_check_box(lv_obj_t *obj) {
    lv_obj_remove_style(obj, get_style_options_check_box_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_options_check_box_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
};

//
// Style: defaultTab
//

void init_style_default_tab_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_layout(style, LV_LAYOUT_FLEX);
    lv_style_set_bg_opa(style, 255);
    lv_style_set_flex_flow(style, LV_FLEX_FLOW_COLUMN);
    lv_style_set_pad_row(style, 0);
};

lv_style_t *get_style_default_tab_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_default_tab_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_default_tab(lv_obj_t *obj) {
    lv_obj_add_style(obj, get_style_default_tab_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_default_tab(lv_obj_t *obj) {
    lv_obj_remove_style(obj, get_style_default_tab_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
//
//

void add_style(lv_obj_t *obj, int32_t styleIndex) {
    typedef void (*AddStyleFunc)(lv_obj_t *obj);
    static const AddStyleFunc add_style_funcs[] = {
        add_style_start_top_bar_label,
        add_style_settings_screen_button,
        add_style_settings_screen_button_with_content,
        add_style_settings_top_bar_container,
        add_style_settings_top_bar_back_button,
        add_style_default_screen,
        add_style_settings_bar_top_label,
        add_style_start_value_grey_label,
        add_style_start_value_green_label,
        add_style_start_value_third_container,
        add_style_start_top_bar_container,
        add_style_start_bottom_bar_container,
        add_style_start_top_bar_button_left,
        add_style_start_top_bar_button_right,
        add_style_start_evaluation_bar,
        add_style_start_bottom_bar_image_container,
        add_style_start_bottom_bar_date_time_label,
        add_style_start_bottom_bar_settings_button,
        add_style_start_value_big_green_label,
        add_style_start_value_unit_label,
        add_style_settings_button_text,
        add_style_options_check_box,
        add_style_default_tab,
    };
    add_style_funcs[styleIndex](obj);
}

void remove_style(lv_obj_t *obj, int32_t styleIndex) {
    typedef void (*RemoveStyleFunc)(lv_obj_t *obj);
    static const RemoveStyleFunc remove_style_funcs[] = {
        remove_style_start_top_bar_label,
        remove_style_settings_screen_button,
        remove_style_settings_screen_button_with_content,
        remove_style_settings_top_bar_container,
        remove_style_settings_top_bar_back_button,
        remove_style_default_screen,
        remove_style_settings_bar_top_label,
        remove_style_start_value_grey_label,
        remove_style_start_value_green_label,
        remove_style_start_value_third_container,
        remove_style_start_top_bar_container,
        remove_style_start_bottom_bar_container,
        remove_style_start_top_bar_button_left,
        remove_style_start_top_bar_button_right,
        remove_style_start_evaluation_bar,
        remove_style_start_bottom_bar_image_container,
        remove_style_start_bottom_bar_date_time_label,
        remove_style_start_bottom_bar_settings_button,
        remove_style_start_value_big_green_label,
        remove_style_start_value_unit_label,
        remove_style_settings_button_text,
        remove_style_options_check_box,
        remove_style_default_tab,
    };
    remove_style_funcs[styleIndex](obj);
}

