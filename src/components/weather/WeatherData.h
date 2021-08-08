#pragma once

#include <cstdint>
#include <time.h>


namespace Pinetime {
  namespace Tools {

    class WeatherData {

    public:

      WeatherData();
      ~WeatherData();

      time_t UpdateTime() const { return updateTime; }
      uint16_t CurrentOwmWeatherId() const;
      uint16_t CurrentTemperature() const;

      uint16_t ForecastTemperature(uint8_t daysDelta, uint8_t hour) const;
      uint8_t ForecastPrecipitation(uint8_t daysDelta, uint8_t hour) const;

      void SaveCurrentWeather(uint16_t owmWeatherId, uint16_t temperature);
      bool SaveForecast(uint8_t day, uint8_t hour, uint16_t temperature, uint8_t precipitation);

      bool CurrentDataIsOutdated() const;
      double HoursUntil(uint8_t day, uint8_t hour);

    private:

      struct Forecast {
        uint8_t day;            // day of the month, 1..31; 0 means invalid/unused record
        uint8_t hour;           // 0..23
        uint16_t temperature;   // degress Kelvin * 100
        uint8_t precipitation;  // probability of precipitation, 0..100 (percent)
      };

      time_t updateTime;
      uint16_t currentOwmWeatherId;
      uint16_t currentTemperature;
      Forecast *forecasts;

      void PurgeOutdatedForecasts();
      int FindForecastIndex(uint8_t daysDelta, uint8_t hour) const;

    };

  }
}
