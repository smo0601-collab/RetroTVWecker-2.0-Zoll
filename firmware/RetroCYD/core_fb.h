// Teil von RetroCYD - wird von RetroCYD.ino eingebunden
// ============================================================================
//  BILDPUFFER fuer die Arcade-Spiele (flimmerfrei)
// ============================================================================
//  Jedes Bild wird zuerst in einen Puffer im Arbeitsspeicher gezeichnet und
//  dann in einem Rutsch zum Display geschickt. Reicht der Speicher nicht fuer
//  den ganzen Bildschirm, wird in 2, 4 oder 8 Streifen gezeichnet - die Spiele
//  merken davon nichts, weil alle Zeichenbefehle ueber die c...()-Helfer laufen.
//
//  Spiele laufen in einem festen Takt von 30 Schritten pro Sekunde:
//    update()  - Spiellogik, ein Schritt (E.x = Taste x neu gedrueckt)
//    render()  - zeichnet das komplette Bild mit den c...()-Helfern
// ============================================================================
#pragma once

static LGFX_Sprite canvas(&tft);
bool fbOk = false;
int fbBandH = 0;        // Hoehe eines Streifens
int cvY0 = 0;           // Bildschirm-Y, an dem der aktuelle Streifen beginnt
uint32_t fbLast = 0, fbAcc = 0;
uint32_t tick = 0;      // zaehlt die Spielschritte (30 pro Sekunde)
const uint32_t TICK_MS = 33;

void fbEnd() {
  if (fbOk) canvas.deleteSprite();
  fbOk = false;
}

bool fbBegin() {
  fbEnd();
  canvas.setColorDepth(8);     // 256 Farben - halber Speicher, passt zum Retro-Look
  canvas.setPsram(false);
  const int div[4] = {1, 2, 4, 8};
  for (int d : div) {
    int h = (SCR_H + d - 1) / d;
    if (canvas.createSprite(SCR_W, h)) { fbBandH = h; fbOk = true; break; }
  }
  fbLast = millis();
  fbAcc = 0;
  tick = 0;
  Serial.printf("Bildpuffer %dx%d: %s (frei: %u)\n", SCR_W, fbBandH, fbOk ? "OK" : "FEHLER",
                (unsigned)ESP.getFreeHeap());
  return fbOk;
}

void fbResume() { fbLast = millis(); fbAcc = 0; }

void fbStep(void (*update)(), void (*render)()) {
  uint32_t now = millis();
  fbAcc += now - fbLast;
  fbLast = now;
  if (fbAcc > 120) fbAcc = 120;            // nach Haengern nicht ewig nachholen
  while (fbAcc >= TICK_MS) {
    fbAcc -= TICK_MS;
    update();
    tick++;
    clearEdges();
    if (state != ST_GAME || paused) return;
  }
  if (!fbOk) return;
  for (int y0 = 0; y0 < SCR_H; y0 += fbBandH) {
    cvY0 = y0;
    canvas.fillScreen(C_BG);
    render();
    canvas.pushSprite(0, y0);
  }
}

// ---------------------------------------------------------------------------
//  Zeichen-Helfer (Bildschirmkoordinaten, werden auf den Streifen umgerechnet)
// ---------------------------------------------------------------------------
inline bool cVis(int y, int h) { return y + h > cvY0 && y < cvY0 + fbBandH; }
inline void cR(int x, int y, int w, int h, uint16_t c)  { if (cVis(y, h)) canvas.fillRect(x, y - cvY0, w, h, c); }
inline void cRO(int x, int y, int w, int h, uint16_t c) { if (cVis(y, h)) canvas.drawRect(x, y - cvY0, w, h, c); }
inline void cRR(int x, int y, int w, int h, int r, uint16_t c) { if (cVis(y, h)) canvas.fillRoundRect(x, y - cvY0, w, h, r, c); }
inline void cH(int x, int y, int w, uint16_t c)         { if (cVis(y, 1)) canvas.drawFastHLine(x, y - cvY0, w, c); }
inline void cV(int x, int y, int h, uint16_t c)         { if (cVis(y, h)) canvas.drawFastVLine(x, y - cvY0, h, c); }
inline void cP(int x, int y, uint16_t c)                { if (cVis(y, 1)) canvas.drawPixel(x, y - cvY0, c); }
inline void cL(int x0, int y0, int x1, int y1, uint16_t c) {
  if (cVis(min(y0, y1), abs(y1 - y0) + 1)) canvas.drawLine(x0, y0 - cvY0, x1, y1 - cvY0, c);
}
inline void cC(int x, int y, int r, uint16_t c)  { if (cVis(y - r, 2 * r + 1)) canvas.fillCircle(x, y - cvY0, r, c); }
inline void cCO(int x, int y, int r, uint16_t c) { if (cVis(y - r, 2 * r + 1)) canvas.drawCircle(x, y - cvY0, r, c); }
inline void cE(int x, int y, int rx, int ry, uint16_t c) { if (cVis(y - ry, 2 * ry + 1)) canvas.fillEllipse(x, y - cvY0, rx, ry, c); }
inline void cTri(int x0, int y0, int x1, int y1, int x2, int y2, uint16_t c) {
  int ymin = min(y0, min(y1, y2)), ymax = max(y0, max(y1, y2));
  if (cVis(ymin, ymax - ymin + 1)) canvas.fillTriangle(x0, y0 - cvY0, x1, y1 - cvY0, x2, y2 - cvY0, c);
}

void cTxt(const char* s, int x, int y, float size, uint16_t c,
          uint8_t datum = textdatum_t::middle_center) {
  int h = (int)(8 * size) + 2;
  if (!cVis(y - h, 2 * h)) return;
  canvas.setTextDatum(datum);
  canvas.setTextSize(size);
  canvas.setTextColor(c);
  canvas.drawString(s, x, y - cvY0);
}

void cNum(const char* fmt, long v, int x, int y, float size, uint16_t c,
          uint8_t datum = textdatum_t::top_left) {
  char buf[24];
  snprintf(buf, sizeof(buf), fmt, v);
  cTxt(buf, x, y, size, c, datum);
}

// Kleine Pixelgrafik aus Bitmuster zeichnen. rows[] enthaelt pro Zeile ein
// Bitmuster (linkes Pixel = hoechstes der w Bits), s = Vergroesserung.
void cSpr(const uint16_t* rows, int w, int h, int x, int y, uint16_t col, int s = 1,
          bool flip = false) {
  if (!cVis(y, h * s)) return;
  for (int r = 0; r < h; r++) {
    uint16_t bits = rows[r];
    int c = 0;
    while (c < w) {
      if (bits & (1 << (w - 1 - c))) {
        int start = c;
        while (c < w && (bits & (1 << (w - 1 - c)))) c++;
        int len = c - start;
        int px = flip ? x + (w - start - len) * s : x + start * s;
        canvas.fillRect(px, y + r * s - cvY0, len * s, s, col);
      } else {
        c++;
      }
    }
  }
}

// HUD-Leiste oben (Punkte links, Highscore rechts, optional Mitte)
void cHud(uint32_t score, const char* mid = nullptr) {
  cNum("%ld", (long)score, 6, 4, 1, C_WHITE);
  cNum("HI %ld", (long)max(score, highscore[curGame]), SCR_W - 6, 4, 1, C_YELLOW,
       textdatum_t::top_right);
  if (mid) cTxt(mid, SCR_W / 2, 4, 1, C_CYAN, textdatum_t::top_center);
}

// ---------------------------------------------------------------------------
//  Kleine Mathe-Helfer fuer die Spiele
// ---------------------------------------------------------------------------
inline float frand(float a, float b) { return a + (b - a) * (esp_random() & 0xFFFF) / 65535.0f; }
inline int irand(int a, int b) { return a + (int)(esp_random() % (uint32_t)(b - a + 1)); }
inline bool hitBox(float ax, float ay, float aw, float ah, float bx, float by, float bw, float bh) {
  return ax < bx + bw && ax + aw > bx && ay < by + bh && ay + ah > by;
}
inline float dist2(float ax, float ay, float bx, float by) {
  return (ax - bx) * (ax - bx) + (ay - by) * (ay - by);
}
void fbNoRedraw() { fbResume(); }

// ---------------------------------------------------------------------------
//  Spielspeicher: Es laeuft immer nur ein Spiel. Grosse Arrays bekommen ihren
//  Speicher deshalb erst beim Spielstart aus einem gemeinsamen Block, der beim
//  Verlassen wieder freigegeben wird. Das haelt den festen Arbeitsspeicher
//  klein (der ESP32 braucht viel davon fuer Bluetooth).
// ---------------------------------------------------------------------------
const size_t GAME_MEM_SIZE = 8192;
uint8_t* gameMem = nullptr;
size_t gameMemUsed = 0;

void gameMemBegin() {
  if (!gameMem) gameMem = (uint8_t*)malloc(GAME_MEM_SIZE);
  gameMemUsed = 0;
  if (gameMem) memset(gameMem, 0, GAME_MEM_SIZE);
}

void gameMemEnd() {
  free(gameMem);
  gameMem = nullptr;
  gameMemUsed = 0;
}

void* gameAlloc(size_t n) {
  n = (n + 3) & ~(size_t)3;
  if (!gameMem || gameMemUsed + n > GAME_MEM_SIZE) {
    Serial.printf("Spielspeicher reicht nicht (%u + %u)\n", (unsigned)gameMemUsed, (unsigned)n);
    return nullptr;
  }
  void* p = gameMem + gameMemUsed;
  gameMemUsed += n;
  return p;
}

// Array mit Speicher aus dem Spielblock; funktioniert wie ein normales Array
// (auch in for-Schleifen wie "for (auto& x : liste)").
template <typename T, int N>
struct GArr {
  T* p = nullptr;
  void alloc() { p = (T*)gameAlloc(sizeof(T) * N); }
  T* begin() { return p; }
  T* end() { return p + N; }
  T& operator[](int i) { return p[i]; }
  static constexpr size_t bytes() { return sizeof(T) * N; }
};
