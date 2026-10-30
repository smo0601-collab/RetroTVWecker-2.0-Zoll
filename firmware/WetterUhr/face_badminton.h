#pragma once
/*
  face_badminton.h - Zifferblatt "Badminton" (Zaehlweise nach den Badminton-Regeln)
  Seitenansicht einer Halle mit Publikum: zwei Spieler (Rot und Blau) spielen ein Match ueber
  bis zu drei Saetze. Gezaehlt wird wie im echten Spiel:
    - Rallye-Punkt-System: jeder Ballwechsel gibt einen Punkt, egal wer aufgeschlagen hat
    - ein Satz geht bis 21 Punkte mit 2 Punkten Vorsprung; bei 29:29 entscheidet der naechste Punkt
    - zwei gewonnene Saetze gewinnen das Match (bei 1:1 entscheidet der dritte Satz)
    - wer den Ballwechsel gewinnt, schlaegt auf; den neuen Satz beginnt der Gewinner des vorigen
    - Aufschlag von rechts bei geradem eigenem Punktestand, von links bei ungeradem ("R"/"L")
    - bei 11 Punkten Pause; nach jedem Satz und im dritten Satz bei 11 Punkten Seitenwechsel
  Spannung:
    - verschiedene Schlaege: Clear (hoch und weit), Drop (kurz hinters Netz), Smash im Sprung mit
      Geschwindigkeitsanzeige, Drive (flach und schnell), Netzball, Lift; der Gegner antwortet passend
    - Ballwechsel unterschiedlich lang, bei Satz- und Matchball besonders lang; Rallye-Zaehler
    - Punkte durch Gewinnschlag (der Gegner hechtet vergeblich), Netzfehler oder Aus; bei knappen
      Aus-Baellen gibt es eine Hawk-Eye-Ueberpruefung, die das Urteil auch kippen kann
    - Momentum: Serien, Aufholjagden, enge Saetze; wer den ersten Satz verliert, kaempft sich oft zurueck
    - Anzeigen "SATZBALL", "MATCHBALL", "VERLAENGERUNG", jubelndes Publikum, Sieger jubelt, Verlierer
      laesst den Kopf haengen; zum Matchgewinn Konfetti
  Der Spielstand steht gross auf dem Hallenboden (so, wie die Spieler stehen), daneben je zwei Lampen
  fuer gewonnene Saetze, ein orangefarbener Punkt markiert den Aufschlaeger. Oben die Uhrzeit.
  Zur vollen Minute spruehen Funken am Netz.
  Spielstand bleibt erhalten: Beim Wechsel zu einem anderen Design laeuft das Match spaeter genau dort
  weiter. Zusaetzlich wird der Stand im Flash gesichert (nach jedem Punkt, hoechstens alle 8 s; Satz-
  und Seitenwechsel sofort), damit er auch einen Neustart oder Stromausfall uebersteht. Erst wenn ein
  Sieger feststeht, beginnt ein neues Match.
  Uebergang: Jalousie (Streifen schliessen sich).
*/
#include "faces_fx.h"
#include <Preferences.h>
#include <stddef.h>

namespace f_bad {
using namespace fc;

enum : uint8_t { W_WALL = 1, W_WALL2, W_FLOOR, W_FLOOR2, W_LINE, W_NET, W_POST, W_TAPE, W_SKIN, W_SHIRT_L, W_SHIRT_R,
                 W_SHORTS, W_SHOE, W_FRAME, W_STRING, W_HANDLE, W_FEATH, W_CORK, W_SHAD, W_DIG, W_SH, W_TXT, W_SPARK,
                 W_PANEL, W_GOLD, W_LAMPOFF, W_STAND, W_STAND2, W_FAN1, W_FAN2, W_FAN3, W_FAN4, W_HAIR, W_TRAIL,
                 W_HAWK, W_IN, W_OUT, W_FLASH };

static const int FLOOR_Y = 100, FEET_Y = 106, NET_X = 80, NET_TOP = 78;
static const int HOME[2] = {42, 118};
static const float WINDUP = 0.12f, SWING = 0.28f, BACK = 0.30f;
static const uint32_t CONTACT_MS = 290;            // Schlagbeginn bis Ballkontakt

// Schlagarten
enum : uint8_t { SH_SERVE_LOW, SH_SERVE_HIGH, SH_CLEAR, SH_DROP, SH_SMASH, SH_DRIVE, SH_NET, SH_LIFT };
// Ende eines Ballwechsels
enum : uint8_t { E_RET, E_WIN, E_NET, E_OUT, E_CH_IN, E_CH_OUT };

struct Flight { float sx, sy, ex, ey, H; uint32_t t0; uint16_t T; uint8_t shot, end; };

enum : uint8_t { ST_RALLY, ST_REST, ST_BANNER, ST_SWAP };
enum : uint8_t { BK_PAUSE, BK_SWAP11, BK_GAME, BK_MATCH };
enum : uint8_t { ACT_SERVE, ACT_NEWGAME };

static Flight   fl;
static uint8_t  st, hitter, figAt[2];              // figAt[Platz] = Figur (0 Rot, 1 Blau) auf dem Platz (0 links, 1 rechts)
static uint8_t  pts[2], games[2], gameNo, server, bannerKind, swapAct, gameWinner, winnerFig;
static uint8_t  planWin, planEnd, nextShot, rally, setFirstWinner;
static int8_t   streak;                            // >0: Serie von Rot, <0: Serie von Blau
static float    setBias;                           // Grundchance von Rot in diesem Satz
static bool     intervalDone, serving;
static int      hitsLeft;
static uint32_t lastT, until, swingAt[2], jumpAt[2], lungeAt[2], celebUntil, cheerAt, cheerUntil, flashUntil, scoreFlash[2];
static uint8_t  cheerPower, celebFig;
static float    px[2], tx[2], fdir[2], trailX[4], trailY[4];
static uint8_t  lastMm;
static char     flash[24];
static uint8_t  flashCol;
static Sparks<40> sp;

// ---- Spielstand sichern (RAM beim Designwechsel, Flash fuer Neustart/Stromausfall) ----
static bool     live = false;                      // Match laeuft (Zustand im RAM gueltig)
static bool     loaded = false;                    // Flash nach dem Start schon gelesen
static bool     dirty = false, resumed = false;
static uint32_t savedAt = 0;
static const uint32_t SAVE_GAP_MS = 8000;
struct Saved { uint8_t ver, p0, p1, g0, g1, gameNo, server, fig0, firstW, interval; int8_t streak; uint8_t sum; float bias; };

static uint8_t savedSum(const Saved& v) {
  const uint8_t* b = (const uint8_t*)&v;
  uint8_t x = 0x5A;
  for (size_t i = 0; i < offsetof(Saved, sum); i++) x = (uint8_t)((x << 1 | x >> 7) ^ b[i]);
  return x;
}
static void saveNow(uint32_t now) {
  Saved v;
  memset(&v, 0, sizeof(v));
  v.ver = 1; v.p0 = pts[0]; v.p1 = pts[1]; v.g0 = games[0]; v.g1 = games[1]; v.gameNo = gameNo; v.server = server;
  v.fig0 = figAt[0]; v.firstW = setFirstWinner; v.interval = intervalDone; v.streak = streak; v.bias = setBias;
  v.sum = savedSum(v);
  Preferences p;
  if (p.begin("badm", false)) { p.putBytes("m", &v, sizeof(v)); p.end(); }
  dirty = false; savedAt = now ? now : 1;
}
static void saveClear() {
  Preferences p;
  if (p.begin("badm", false)) { p.remove("m"); p.end(); }
  dirty = false;
}
static bool loadSaved() {
  Saved v;
  Preferences p;
  if (!p.begin("badm", true)) return false;
  const size_t n = p.getBytes("m", &v, sizeof(v));
  p.end();
  if (n != sizeof(v) || v.ver != 1 || v.sum != savedSum(v)) return false;
  if (v.g0 > 1 || v.g1 > 1 || v.gameNo < 1 || v.gameNo > 3 || v.p0 > 30 || v.p1 > 30 || v.server > 1 || v.fig0 > 1) return false;
  pts[0] = v.p0; pts[1] = v.p1; games[0] = v.g0; games[1] = v.g1; gameNo = v.gameNo; server = v.server;
  figAt[0] = v.fig0; figAt[1] = (uint8_t)(1 - v.fig0); setFirstWinner = v.firstW; intervalDone = v.interval != 0;
  streak = v.streak; setBias = (v.bias > 0.1f && v.bias < 0.9f) ? v.bias : 0.5f;
  return true;
}

static float dirOf(int side) { return side == 0 ? 1.0f : -1.0f; }
static int   slotOf(int fig) { return figAt[0] == fig ? 0 : 1; }
static const char* figName(int f) { return f == 0 ? "ROT" : "BLAU"; }

// Satzgewinn: 21 mit 2 Punkten Vorsprung, bei 29:29 gewinnt 30
static bool gameWon(int f) {
  const int a = pts[f], b = pts[1 - f];
  return (a >= 21 && a - b >= 2) || a >= 30;
}
// Haette Figur f mit dem naechsten Punkt den Satz?
static bool gamePoint(int f) {
  pts[f]++;
  const bool w = gameWon(f);
  pts[f]--;
  return w;
}

static void say(const char* s, uint8_t col, uint32_t now, uint32_t ms) {
  strlcpy(flash, s, sizeof(flash)); flashCol = col; flashUntil = now + ms;
}
static void cheer(uint8_t power, uint32_t now, uint32_t ms) {
  if (power >= cheerPower || (int32_t)(now - cheerUntil) > 0) { cheerPower = power; cheerAt = now; cheerUntil = now + ms; }
}

// Zielzonen auf der Seite toSide: 0 hinten, 1 Mitte, 2 vorne am Netz
static float zoneX(int toSide, int zone) {
  static const float L[3][2] = {{16, 32}, {34, 56}, {60, 70}};
  const float a = L[zone][0] + rf() * (L[zone][1] - L[zone][0]);
  return toSide == 0 ? a : 160.0f - a;
}
static int zoneOfX(int side, float x) {
  const float d = side == 0 ? x : 160.0f - x;
  return d < 33 ? 0 : (d < 58 ? 1 : 2);
}

// Antwort auf einen eingehenden Schlag (Taktik)
static uint8_t answerTo(uint8_t in, bool last) {
  const int r = rn(100);
  switch (in) {
    case SH_SERVE_HIGH: case SH_CLEAR: case SH_LIFT:
      if (last) return r < 60 ? SH_SMASH : SH_DROP;
      return r < 40 ? SH_SMASH : (r < 70 ? SH_CLEAR : SH_DROP);
    case SH_DROP: case SH_NET: case SH_SERVE_LOW:
      if (last) return r < 50 ? SH_NET : SH_DRIVE;
      return r < 35 ? SH_NET : SH_LIFT;
    case SH_SMASH:
      return r < 55 ? SH_LIFT : SH_DRIVE;
    default:                                                     // Drive
      return r < 45 ? SH_DRIVE : (r < 70 ? SH_DROP : SH_CLEAR);
  }
}
static bool highShot(uint8_t s) { return s == SH_SERVE_HIGH || s == SH_CLEAR || s == SH_LIFT; }

static void startFlight(int from, uint32_t t0, bool serve, uint32_t now) {
  const int to = 1 - from;
  const bool last = hitsLeft <= 1;
  const uint8_t end = last ? planEnd : E_RET;
  const uint8_t shot = nextShot;
  const float prevX = fl.ex, prevY = fl.ey;
  fl.sx = serve ? px[from] + dirOf(from) * 11.0f : prevX;
  fl.sy = serve ? 67.0f : prevY;
  fl.shot = shot; fl.end = end;

  // Ziel und Flugbahn je Schlagart
  int zone;
  switch (shot) {
    case SH_SERVE_HIGH: case SH_CLEAR: case SH_LIFT: zone = 0; fl.H = 30.0f + (float)rn(7); fl.T = (uint16_t)(1050 + rn(250)); break;
    case SH_DROP:  zone = 2; fl.H = 12.0f + (float)rn(5); fl.T = (uint16_t)(850 + rn(120)); break;
    case SH_SMASH: zone = 1 + (rn(3) == 0 ? -1 : 0); fl.H = 1.0f; fl.T = (uint16_t)(360 + rn(90)); break;
    case SH_DRIVE: zone = 1; fl.H = 4.0f + (float)rn(3); fl.T = (uint16_t)(520 + rn(110)); break;
    case SH_NET:   zone = 2; fl.H = 7.0f; fl.T = (uint16_t)(620 + rn(80)); break;
    default:       zone = 2; fl.H = 9.0f; fl.T = 720; break;            // kurzer Aufschlag
  }
  if (end == E_WIN) {                                             // Gewinnschlag: weg vom Gegner
    const int rz = zoneOfX(to, px[to]);
    if (shot == SH_DROP || shot == SH_NET) zone = 2;
    else if (shot != SH_SMASH) zone = rz >= 1 ? 0 : 2;
  }
  fl.ex = zoneX(to, zone);
  fl.ey = 64.0f + (float)rn(7);

  if (end == E_RET) {
    nextShot = answerTo(shot, hitsLeft <= 2);
    if (nextShot == SH_SMASH && highShot(shot)) fl.ey = 54.0f;     // Antwort im Sprung: hoher Treffpunkt
  } else if (end == E_WIN) {
    fl.ey = 101.0f;
  } else if (end == E_NET) {                                     // bleibt im Netz haengen
    fl.ex = NET_X - dirOf(from) * 3.0f; fl.ey = 90.0f; fl.H = shot == SH_SMASH ? 1.0f : 8.0f;
    fl.T = (uint16_t)(fl.T * 0.55f);
  } else {                                                        // Aus oder knapp (Hawk-Eye)
    const bool close = end == E_CH_IN || end == E_CH_OUT;
    const float base = to == 0 ? 12.0f : 148.0f, out = to == 0 ? -1.0f : 1.0f;
    fl.ex = base + out * (close ? 1.0f : 4.0f + (float)rn(5));
    fl.ey = 101.0f;
    if (shot != SH_SMASH) { fl.H = fmaxf(fl.H, 26.0f); fl.T = (uint16_t)(fl.T + 150); }
  }
  fl.t0 = t0;
  hitter = (uint8_t)from;
  rally++;

  // Laufwege, Schlaege, Sprung
  const float want = fl.ex - dirOf(to) * 11.0f;
  if (end == E_RET) {
    tx[to] = want;
    swingAt[to] = t0 + fl.T - CONTACT_MS;
    if (nextShot == SH_SMASH && fl.ey < 60.0f) jumpAt[to] = t0 + fl.T - 380;
  } else if (end == E_WIN) {                                      // Gegner hechtet vergeblich
    tx[to] = px[to] + (want - px[to]) * 0.65f;
    swingAt[to] = t0 + fl.T - 120;
    lungeAt[to] = t0 + fl.T - 200;
  } else if (end == E_OUT || end == E_CH_IN || end == E_CH_OUT) {
    tx[to] = px[to] + (want - px[to]) * 0.5f;                     // laesst ihn fallen
  }
  tx[from] = HOME[from] + (float)(rn(15) - 7);
  if (serve) swingAt[from] = now;
  for (int k = 0; k < 4; k++) { trailX[k] = fl.sx; trailY[k] = fl.sy; }

  if (shot == SH_SMASH) {
    char b[24];
    snprintf(b, sizeof(b), "SMASH %d KM/H", 240 + rn(160));
    say(b, W_GOLD, t0, 1100);
  }
  hitsLeft--;
}

// Chance von Rot fuer den naechsten Ballwechsel: Satz-Grundstaerke, Serie, Aufholjagd
static float chanceRed() {
  float p = setBias + (float)streak * 0.035f;
  const int diff = (int)pts[0] - (int)pts[1];
  p -= fcl((float)diff * 0.022f, -0.16f, 0.16f);
  return fcl(p, 0.22f, 0.78f);
}

// Neuer Ballwechsel: Gewinner, Art des Endes und Laenge werden vorab festgelegt
static void startServe(uint32_t now) {
  const int w = rf() < chanceRed() ? 0 : 1;
  const int r = rn(100);
  planEnd = r < 52 ? E_WIN : (r < 70 ? E_NET : (r < 86 ? E_OUT : (r < 93 ? E_CH_IN : E_CH_OUT)));
  planWin = (uint8_t)w;
  const int lastHitter = (planEnd == E_WIN || planEnd == E_CH_IN) ? w : 1 - w;
  // Laenge: meist kurz bis mittel, manchmal lang, bei Satz-/Matchball laenger
  const bool hot = gamePoint(0) || gamePoint(1);
  int n = rn(100) < (hot ? 55 : 18) ? 8 + rn(13) : 1 + rn(7);
  if (((n & 1) == 1) != (lastHitter == server)) n++;            // ungerade Anzahl: letzter Schlag vom Aufschlaeger
  hitsLeft = n;
  rally = 0;
  st = ST_RALLY; serving = true;
  tx[0] = HOME[0]; tx[1] = HOME[1];
  nextShot = (n > 1 && rn(3) == 0) ? SH_SERVE_HIGH : SH_SERVE_LOW;
  startFlight(slotOf(server), now + CONTACT_MS, true, now);
}

static void beginGame(uint32_t now) {
  pts[0] = pts[1] = 0; intervalDone = false; streak = 0;
  if (gameNo == 1)      setBias = 0.42f + rf() * 0.16f;
  else if (gameNo == 2) setBias = setFirstWinner == 0 ? 0.34f : 0.66f;     // Verlierer des 1. Satzes kaempft sich zurueck
  else                  setBias = 0.46f + rf() * 0.08f;
  startServe(now);
}

static void newMatch(uint32_t now) {
  live = true;
  games[0] = games[1] = 0; gameNo = 1;
  figAt[0] = 0; figAt[1] = 1;
  px[0] = tx[0] = HOME[0]; px[1] = tx[1] = HOME[1];
  server = (uint8_t)rn(2);
  beginGame(now);
}

static void enter(uint32_t now) {
  pal(W_WALL, 28, 56, 74); pal(W_WALL2, 20, 40, 56);
  pal(W_FLOOR, 168, 112, 58); pal(W_FLOOR2, 140, 90, 44); pal(W_LINE, 240, 240, 230);
  pal(W_NET, 150, 160, 170); pal(W_POST, 200, 200, 210); pal(W_TAPE, 250, 250, 250);
  pal(W_SKIN, 240, 192, 150); pal(W_SHIRT_L, 224, 56, 56); pal(W_SHIRT_R, 60, 120, 235);
  pal(W_SHORTS, 235, 235, 240); pal(W_SHOE, 40, 40, 50);
  pal(W_FRAME, 215, 215, 225); pal(W_STRING, 120, 125, 140); pal(W_HANDLE, 235, 205, 120);
  pal(W_FEATH, 250, 250, 250); pal(W_CORK, 255, 150, 40); pal(W_SHAD, 110, 70, 30);
  pal(W_DIG, 255, 255, 255); pal(W_SH, 8, 20, 32); pal(W_TXT, 255, 245, 220); pal(W_SPARK, 255, 220, 60);
  pal(W_PANEL, 10, 24, 40); pal(W_GOLD, 255, 210, 70); pal(W_LAMPOFF, 90, 60, 30);
  pal(W_STAND, 16, 30, 42); pal(W_STAND2, 24, 44, 60);
  pal(W_FAN1, 200, 70, 70); pal(W_FAN2, 70, 110, 200); pal(W_FAN3, 220, 200, 90); pal(W_FAN4, 120, 170, 120);
  pal(W_HAIR, 70, 50, 34); pal(W_TRAIL, 255, 230, 180);
  pal(W_HAWK, 20, 60, 110); pal(W_IN, 90, 230, 110); pal(W_OUT, 255, 80, 80); pal(W_FLASH, 255, 255, 255);
  swingAt[0] = swingAt[1] = now - 5000;
  jumpAt[0] = jumpAt[1] = lungeAt[0] = lungeAt[1] = now - 5000;
  fdir[0] = 1.0f; fdir[1] = -1.0f;
  fl.ex = 60.0f; fl.ey = 67.0f; fl.shot = SH_SERVE_LOW;
  hitter = 0; winnerFig = 0; gameWinner = 0; swapAct = ACT_SERVE; setFirstWinner = 0;
  celebUntil = cheerUntil = flashUntil = now; cheerAt = now; cheerPower = 0; scoreFlash[0] = scoreFlash[1] = now;
  flash[0] = 0;
  lastT = now; lastMm = 255;
  sp.clear();
  resumed = false;
  if (!live && !loaded) {                                             // erster Aufruf nach dem Start: Flash lesen
    loaded = true;
    if (loadSaved()) { live = true; st = ST_REST; px[0] = tx[0] = HOME[0]; px[1] = tx[1] = HOME[1]; }
  }
  if (!live) { newMatch(now); return; }
  // Match fortsetzen: laufenden Ballwechsel neu beginnen, Anzeigen und Pausen fortfuehren
  if (st == ST_RALLY) { st = ST_REST; tx[0] = HOME[0]; tx[1] = HOME[1]; }
  if (st == ST_REST) {
    until = now + 1600; resumed = true;
    winnerFig = gameWon(0) ? 0 : (gameWon(1) ? 1 : server);          // ein schon entschiedener Satz wird abgeschlossen
  }
  else if (st == ST_BANNER) until = now + 1500;
  swingAt[0] = swingAt[1] = now - 5000;
  say("FORTSETZUNG", W_GOLD, now, 1600);
}

// Schlaegerwinkel: 0 = senkrecht nach oben, positiv = zum Netz hin
static float swingAngle(uint32_t now, uint32_t start) {
  const float t = (float)(int32_t)(now - start) * 0.001f;
  const float idle = 1.0f;
  if (t < 0.0f) return idle;
  if (t < WINDUP) { const float u = t / WINDUP; return idle + (-0.9f - idle) * u * (2.0f - u); }
  if (t < WINDUP + SWING) return -0.9f + 2.3f * ((t - WINDUP) / SWING);
  if (t < WINDUP + SWING + BACK) return 1.4f + (idle - 1.4f) * ((t - WINDUP - SWING) / BACK);
  return idle;
}

// Spieler zeichnen. Sprung, Hechten, Jubel und hängender Kopf als Posen
static void player(int slot, uint32_t now) {
  const float d = fdir[slot];
  const int fig = figAt[slot];
  int x = (int)px[slot];
  // Sprung (Smash): 0,6 s, Hoehe 9
  int jy = 0;
  const float jt = (float)(int32_t)(now - jumpAt[slot]) * 0.001f;
  if (jt >= 0.0f && jt < 0.6f) jy = -(int)(sinf(jt / 0.6f * 3.1416f) * 9.0f);
  // Hechten: 0,9 s tief in die Knie, Koerper nach vorn
  const float lt = (float)(int32_t)(now - lungeAt[slot]) * 0.001f;
  const bool lunge = lt >= 0.0f && lt < 0.9f;
  // nach dem Punkt: Sieger jubelt, Verlierer laesst den Kopf haengen
  const bool after = (st == ST_REST || st == ST_BANNER) && (int32_t)(celebUntil - now) > 0;
  const bool happy = after && fig == celebFig, sad = after && fig != celebFig;
  if (happy && (st == ST_BANNER || cheerPower >= 2)) jy = -(int)(fabsf(sinf((float)(now % 600) / 600.0f * 3.1416f)) * 6.0f);
  const int crouch = lunge ? 5 : (sad ? 2 : 0);
  const int lean = lunge ? (int)(d * 3.0f) : 0;
  const int fy = FEET_Y + jy;
  const bool moving = fabsf(tx[slot] - px[slot]) > 1.0f && jy == 0;
  const int step = moving ? (int)(sinf((float)(now & 0xFFFF) * 0.025f) * 3.0f) : (lunge ? 4 : 0);
  const uint8_t shirt = fig == 0 ? W_SHIRT_L : W_SHIRT_R;
  for (int k = 0; k < 2; k++) {                                           // Beine
    fb->drawLine(x - 2 + k, fy - 8 + crouch, x - 2 + k - (lunge ? (int)(d * 4) : -step), fy - 1, W_SKIN);
    fb->drawLine(x + 1 + k, fy - 8 + crouch, x + 1 + k + (lunge ? (int)(d * 5) : -step), fy - 1, W_SKIN);
  }
  fb->fillRect(x - 3 + (lunge ? -(int)(d * 4) : step), fy - 1, 4, 2, W_SHOE);
  fb->fillRect(x + 1 + (lunge ? (int)(d * 5) : -step), fy - 1, 4, 2, W_SHOE);
  x += lean;
  fb->fillRect(x - 3, fy - 22 + crouch, 7, 10, shirt);                    // Rumpf, Hose, Kopf
  fb->fillRect(x - 3, fy - 12 + crouch, 7, 5, W_SHORTS);
  const int hx0 = x + (sad ? (int)(d * 1) : 0), hy0 = fy - 27 + crouch + (sad ? 2 : 0);
  fb->fillCircle(hx0, hy0, 4, W_SKIN);
  fb->fillRect(hx0 - 3, hy0 - 4, 7, 2, W_HANDLE);
  fb->drawPixel(hx0 + (int)d * 2, hy0 + (sad ? 1 : 0), W_SHOE);
  // Arm und Schlaeger
  float a = swingAngle(now, swingAt[slot]);
  if (happy) a = -0.25f + sinf((float)(now % 500) / 500.0f * 6.2832f) * 0.2f;   // Schlaeger hoch
  else if (sad) a = 2.6f;                                                  // haengt nach unten
  else if (lunge) a = 1.9f;                                                // tief gestreckt
  const float ux = d * sinf(a), uy = -cosf(a);
  const float shx = x + d * 2.0f, shy = (float)(fy - 20 + crouch);
  for (int k = 0; k < 2; k++)
    fb->drawLine((int)shx, (int)shy + k, (int)(shx + ux * 9), (int)(shy + uy * 9) + k, W_SKIN);
  fb->drawLine((int)(shx + ux * 9), (int)(shy + uy * 9), (int)(shx + ux * 16), (int)(shy + uy * 16), W_HANDLE);
  const int hx = (int)(shx + ux * 21), hy = (int)(shy + uy * 21);
  fb->drawCircle(hx, hy, 4, W_FRAME);
  fb->drawPixel(hx, hy, W_STRING);
  fb->drawPixel(hx - 1, hy, W_STRING);
  fb->drawPixel(hx, hy - 1, W_STRING);
  if (happy) {                                                             // freie Faust hoch
    const float ox = -d;
    fb->drawLine(x - (int)d * 2, fy - 20, x + (int)(ox * 5), fy - 30, W_SKIN);
  }
  if (jy == 0) fb->drawFastHLine(x - 4, FEET_Y + 1, 9, W_SHAD);            // Schatten
  else fb->drawFastHLine(x - 3, FEET_Y + 1, 7, W_SHAD);
}

static void shuttle(int x, int y, float dx, float dy) {
  float l = sqrtf(dx * dx + dy * dy);
  if (l < 0.001f) { dx = 0; dy = -1; l = 1; }
  dx /= l; dy /= l;
  const float tx0 = x - dx * 5.0f, ty0 = y - dy * 5.0f;
  fb->drawLine(x, y, (int)tx0, (int)ty0, W_FEATH);
  fb->drawPixel((int)(tx0 - dy * 1.5f), (int)(ty0 + dx * 1.5f), W_FEATH);
  fb->drawPixel((int)(tx0 + dy * 1.5f), (int)(ty0 - dx * 1.5f), W_FEATH);
  fb->fillRect(x - 1, y - 1, 2, 2, W_CORK);
}

// ---- Spielablauf nach einem Punkt ----
static void setBanner(uint8_t kind, uint32_t now, uint32_t ms) {
  bannerKind = kind; st = ST_BANNER; until = now + ms;
  tx[0] = HOME[0]; tx[1] = HOME[1];
}

static void afterPoint(uint32_t now) {
  const int f = winnerFig, o = 1 - f;
  if (gameWon(f)) {
    games[f]++; gameWinner = (uint8_t)f; server = (uint8_t)f;
    if (gameNo == 1) setFirstWinner = (uint8_t)f;
    celebFig = (uint8_t)f; celebUntil = now + 4000;
    if (games[f] >= 2) {
      saveClear();                                                    // Sieger steht fest: gespeicherten Stand loeschen
      setBanner(BK_MATCH, now, 5200);
      cheer(3, now, 5000);
      static const uint8_t cc[4] = {W_FAN1, W_FAN2, W_FAN3, W_FAN4};
      for (int k = 0; k < 8; k++) sp.burst(20.0f + rn(120), 40.0f + rn(20), 4, 40.0f, cc[k & 3]);
    } else {
      setBanner(BK_GAME, now, 3200);
      cheer(2, now, 2800);
    }
    return;
  }
  if (!intervalDone && (pts[f] >= 11 || pts[o] >= 11)) {            // Pause bei 11 Punkten
    intervalDone = true;
    if (gameNo == 3) setBanner(BK_SWAP11, now, 2000);
    else             setBanner(BK_PAUSE, now, 1800);
    return;
  }
  startServe(now);
}

static void beginSwap(uint8_t act) {
  st = ST_SWAP; swapAct = act;
  tx[0] = HOME[1]; tx[1] = HOME[0];
}

static void finishSwap(uint32_t now) {
  const uint8_t t = figAt[0]; figAt[0] = figAt[1]; figAt[1] = t;     // Figuren tauschen den Platz
  px[0] = HOME[0]; px[1] = HOME[1]; tx[0] = HOME[0]; tx[1] = HOME[1];
  fdir[0] = 1.0f; fdir[1] = -1.0f;
  if (swapAct == ACT_NEWGAME) { gameNo++; server = gameWinner; beginGame(now); }
  else startServe(now);
  saveNow(now);                                                       // neuer Satz bzw. Seitenwechsel sofort sichern
}

static void bannerEnd(uint32_t now) {
  switch (bannerKind) {
    case BK_PAUSE: startServe(now); break;
    case BK_SWAP11: beginSwap(ACT_SERVE); break;
    case BK_GAME: beginSwap(ACT_NEWGAME); break;
    default: newMatch(now); break;
  }
}

// Ballwechsel ist vorbei: Punkt vergeben, Anzeigen und Publikum
static void pointScored(uint32_t now) {
  winnerFig = planWin;
  const bool wasHot = gamePoint(0) || gamePoint(1);
  pts[winnerFig]++;
  dirty = true;
  if (gameWon(winnerFig)) saveNow(now);                               // Satz entschieden: sofort sichern
  scoreFlash[winnerFig] = now + 700;
  server = winnerFig;
  if ((streak > 0) == (winnerFig == 0) && streak != 0) streak += winnerFig == 0 ? 1 : -1;
  else streak = winnerFig == 0 ? 1 : -1;
  if (streak > 4) streak = 4;
  if (streak < -4) streak = -4;
  celebFig = winnerFig; celebUntil = now + 1500;
  const uint8_t pw = (rally >= 10 || wasHot) ? 2 : (rally >= 5 ? 1 : 0);
  if (pw) cheer(pw, now, pw == 2 ? 2200 : 1400);
  switch (fl.end) {
    case E_WIN: {
      const char* m = fl.shot == SH_SMASH ? "SMASH!" : fl.shot == SH_DROP ? "STOPPBALL!" : fl.shot == SH_NET ? "NETZROLLER!" : "PUNKT!";
      if (rally >= 10) m = "WAS FUER EIN BALLWECHSEL!";
      say(m, W_GOLD, now, 1300);
      break;
    }
    case E_NET: say("NETZ!", W_OUT, now, 1200); break;
    case E_OUT: say("AUS!", W_OUT, now, 1200); break;
    default: say("AUS?  CHALLENGE!", W_GOLD, now, 1100); break;
  }
}

static void cellS(int x, int y, int s, int, int) { fb->fillRect(x + 1, y + 1, s, s, W_SH); }
static void cellD(int x, int y, int s, int, int) { fb->fillRect(x, y, s, s, W_DIG); }

static void lamps(int x, int fig, int dir) {                               // zwei Lampen fuer gewonnene Saetze
  for (int i = 0; i < 2; i++) {
    const int lx = x + dir * i * 5;
    const bool on = games[fig] > i;
    fb->fillRect(lx, 104, 4, 4, on ? (fig == 0 ? W_SHIRT_L : W_SHIRT_R) : W_LAMPOFF);
    fb->drawRect(lx, 104, 4, 4, W_SH);
  }
}

static void draw(const FaceCtx& c) {
  const uint32_t now = c.now;
  const float dt = frameDt(now, lastT);
  if (c.timeOk) {
    if (lastMm != 255 && c.mm != lastMm) sp.burst((float)NET_X, (float)NET_TOP, 16, 55, W_SPARK);
    lastMm = c.mm;
  }

  // ---- Bewegung der Spieler ----
  const float spd = st == ST_SWAP ? 95.0f : 78.0f;
  for (int s = 0; s < 2; s++) {
    const float dx = tx[s] - px[s], mv = spd * dt;
    px[s] += fabsf(dx) <= mv ? dx : (dx > 0 ? mv : -mv);
    px[s] = fcl(px[s], 8.0f, 152.0f);
    if (st != ST_SWAP) {                                               // nie ueber das Netz laufen (ausser beim Seitenwechsel)
      if (s == 0) px[s] = fminf(px[s], (float)NET_X - 7.0f);
      else        px[s] = fmaxf(px[s], (float)NET_X + 7.0f);
    }
    fdir[s] = st == ST_SWAP ? (fabsf(dx) > 1.0f ? (dx > 0 ? 1.0f : -1.0f) : fdir[s]) : dirOf(s);
  }

  // ---- Spielablauf ----
  if (st == ST_RALLY) {
    if ((int32_t)(now - fl.t0) >= (int32_t)fl.T) {
      serving = false;
      if (fl.end != E_RET) {                                          // Ballwechsel vorbei
        pointScored(now);
        sp.burst(fl.ex, fl.end == E_NET ? 92.0f : 99.0f, 6, 30, W_FEATH);
        st = ST_REST;
        until = now + ((fl.end == E_CH_IN || fl.end == E_CH_OUT) ? 3400 : (rally >= 10 ? 2000 : 1400));
        tx[0] = HOME[0]; tx[1] = HOME[1];
      } else {
        const int r = 1 - hitter;
        sp.burst(fl.ex, fl.ey, 3, 30, W_FEATH);
        startFlight(r, fl.t0 + fl.T, false, now);
      }
    }
  } else if (st == ST_REST) {
    if ((int32_t)(now - until) >= 0) { resumed = false; afterPoint(now); }
  } else if (st == ST_BANNER) {
    if ((int32_t)(now - until) >= 0) bannerEnd(now);
  } else if (st == ST_SWAP) {
    if (fabsf(px[0] - HOME[1]) < 1.5f && fabsf(px[1] - HOME[0]) < 1.5f) finishSwap(now);
  }
  sp.step(dt, 60.0f);
  if (dirty && (st == ST_RALLY || st == ST_REST) && (savedAt == 0 || now - savedAt >= SAVE_GAP_MS)) saveNow(now);

  // ---- Halle mit Tribuene ----
  fb->fillRect(0, 0, FB_W, FLOOR_Y, W_WALL);
  for (int x = 10; x < FB_W; x += 20) fb->drawFastVLine(x, 36, 44, W_WALL2);
  fb->fillRect(0, 80, FB_W, 16, W_STAND);
  fb->drawFastHLine(0, 80, FB_W, W_STAND2);
  fb->drawFastHLine(0, 88, FB_W, W_STAND2);
  const bool cheering = (int32_t)(cheerUntil - now) > 0;
  static const uint8_t FAN[4] = {W_FAN1, W_FAN2, W_FAN3, W_FAN4};
  for (int i = 0; i < 27; i++) {                                         // Zuschauer (zwei Reihen)
    const int row = i & 1, x = 3 + i * 6, y0 = row ? 90 : 83;
    const uint32_t h = (uint32_t)i * 2654435761u;
    int jy = 0;
    bool arms = false;
    if (cheering) {
      const float a = (float)(now - cheerAt + (h & 511)) / (cheerPower >= 2 ? 160.0f : 260.0f);
      jy = -(int)(fabsf(sinf(a)) * (cheerPower >= 2 ? 3.0f : 2.0f));
      arms = cheerPower >= 1 && ((h >> 9) % 3 != 0);
    }
    const uint8_t sc = FAN[(h >> 4) & 3];
    fb->fillRect(x - 2, y0 + 2 + jy, 4, 4, sc);
    fb->fillRect(x - 1, y0 - 1 + jy, 2, 3, W_SKIN);
    fb->drawPixel(x - 1, y0 - 1 + jy, W_HAIR); fb->drawPixel(x, y0 - 1 + jy, W_HAIR);
    if (arms) { fb->drawPixel(x - 3, y0 + jy, W_SKIN); fb->drawPixel(x + 2, y0 + jy, W_SKIN); }
    if (cheerPower >= 3 && cheering && ((now / 90 + h) % 23) == 0) fb->drawPixel(x, y0 - 3 + jy, W_FLASH);   // Blitzlicht
  }
  fb->fillRect(0, 96, FB_W, 4, W_WALL2);
  fb->fillRect(0, FLOOR_Y, FB_W, FB_H - FLOOR_Y, W_FLOOR);
  fb->drawFastHLine(0, 107, FB_W, W_FLOOR2);
  fb->drawFastHLine(0, 114, FB_W, W_FLOOR2);
  fb->drawFastHLine(0, FLOOR_Y, FB_W, W_LINE);
  fb->drawLine(12, FLOOR_Y, 3, FB_H - 1, W_LINE);
  fb->drawLine(148, FLOOR_Y, 157, FB_H - 1, W_LINE);

  // ---- Zeit ----
  drawTime(26, 6, 4, c, cellS);
  drawTime(26, 6, 4, c, cellD);

  // ---- Netz ----
  fb->fillRect(NET_X - 1, NET_TOP, 3, FEET_Y - 2 - NET_TOP, W_POST);
  for (int y = NET_TOP + 3; y < 100; y += 2)
    for (int x = NET_X - 8 + (((y - NET_TOP) / 2) & 1) * 2; x <= NET_X + 8; x += 4) fb->drawPixel(x, y, W_NET);
  fb->fillRect(NET_X - 9, NET_TOP, 19, 2, W_TAPE);

  // ---- Spieler ----
  player(0, now);
  player(1, now);

  // ---- Aufschlag-Anzeige ueber dem Aufschlaeger: Federball und R (rechts) / L (links) ----
  if (serving && st == ST_RALLY) {
    const int ss = slotOf(server), sx = (int)px[ss];
    fb->fillRect(sx - 4, FEET_Y - 42, 2, 2, W_CORK);
    fb->drawLine(sx - 4, FEET_Y - 43, sx - 7, FEET_Y - 47, W_FEATH);
    fb->drawLine(sx - 3, FEET_Y - 43, sx - 3, FEET_Y - 48, W_FEATH);
    txt(sx + 1, FEET_Y - 46, (pts[server] & 1) ? "L" : "R", W_GOLD);
  }

  // ---- Federball ----
  if ((st == ST_REST && !resumed) || (st == ST_BANNER && (int32_t)(now - until) < -1000)) {
    const int x = (int)fl.ex;
    if (fl.end == E_NET) { fb->fillRect(x - 1, 99, 2, 2, W_CORK); fb->drawFastHLine(x - (x < NET_X ? 4 : -1), 100, 4, W_FEATH); }
    else { fb->drawFastHLine(x, 100, 4, W_FEATH); fb->fillRect(x - 2, 99, 2, 2, W_CORK); }
  } else if (st == ST_RALLY) {
    float u = (float)(int32_t)(now - fl.t0) / (float)fl.T;
    if (u < 0.0f) u = 0.0f;
    if (u > 1.0f) u = 1.0f;
    const float x = fl.sx + (fl.ex - fl.sx) * u;
    const float y = fl.sy + (fl.ey - fl.sy) * u - 4.0f * fl.H * u * (1.0f - u);
    const float vx = fl.ex - fl.sx, vy = (fl.ey - fl.sy) - 4.0f * fl.H * (1.0f - 2.0f * u);
    if (fl.shot == SH_SMASH && u > 0.0f) {                                 // Leuchtspur beim Smash
      for (int k = 3; k > 0; k--) { trailX[k] = trailX[k - 1]; trailY[k] = trailY[k - 1]; }
      trailX[0] = x; trailY[0] = y;
      for (int k = 1; k < 4; k++) fb->drawLine((int)trailX[k - 1], (int)trailY[k - 1], (int)trailX[k], (int)trailY[k], k == 1 ? W_TRAIL : W_GOLD);
    }
    fb->drawFastHLine((int)x - 2, 112, 5, W_SHAD);
    shuttle((int)x, (int)y, vx, vy);
  }
  sp.draw();

  // ---- Hawk-Eye ----
  if (st == ST_REST && !resumed && (fl.end == E_CH_IN || fl.end == E_CH_OUT)) {
    const float t = 3.4f - (float)(int32_t)(until - now) * 0.001f;
    if (t > 0.9f) {
      const int bx = 50, by = 40;
      fb->fillRect(bx, by, 60, 34, W_SH);
      fb->fillRect(bx + 1, by + 1, 58, 32, W_HAWK);
      txtc(80, by + 3, "HAWK-EYE", W_TXT);
      fb->fillRect(bx + 6, by + 14, 48, 12, W_FLOOR);
      fb->fillRect(bx + 29, by + 14, 3, 12, W_LINE);                       // Linie
      const float a = fcl((t - 0.9f) / 1.0f, 0.0f, 1.0f);
      const bool in = fl.end == E_CH_IN;
      const int mx = bx + 30 + (int)((in ? -1.5f : 4.5f) * a) + (in ? 0 : 0);
      fb->fillRect(mx - 2, by + 18, 4, 4, W_CORK);
      if (t > 2.0f) {
        txtc(80, by + 27 - 1, in ? "IN!" : "AUS!", ((now / 200) & 1) ? W_TXT : (in ? W_IN : W_OUT));
      }
    }
  }

  // ---- Spielstand auf dem Hallenboden ----
  const int fl0 = figAt[0], fr0 = figAt[1];                         // wer links / rechts steht
  char b[28];
  snprintf(b, sizeof(b), "%u", pts[fl0]);
  const bool f0 = (int32_t)(scoreFlash[fl0] - now) > 0 && ((now / 120) & 1);
  const bool f1 = (int32_t)(scoreFlash[fr0] - now) > 0 && ((now / 120) & 1);
  txtr(75, 104, b, W_SH, 2); txtr(74, 103, b, f0 ? W_GOLD : W_TXT, 2);
  snprintf(b, sizeof(b), "%u", pts[fr0]);
  txt(88, 104, b, W_SH, 2);  txt(87, 103, b, f1 ? W_GOLD : W_TXT, 2);
  txtc(81, 104, ":", W_SH, 2); txtc(80, 103, ":", W_TXT, 2);
  lamps(41, fl0, 1);
  lamps(112, fr0, 1);
  const int sdot = slotOf(server);                                  // orangefarbener Punkt = Aufschlaeger
  fb->fillRect(sdot == 0 ? 44 : 115, 111, 3, 3, W_CORK);
  if (c.timeOk) snprintf(b, sizeof(b), "%02u.%02u.", c.mday, c.mon); else snprintf(b, sizeof(b), "--.--.");
  txtS(3, 104, b, W_TXT, W_SH);
  if (c.timeOk) txtS(3, 112, WD_SHORT[c.wday % 7], W_TXT, W_SH);
  if (c.hasWx) {
    const int w = tempW(c.temp, 1);
    tempTxt(FB_W - 3 - w + 1, 105, c.temp, W_SH);
    tempTxt(FB_W - 3 - w, 104, c.temp, W_TXT);
  } else txtr(FB_W - 3, 104, "--", W_TXT);
  snprintf(b, sizeof(b), "SATZ %u", gameNo);
  txtS(FB_W - 3 - txtw(b), 112, b, W_TXT, W_SH);
  if (c.showSec && c.timeOk) fb->fillRect(0, FB_H - 1, ((c.ss + 1) * FB_W) / 60, 1, W_LINE);

  // ---- Laufschrift-Zeile unter der Uhr: Meldung, Satz-/Matchball oder Rallye-Zaehler ----
  if (st != ST_BANNER) {
    const bool fl_on = (int32_t)(flashUntil - now) > 0 && flash[0];
    if (fl_on) {
      txtS(80 - txtw(flash) / 2, 38, flash, flashCol, W_SH);
    } else if (st == ST_RALLY || st == ST_REST) {
      int gp = -1;
      if (gamePoint(0)) gp = 0; else if (gamePoint(1)) gp = 1;
      if (gp >= 0) {
        const bool mb = games[gp] == 1;
        snprintf(b, sizeof(b), "%s %s", mb ? "MATCHBALL" : "SATZBALL", figName(gp));
        if ((now / 400) & 1) txtS(80 - txtw(b) / 2, 38, b, gp == 0 ? W_SHIRT_L : W_SHIRT_R, W_SH);
      } else if (pts[0] >= 20 && pts[0] == pts[1]) {
        txtS(80 - txtw("VERLAENGERUNG") / 2, 38, "VERLAENGERUNG", W_GOLD, W_SH);
      } else if (st == ST_RALLY && rally >= 5) {
        snprintf(b, sizeof(b), "RALLYE %u", rally);
        txtS(80 - txtw(b) / 2, 38, b, rally >= 10 ? W_GOLD : W_TXT, W_SH);
      }
    }
  }

  // ---- Anzeigetafel bei Pause, Seitenwechsel, Satz- und Spielende ----
  if (st == ST_BANNER) {
    char t[20], d[32];
    uint8_t tcol = W_GOLD;
    switch (bannerKind) {
      case BK_PAUSE:  snprintf(t, sizeof(t), "PAUSE");         snprintf(d, sizeof(d), "BEI 11 PUNKTEN"); break;
      case BK_SWAP11: snprintf(t, sizeof(t), "SEITENWECHSEL"); snprintf(d, sizeof(d), "SATZ 3 - BEI 11 PUNKTEN"); break;
      case BK_GAME:   snprintf(t, sizeof(t), "SATZ %u", gameNo);
                      snprintf(d, sizeof(d), "%s %u:%u", figName(gameWinner), pts[gameWinner], pts[1 - gameWinner]); break;
      default:        snprintf(t, sizeof(t), "SIEG %s", figName(gameWinner));
                      snprintf(d, sizeof(d), "%u:%u SAETZE", games[gameWinner], games[1 - gameWinner]);
                      tcol = gameWinner == 0 ? W_SHIRT_L : W_SHIRT_R; break;
    }
    const int sz = txtw(t, 2) <= 110 ? 2 : 1;
    const int pw = imax(txtw(d), txtw(t, sz)) + 16;
    const int px0 = 80 - pw / 2;
    fb->fillRect(px0, 40, pw, 34, W_SH);
    fb->fillRect(px0 + 1, 41, pw - 2, 32, W_PANEL);
    fb->drawRect(px0 + 2, 42, pw - 4, 30, tcol);
    txtc(80, sz == 2 ? 45 : 49, t, tcol, sz);
    txtc(80, 62, d, W_TXT);
  }
}

static void cover(float p, bool, uint32_t) {
  if (p <= 0.0f) return;
  uint8_t* bf = buf();
  for (int i = 0; i < FB_W / 8; i++) {
    float q = p * 1.5f - ((i & 1) ? 0.25f : 0.0f);
    q = fcl(q, 0.0f, 1.0f);
    const int h = (int)(q * FB_H);
    if (h <= 0) continue;
    for (int y = 0; y < h; y++) {
      const int yy = (i & 1) ? FB_H - 1 - y : y;
      memset(bf + yy * FB_W + i * 8, W_WALL2, 8);
    }
    const int ey = (i & 1) ? FB_H - 1 - h : h;                   // helle Kante
    if (ey >= 0 && ey < FB_H && h < FB_H) memset(bf + ey * FB_W + i * 8, W_LINE, 8);
  }
}

}  // namespace f_bad
