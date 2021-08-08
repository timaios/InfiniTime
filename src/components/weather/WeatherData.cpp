#include "WeatherData.h"

#include <cstdint>
#include <time.h>
#include <string.h>
#include <math.h>


#define MAX_FORECASTS   64


using namespace Pinetime::Tools;


WeatherData::WeatherData() {

  updateTime = static_cast<time_t>(0);
  forecasts = new Forecast[MAX_FORECASTS];
  for (int i = 0; i < MAX_FORECASTS; i++)
    forecasts[i].day = 0;

}


WeatherData::~WeatherData() {

  delete[] forecasts;

}


uint16_t WeatherData::CurrentOwmWeatherId() const {

  if (CurrentDataIsOutdated())
    return 0;
  else
    return currentOwmWeatherId;

}


uint16_t WeatherData::CurrentTemperature() const {

  if (CurrentDataIsOutdated())
    return 0;
  else
    return currentTemperature;

}


uint16_t WeatherData::ForecastTemperature(uint8_t daysDelta, uint8_t hour) const {

  int index = FindForecastIndex(daysDelta, hour);
  if (index >= 0)
    return forecasts[index].temperature;
  else
    return 0;

}


uint8_t WeatherData::ForecastPrecipitation(uint8_t daysDelta, uint8_t hour) const {

  int index = FindForecastIndex(daysDelta, hour);
  if (index >= 0)
    return forecasts[index].precipitation;
  else
    return 0;

}


void WeatherData::SaveCurrentWeather(uint16_t owmWeatherId, uint16_t temperature) {

  // save the data and delete outdated forecasts
  time(&updateTime);
  currentOwmWeatherId = owmWeatherId;
  currentTemperature = temperature;
  PurgeOutdatedForecasts();

}


bool WeatherData::SaveForecast(uint8_t day, uint8_t hour, uint16_t temperature, uint8_t precipitation) {

  // delete outdated forecasts
  PurgeOutdatedForecasts();

  // outdated info won't be saved
  if (HoursUntil(day, hour) < 0.0)
    return false;

  // check if there's data to be replaced
  int index = -1;
  for (int i = 0; i < MAX_FORECASTS; i++) {
    if ((forecasts[i].day == day) && (forecasts[i].hour == hour)) {
      index = i;
      break;
    }
  }

  // find a free slot to store the forecast data if there's no record to be replaced
  if (index < 0) {
    for (int i = 0; i < MAX_FORECASTS; i++) {
      if (forecasts[i].day == 0) {
        index = i;
        break;
      }
    }
    if (index < 0)
      return false;
  }

  // save the forecast
  forecasts[index].day = day;
  forecasts[index].hour = hour;
  forecasts[index].temperature = temperature;
  forecasts[index].precipitation = precipitation;
  return true;

}


bool WeatherData::CurrentDataIsOutdated() const {

  // weather data should be updated at least once within two hours
  time_t now;
  time(&now);
  double delta = difftime(now, updateTime);
  if (delta < 0.0)
    delta *= -1.0;
  return (delta > (2.0 * 60.0 * 60.0));

}


double WeatherData::HoursUntil(uint8_t day, uint8_t hour) {

  // prepare a time structure that we can put the day and hour into
  time_t now;
  time(&now);
  struct tm *nowInfo = localtime(&now);
  struct tm timeInfo = *nowInfo;
  timeInfo.tm_mday = static_cast<int>(day);
  timeInfo.tm_hour = static_cast<int>(hour);
  timeInfo.tm_min = 0;
  timeInfo.tm_sec = 0;

  // calculate the difference if the given day is in the current month
  double diff1 = difftime(mktime(&timeInfo), now);

  // calculate the difference if the given day is in the previous month
  // mktime returns -1 if the date is invalid (e.g. Feb 31st), which creates a high difference value that won't be used
  struct tm wrapPred = timeInfo;
  wrapPred.tm_mon--;
  if (wrapPred.tm_mon < 0) {
    wrapPred.tm_mon = 11;
    wrapPred.tm_year--;
  }
  double diff2 = difftime(mktime(&wrapPred), now);

  // calculate the difference if the given day is in the next month
  // mktime returns -1 if the date is invalid (e.g. Feb 31st), which creates a high difference value that won't be used
  struct tm wrapSucc = timeInfo;
  wrapSucc.tm_mon++;
  if (wrapSucc.tm_mon > 11) {
    wrapSucc.tm_mon = 0;
    wrapSucc.tm_year++;
  }
  double diff3 = difftime(mktime(&wrapSucc), now);

  // get the smallest difference
  double diff = diff1;
  if (abs(diff2) < abs(diff))
    diff = diff2;
  if (abs(diff3) < abs(diff))
    diff = diff3;

  // return the difference in terms of hours
  return diff / (60.0 * 60.0);

}


void WeatherData::PurgeOutdatedForecasts() {

  // delete all forecasts that are already in the past
  for (int i = 0; i < MAX_FORECASTS; i++) {

    // don't check unused records
    if (forecasts[i].day == 0)
      continue;

    if (HoursUntil(forecasts[i].day, forecasts[i].hour) < 0.0)
      forecasts[i].day = 0;
  }

}


int WeatherData::FindForecastIndex(uint8_t daysDelta, uint8_t hour) const {

  // get the day for the given delta
  time_t deltaTime;
  time(&deltaTime);
  deltaTime += static_cast<time_t>(static_cast<int>(daysDelta) * 24 * 60 * 60);
  struct tm *timeInfo = localtime(&deltaTime);
  uint8_t day = static_cast<uint8_t>(timeInfo->tm_mday);

  // find the corresponding forecast
  int index = -1;
  int minDiff = 100;
  for (int i = 0; i < MAX_FORECASTS; i++) {
    if (forecasts[i].day == day) {
      int diff = static_cast<int>(forecasts[i].hour) - hour;
      if (diff < 0)
        diff *= -1;
      if (diff < minDiff) {
        index = i;
        minDiff = diff;
      }
    }
  }
  return index;

}
