#pragma once
/*
  buzzer.h - aktiver Piezo-Summer (3 V, mit eingebautem Tongeber)

  Ein aktiver Summer piept von selbst, sobald Spannung anliegt. Es reicht also ein
  einfacher Ausgang (an/aus), kein PWM-Ton. Die Muster laufen ohne delay() in der
  Hauptschleife: gerade Eintraege = Summer an (ms), ungerade = Pause (ms).

  Anschluss: siehe config.h (PIN_BUZZER) und README, Abschnitt "Summer und Wecker".
*/
#include <Arduino.h>
#include "config.h"

namespace bz {

static const uint16_t P_CLICK[] = {25};                                   // Quittung (Touch)
static const uint16_t P_HOUR[]  = {80, 120, 80};                          // Stundensignal: zweimal kurz
static const uint16_t P_TEST[]  = {150, 100, 150, 100, 400};              // Test
static const uint16_t P_ALARM[] = {100, 100, 100, 100, 100, 100, 100, 700};  // Wecker (wiederholt)
#define BZ_PAT(p) (p), (uint8_t)(sizeof(p) / sizeof((p)[0]))

static const uint16_t* pat = nullptr;
static uint8_t  len = 0, pos = 0;
static bool     rep = false;
static uint32_t t0 = 0;

static constexpr bool present() { return PIN_BUZZER >= 0; }

static void out(bool on) {
#if PIN_BUZZER >= 0
  digitalWrite(PIN_BUZZER, (on == (BUZZER_ACTIVE_HIGH != 0)) ? HIGH : LOW);
#else
  (void)on;
#endif
}

static void init() {
#if PIN_BUZZER >= 0
  out(false);                    // Pegel vor dem Umschalten auf Ausgang setzen (kein Piepser beim Start)
  pinMode(PIN_BUZZER, OUTPUT);
  out(false);
#endif
}

static void stop() { pat = nullptr; out(false); }
static bool busy() { return pat != nullptr; }

// Muster starten (ersetzt ein laufendes). loop = endlos wiederholen bis stop().
static void play(const uint16_t* p, uint8_t n, bool loop = false) {
  if (!present() || !p || !n) return;
  pat = p; len = n; pos = 0; rep = loop; t0 = millis();
  out(true);
}

static void tick(uint32_t now) {
  if (!pat) return;
  if (now - t0 < pat[pos]) return;
  t0 = now;
  if (++pos >= len) {
    if (!rep) { stop(); return; }
    pos = 0;
  }
  out((pos & 1) == 0);
}

}  // namespace bz

// ---- Wecker-Zustand (Steuerung in WetterUhr.ino: soundTick) ----
static bool     gAlarmRinging = false;
static uint32_t gAlarmSince   = 0;

static void alarmStop() {
  if (!gAlarmRinging) return;
  gAlarmRinging = false;
  bz::stop();
}
