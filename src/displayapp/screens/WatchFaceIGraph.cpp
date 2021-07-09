#include <libs/lvgl/lvgl.h>
#include "WatchFaceIGraph.h"
#include "BatteryIcon.h"
#include "BleIcon.h"
#include "Symbols.h"
#include "NotificationIcon.h"

#include <cmath>

LV_IMG_DECLARE(bg_igraph);

using namespace Pinetime::Applications::Screens;

#define HOUR_LENGTH   70
#define MINUTE_LENGTH 90
#define SECOND_LENGTH 110
#define PI            3.14159265358979323846


WatchFaceIGraph::WatchFaceIGraph(Pinetime::Applications::DisplayApp* app,
                                 Controllers::DateTime& dateTimeController,
                                 Controllers::Battery& batteryController,
                                 Controllers::Ble& bleController,
                                 Controllers::NotificationManager& notificationManager,
                                 Controllers::Settings& settingsController)
  : Screen(app),
    dateTimeController {dateTimeController},
    batteryController {batteryController},
    bleController {bleController},
    notificationManager {notificationManager},
    settingsController {settingsController} {

  settingsController.SetClockFace(3);

  lv_obj_t* bg_clock_img = lv_img_create(lv_scr_act(), NULL);
  lv_img_set_src(bg_clock_img, &bg_igraph);
  lv_obj_align(bg_clock_img, NULL, LV_ALIGN_CENTER, 0, 0);

  prevDayOfWeek = dateTimeController.DayOfWeek();
  lblDayOfWeek = lv_label_create(lv_scr_act(), NULL);
  lv_obj_set_style_local_text_color(lblDayOfWeek, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xfe3b30));
  lv_label_set_text_static(lblDayOfWeek, Controllers::DateTime::DayOfWeekShortToString(prevDayOfWeek));
  lv_label_set_align(lblDayOfWeek, LV_LABEL_ALIGN_CENTER);
  lv_obj_align(lblDayOfWeek, NULL, LV_ALIGN_CENTER, 0, -72);
  lv_obj_set_auto_realign(lblDayOfWeek, true);

  prevDayOfMonth = dateTimeController.Day();
  lblDayOfMonth = lv_label_create(lv_scr_act(), NULL);
  lv_obj_set_style_local_text_color(lblDayOfMonth, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
  lv_label_set_text_fmt(lblDayOfMonth, "%d", static_cast<int>(prevDayOfMonth));
  lv_label_set_align(lblDayOfMonth, LV_LABEL_ALIGN_CENTER);
  lv_obj_align(lblDayOfMonth, NULL, LV_ALIGN_CENTER, 0, -52);
  lv_obj_set_auto_realign(lblDayOfMonth, true);

  arcSteps = lv_arc_create(lv_scr_act(), NULL);
  lv_arc_set_rotation(arcSteps, -90);
  lv_arc_set_bg_angles(arcSteps, 0, 360);
  lv_arc_set_angles(arcSteps, 0, 90);
  lv_obj_set_size(arcSteps, 64, 64);
  lv_obj_align(arcSteps, NULL, LV_ALIGN_CENTER, 0, 60);

  lv_style_init(&arcStepsStyle_bg);
  lv_style_set_line_width(&arcStepsStyle_bg, LV_STATE_DEFAULT, 6);
  lv_style_set_line_color(&arcStepsStyle_bg, LV_STATE_DEFAULT, lv_color_hex(0x213300));
  lv_obj_add_style(arcSteps, LV_ARC_PART_BG, &arcStepsStyle_bg);

  lv_style_init(&arcStepsStyle_fg);
  lv_style_set_line_width(&arcStepsStyle_fg, LV_STATE_DEFAULT, 6);
  lv_style_set_line_color(&arcStepsStyle_fg, LV_STATE_DEFAULT, lv_color_hex(0xa6fe00));
  lv_obj_add_style(arcSteps, LV_ARC_PART_INDIC, &arcStepsStyle_fg);

  UpdateClock();

}

WatchFaceIGraph::~WatchFaceIGraph() {

  lv_style_reset(&arcStepsStyle_bg);
  lv_style_reset(&arcStepsStyle_fg);

  lv_obj_clean(lv_scr_act());

}

void WatchFaceIGraph::UpdateClock() {

  //TODO

}

bool WatchFaceIGraph::Refresh() {

  Pinetime::Controllers::DateTime::Days dayOfWeek = dateTimeController.DayOfWeek();
  uint8_t dayOfMonth = dateTimeController.Day();

  if (dayOfWeek != prevDayOfWeek) {
    lv_label_set_text_static(lblDayOfWeek, Controllers::DateTime::DayOfWeekShortToString(dayOfWeek));
    prevDayOfWeek = dayOfWeek;
  }

  if (dayOfMonth != prevDayOfMonth) {
    lv_label_set_text_fmt(lblDayOfMonth, "%d", static_cast<int>(dayOfMonth));
    prevDayOfMonth = dayOfMonth;
  }

  return true;
}
