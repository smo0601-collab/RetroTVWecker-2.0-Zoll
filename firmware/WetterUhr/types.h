#pragma once
/*
  Gemeinsame Typen. Sie stehen absichtlich in einer Header-Datei: Die Arduino IDE
  erzeugt Funktions-Prototypen am Anfang der .ino-Datei, und Typen, die in
  Funktionsparametern vorkommen, muessen dort schon bekannt sein.
*/
#include <Arduino.h>

enum Align : uint8_t { AL_LEFT, AL_CENTER, AL_RIGHT };

struct Weather {
  bool     valid = false;
  float    temp = 0, feels = 0, wind = 0;
  int      hum = 0, code = 0;
  bool     isDay = true;
  int      nDays = 0;
  int      dCode[3] = {0, 0, 0};
  float    dMax[3] = {0, 0, 0};
  float    dMin[3] = {0, 0, 0};
  char     dDate[3][11] = {{0}, {0}, {0}};
  uint32_t fetchedMs = 0;
  uint32_t version = 0;
};
