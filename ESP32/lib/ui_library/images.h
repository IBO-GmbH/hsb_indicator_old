#ifndef EEZ_LVGL_UI_IMAGES_H
#define EEZ_LVGL_UI_IMAGES_H

#include <lvgl/lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern const lv_img_dsc_t img_arrow_right;
extern const lv_img_dsc_t img_arrow_left;
extern const lv_img_dsc_t img_settings_cog;
extern const lv_img_dsc_t img_key;
extern const lv_img_dsc_t img_rooms;
extern const lv_img_dsc_t img_options;
extern const lv_img_dsc_t img_list;
extern const lv_img_dsc_t img_wifi;
extern const lv_img_dsc_t img_outdoor;
extern const lv_img_dsc_t img_language;
extern const lv_img_dsc_t img_connected;
extern const lv_img_dsc_t img_connected_alpha8bit;
extern const lv_img_dsc_t img_indoor_alpha4bit;
extern const lv_img_dsc_t img_indoor_alpha8bit;
extern const lv_img_dsc_t img_humidity_alpha8bit;
extern const lv_img_dsc_t img_outdoor_alpha8bit;
extern const lv_img_dsc_t img_delete;
extern const lv_img_dsc_t img_add_room_white;
extern const lv_img_dsc_t img_fan;
extern const lv_img_dsc_t img_offset;
extern const lv_img_dsc_t img_indoor;
extern const lv_img_dsc_t img_humidity;
extern const lv_img_dsc_t img_visible;
extern const lv_img_dsc_t img_outdoor_dark;

#ifndef EXT_IMG_DESC_T
#define EXT_IMG_DESC_T
typedef struct _ext_img_desc_t {
    const char *name;
    const lv_img_dsc_t *img_dsc;
} ext_img_desc_t;
#endif

extern const ext_img_desc_t images[24];


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_IMAGES_H*/