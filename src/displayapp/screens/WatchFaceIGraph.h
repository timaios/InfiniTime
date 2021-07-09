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

        lv_obj_t *lblDayOfWeek;
        lv_obj_t *lblDayOfMonth;

        lv_obj_t *arcSteps;
        lv_style_t arcStepsStyle_bg;
        lv_style_t arcStepsStyle_fg;

        Controllers::DateTime& dateTimeController;
        Controllers::Battery& batteryController;
        Controllers::Ble& bleController;
        Controllers::NotificationManager& notificationManager;
        Controllers::Settings& settingsController;

        void UpdateClock();
      };
    }
  }
}
