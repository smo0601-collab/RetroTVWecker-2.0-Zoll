// Teil von RetroCYD - wird von RetroCYD.ino eingebunden
// ============================================================================
//  SPIEL-SOUNDS ueber den Lautsprecher-Anschluss des CYD ("SPEAK", GPIO 26)
// ============================================================================
//  - Ton-Engine: sfx_core.h (gleiche Datei wie in der Wetteruhr)
//  - Jede Vibration (rumble) klingt automatisch mit (Tick, Rums, Krach, Explosion
//    je nach Staerke). Wichtige Ereignisse haben eigene Klaenge (Schuss, Sprung,
//    Muenze, Reihe weg ...); die haben Vorrang vor dem automatischen Klang.
//  - Menue: Klick beim Blaettern, Auswahl, Zurueck; Spielstart, Pause,
//    Game Over und Rekord-Fanfare.
//  - Ein/aus, Lautstaerke und alle Klaenge zum Anhoeren: Web-Oberflaeche.
// ============================================================================
#pragma once
#include "sfx_core.h"

namespace Snd {
  using namespace sfx;
  enum : uint8_t { P_AUTO = 1, P_GAME = 2, P_BIG = 3 };

  enum Id : uint8_t {
    MOVE, SELECT, BACK, START, OVER, RECORD, PAUSE,
    TICK, THUD, CRASH, BOOM,
    SHOOT, LASER, ENEMY, JUMP, FLAP, EAT, COIN, POWER, LINE, LINE4, ROTATE, DROP,
    BOUNCE, WALL, BRICK, LIFE, DIE, WIN, LOSE, CARD, FLIP, MATCH, MISS, TOGGLE, SLIDE,
    COUNT1, GO, LOOP, DIG, SPLASH, HONK,
    NUM
  };

  static const N s_move[]   = {{1400,0,12,TN}};
  static const N s_select[] = {{C6,0,50,TN},{G6,0,90,DC}};
  static const N s_back[]   = {{G5,0,50,TN},{C5,0,90,DC}};
  static const N s_start[]  = {{C5,0,80,TN},{E5,0,80,TN},{G5,0,80,TN},{C6,0,260,DC}};
  static const N s_over[]   = {{G5,0,170,TN},{0,0,30,TN},{E5,0,170,TN},{0,0,30,TN},{C5,0,170,TN},{0,0,30,TN},{C5,150,650,VB|DC}};
  static const N s_record[] = {{G5,0,90,TN},{0,0,25,TN},{G5,0,90,TN},{0,0,25,TN},{G5,0,90,TN},{0,0,25,TN},{C6,0,300,TN},{0,0,40,TN},
                               {G5,0,120,TN},{C6,0,120,TN},{E6,0,550,DC}};
  static const N s_pause[]  = {{E6,0,60,TN},{0,0,40,TN},{C6,0,90,DC}};
  static const N s_tick[]   = {{1400,0,22,NZ|DC}};
  static const N s_thud[]   = {{320,120,90,NZ|DC}};
  static const N s_crash[]  = {{900,150,260,NZ|DC}};
  static const N s_boom[]   = {{600,50,650,NZ|DC}};
  static const N s_shoot[]  = {{1800,500,70,TN}};
  static const N s_laser[]  = {{2600,900,90,TN}};
  static const N s_enemy[]  = {{700,280,90,TN}};
  static const N s_jump[]   = {{400,1100,110,TN}};
  static const N s_flap[]   = {{500,820,45,TN}};
  static const N s_eat[]    = {{C6,0,40,TN},{E6,0,70,DC}};
  static const N s_coin[]   = {{B5,0,60,TN},{E6,0,220,DC}};
  static const N s_power[]  = {{C5,0,50,TN},{E5,0,50,TN},{G5,0,50,TN},{C6,0,50,TN},{E6,0,50,TN},{G6,0,220,DC}};
  static const N s_line[]   = {{C6,0,60,TN},{E6,0,60,TN},{G6,0,60,TN},{C7,0,220,DC}};
  static const N s_line4[]  = {{C6,0,60,TN},{E6,0,60,TN},{G6,0,60,TN},{C7,0,120,TN},{0,0,30,TN},{G6,0,60,TN},{C7,0,60,TN},{E7,0,350,DC}};
  static const N s_rotate[] = {{1600,0,15,TN}};
  static const N s_drop[]   = {{220,90,70,DC}};
  static const N s_bounce[] = {{880,0,25,DC}};
  static const N s_wall[]   = {{440,0,25,DC}};
  static const N s_brick[]  = {{1320,0,35,DC}};
  static const N s_life[]   = {{C6,0,80,TN},{E6,0,80,TN},{G6,0,80,TN},{C7,0,80,TN},{0,0,40,TN},{G6,0,80,TN},{C7,0,260,DC}};
  static const N s_die[]    = {{1200,180,550,VB},{0,0,40,TN},{400,60,300,NZ|DC}};
  static const N s_win[]    = {{G5,0,100,TN},{C6,0,100,TN},{E6,0,100,TN},{G6,0,450,DC}};
  static const N s_lose[]   = {{E5,0,200,TN},{DS5,0,200,TN},{D5,0,200,TN},{CS5,0,650,VB|DC}};
  static const N s_card[]   = {{3000,0,28,NZ|DC}};
  static const N s_flip[]   = {{1000,1800,40,TN}};
  static const N s_match[]  = {{E6,0,70,TN},{NA6,0,200,DC}};
  static const N s_miss[]   = {{300,0,120,TN},{250,0,180,DC}};
  static const N s_toggle[] = {{1200,0,25,DC}};
  static const N s_slide[]  = {{500,700,40,TN}};
  static const N s_count1[] = {{NA5,0,150,TN}};
  static const N s_go[]     = {{NA6,0,450,DC}};
  static const N s_loop[]   = {{600,1400,300,VB}};
  static const N s_dig[]    = {{220,0,18,NZ}};
  static const N s_splash[] = {{2500,400,300,NZ|DC}};
  static const N s_honk[]   = {{415,0,140,VB},{0,0,50,TN},{415,0,220,VB}};

  static const S ALL[NUM] = {
    SFX_S(s_move), SFX_S(s_select), SFX_S(s_back), SFX_S(s_start), SFX_S(s_over), SFX_S(s_record), SFX_S(s_pause),
    SFX_S(s_tick), SFX_S(s_thud), SFX_S(s_crash), SFX_S(s_boom),
    SFX_S(s_shoot), SFX_S(s_laser), SFX_S(s_enemy), SFX_S(s_jump), SFX_S(s_flap), SFX_S(s_eat), SFX_S(s_coin), SFX_S(s_power),
    SFX_S(s_line), SFX_S(s_line4), SFX_S(s_rotate), SFX_S(s_drop),
    SFX_S(s_bounce), SFX_S(s_wall), SFX_S(s_brick), SFX_S(s_life), SFX_S(s_die), SFX_S(s_win), SFX_S(s_lose), SFX_S(s_card),
    SFX_S(s_flip), SFX_S(s_match), SFX_S(s_miss), SFX_S(s_toggle), SFX_S(s_slide),
    SFX_S(s_count1), SFX_S(s_go), SFX_S(s_loop), SFX_S(s_dig), SFX_S(s_splash), SFX_S(s_honk)};
  // Namen fuer die Web-Oberflaeche (Anhoeren), gleiche Reihenfolge
  static const char* const NAMES[NUM] = {
    "Menü: blättern", "Menü: auswählen", "Menü: zurück", "Spielstart", "Game Over", "Neuer Rekord", "Pause",
    "Treffer klein", "Rums", "Krach", "Explosion",
    "Schuss", "Laser", "Gegner schießt", "Sprung", "Flattern", "Fressen", "Münze", "Power-up", "Reihe weg", "Vier Reihen",
    "Drehen", "Aufsetzen", "Ball: Schläger", "Ball: Wand", "Stein getroffen", "Extraleben", "Leben verloren", "Gewonnen",
    "Verloren", "Karte", "Karte umdrehen", "Paar gefunden", "Kein Paar", "Licht schalten", "Schieben",
    "Ampel", "Los!", "Looping", "Graben", "Platsch", "Hupe"};

  static bool on = true;
  static uint8_t vol = 50;
  static bool testVol = false;

  static void apply() { setVolume(on ? vol : 0); }
  static void begin(bool enabled, uint8_t v) { on = enabled; vol = v; sfx::begin(26, 0); apply(); }
  static void config(bool enabled, uint8_t v) { on = enabled; vol = v; testVol = false; apply(); }

  static void fx(Id id, uint8_t p = P_GAME) { if (on && id < NUM) play(ALL[id], p); }
  static void big(Id id) { fx(id, P_BIG); }
  // Ton mit veraenderlicher Hoehe (z. B. 2048: je groesser die Zahl, desto hoeher)
  static void pitch(uint16_t f0, uint16_t f1, uint16_t ms, uint8_t k = TN, uint8_t p = P_GAME) { if (on) sfx::tone(f0, f1, ms, k, p); }

  // automatischer Klang zu jeder Vibration (nur, wenn gerade kein eigener Klang laeuft)
  static void hit(uint16_t ms, uint8_t strength) {
    if (!on) return;
    if (strength >= 0xC0 || ms >= 450) play(ALL[BOOM], P_AUTO);
    else if (strength >= 0x80) play(ALL[CRASH], P_AUTO);
    else if (strength >= 0x50) play(ALL[THUD], P_AUTO);
    else play(ALL[TICK], P_AUTO);
  }

  // Anhoeren in der Web-Oberflaeche (Lautstaerke vom Regler, auch wenn der Ton aus ist)
  static void test(int id, int v) {
    if (id < 0) { stop(); return; }
    if (id >= NUM) return;
    if (v >= 0) { setVolume((uint8_t)constrain(v, 0, 100)); testVol = true; }
    play(ALL[id], 4);
  }
  // Motorgeraeusch (Dauerton, z. B. Rennwagen); geht aus, wenn es nicht mehr angefordert wird
  static bool motOn = false;
  static uint32_t motAt = 0;
  static void engine(uint16_t f) { if (!on) return; sfx::motor(f); motOn = true; motAt = millis(); }
  // aus loop(): Test-Lautstaerke zuruecksetzen, Motor aus (Pause, Menue, nicht mehr angefordert)
  static void service(bool inGame) {
    if (testVol && !busy()) { testVol = false; apply(); }
    if (motOn && (!inGame || millis() - motAt > 120)) { sfx::motor(0); motOn = false; }
  }
}
