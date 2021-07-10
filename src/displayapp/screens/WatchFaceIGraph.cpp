#include <libs/lvgl/lvgl.h>
#include "WatchFaceIGraph.h"
#include "BatteryIcon.h"
#include "BleIcon.h"
#include "Symbols.h"
#include "NotificationIcon.h"

#include <cmath>

LV_IMG_DECLARE(bg_igraph);

using namespace Pinetime::Applications::Screens;

#define PI  3.14159265358979323846


WatchFaceIGraph::WatchFaceIGraph(Pinetime::Applications::DisplayApp* app,
                                 Controllers::DateTime& dateTimeController,
                                 Controllers::Battery& batteryController,
                                 Controllers::Ble& bleController,
                                 Controllers::NotificationManager& notificationManager,
                                 Controllers::Settings& settingsController,
				 Controllers::MotionController& motionController)
  : Screen(app),
    dateTimeController {dateTimeController},
    batteryController {batteryController},
    bleController {bleController},
    notificationManager {notificationManager},
    settingsController {settingsController},
    motionController {motionController} {

  settingsController.SetClockFace(3);

  lv_obj_t* bg_clock_img = lv_img_create(lv_scr_act(), NULL);
  lv_img_set_src(bg_clock_img, &bg_igraph);
  lv_obj_align(bg_clock_img, NULL, LV_ALIGN_CENTER, 0, 0);

  prevDayOfWeek = dateTimeController.DayOfWeek();
  prevDayOfMonth = dateTimeController.Day();
  prevHours = dateTimeController.Hours();
  prevMinutes = dateTimeController.Minutes();
  prevSeconds = dateTimeController.Seconds();
  prevSteps = motionController.NbSteps();

  lblDayOfWeek = lv_label_create(lv_scr_act(), NULL);
  lv_obj_set_style_local_text_color(lblDayOfWeek, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xfe3b30));
  lv_label_set_text_static(lblDayOfWeek, Controllers::DateTime::DayOfWeekShortToString(prevDayOfWeek));
  lv_label_set_align(lblDayOfWeek, LV_LABEL_ALIGN_CENTER);
  lv_obj_align(lblDayOfWeek, NULL, LV_ALIGN_CENTER, 0, -72);
  lv_obj_set_auto_realign(lblDayOfWeek, true);

  lblDayOfMonth = lv_label_create(lv_scr_act(), NULL);
  lv_obj_set_style_local_text_color(lblDayOfMonth, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
  lv_label_set_text_fmt(lblDayOfMonth, "%d", static_cast<int>(prevDayOfMonth));
  lv_label_set_align(lblDayOfMonth, LV_LABEL_ALIGN_CENTER);
  lv_obj_align(lblDayOfMonth, NULL, LV_ALIGN_CENTER, 0, -52);
  lv_obj_set_auto_realign(lblDayOfMonth, true);

  arcSteps = lv_arc_create(lv_scr_act(), NULL);
  lv_arc_set_bg_angles(arcSteps, 0, 360);
  lv_arc_set_angles(arcSteps, 270, StepsEndAngle(prevSteps));
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

  hoursHandInner = lv_line_create(lv_scr_act(), NULL);
  hoursHandOuter = lv_line_create(lv_scr_act(), NULL);
  minutesHandInner = lv_line_create(lv_scr_act(), NULL);
  minutesHandOuter = lv_line_create(lv_scr_act(), NULL);
  secondsHand = lv_line_create(lv_scr_act(), NULL);
  secondsTail = lv_line_create(lv_scr_act(), NULL);

  lv_style_init(&hmHandStyleInner);
  lv_style_set_line_width(&hmHandStyleInner, LV_STATE_DEFAULT, 5);
  lv_style_set_line_color(&hmHandStyleInner, LV_STATE_DEFAULT, LV_COLOR_WHITE);
  lv_style_set_line_rounded(&hmHandStyleInner, LV_STATE_DEFAULT, false);
  lv_obj_add_style(hoursHandInner, LV_LINE_PART_MAIN, &hmHandStyleInner);
  lv_obj_add_style(minutesHandInner, LV_LINE_PART_MAIN, &hmHandStyleInner);

  lv_style_init(&hmHandStyleOuter);
  lv_style_set_line_width(&hmHandStyleOuter, LV_STATE_DEFAULT, 11);
  lv_style_set_line_color(&hmHandStyleOuter, LV_STATE_DEFAULT, LV_COLOR_WHITE);
  lv_style_set_line_rounded(&hmHandStyleOuter, LV_STATE_DEFAULT, true);
  lv_obj_add_style(hoursHandOuter, LV_LINE_PART_MAIN, &hmHandStyleOuter);
  lv_obj_add_style(minutesHandOuter, LV_LINE_PART_MAIN, &hmHandStyleOuter);

  lv_style_init(&secondsHandStyle);
  lv_style_set_line_width(&secondsHandStyle, LV_STATE_DEFAULT, 3);
  lv_style_set_line_color(&secondsHandStyle, LV_STATE_DEFAULT, lv_color_hex(0xfe3b30));
  lv_style_set_line_rounded(&secondsHandStyle, LV_STATE_DEFAULT, true);
  lv_obj_add_style(secondsHand, LV_LINE_PART_MAIN, &secondsHandStyle);
  lv_obj_add_style(secondsTail, LV_LINE_PART_MAIN, &secondsHandStyle);

  UpdateHands(prevHours, prevMinutes, prevSeconds, true);

}

WatchFaceIGraph::~WatchFaceIGraph() {

  lv_style_reset(&arcStepsStyle_bg);
  lv_style_reset(&arcStepsStyle_fg);
  lv_style_reset(&hmHandStyleInner);
  lv_style_reset(&hmHandStyleOuter);
  lv_style_reset(&secondsHandStyle);

  lv_obj_clean(lv_scr_act());

}

void WatchFaceIGraph::UpdateHands(uint8_t hours, uint8_t minutes, uint8_t seconds, bool forceUpdate) {

  int16_t cx = LV_HOR_RES / 2;
  int16_t cy = LV_VER_RES / 2;

  if ((hours != prevHours) || (minutes != prevMinutes) || forceUpdate) {

    double radians = ((static_cast<double>(hours % 12) * 60.0) + static_cast<double>(minutes)) / (12.0 * 60.0) * 2.0 * PI;
    double sine = sin(radians);
    double cosine = cos(radians);

    hoursHandInnerPoints[0].x = cx + RoundedCoord(5.0 * sine);
    hoursHandInnerPoints[0].y = cy - RoundedCoord(5.0 * cosine);
    hoursHandInnerPoints[1].x = cx + RoundedCoord(20.0 * sine);
    hoursHandInnerPoints[1].y = cy - RoundedCoord(20.0 * cosine);
    lv_line_set_points(hoursHandInner, hoursHandInnerPoints, 2);

    hoursHandOuterPoints[0].x = cx + RoundedCoord(20.0 * sine);
    hoursHandOuterPoints[0].y = cy - RoundedCoord(20.0 * cosine);
    hoursHandOuterPoints[1].x = cx + RoundedCoord(60.0 * sine);
    hoursHandOuterPoints[1].y = cy - RoundedCoord(60.0 * cosine);
    lv_line_set_points(hoursHandOuter, hoursHandOuterPoints, 2);

  }

  if ((minutes != prevMinutes) || (seconds != prevSeconds) || forceUpdate) {

    double radians = ((static_cast<double>(minutes) * 60.0) + static_cast<double>(seconds)) / (60.0 * 60.0) * 2.0 * PI;
    double sine = sin(radians);
    double cosine = cos(radians);

    minutesHandInnerPoints[0].x = cx + RoundedCoord(5.0 * sine);
    minutesHandInnerPoints[0].y = cy - RoundedCoord(5.0 * cosine);
    minutesHandInnerPoints[1].x = cx + RoundedCoord(20.0 * sine);
    minutesHandInnerPoints[1].y = cy - RoundedCoord(20.0 * cosine);
    lv_line_set_points(minutesHandInner, minutesHandInnerPoints, 2);

    minutesHandOuterPoints[0].x = cx + RoundedCoord(20.0 * sine);
    minutesHandOuterPoints[0].y = cy - RoundedCoord(20.0 * cosine);
    minutesHandOuterPoints[1].x = cx + RoundedCoord(104.0 * sine);
    minutesHandOuterPoints[1].y = cy - RoundedCoord(104.0 * cosine);
    lv_line_set_points(minutesHandOuter, minutesHandOuterPoints, 2);

  }

  if ((seconds != prevSeconds) || forceUpdate) {

    double radians = static_cast<double>(seconds) / 60.0 * 2.0 * PI;
    double sine = sin(radians);
    double cosine = cos(radians);

    secondsHandPoints[0].x = cx + RoundedCoord(4.0 * sine);
    secondsHandPoints[0].y = cy - RoundedCoord(4.0 * cosine);
    secondsHandPoints[1].x = cx + RoundedCoord(114.0 * sine);
    secondsHandPoints[1].y = cy - RoundedCoord(114.0 * cosine);
    lv_line_set_points(secondsHand, secondsHandPoints, 2);

    secondsTailPoints[0].x = cx - RoundedCoord(4.0 * sine);
    secondsTailPoints[0].y = cy + RoundedCoord(4.0 * cosine);
    secondsTailPoints[1].x = cx - RoundedCoord(20.0 * sine);
    secondsTailPoints[1].y = cy + RoundedCoord(20.0 * cosine);
    lv_line_set_points(secondsTail, secondsTailPoints, 2);

  }

}

bool WatchFaceIGraph::Refresh() {

  Pinetime::Controllers::DateTime::Days dayOfWeek = dateTimeController.DayOfWeek();
  uint8_t dayOfMonth = dateTimeController.Day();
  uint8_t hours = dateTimeController.Hours();
  uint8_t minutes = dateTimeController.Minutes();
  uint8_t seconds = dateTimeController.Seconds();
  uint32_t steps = motionController.NbSteps();

  if (dayOfWeek != prevDayOfWeek)
    lv_label_set_text_static(lblDayOfWeek, Controllers::DateTime::DayOfWeekShortToString(dayOfWeek));

  if (dayOfMonth != prevDayOfMonth)
    lv_label_set_text_fmt(lblDayOfMonth, "%d", static_cast<int>(dayOfMonth));

  if (steps != prevSteps)
    lv_arc_set_angles(arcSteps, 270, StepsEndAngle(steps));

  UpdateHands(hours, minutes, seconds);

  prevDayOfWeek = dayOfWeek;
  prevDayOfMonth = dayOfMonth;
  prevHours = hours;
  prevMinutes = minutes;
  prevSeconds = seconds;
  prevSteps = steps;

  return true;

}

int16_t WatchFaceIGraph::RoundedCoord(double value) {
  if (value < 0.0)
    return static_cast<int16_t>(value - 0.5);
  else
    return static_cast<int16_t>(value + 0.5);
}

uint16_t WatchFaceIGraph::StepsEndAngle(uint32_t value) {
  double steps = static_cast<double>(value);
  double goal = static_cast<double>(settingsController.GetStepsGoal());
  double angle = ((steps >= goal) || (goal <= 0.0)) ? 360.0 : steps / goal * 360.0;
  if (angle < 90.0)
    return static_cast<uint16_t>(270.0 + angle + 0.5);
  else
    return static_cast<uint16_t>(angle - 90.0 + 0.5);
}
