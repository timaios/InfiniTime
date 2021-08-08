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
#include "components/motion/MotionController.h"

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
                        Controllers::Settings& settingsController,
                        Controllers::MotionController& motionController);

        ~WatchFaceIGraph() override;

        bool Refresh() override;

      private:
        Pinetime::Controllers::DateTime::Days prevDayOfWeek;
        uint8_t prevDayOfMonth;
        uint8_t prevHours;
        uint8_t prevMinutes;
        uint8_t prevSeconds;
        uint32_t prevSteps;
        int prevBatteryPercent;
        bool prevCharging;
        bool prevBleConnected;

        lv_obj_t *lblDayOfWeek;
        lv_obj_t *lblDayOfMonth;

        lv_obj_t *arcSteps;
        lv_style_t arcStepsStyle_bg;
        lv_style_t arcStepsStyle_fg;

        lv_obj_t *arcBattery;
        lv_obj_t *lblBattery;
        lv_style_t arcBatteryStyle_bg;
        lv_style_t arcBatteryStyle_fg;

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

        lv_obj_t *chargingIcon;
        lv_obj_t *bleIcon;

//	lv_obj_t *lblDebug;

        Controllers::DateTime& dateTimeController;
        Controllers::Battery& batteryController;
        Controllers::Ble& bleController;
        Controllers::NotificationManager& notificationManager;
        Controllers::Settings& settingsController;
        Controllers::MotionController& motionController;

        void UpdateHands(uint8_t hours, uint8_t minutes, uint8_t seconds, bool forceUpdate = false);
        void UpdateBatteryPercent(int batteryPercent, bool forceUpdate = false);
        int16_t RoundedCoord(double value);
        uint16_t StepsEndAngle(uint32_t steps);
      };
    }
  }
}
