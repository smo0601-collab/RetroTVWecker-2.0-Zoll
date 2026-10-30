#pragma once
/*
  face_iss.h - Zifferblatt "ISS-Tracker"
  Weltkarte mit der echten aktuellen Position der Internationalen Raumstation (Abruf alle paar Sekunden,
  zwischen den Abrufen wird die Bewegung fortgeschrieben). Die Karte zeigt Tag und Nacht der Erde nach
  dem echten Sonnenstand. Gestrichelt: die berechnete Bahn (30 Minuten zurueck, rund 90 Minuten voraus).
  Kopfzeile: Uhrzeit, Koordinaten, Geschwindigkeit; Fusszeile: Entfernung zu dir und Besatzung.
  "UEBER DIR" erscheint, wenn die Station ueber deinem Horizont steht.
  Weihnachtsschlitten (einstellbar: aus / immer / automatisch vom 1. bis 26.12.): Statt der ISS fliegt der
  Schlitten des Weihnachtsmanns mit Rentieren ueber die Karte, immer dort, wo gerade Mitternacht ist
  (also mit 15 Grad pro Stunde nach Westen), mit Glitzerspur. Er braucht kein Internet.
  Uebergang: Eine Bahnlinie schiebt sich quer ueber den Schirm.
*/
#include "faces_fx.h"
#include "astro.h"
#include "iss_map.h"

namespace f_iss {
using namespace fc;

enum : uint8_t { I_OCD = 1, I_OCN, I_LND, I_LNN, I_BG, I_ORB, I_ORB2, I_ISS, I_HOME, I_DIG, I_SH, I_DIM, I_OK, I_FOOT,
                 I_SLED, I_SLED2, I_DEER, I_DEER2, I_NOSE, I_SANTA, I_GLIT };

static const int MX = 0, MY = 26, MW = 160, MH = 80;           // Kartenfenster im Bild
static int16_t  termRow[MW];                                    // Zeile der Tag/Nacht-Grenze je Spalte
static bool     dayAbove[MW];                                   // true: oberhalb der Grenze ist Tag
static uint32_t lastT, termAt;
static bool     haveTerm;
static float    sunAltHome;
static uint32_t visAt;
static bool     visible;

static void enter(uint32_t now) {
  pal(I_OCD, 22, 84, 160); pal(I_OCN, 6, 18, 52); pal(I_LND, 72, 150, 76); pal(I_LNN, 18, 46, 34);
  pal(I_BG, 6, 8, 24); pal(I_ORB, 255, 220, 120); pal(I_ORB2, 140, 120, 70); pal(I_ISS, 255, 255, 255); pal(I_HOME, 255, 90, 90);
  pal(I_DIG, 244, 248, 255); pal(I_SH, 2, 4, 14); pal(I_DIM, 150, 170, 210); pal(I_OK, 120, 255, 150); pal(I_FOOT, 110, 190, 250);
  pal(I_SLED, 220, 30, 40); pal(I_SLED2, 255, 210, 70); pal(I_DEER, 150, 96, 50); pal(I_DEER2, 230, 200, 160);
  pal(I_NOSE, 255, 40, 40); pal(I_SANTA, 255, 255, 255); pal(I_GLIT, 255, 240, 160);
  haveTerm = false; termAt = 0; lastT = now; visAt = 0; visible = false; sunAltHome = -90;
}

static void cellS(int x, int y, int s, int, int) { fb->fillRect(x + 1, y + 1, s, s, I_SH); }
static void cellD(int x, int y, int s, int, int) { fb->fillRect(x, y, s, s, I_DIG); }

static inline int xOfLon(float lon) { return MX + (int)((lon + 180.0f) * (MW / 360.0f)); }
static inline int yOfLat(float lat) { return MY + (int)((90.0f - lat) * (MH / 180.0f)); }

// Sonnenstand -> Tag/Nacht-Grenze je Spalte
static void updateTerminator(double unixT) {
  const double d = astro::dayNumber(unixT);
  float sl, sr, ra, dec;
  astro::sunEcl(d, sl, sr);
  astro::eclToEq(sl, 0.0f, d, ra, dec);
  const double gmst = astro::wrap360(280.46061837 + 360.98564736629 * (d - 1.5));
  const float subLon = (float)astro::wrap360(ra - gmst);          // Laenge, an der die Sonne im Zenit steht
  float dr = astro::rad(dec);
  if (fabsf(dr) < 0.01f) dr = dr < 0 ? -0.01f : 0.01f;
  for (int x = 0; x < MW; x++) {
    const float lon = -180.0f + ((float)x + 0.5f) * (360.0f / MW);
    const float H = astro::rad(lon - subLon);
    const float latT = atanf(-cosf(H) / tanf(dr)) * (180.0f / (float)astro::PI_D);
    termRow[x] = (int16_t)(MY + (int)((90.0f - latT) * (MH / 180.0f)));
    dayAbove[x] = dr > 0;                                          // bei Sonne im Norden liegt der Tag noerdlich der Grenze
  }
  haveTerm = true;
}

// Bahnmodell: Kreisbahn mit 51,64 Grad Neigung, 92,9 min Umlauf
static void orbitAt(float lat0, float lon0, bool asc, float tMin, float& lat, float& lon) {
  const float inc = astro::rad(51.64);
  float sn = sinf(astro::rad(lat0)) / sinf(inc);
  if (sn > 1) sn = 1; if (sn < -1) sn = -1;
  float u0 = asinf(sn);
  if (!asc) u0 = (float)astro::PI_D - u0;
  const float u = u0 + tMin * (2.0f * (float)astro::PI_D / 92.9f);
  lat = asinf(sinf(inc) * sinf(u)) * (180.0f / (float)astro::PI_D);
  auto lamCont = [&](float uu) {
    const float l = atan2f(cosf(inc) * sinf(uu), cosf(uu));
    const float n = roundf((uu - l) / (2.0f * (float)astro::PI_D));
    return l + n * 2.0f * (float)astro::PI_D;
  };
  lon = lon0 + (lamCont(u) - lamCont(u0)) * (180.0f / (float)astro::PI_D) - 0.2507f * tMin;
  lon = fmodf(lon + 540.0f, 360.0f); if (lon < 0) lon += 360.0f; lon -= 180.0f;
}

static float distKm(float la1, float lo1, float la2, float lo2) {
  const float p1 = astro::rad(la1), p2 = astro::rad(la2), dl = astro::rad(lo2 - lo1);
  const float a = sinf((p2 - p1) / 2) * sinf((p2 - p1) / 2) + cosf(p1) * cosf(p2) * sinf(dl / 2) * sinf(dl / 2);
  return 12742.0f * asinf(sqrtf(a > 1 ? 1 : a));
}

// ---- Weihnachtsschlitten ----
// Position: Laenge, an der gerade Mitternacht ist; Breite pendelt langsam zwischen ca. 55 N und 35 S
static void sleighPos(double unixT, float& lat, float& lon) {
  const double h = fmod(unixT / 3600.0, 24.0);
  lon = (float)(-15.0 * h);
  while (lon < -180.0f) lon += 360.0f;
  lat = 10.0f + 45.0f * sinf((float)fmod(unixT / 2700.0, 6.2831853));
}

// Schlitten mit zwei Rentieren, fliegt nach links (Westen). (x,y) = Schlitten-Mitte
static void sleigh(int x, int y, uint32_t now) {
  const int b = ((now / 200) & 1);                                 // Galopp
  for (int k = 0; k < 2; k++) {                                    // Rentiere
    const int dx = x - 9 - k * 7, dy = y - 1 - ((b + k) & 1);
    fb->fillRect(dx, dy, 5, 2, I_DEER);
    fb->drawPixel(dx - 1, dy - 1, I_DEER); fb->drawPixel(dx - 1, dy - 2, I_DEER2);          // Kopf, Geweih
    fb->drawPixel(dx, dy - 3, I_DEER2); fb->drawPixel(dx - 2, dy - 3, I_DEER2);
    fb->drawPixel(dx, dy + 2, I_DEER); fb->drawPixel(dx + 4, dy + 2, I_DEER);               // Beine
    if (k == 1) fb->drawPixel(dx - 2, dy - 1, ((now / 300) & 1) ? I_NOSE : I_DEER);         // Rudolph
  }
  fb->drawLine(x - 9, y, x - 3, y + 1, I_SLED2);                   // Zuegel
  fb->fillRect(x - 3, y - 1, 7, 3, I_SLED);                         // Schlitten
  fb->drawFastHLine(x - 4, y + 3, 9, I_SLED2);                      // Kufe
  fb->drawPixel(x - 4, y + 2, I_SLED2);
  fb->fillRect(x, y - 4, 3, 3, I_SLED);                             // Weihnachtsmann
  fb->drawPixel(x + 1, y - 5, I_SANTA); fb->drawPixel(x, y - 3, I_SANTA);
  fb->fillRect(x + 3, y - 3, 2, 2, I_DEER);                         // Geschenkesack
}

static void drawSleigh(const FaceCtx& c) {
  const uint32_t now = c.now;
  char b[40];
  if (!(c.timeOk && c.unixT > 0)) {
    txtc(80, 109, "WARTE AUF UHRZEIT", I_DIM);
    return;
  }
  float la, lo;
  sleighPos(c.unixT, la, lo);
  // Glitzerspur: wo der Schlitten in den letzten 2 Stunden war
  for (int k = 1; k <= 24; k++) {
    float pla, plo;
    sleighPos(c.unixT - k * 300.0, pla, plo);
    const int x = xOfLon(plo) + (int)((now / 90 + k * 7) % 3) - 1, y = yOfLat(pla) + (int)((now / 130 + k * 3) % 3) - 1;
    if (((now / 120) + (uint32_t)k) % 3) fb->drawPixel(x, y, k < 8 ? I_GLIT : I_ORB2);
  }
  const int sx = xOfLon(lo), sy = yOfLat(la);
  sleigh(sx, sy, now);
  if (sx < 30) sleigh(sx + MW, sy, now);                            // Kartenrand: auch rechts zeigen
  // Kopf
  snprintf(b, sizeof(b), "%d%c %d%c", (int)fabsf(la), la >= 0 ? 'N' : 'S', (int)fabsf(lo), lo >= 0 ? 'E' : 'W');
  txtr(FB_W - 3, 3, b, I_DIG);
  txtr(FB_W - 3, 13, "WEIHNACHTSMANN", I_SLED2);
  // Fuss: Entfernung und wann er bei dir ist (lokale Mitternacht)
  const float dk = distKm(c.lat, c.lon, la, lo);
  snprintf(b, sizeof(b), "%d KM", (int)dk);
  txt(3, 109, b, I_DIG);
  int m = 24 * 60 - (c.hh * 60 + c.mm);
  if (m >= 24 * 60) m -= 24 * 60;
  if (dk < 900.0f)      snprintf(b, sizeof(b), "UEBER DIR!");
  else if (m <= 15)     snprintf(b, sizeof(b), "GLEICH BEI DIR");
  else                  snprintf(b, sizeof(b), "BEI DIR IN %d:%02d", m / 60, m % 60);
  txtr(FB_W - 3, 109, b, (dk < 900.0f || m <= 15) ? I_OK : I_DIM);
}

static void draw(const FaceCtx& c) {
  const uint32_t now = c.now;
  frameDt(now, lastT);
  const IssData* I = c.iss;

  if (c.timeOk && c.unixT > 0 && (!haveTerm || now - termAt > 30000)) { termAt = now; updateTerminator(c.unixT); }

  fb->fillScreen(I_BG);

  // ---- Karte ----
  uint8_t* base = buf();
  for (int y = 0; y < MH; y++) {
    uint8_t* row = base + (MY + y) * FB_W + MX;
    const uint8_t* mrow = ISS_LAND + y * (MW / 8);
    const int gy = MY + y;
    for (int x = 0; x < MW; x++) {
      const bool land = (mrow[x >> 3] >> (7 - (x & 7))) & 1;
      bool night = false;
      if (haveTerm) night = dayAbove[x] ? (gy > termRow[x]) : (gy < termRow[x]);
      row[x] = land ? (night ? I_LNN : I_LND) : (night ? I_OCN : I_OCD);
    }
  }
  fb->drawRect(MX, MY - 1, MW, MH + 2, I_SH);

  // Heimatort
  {
    const int hx = xOfLon(c.lon), hy = yOfLat(c.lat);
    fb->drawPixel(hx, hy, I_HOME); fb->drawCircle(hx, hy, 2, I_HOME);
  }

  if (c.opts & OPT_SLEIGH) {                                      // Weihnachtsschlitten statt ISS
    drawTime(3, 3, 2, c, cellS);
    drawTime(3, 3, 2, c, cellD);
    if (c.timeOk) { char d[12]; snprintf(d, sizeof(d), "%02u.%02u.", c.mday, c.mon); txt(62, 3, d, I_DIM); }
    drawSleigh(c);
    return;
  }

  // ---- ISS ----
  bool haveIss = I && I->status != 0;
  float ilat = 0, ilon = 0;
  if (haveIss) {
    const float dt = (float)(uint32_t)(now - I->fetchedMs) * 0.001f;
    ilat = I->lat + I->vlat * (dt < 30.0f ? dt : 30.0f);
    ilon = I->lon + I->vlon * (dt < 30.0f ? dt : 30.0f);
    if (ilat > 90) ilat = 90; if (ilat < -90) ilat = -90;
    if (ilon > 180) ilon -= 360; if (ilon < -180) ilon += 360;
    const bool asc = I->vlat >= 0;
    // Bahnspur
    float pla = ilat, plo = ilon;
    int px = xOfLon(plo), py = yOfLat(pla);
    for (int k = -6; k <= 36; k++) {                              // 5-Minuten-Schritte: 30 min zurueck bis 180 min? (hier -30 .. +180 min)
      if (k == 0) continue;
      float la, lo;
      orbitAt(ilat, ilon, asc, (float)k * 2.5f, la, lo);
      const int x = xOfLon(lo), y = yOfLat(la);
      if (k > 0 && ((k & 1) == 0)) fb->drawPixel(x, y, k < 18 ? I_ORB : I_ORB2);
      else if (k < 0) fb->drawPixel(x, y, I_ORB2);
      (void)px; (void)py;
    }
    // Sichtfeld (Footprint) und Marker
    const int ix = xOfLon(ilon), iy = yOfLat(ilat);
    for (int off = -MW; off <= MW; off += MW) fb->drawCircle(ix + off, iy, 9, I_FOOT);
    fb->fillRect(ix - 1, iy - 1, 3, 3, I_ISS);
    fb->drawFastHLine(ix - 4, iy, 3, I_ISS); fb->drawFastHLine(ix + 2, iy, 3, I_ISS);
    fb->drawFastVLine(ix - 3, iy - 2, 5, I_ORB); fb->drawFastVLine(ix + 3, iy - 2, 5, I_ORB);
    if (now / 400 & 1) fb->drawCircle(ix, iy, 5, I_ISS);

    // Sichtbarkeit alle 5 s aus dem Heimatort rechnen
    if (now - visAt > 5000 && c.timeOk && c.unixT > 0) {
      visAt = now;
      float az;
      astro::sunAltAz(c.unixT, c.lat, c.lon, sunAltHome, az);
    }
  }

  // ---- Kopf ----
  drawTime(3, 3, 2, c, cellS);
  drawTime(3, 3, 2, c, cellD);
  char b[40];
  if (haveIss) {
    snprintf(b, sizeof(b), "%d%c %d%c", (int)fabsf(ilat), ilat >= 0 ? 'N' : 'S', (int)fabsf(ilon), ilon >= 0 ? 'E' : 'W');
    txtr(FB_W - 3, 3, b, I_DIG);
    if (I->speedKmh > 1000) { snprintf(b, sizeof(b), "%d KM/H", (int)I->speedKmh); txtr(FB_W - 3, 13, b, I_DIM); }
  } else txtr(FB_W - 3, 3, "LADE ISS-DATEN", I_DIM);
  if (c.timeOk) {
    snprintf(b, sizeof(b), "%02u.%02u.", c.mday, c.mon);
    txt(62, 3, b, I_DIM);
  }

  // ---- Fuss ----
  if (haveIss) {
    const float dk = distKm(c.lat, c.lon, ilat, ilon);
    // Hoehe ueber dem Horizont (Station in ca. 420 km Hoehe)
    const float R = 6371.0f, h = 420.0f, psi = dk / R;
    const float el = atan2f(cosf(psi) - R / (R + h), sinf(psi)) * (180.0f / (float)astro::PI_D);
    const bool above = el > 0.0f;
    const bool nightHere = sunAltHome < -6.0f;
    snprintf(b, sizeof(b), "ENTF %d KM", (int)dk);
    txt(3, 109, b, above ? I_OK : I_DIG);
    if (above) txtr(FB_W - 3, 109, (nightHere ? "UEBER DIR - SICHTBAR" : "UEBER DIR"), I_OK);
    else if (I->crew) { snprintf(b, sizeof(b), "BESATZUNG %d", I->crew); txtr(FB_W - 3, 109, b, I_DIM); }
    if (I->status == 2 && now - I->fetchedMs > 60000) txt(3, 118 - 8, "DATEN VERALTET", I_ORANGE);
  } else {
    txtc(80, 109, c.wifi ? "WARTE AUF DATEN" : "KEIN WLAN", c.wifi ? I_DIM : I_RED);
  }
}

static void cover(float p, bool, uint32_t now) {
  if (p <= 0.0f) return;
  uint8_t* bf = buf();
  if (p >= 0.97f) { memset(bf, I_BG, FB_W * FB_H); return; }
  const int edge = (int)(p / 0.97f * (FB_W + 40)) - 20;
  for (int y = 0; y < FB_H; y++) {
    const int e = edge + (y >> 2) % 9 - 4 - y / 6;                // schraege Kante
    const int w = e < 0 ? 0 : (e > FB_W ? FB_W : e);
    if (w > 0) memset(bf + y * FB_W, I_BG, w);
    if (w > 0 && w < FB_W) bf[y * FB_W + w] = I_ORB;
  }
  (void)now;
}

}  // namespace f_iss
