#pragma once
/*
  buzzer.h - Ton fuer Wecker, Stundensignal, Touch-Klick und (CYD) Klang-Effekte

  ESP32-C3: aktiver Piezo-Summer (3 V, mit eingebautem Tongeber), einfacher Ausgang an/aus. Die Muster laufen ohne
  delay() in der Hauptschleife: gerade Eintraege = Summer an (ms), ungerade = Pause (ms).
  CYD: kleiner Lautsprecher am Anschluss "SPEAK" (GPIO 26). Hier gibt es Melodien und Effekte (sfx_core.h):
  waehlbarer Weckton und Stundengong, Lautstaerke, Einblendungen an Feiertagen, Silvester, UFO, Wetterwechsel.

  Anschluss: siehe config.h (PIN_BUZZER) und README, Abschnitt "Summer und Wecker".
*/
#include <Arduino.h>
#include "config.h"

#if PIN_BUZZER >= 0 && BUZZER_TONE_HZ > 0
#define SND_RICH 1
#include "sfx_core.h"
#else
#define SND_RICH 0
#endif

// Anzahl der Auswahlmoeglichkeiten (auch fuer settings.h / webui.h)
#define SND_AL_COUNT  9     // Wecktoene
#define SND_HR_COUNT  6     // Stundengong
// Effekte (Bits in gSet.sndFx)
enum : uint8_t { FXB_POP = 1, FXB_NYE = 2, FXB_UFO = 4, FXB_STYLE = 8, FXB_WX = 16 };
#define SND_FX_DEFAULT (FXB_POP | FXB_NYE | FXB_UFO | FXB_WX)
// Effekt-Kennungen (Test im Browser, Ereignisse aus den Designs)
enum : uint8_t {
  FX_XMAS, FX_NEWYEAR, FX_CARNIVAL, FX_HALLOWEEN, FX_LOVE, FX_EASTER, FX_SCHOOL, FX_PARTY,   // Feiertags-Einblendung
  FX_NYTICK, FX_UFO, FX_STYLE, FX_RAIN, FX_STORM, FX_SNOW, FX_COUNT
};
#define FX_POP_COUNT 8      // die ersten 8 sind Feiertags-Klaenge

// Ereignisse aus den Designs (z. B. UFO taucht auf); werden in soundTick() abgeholt
static volatile uint16_t gSndEv = 0;
static inline void sndEvent(uint8_t fx) { gSndEv |= (uint16_t)(1u << fx); }

namespace bz {

static const uint16_t P_CLICK[] = {25};                                   // Quittung (Touch)
static const uint16_t P_HOUR[]  = {80, 120, 80};                          // Stundensignal: zweimal kurz
static const uint16_t P_TEST[]  = {150, 100, 150, 100, 400};              // Test
static const uint16_t P_ALARM[] = {100, 100, 100, 100, 100, 100, 100, 700};  // Wecker (wiederholt)
#define BZ_PAT(p) (p), (uint8_t)(sizeof(p) / sizeof((p)[0]))

static constexpr bool present() { return PIN_BUZZER >= 0; }
static constexpr bool rich() { return SND_RICH != 0; }

#if SND_RICH
// =====================================================================
// Lautsprecher (CYD): Klaenge
// =====================================================================
using namespace sfx;
// Vorrang: Klick < Effekt < Gong < Test < Wecker
enum : uint8_t { PR_CLICK = 1, PR_FX = 2, PR_CHIME = 3, PR_TEST = 4, PR_ALARM = 5 };

// ---- Wecktoene (jeweils ein Durchgang; der Wecker wiederholt mit Pause) ----
static const N A_BEEP[] = {{2000,0,100,TN},{0,0,100,TN},{2000,0,100,TN},{0,0,100,TN},{2000,0,100,TN},{0,0,100,TN},{2000,0,100,TN}};
static const N A_BELL[] = {{2500,0,40,DC},{2650,0,40,DC},{2500,0,40,DC},{2650,0,40,DC},{2500,0,40,DC},{2650,0,40,DC},
                           {2500,0,40,DC},{2650,0,40,DC},{2500,0,40,DC},{2650,0,40,DC},{2500,0,40,DC},{2650,0,40,DC},
                           {2500,0,40,DC},{2650,0,40,DC},{2500,0,40,DC},{2650,0,40,DC},{2500,0,40,DC},{2650,0,40,DC}};
static const N A_FANFARE[] = {{G5,0,110,TN},{0,0,30,TN},{G5,0,110,TN},{0,0,30,TN},{G5,0,110,TN},{0,0,30,TN},{C6,0,420,TN},{0,0,60,TN},
                              {G5,0,150,TN},{0,0,20,TN},{C6,0,150,TN},{E6,0,700,DC}};
static const N A_CUCKOO[] = {{C6,0,260,DC},{0,0,90,TN},{NA5,0,420,DC},{0,0,320,TN},{C6,0,260,DC},{0,0,90,TN},{NA5,0,420,DC}};
static const N A_GONG[] = {{E5,0,700,DC},{0,0,40,TN},{C5,0,1100,DC}};
static const N A_MORNING[] = {   // Grieg, "Morgenstimmung" (Peer Gynt), Anfang
  {G5,0,330,TN},{E5,0,165,TN},{D5,0,165,TN},{C5,0,165,TN},{D5,0,165,TN},{E5,0,165,TN},
  {G5,0,330,TN},{E5,0,165,TN},{D5,0,165,TN},{C5,0,165,TN},{D5,0,165,TN},{E5,0,165,TN},{D5,0,165,TN},{E5,0,165,TN},
  {G5,0,330,TN},{E5,0,165,TN},{G5,0,330,TN},{NA5,0,165,TN},{E5,0,330,TN},{NA5,0,165,TN},{G5,0,330,TN},{E5,0,165,TN},
  {D5,0,330,TN},{C5,0,700,DC}};
static const N A_ARCADE[] = {{C5,0,70,TN},{E5,0,70,TN},{G5,0,70,TN},{C6,0,70,TN},{E6,0,70,TN},{G6,0,70,TN},{C7,0,240,DC},
                             {0,0,120,TN},{G6,0,70,TN},{C7,0,320,DC}};
static const N A_SIREN[] = {{600,1400,450,TN},{1400,600,450,TN},{600,1400,450,TN},{1400,600,450,TN}};
static const N A_BIRDS[] = {{3000,4200,60,TN},{0,0,40,TN},{3200,4400,60,TN},{0,0,40,TN},{4000,2800,90,TN},{0,0,260,TN},
                            {3500,4500,45,TN},{0,0,30,TN},{3600,4600,45,TN},{0,0,30,TN},{3700,4700,45,TN},{0,0,30,TN},{4200,3000,110,TN}};
struct Tone { S s; uint16_t gap; };
static const Tone ALARMS[SND_AL_COUNT] = {
  {SFX_S(A_BEEP), 700}, {SFX_S(A_BELL), 700}, {SFX_S(A_FANFARE), 900}, {SFX_S(A_CUCKOO), 1200}, {SFX_S(A_GONG), 900},
  {SFX_S(A_MORNING), 900}, {SFX_S(A_ARCADE), 600}, {SFX_S(A_SIREN), 300}, {SFX_S(A_BIRDS), 900}};

// ---- Stundengong ----
static const N H_BEEP[] = {{1800,0,80,TN},{0,0,120,TN},{1800,0,80,TN}};
static const N H_WESTMINSTER[] = {{E5,0,450,DC},{C5,0,450,DC},{D5,0,450,DC},{G4,0,850,DC},{0,0,150,TN},
                                  {G4,0,450,DC},{D5,0,450,DC},{E5,0,450,DC},{C5,0,1100,DC}};
static const N H_CUCKOO[] = {{C6,0,240,DC},{0,0,80,TN},{NA5,0,380,DC}};          // einmal je Stunde
static const N H_BELL[] = {{C5,0,950,DC}};                                       // einmal je Stunde
static const N H_COIN[] = {{B5,0,70,TN},{E6,0,450,DC}};
static const N H_GLOCKEN[] = {{C6,0,170,DC},{E6,0,170,DC},{G6,0,170,DC},{C7,0,650,DC}};
static const S HOURS[SND_HR_COUNT] = {SFX_S(H_BEEP), SFX_S(H_WESTMINSTER), SFX_S(H_CUCKOO), SFX_S(H_BELL), SFX_S(H_COIN), SFX_S(H_GLOCKEN)};
static bool hourCounts(uint8_t i) { return i == 2 || i == 3; }    // Kuckuck und Glocke zaehlen die Stunde

// ---- Effekte ----
static const N F_XMAS[] = {   // "Jingle Bells" (traditionell), Refrain-Anfang, mit Glockenklang
  {E5,0,170,DC},{0,0,20,TN},{E5,0,170,DC},{0,0,20,TN},{E5,0,360,DC},{0,0,30,TN},
  {E5,0,170,DC},{0,0,20,TN},{E5,0,170,DC},{0,0,20,TN},{E5,0,360,DC},{0,0,30,TN},
  {E5,0,170,DC},{G5,0,170,DC},{C5,0,260,DC},{D5,0,90,DC},{E5,0,700,DC}};
static const N F_NEWYEAR[] = {{900,2600,650,TN},{0,0,60,TN},{500,0,450,NZ|DC},{0,0,120,TN},
                              {1100,3000,550,TN},{0,0,50,TN},{700,0,500,NZ|DC},{0,0,80,TN},{400,0,700,NZ|DC}};
static const N F_CARNIVAL[] = {   // Tusch: tä-tä-tä-täää (dreimal)
  {G5,0,90,TN},{0,0,25,TN},{G5,0,90,TN},{0,0,25,TN},{G5,0,90,TN},{0,0,25,TN},{C6,0,380,VB},{0,0,90,TN},
  {NA5,0,90,TN},{0,0,25,TN},{NA5,0,90,TN},{0,0,25,TN},{NA5,0,90,TN},{0,0,25,TN},{D6,0,380,VB},{0,0,90,TN},
  {B5,0,90,TN},{0,0,25,TN},{B5,0,90,TN},{0,0,25,TN},{B5,0,90,TN},{0,0,25,TN},{E6,0,800,VB|DC}};
static const N F_HALLOWEEN[] = {{330,250,700,VB},{0,0,120,TN},{311,220,900,VB|DC},{0,0,200,TN},{1600,900,180,TN},{900,1500,180,TN},{1500,700,300,DC}};
static const N F_LOVE[] = {{C5,0,80,DC},{E5,0,80,DC},{G5,0,80,DC},{C6,0,80,DC},{E6,0,80,DC},{G6,0,80,DC},{C7,0,700,DC}};
static const N F_EASTER[] = {{500,1100,90,TN},{0,0,80,TN},{550,1200,90,TN},{0,0,80,TN},{600,1300,90,TN},{0,0,160,TN},{C6,0,260,DC},{0,0,80,TN},{NA5,0,400,DC}};
static const N F_SCHOOL[] = {{2500,0,40,DC},{2650,0,40,DC},{2500,0,40,DC},{2650,0,40,DC},{2500,0,40,DC},{2650,0,40,DC},
                             {2500,0,40,DC},{2650,0,40,DC},{2500,0,40,DC},{2650,0,40,DC},{0,0,150,TN},
                             {C6,0,110,TN},{E6,0,110,TN},{G6,0,110,TN},{C7,0,450,DC}};
static const N F_PARTY[] = {{520,560,320,VB},{0,0,60,TN},{520,760,520,VB},{0,0,90,TN},{G5,0,100,TN},{C6,0,100,TN},{E6,0,500,DC}};
static const N F_NYTICK[] = {{1000,0,60,DC}};
static const N F_UFO[] = {{700,1500,260,VB},{1500,700,260,VB},{700,1500,260,VB},{1500,700,260,VB},{700,1700,500,VB|DC}};
static const N F_STYLE[] = {{C6,0,45,TN},{G6,0,90,DC}};
static const N F_RAIN[] = {{2400,1700,25,TN},{0,0,140,TN},{2800,2000,25,TN},{0,0,90,TN},{2200,1600,25,TN},{0,0,210,TN},
                           {2600,1900,25,TN},{0,0,120,TN},{3000,2100,25,TN},{0,0,70,TN},{2300,1700,25,TN}};
static const N F_STORM[] = {{2200,500,90,NZ},{0,0,60,TN},{260,60,1600,NZ|DC}};
static const N F_SNOW[] = {{E7,0,220,DC},{0,0,90,TN},{B6,0,220,DC},{0,0,90,TN},{G6,0,220,DC},{0,0,90,TN},{E6,0,500,DC}};
static const S FXS[FX_COUNT] = {SFX_S(F_XMAS), SFX_S(F_NEWYEAR), SFX_S(F_CARNIVAL), SFX_S(F_HALLOWEEN), SFX_S(F_LOVE), SFX_S(F_EASTER),
                                SFX_S(F_SCHOOL), SFX_S(F_PARTY), SFX_S(F_NYTICK), SFX_S(F_UFO), SFX_S(F_STYLE), SFX_S(F_RAIN),
                                SFX_S(F_STORM), SFX_S(F_SNOW)};
static const N C_CLICK[] = {{1800,0,12,TN}};

static void init() { sfx::begin(PIN_BUZZER); }
static void stop() { sfx::stop(); }
static bool busy() { return sfx::busy(); }
static void tick(uint32_t) {}
static void click() { sfx::play(SFX_S(C_CLICK), PR_CLICK); }
static void alarm(uint8_t i) { const Tone& t = ALARMS[i < SND_AL_COUNT ? i : 0]; sfx::play(t.s, PR_ALARM, 1, true, t.gap); }
static void chime(uint8_t i, int hour, uint8_t prio = PR_CHIME) {
  if (i >= SND_HR_COUNT) i = 0;
  int n = 1;
  if (hourCounts(i)) { n = hour % 12; if (n == 0) n = 12; }
  sfx::play(HOURS[i], prio, (uint8_t)n, false, i == 2 ? 300 : 120);
}
static void fx(uint8_t id, uint8_t prio = PR_FX) { if (id < FX_COUNT) sfx::play(FXS[id], prio); }
static void alarmTest(uint8_t i) { const Tone& t = ALARMS[i < SND_AL_COUNT ? i : 0]; sfx::play(t.s, PR_TEST, 1); }
static void test() { chime(0, 0, PR_TEST); }
static void setVolume(uint8_t v) { sfx::setVolume(v); }

#else
// =====================================================================
// Aktiver Summer (ESP32-C3): nur an/aus
// =====================================================================
static const uint16_t* pat = nullptr;
static uint8_t  len = 0, pos = 0;
static bool     rep = false;
static uint32_t t0 = 0;
static bool level = false;

static void out(bool on) {
#if PIN_BUZZER >= 0
  level = on;
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
static void click() { play(BZ_PAT(P_CLICK)); }
static void alarm(uint8_t) { play(BZ_PAT(P_ALARM), true); }
static void chime(uint8_t, int, uint8_t = 0) { play(BZ_PAT(P_HOUR)); }
static void fx(uint8_t, uint8_t = 0) {}
static void alarmTest(uint8_t) { play(BZ_PAT(P_ALARM)); }
static void test() { play(BZ_PAT(P_TEST)); }
static void setVolume(uint8_t) {}
#endif

}  // namespace bz

// Feiertags-Klang zur Einblendung (Kennung aus cal.h; wird nach cal.h eingebunden). 255 = still (stille Feiertage)
static uint8_t sndPopFx(uint8_t id) {
  using namespace cal;
  switch (id) {
    case CD_KARFREITAG: case CD_ALLERHEILIGEN: case CD_BUSSTAG: return 255;
    case CD_WEIH1: case CD_WEIH2: case CD_HEILIGABEND: case CD_NIKOLAUS:
    case CD_ADVENT1: case CD_ADVENT2: case CD_ADVENT3: case CD_ADVENT4: return FX_XMAS;
    case CD_NEUJAHR: case CD_SILVESTER: return FX_NEWYEAR;
    case CD_WEIBER: case CD_ROSENMO: case CD_FASCHING: case CD_MARTIN: return FX_CARNIVAL;   // 11.11.: Sessionsbeginn
    case CD_HALLOWEEN: return FX_HALLOWEEN;
    case CD_VALENTIN: case CD_MUTTERTAG: return FX_LOVE;
    case CD_OSTERSO: case CD_OSTERMO: return FX_EASTER;
    case CD_FERIENSTART: case CD_LASTSCHOOL: return FX_SCHOOL;
    default: return FX_PARTY;
  }
}

// ---- Wecker-Zustand (Steuerung in WetterUhr.ino: soundTick) ----
static bool     gAlarmRinging = false;
static uint32_t gAlarmSince   = 0;

static void alarmStop() {
  if (!gAlarmRinging) return;
  gAlarmRinging = false;
  bz::stop();
}
