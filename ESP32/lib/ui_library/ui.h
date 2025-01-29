#ifndef EEZ_LVGL_UI_GUI_H
#define EEZ_LVGL_UI_GUI_H

#include <lvgl/lvgl.h>

#include "eez-flow.h"



#if !defined(EEZ_FOR_LVGL)
#include "screens.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

<<<<<<< HEAD
extern const uint8_t assets[51516];
=======
extern const uint8_t assets[10956];
>>>>>>> 3c6b74bdf66661c4d7d18bfbada1c9d684fdfdee

void ui_init();
void ui_tick();

#if !defined(EEZ_FOR_LVGL)
void loadScreen(enum ScreensEnum screenId);
#endif

#ifdef __cplusplus
}
#endif

#endif // EEZ_LVGL_UI_GUI_H