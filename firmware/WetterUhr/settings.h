#pragma once
/*
  Einstellungen, die ueber die Browser-Seite geaendert und dauerhaft im
  Flash (NVS, Namespace "wuhr") gespeichert werden.
*/
#include <Arduino.h>
#include <Preferences.h>
#include "config.h"

struct Settings {
  char     ssid[33];
  char     pass[65];
  char     tz[64];
  char     ntp[48];
  bool     ntpOn;      // Uhrzeit per NTP (sonst nur manuell)
  char     host[24];
  char     webPw[33];
  float    lat, lon;
  uint8_t  bright;     // 5..100
  bool     seconds;
  uint8_t  rotation;   // 1 oder 3
  uint16_t updMin;     // Wetter-Intervall in Minuten
  uint8_t  radarKm;    // Flugradar-Radius in km (5..100)
  uint8_t  clockStyle; // 0..CLOCK_STYLE_COUNT-1
  uint8_t  accent;     // 0..8
  bool     dimOn;      // Zeitplan-Dimmung aktiv
  uint16_t dimStart;   // Minuten seit Mitternacht
  uint16_t dimEnd;
  uint8_t  dimLevel;   // 5..100 %
  bool     dimTv;      // im Dimm-Zeitfenster (Nachtmodus) das Testbild zeigen
  bool     autoOn;     // Designs automatisch wechseln
  bool     autoRnd;    // zufaellige Reihenfolge
  uint64_t autoMask;   // Bit n = Design n nimmt am Wechsel teil
  uint32_t autoSec;    // Intervall in Sekunden
  bool     tickOn;     // Laufschrift aktiv
  bool     tickLoop;   // Dauerschleife (sonst einmalig)
  bool     tickBg;     // Hintergrundband
  uint8_t  tickPos;    // 0 oben, 1 Mitte, 2 unten
  uint8_t  tickBig;    // 0 mittel, 1 gross
  uint8_t  tickColor;  // 0..8 Akzentfarben, 9 weiss
  uint8_t  tickSpeed;  // 1..10
  uint16_t tickPause;  // Sekunden zwischen Durchlaeufen (Schleife)
  char     tick[161];  // Text (UTF-8)
  bool     bzHour;     // Summer: Stundensignal
  bool     bzQuiet;    // Summer: kein Stundensignal im Dimm-Zeitfenster
  bool     bzTouch;    // Summer: Klick bei Touch
  bool     alOn;       // Wecker aktiv
  uint16_t alTime;     // Weckzeit, Minuten seit Mitternacht
  uint8_t  alDays;     // Bit 0 = Mo ... Bit 6 = So
  bool     ufoOn;      // Flugradar: UFO-Sichtungen
  uint8_t  sleigh;     // ISS-Tracker: 0 ISS, 1 Schlitten, 2 Schlitten im Dezember (1.-26.)
  bool     nyeAuto;    // Silvester-Automatik
};

static Settings    gSet;
static Preferences prefs;

// Design-Nummern, Version 2 (Oktober 2026): 8 klassische und 6 Themen-Designs entfernt, 6 neue angehaengt.
// Gespeicherte Auswahl und Wechsel-Liste aus Version 1 (40 Designs) werden beim ersten Start umgerechnet.
#define STYLE_VERSION 2
static const int8_t STYLE_V1_SEL[40]  = {1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 18, 23, 6, 19, 20, 20, 21, 22, 22, 23, 25, 24, 25};   // alte Nummer -> neue (aehnliches Design)
static const int8_t STYLE_V1_LIST[40] = {-1, -1, -1, -1, -1, -1, -1, 0, 1, -1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, -1, -1, -1, 19, 20, -1, 21, -1, 22, 23, -1, 24, 25};   // alte Nummer -> neue, -1 = entfernt

static void settingsSave();

static void settingsLoad() {
  prefs.begin("wuhr", true);   // nur lesen
  strlcpy(gSet.ssid,  prefs.getString("ssid",  DEFAULT_WIFI_SSID).c_str(), sizeof(gSet.ssid));
  strlcpy(gSet.pass,  prefs.getString("pass",  DEFAULT_WIFI_PASS).c_str(), sizeof(gSet.pass));
  strlcpy(gSet.tz,    prefs.getString("tz",    DEFAULT_TZ).c_str(),        sizeof(gSet.tz));
  strlcpy(gSet.ntp,   prefs.getString("ntp",   DEFAULT_NTP).c_str(),       sizeof(gSet.ntp));
  strlcpy(gSet.host,  prefs.getString("host",  DEFAULT_HOSTNAME).c_str(),  sizeof(gSet.host));
  strlcpy(gSet.webPw, prefs.getString("webpw", "").c_str(),                sizeof(gSet.webPw));
  gSet.lat      = prefs.getFloat("lat", DEFAULT_LAT);
  gSet.lon      = prefs.getFloat("lon", DEFAULT_LON);
  gSet.bright   = prefs.getUChar("bright", DEFAULT_BRIGHTNESS);
  gSet.seconds  = prefs.getBool("sec", DEFAULT_SECONDS != 0);
  gSet.rotation = prefs.getUChar("rot", DEFAULT_ROTATION);
  gSet.updMin   = prefs.getUShort("upd", DEFAULT_UPDATE_MIN);
  gSet.radarKm  = prefs.getUChar("rkm", DEFAULT_RADAR_KM);
  gSet.clockStyle = prefs.getUChar("cs", DEFAULT_CLOCK_STYLE);
  gSet.accent     = prefs.getUChar("acc", DEFAULT_ACCENT);
  gSet.dimOn      = prefs.getBool("dimon", DEFAULT_DIM_ENABLE != 0);
  gSet.dimStart   = prefs.getUShort("dims", DEFAULT_DIM_START);
  gSet.dimEnd     = prefs.getUShort("dime", DEFAULT_DIM_END);
  gSet.dimLevel   = prefs.getUChar("diml", DEFAULT_DIM_LEVEL);
  gSet.dimTv      = prefs.getBool("dimtv", DEFAULT_DIM_TESTCARD != 0);
  gSet.autoOn   = prefs.getBool("auto", DEFAULT_AUTO_ON != 0);
  gSet.autoRnd  = prefs.getBool("arnd", false);
  gSet.autoMask = (uint64_t)prefs.getUInt("amask", DEFAULT_AUTO_MASK) | ((uint64_t)prefs.getUInt("amaskh", 0) << 32);
  gSet.autoSec  = prefs.getUInt("asec", DEFAULT_AUTO_SEC);
  gSet.tickOn    = prefs.getBool("tkon", false);
  gSet.tickLoop  = prefs.getBool("tkloop", false);
  gSet.tickBg    = prefs.getBool("tkbg", true);
  gSet.tickPos   = prefs.getUChar("tkpos", 2);
  gSet.tickBig   = prefs.getUChar("tkbig", 0);
  gSet.tickColor = prefs.getUChar("tkcol", 9);
  gSet.tickSpeed = prefs.getUChar("tkspd", 5);
  gSet.tickPause = prefs.getUShort("tkpause", 10);
  strlcpy(gSet.tick, prefs.getString("tk", DEFAULT_TICK_TEXT).c_str(), sizeof(gSet.tick));
  gSet.ntpOn   = prefs.getBool("ntpon", DEFAULT_NTP_ON != 0);
  gSet.bzHour  = prefs.getBool("bzhr", DEFAULT_HOUR_BEEP != 0);
  gSet.bzQuiet = prefs.getBool("bzq", DEFAULT_BEEP_QUIET != 0);
  gSet.bzTouch = prefs.getBool("bztc", DEFAULT_TOUCH_BEEP != 0);
  gSet.alOn    = prefs.getBool("alon", DEFAULT_ALARM_ON != 0);
  gSet.alTime  = prefs.getUShort("alt", DEFAULT_ALARM_TIME);
  gSet.alDays  = prefs.getUChar("ald", DEFAULT_ALARM_DAYS);
  gSet.ufoOn   = prefs.getBool("ufo", DEFAULT_UFO != 0);
  gSet.sleigh  = prefs.getUChar("sled", DEFAULT_SLEIGH);
  gSet.nyeAuto = prefs.getBool("nyeauto", DEFAULT_NYE_AUTO != 0);
  const bool hasOld = prefs.isKey("cs") && prefs.getUChar("sver", 1) < STYLE_VERSION;
  prefs.end();

  if (hasOld) {                                    // Design-Nummern aus Version 1 umrechnen
    const uint8_t oldCs = gSet.clockStyle;
    gSet.clockStyle = (oldCs < 40) ? (uint8_t)STYLE_V1_SEL[oldCs] : 0;
    uint64_t m = 0;
    for (int i = 0; i < 40; i++)
      if ((gSet.autoMask >> i) & 1) { const int n = STYLE_V1_LIST[i]; if (n >= 0) m |= (1ULL << n); }
    gSet.autoMask = m;
  }

  // Plausibilitaet erzwingen (falls Flash-Inhalt unerwartet ist)
  if (gSet.bright < 5 || gSet.bright > 100) gSet.bright = 100;
  if (gSet.rotation != 1 && gSet.rotation != 3) gSet.rotation = 1;
  if (gSet.updMin < 5 || gSet.updMin > 120) gSet.updMin = DEFAULT_UPDATE_MIN;
  if (gSet.radarKm < 5 || gSet.radarKm > 100) gSet.radarKm = DEFAULT_RADAR_KM;
  if (!(gSet.lat >= -90.0f && gSet.lat <= 90.0f)) gSet.lat = DEFAULT_LAT;
  if (!(gSet.lon >= -180.0f && gSet.lon <= 180.0f)) gSet.lon = DEFAULT_LON;
  if (gSet.clockStyle >= CLOCK_STYLE_COUNT) gSet.clockStyle = 0;
  if (gSet.accent >= 9) gSet.accent = 0;
  if (gSet.tickPos > 2) gSet.tickPos = 2;
  if (gSet.tickBig > 1) gSet.tickBig = 0;
  if (gSet.tickColor > 9) gSet.tickColor = 9;
  if (gSet.tickSpeed < 1 || gSet.tickSpeed > 10) gSet.tickSpeed = 5;
  if (gSet.tickPause > 3600) gSet.tickPause = 10;
  gSet.autoMask &= (1ULL << CLOCK_STYLE_COUNT) - 1;
  if (gSet.autoSec < 10 || gSet.autoSec > 86400) gSet.autoSec = DEFAULT_AUTO_SEC;
  if (gSet.dimStart >= 1440) gSet.dimStart = DEFAULT_DIM_START;
  if (gSet.dimEnd >= 1440)   gSet.dimEnd = DEFAULT_DIM_END;
  if (gSet.alTime >= 1440) gSet.alTime = DEFAULT_ALARM_TIME;
  gSet.alDays &= 0x7F;
  if (gSet.sleigh > 2) gSet.sleigh = DEFAULT_SLEIGH;
  if (gSet.dimLevel < 5 || gSet.dimLevel > 100) gSet.dimLevel = DEFAULT_DIM_LEVEL;
  if (gSet.tz[0] == 0)   strlcpy(gSet.tz, DEFAULT_TZ, sizeof(gSet.tz));
  if (hasOld) { settingsSave(); }
  if (gSet.ntp[0] == 0)  strlcpy(gSet.ntp, DEFAULT_NTP, sizeof(gSet.ntp));
  if (gSet.host[0] == 0) strlcpy(gSet.host, DEFAULT_HOSTNAME, sizeof(gSet.host));
}

static void settingsSave() {
  prefs.begin("wuhr", false);
  prefs.putString("ssid", gSet.ssid);
  prefs.putString("pass", gSet.pass);
  prefs.putString("tz", gSet.tz);
  prefs.putString("ntp", gSet.ntp);
  prefs.putString("host", gSet.host);
  prefs.putString("webpw", gSet.webPw);
  prefs.putFloat("lat", gSet.lat);
  prefs.putFloat("lon", gSet.lon);
  prefs.putUChar("bright", gSet.bright);
  prefs.putBool("sec", gSet.seconds);
  prefs.putUChar("rot", gSet.rotation);
  prefs.putUShort("upd", gSet.updMin);
  prefs.putUChar("rkm", gSet.radarKm);
  prefs.putUChar("cs", gSet.clockStyle);
  prefs.putUChar("sver", STYLE_VERSION);
  prefs.putUChar("acc", gSet.accent);
  prefs.putBool("dimon", gSet.dimOn);
  prefs.putUShort("dims", gSet.dimStart);
  prefs.putUShort("dime", gSet.dimEnd);
  prefs.putUChar("diml", gSet.dimLevel);
  prefs.putBool("dimtv", gSet.dimTv);
  prefs.putBool("auto", gSet.autoOn);
  prefs.putBool("arnd", gSet.autoRnd);
  prefs.putUInt("amask", (uint32_t)gSet.autoMask);
  prefs.putUInt("amaskh", (uint32_t)(gSet.autoMask >> 32));
  prefs.putUInt("asec", gSet.autoSec);
  prefs.putBool("tkon", gSet.tickOn);
  prefs.putBool("tkloop", gSet.tickLoop);
  prefs.putBool("tkbg", gSet.tickBg);
  prefs.putUChar("tkpos", gSet.tickPos);
  prefs.putUChar("tkbig", gSet.tickBig);
  prefs.putUChar("tkcol", gSet.tickColor);
  prefs.putUChar("tkspd", gSet.tickSpeed);
  prefs.putUShort("tkpause", gSet.tickPause);
  prefs.putString("tk", gSet.tick);
  prefs.putBool("ntpon", gSet.ntpOn);
  prefs.putBool("bzhr", gSet.bzHour);
  prefs.putBool("bzq", gSet.bzQuiet);
  prefs.putBool("bztc", gSet.bzTouch);
  prefs.putBool("alon", gSet.alOn);
  prefs.putUShort("alt", gSet.alTime);
  prefs.putUChar("ald", gSet.alDays);
  prefs.putBool("ufo", gSet.ufoOn);
  prefs.putUChar("sled", gSet.sleigh);
  prefs.putBool("nyeauto", gSet.nyeAuto);
  prefs.end();
}

// Nur die Helligkeit schreiben (haeufig geaendert, schont den Flash)
static void settingsSaveBrightness() {
  prefs.begin("wuhr", false);
  prefs.putUChar("bright", gSet.bright);
  prefs.end();
}

static void settingsFactoryReset() {
  prefs.begin("wuhr", false);
  prefs.clear();
  prefs.end();
}
