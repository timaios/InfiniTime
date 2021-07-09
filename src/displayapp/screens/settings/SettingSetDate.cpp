#include "SettingSetDate.h"
#include <lvgl/lvgl.h>
#include <hal/nrf_rtc.h>
#include <nrf_log.h>
#include "displayapp/DisplayApp.h"
#include "displayapp/screens/Symbols.h"

#define POS_X_DAY   -72
#define POS_X_MONTH   0
#define POS_X_YEAR   72
#define POS_Y_PLUS  -50
#define POS_Y_TEXT   -6
#define POS_Y_MINUS  40

using namespace Pinetime::Applications::Screens;

namespace {
  static void event_handler(lv_obj_t * obj, lv_event_t event) {
    SettingSetDate* screen = static_cast<SettingSetDate *>(obj->user_data);
    screen->HandleButtonPress(obj, event);
  }
}

SettingSetDate::SettingSetDate(
  Pinetime::Applications::DisplayApp *app, Pinetime::Controllers::DateTime &dateTimeController) :
  Screen(app),
  dateTimeController {dateTimeController}
{

  lv_obj_t * container1 = lv_cont_create(lv_scr_act(), nullptr);

  //lv_obj_set_style_local_bg_color(container1, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x111111));
  lv_obj_set_style_local_bg_opa(container1, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);
  lv_obj_set_style_local_pad_all(container1, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 10);
  lv_obj_set_style_local_pad_inner(container1, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 5);
  lv_obj_set_style_local_border_width(container1, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 0);
  lv_obj_set_pos(container1, 30, 60);
  lv_obj_set_width(container1, LV_HOR_RES - 50);
  lv_obj_set_height(container1, LV_VER_RES - 60);
  lv_cont_set_layout(container1, LV_LAYOUT_COLUMN_LEFT);

  lv_obj_t * title = lv_label_create(lv_scr_act(), NULL);  
  lv_label_set_text_static(title, "Set current date");
  lv_label_set_align(title, LV_LABEL_ALIGN_CENTER);
  lv_obj_align(title, lv_scr_act(), LV_ALIGN_IN_TOP_MID, 15, 15);

  lv_obj_t * icon = lv_label_create(lv_scr_act(), NULL);
  lv_obj_set_style_local_text_color(icon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_ORANGE);
  
  lv_label_set_text_static(icon, Symbols::clock);
  lv_label_set_align(icon, LV_LABEL_ALIGN_CENTER);
  lv_obj_align(icon, title, LV_ALIGN_OUT_LEFT_MID, -10, 0);

  dayValue = static_cast<int>(dateTimeController.Day());
  lblDay = lv_label_create(lv_scr_act(), NULL);
  //lv_obj_set_style_local_text_font(lblDay, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &jetbrains_mono_42);
  lv_label_set_text_fmt(lblDay, "%d", dayValue);
  lv_label_set_align(lblDay, LV_LABEL_ALIGN_CENTER);
  lv_obj_align(lblDay, lv_scr_act(), LV_ALIGN_CENTER, POS_X_DAY, POS_Y_TEXT);

  monthValue = static_cast<int>(dateTimeController.Month());
  lblMonth = lv_label_create(lv_scr_act(), NULL);
  //lv_obj_set_style_local_text_font(lblMonth, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &jetbrains_mono_42);
  UpdateMonthLabel();

  yearValue = static_cast<int>(dateTimeController.Year());
  if (yearValue < 2021)
    yearValue = 2021;
  lblYear = lv_label_create(lv_scr_act(), NULL);
  //lv_obj_set_style_local_text_font(lblYear, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &jetbrains_mono_42);
  lv_label_set_text_fmt(lblYear, "%d", yearValue);
  lv_label_set_align(lblYear, LV_LABEL_ALIGN_CENTER);
  lv_obj_align(lblYear, lv_scr_act(), LV_ALIGN_CENTER, POS_X_YEAR, POS_Y_TEXT);

  btnDayPlus = lv_btn_create(lv_scr_act(), NULL);
  btnDayPlus->user_data = this;
  lv_obj_set_size(btnDayPlus, 50, 40);
  lv_obj_align(btnDayPlus, lv_scr_act(), LV_ALIGN_CENTER, POS_X_DAY, POS_Y_PLUS);
  lv_obj_set_style_local_value_str(btnDayPlus, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, "+");
  lv_obj_set_event_cb(btnDayPlus, event_handler);

  btnDayMinus = lv_btn_create(lv_scr_act(), NULL);
  btnDayMinus->user_data = this;
  lv_obj_set_size(btnDayMinus, 50, 40);
  lv_obj_align(btnDayMinus, lv_scr_act(), LV_ALIGN_CENTER, POS_X_DAY, POS_Y_MINUS);
  lv_obj_set_style_local_value_str(btnDayMinus, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, "-");
  lv_obj_set_event_cb(btnDayMinus, event_handler);

  btnMonthPlus = lv_btn_create(lv_scr_act(), NULL);
  btnMonthPlus->user_data = this;
  lv_obj_set_size(btnMonthPlus, 50, 40);
  lv_obj_align(btnMonthPlus, lv_scr_act(), LV_ALIGN_CENTER, POS_X_MONTH, POS_Y_PLUS);
  lv_obj_set_style_local_value_str(btnMonthPlus, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, "+");
  lv_obj_set_event_cb(btnMonthPlus, event_handler);

  btnMonthMinus = lv_btn_create(lv_scr_act(), NULL);
  btnMonthMinus->user_data = this;
  lv_obj_set_size(btnMonthMinus, 50, 40);
  lv_obj_align(btnMonthMinus, lv_scr_act(), LV_ALIGN_CENTER, POS_X_MONTH, POS_Y_MINUS);
  lv_obj_set_style_local_value_str(btnMonthMinus, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, "-");
  lv_obj_set_event_cb(btnMonthMinus, event_handler);

  btnYearPlus = lv_btn_create(lv_scr_act(), NULL);
  btnYearPlus->user_data = this;
  lv_obj_set_size(btnYearPlus, 50, 40);
  lv_obj_align(btnYearPlus, lv_scr_act(), LV_ALIGN_CENTER, POS_X_YEAR, POS_Y_PLUS);
  lv_obj_set_style_local_value_str(btnYearPlus, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, "+");
  lv_obj_set_event_cb(btnYearPlus, event_handler);

  btnYearMinus = lv_btn_create(lv_scr_act(), NULL);
  btnYearMinus->user_data = this;
  lv_obj_set_size(btnYearMinus, 50, 40);
  lv_obj_align(btnYearMinus, lv_scr_act(), LV_ALIGN_CENTER, POS_X_YEAR, POS_Y_MINUS);
  lv_obj_set_style_local_value_str(btnYearMinus, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, "-");
  lv_obj_set_event_cb(btnYearMinus, event_handler);

  btnSetTime = lv_btn_create(lv_scr_act(), NULL);
  btnSetTime->user_data = this;
  lv_obj_set_size(btnSetTime, 70, 40);
  lv_obj_align(btnSetTime, lv_scr_act(), LV_ALIGN_CENTER, 0, 90);
  lv_obj_set_style_local_value_str(btnSetTime, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, "Set");
  lv_obj_set_event_cb(btnSetTime, event_handler);
}

SettingSetDate::~SettingSetDate() {
  lv_obj_clean(lv_scr_act());
}

bool SettingSetDate::Refresh() {
  return running;
}


void SettingSetDate::HandleButtonPress(lv_obj_t *object, lv_event_t event) {

  if(object == btnDayPlus && (event == LV_EVENT_PRESSED)) {
    dayValue++;
    if (dayValue > MaximumDayOfMonth())
      dayValue = 1;
    lv_label_set_text_fmt(lblDay, "%d", dayValue);
    lv_obj_align(lblDay, lv_scr_act(), LV_ALIGN_CENTER, POS_X_DAY, POS_Y_TEXT);
    lv_btn_set_state(btnSetTime, LV_BTN_STATE_RELEASED);
  }

  if(object == btnDayMinus && (event == LV_EVENT_PRESSED)) {
    dayValue--;
    if (dayValue < 1)
      dayValue = MaximumDayOfMonth();
    lv_label_set_text_fmt(lblDay, "%d", dayValue);
    lv_obj_align(lblDay, lv_scr_act(), LV_ALIGN_CENTER, POS_X_DAY, POS_Y_TEXT);
    lv_btn_set_state(btnSetTime, LV_BTN_STATE_RELEASED);
  }

  if(object == btnMonthPlus && (event == LV_EVENT_PRESSED)) {
    monthValue++;
    if (monthValue > 12)
      monthValue = 1;
    UpdateMonthLabel();
    lv_btn_set_state(btnSetTime, LV_BTN_STATE_RELEASED);
    CheckDay();
  }

  if(object == btnMonthMinus && (event == LV_EVENT_PRESSED)) {
    monthValue--;
    if (monthValue < 1)
      monthValue = 12;
    UpdateMonthLabel();
    lv_btn_set_state(btnSetTime, LV_BTN_STATE_RELEASED);
    CheckDay();
  }

  if(object == btnYearPlus && (event == LV_EVENT_PRESSED)) {
    yearValue++;
    lv_label_set_text_fmt(lblYear, "%d", yearValue);
    lv_obj_align(lblYear, lv_scr_act(), LV_ALIGN_CENTER, POS_X_YEAR, POS_Y_TEXT);
    lv_btn_set_state(btnSetTime, LV_BTN_STATE_RELEASED);
    CheckDay();
  }

  if(object == btnYearMinus && (event == LV_EVENT_PRESSED)) {
    yearValue--;
    lv_label_set_text_fmt(lblYear, "%d", yearValue);
    lv_obj_align(lblYear, lv_scr_act(), LV_ALIGN_CENTER, POS_X_YEAR, POS_Y_TEXT);
    lv_btn_set_state(btnSetTime, LV_BTN_STATE_RELEASED);
    CheckDay();
  }

  if(object == btnSetTime && (event == LV_EVENT_PRESSED)) {
    NRF_LOG_INFO("Setting date (manually) to %04d-%02d-%02d", yearValue, monthValue, dayValue);
    dateTimeController.SetTime(static_cast<uint16_t>(yearValue),
                               static_cast<uint8_t>(monthValue),
                               static_cast<uint8_t>(dayValue),
                               0,
                               dateTimeController.Hours(),
                               dateTimeController.Minutes(),
                               dateTimeController.Seconds(),
                               nrf_rtc_counter_get(portNRF_RTC_REG));
    lv_btn_set_state(btnSetTime, LV_BTN_STATE_DISABLED);
  }
}

int SettingSetDate::MaximumDayOfMonth() const {
  switch (monthValue) {
    case 2:
      if ((((yearValue % 4) == 0) && ((yearValue % 100) != 0)) || ((yearValue % 400) == 0))
        return 29;
      return 28;
    case 4:
    case 6:
    case 9:
    case 11:
      return 30;
    default:
      return 31;
  }
}

void SettingSetDate::CheckDay() {
  int maxDay = MaximumDayOfMonth();
  if (dayValue > maxDay) {
    dayValue = maxDay;
    lv_label_set_text_fmt(lblDay, "%d", dayValue);
    lv_obj_align(lblDay, lv_scr_act(), LV_ALIGN_CENTER, POS_X_DAY, POS_Y_TEXT);
  }
}

void SettingSetDate::UpdateMonthLabel() {
  switch (monthValue) {
    case 1:
      lv_label_set_text_static(lblMonth, "Jan");
      break;
    case 2:
      lv_label_set_text_static(lblMonth, "Feb");
      break;
    case 3:
      lv_label_set_text_static(lblMonth, "Mar");
      break;
    case 4:
      lv_label_set_text_static(lblMonth, "Apr");
      break;
    case 5:
      lv_label_set_text_static(lblMonth, "May");
      break;
    case 6:
      lv_label_set_text_static(lblMonth, "Jun");
      break;
    case 7:
      lv_label_set_text_static(lblMonth, "Jul");
      break;
    case 8:
      lv_label_set_text_static(lblMonth, "Aug");
      break;
    case 9:
      lv_label_set_text_static(lblMonth, "Sep");
      break;
    case 10:
      lv_label_set_text_static(lblMonth, "Oct");
      break;
    case 11:
      lv_label_set_text_static(lblMonth, "Nov");
      break;
    case 12:
      lv_label_set_text_static(lblMonth, "Dec");
      break;
    default:
      lv_label_set_text_static(lblMonth, "---");
      break;
  }
  lv_label_set_align(lblMonth, LV_LABEL_ALIGN_CENTER);
  lv_obj_align(lblMonth, lv_scr_act(), LV_ALIGN_CENTER, POS_X_MONTH, POS_Y_TEXT);
}
