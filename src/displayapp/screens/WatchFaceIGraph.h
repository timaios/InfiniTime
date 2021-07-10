#pragma once

#include <lvgl/src/lv_core/lv_obj.h>
#include <chrono>
#include <cstdint>
#include <memory>
#include "Screen.h"
#include "ScreenList.h"
#include "components/datetime/DateTimeController.h"
#include "components/battery/BatteryController.h"
#include "components/ble/BleController.h"
#include "components/ble/NotificationManager.h"

namespace Pinetime {
  namespace Controllers {
    class Settings;
    class Battery;
    class Ble;
    class NotificationManager;
  }
  namespace Applications {
    namespace Screens {

      class WatchFaceIGraph : public Screen {
      public:
        WatchFaceIGraph(DisplayApp* app,
                        Controllers::DateTime& dateTimeController,
                        Controllers::Battery& batteryController,
                        Controllers::Ble& bleController,
                        Controllers::NotificationManager& notificationManager,
                        Controllers::Settings& settingsController);

        ~WatchFaceIGraph() override;

        bool Refresh() override;

      private:
        Pinetime::Controllers::DateTime::Days prevDayOfWeek;
        uint8_t prevDayOfMonth;
        uint8_t prevHours;
        uint8_t prevMinutes;
        uint8_t prevSeconds;

        lv_obj_t *lblDayOfWeek;
        lv_obj_t *lblDayOfMonth;

        lv_obj_t *arcSteps;
        lv_style_t arcStepsStyle_bg;
        lv_style_t arcStepsStyle_fg;

        lv_obj_t *hoursHandInner;
        lv_point_t hoursHandInnerPoints[2];
        lv_obj_t *hoursHandOuter;
        lv_point_t hoursHandOuterPoints[2];

        lv_obj_t *minutesHandInner;
        lv_point_t minutesHandInnerPoints[2];
        lv_obj_t *minutesHandOuter;
        lv_point_t minutesHandOuterPoints[2];

        lv_obj_t *secondsHand;
        lv_point_t secondsHandPoints[2];
        lv_obj_t *secondsTail;
        lv_point_t secondsTailPoints[2];

        lv_style_t hmHandStyleInner;
        lv_style_t hmHandStyleOuter;
        lv_style_t secondsHandStyle;

        Controllers::DateTime& dateTimeController;
        Controllers::Battery& batteryController;
        Controllers::Ble& bleController;
        Controllers::NotificationManager& notificationManager;
        Controllers::Settings& settingsController;

        void UpdateHands(uint8_t hours, uint8_t minutes, uint8_t seconds, bool forceUpdate = false);
        int16_t roundedCoord(double value);
      };
    }
  }
}
