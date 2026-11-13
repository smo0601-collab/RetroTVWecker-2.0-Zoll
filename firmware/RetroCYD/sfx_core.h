#pragma once
/*
  sfx_core.h - kleine Ton-Engine fuer den Lautsprecher-Anschluss des CYD ("SPEAK", GPIO 26 ueber Verstaerker)

  Gleiche Datei in der Wetteruhr und in RetroCYD.
  - Ein Klang ist eine Folge von Noten {Startfrequenz, Endfrequenz, Dauer, Art}:
      Endfrequenz 0 = gleichbleibend, sonst gleitet der Ton linear (Laser, Sirene, Pfeifen)
      Art: NZ Rauschen (Explosion, Donner, Knall), DC ausklingen (Glocke, Gong), VB Vibrato, Frequenz 0 = Pause
  - Rechteckton per LEDC (PWM). Die Lautstaerke steuert das Tastverhaeltnis (50 % = am lautesten).
  - Laeuft in einer eigenen kleinen Task (1 ms Takt), damit Melodien sauber bleiben, auch wenn das Bild
    gerade lange zeichnet. Ohne Ton schlaeft die Task (kostet dann nichts).
  - Vorrang: ein neuer Klang ersetzt einen laufenden nur bei gleichem oder hoeherem Vorrang.
*/
#ifndef SFX_HOST
#include <Arduino.h>
#include <esp_arduino_version.h>
#endif
#include <stdint.h>

namespace sfx {

enum : uint8_t { TN = 0, NZ = 1, DC = 2, VB = 4 };
struct N { uint16_t f0, f1, ms; uint8_t k; };
struct S { const N* n; uint8_t len; };
#define SFX_S(a) (sfx::S{ (a), (uint8_t)(sizeof(a) / sizeof((a)[0])) })

// Noten (Hz)
enum : uint16_t {
  G3 = 196, NA3 = 220, B3 = 247,
  C4 = 262, D4 = 294, E4 = 330, F4 = 349, FS4 = 370, G4 = 392, GS4 = 415, NA4 = 440, NAS4 = 466, B4 = 494,
  C5 = 523, CS5 = 554, D5 = 587, DS5 = 622, E5 = 659, F5 = 698, FS5 = 740, G5 = 784, GS5 = 831, NA5 = 880, NAS5 = 932, B5 = 988,
  C6 = 1047, CS6 = 1109, D6 = 1175, DS6 = 1245, E6 = 1319, F6 = 1397, FS6 = 1480, G6 = 1568, NA6 = 1760, B6 = 1976,
  C7 = 2093, E7 = 2637, G7 = 3136
};

// ---------------- Abspielen (reine Logik, auch am PC testbar) ----------------
struct Req { S s; uint8_t prio, times; bool loop; uint16_t gap; };

struct Player {
  Req cur{};
  bool act = false, inGap = false;
  uint8_t idx = 0, rep = 0;
  uint32_t t0 = 0, lfsr = 0xACE1u;
  uint16_t motor = 0;                 // Dauerton (z. B. Motor), wenn kein Klang laeuft
  void start(const Req& r, uint32_t now) { cur = r; act = r.s.n && r.s.len; idx = 0; rep = 0; inGap = false; t0 = now; }
  void stop() { act = false; }
  // liefert Frequenz (0 = still) und Pegel 0..255 fuer den Zeitpunkt now
  void step(uint32_t now, uint32_t& f, uint32_t& lvl) {
    f = 0; lvl = 0;
    while (act) {
      const uint32_t dur = inGap ? cur.gap : cur.s.n[idx].ms;
      if (now - t0 < dur) break;
      t0 += dur;
      if (inGap) { inGap = false; continue; }
      if (++idx >= cur.s.len) {
        idx = 0; rep++;
        if (cur.loop || rep < cur.times) { if (cur.gap) inGap = true; }
        else act = false;
      }
    }
    if (act && !inGap) {
      const N& n = cur.s.n[idx];
      if (n.f0) {
        const int32_t t = (int32_t)(now - t0), d = n.ms ? n.ms : 1;
        int32_t fr = n.f0;
        if (n.f1) fr += ((int32_t)n.f1 - n.f0) * t / d;
        if (n.k & NZ) {                                   // Rauschen: Frequenz springt jede ms zufaellig
          lfsr ^= lfsr << 13; lfsr ^= lfsr >> 17; lfsr ^= lfsr << 5;
          fr = fr * (40 + (int32_t)(lfsr % 120)) / 100;
        }
        if (n.k & VB) {                                   // Vibrato ca. 6 Hz, +-3 %
          const int32_t ph = (int32_t)(now % 166);
          const int32_t tri = ph < 83 ? ph - 41 : 124 - ph;   // -41..41
          fr += fr * tri * 3 / 4100;
        }
        f = (uint32_t)(fr < 60 ? 60 : fr);
        lvl = 255;
        if (n.k & DC) lvl = (uint32_t)(255 - 235 * t / d);
      }
    }
    if (!act && motor) { f = motor; lvl = 200; }
  }
};

#ifndef SFX_HOST
// ---------------- Ausgabe auf dem ESP32 ----------------
static Player pl;
static Req pend{};
static volatile uint32_t pendSer = 0;
static volatile bool pendStop = false;
static volatile uint8_t curPrio = 0;           // Vorrang des laufenden Klangs (0 = nichts)
static volatile bool active = false;
static volatile uint8_t vol = 60;              // 0..100
static volatile uint16_t motorF = 0;
static portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
static TaskHandle_t task = nullptr;
static int pin = -1;
static uint8_t chan = 0;
static uint32_t lastF = 0, lastD = 0;

static void hwFreq(uint32_t f) {
#if ESP_ARDUINO_VERSION_MAJOR >= 3
  ledcChangeFrequency(pin, f, 10);
#else
  ledcChangeFrequency(chan, f, 10);
#endif
}
static void hwDuty(uint32_t d) {
#if ESP_ARDUINO_VERSION_MAJOR >= 3
  ledcWrite(pin, d);
#else
  ledcWrite(chan, d);
#endif
}
static void out(uint32_t f, uint32_t lvl) {
  uint32_t d = 0;
  if (f && lvl && vol) {
    if (f > 9000) f = 9000;
    if (f < 80) f = 80;                                   // LEDC: tiefer geht mit 10 Bit nicht sicher
    const uint32_t v = vol;
    const uint32_t base = 1 + 511 * v * v * v / 1000000;   // 1..512 (kubisch: leise Stufen fein einstellbar)
    d = base * lvl / 255;
    if (!d) d = 1;
  }
  if (d && f != lastF) { hwFreq(f); lastF = f; lastD = 0xFFFF; }
  if (d != lastD) { hwDuty(d); lastD = d; }
}

static void taskFn(void*) {
  uint32_t seen = 0;
  for (;;) {
    if (!pl.act && !motorF) { out(0, 0); active = false; curPrio = 0; ulTaskNotifyTake(pdTRUE, portMAX_DELAY); }
    const uint32_t now = millis();
    portENTER_CRITICAL(&mux);
    const bool st = pendStop; pendStop = false;
    const bool nw = pendSer != seen; seen = pendSer;
    const Req r = pend;
    portEXIT_CRITICAL(&mux);
    if (st) pl.stop();
    if (nw) pl.start(r, now);
    pl.motor = motorF;
    uint32_t f, lvl;
    pl.step(now, f, lvl);
    if (!pl.act) curPrio = 0;
    out(f, lvl);
    vTaskDelay(1);
  }
}

// ch: LEDC-Kanal (nur arduino-esp32 2.x; bei 3.x wird er automatisch gewaehlt)
static void begin(int p, uint8_t ch = 0) {
  if (task || p < 0) return;
  pin = p; chan = ch;
#if ESP_ARDUINO_VERSION_MAJOR >= 3
  ledcAttach(pin, 1000, 10);
#else
  ledcSetup(chan, 1000, 10);
  ledcAttachPin(pin, chan);
#endif
  hwDuty(0); lastF = 1000; lastD = 0;
  xTaskCreatePinnedToCore(taskFn, "sfx", 2560, nullptr, 3, &task, ARDUINO_RUNNING_CORE);
}
static bool ready() { return task != nullptr; }
static void wake() { if (task) xTaskNotifyGive(task); }

// Klang abspielen. times = wie oft, loop = endlos (bis stop), gap = Pause zwischen den Wiederholungen (ms)
static bool play(S s, uint8_t prio = 2, uint8_t times = 1, bool loop = false, uint16_t gap = 0) {
  if (!task || !s.n || !s.len) return false;
  if (active && prio < curPrio) return false;
  portENTER_CRITICAL(&mux);
  pend = Req{s, prio, (uint8_t)(times ? times : 1), loop, gap};
  pendSer = pendSer + 1;
  pendStop = false;
  curPrio = prio; active = true;
  portEXIT_CRITICAL(&mux);
  wake();
  return true;
}
static void stop() {
  if (!task) return;
  portENTER_CRITICAL(&mux);
  pendStop = true; curPrio = 0;
  portEXIT_CRITICAL(&mux);
  wake();
}
static bool busy() { return active && curPrio > 0; }
static uint8_t prio() { return active ? curPrio : 0; }
static void setVolume(uint8_t v) { vol = v > 100 ? 100 : v; }
// Dauerton (0 = aus); laeuft nur, solange kein anderer Klang spielt
static void motor(uint16_t f) { if (motorF != f) { motorF = f; wake(); } }

// Einzelton zum Zusammenbauen im Programm (z. B. Tonhoehe nach Punktzahl); kleiner Ringpuffer
static N dyn[4][2];
static uint8_t dynI = 0;
static bool tone(uint16_t f0, uint16_t f1, uint16_t ms, uint8_t k = TN, uint8_t prio = 2, uint16_t f2 = 0, uint16_t ms2 = 0) {
  N* d = dyn[dynI = (dynI + 1) & 3];
  d[0] = N{f0, f1, ms, k};
  d[1] = N{f2, 0, ms2, k};
  return play(S{d, (uint8_t)(f2 && ms2 ? 2 : 1)}, prio);
}
#endif

}  // namespace sfx
