#pragma once
/*
  astro.h - Sonnen-, Mond- und Planetenstand aus Datum, Uhrzeit und Standort (ohne Internet).
  Verfahren: Paul Schlyter, "How to compute planetary positions" (Kepler-Elemente mit Stoerungstermen,
  Genauigkeit: Sonne/Planeten ca. 1-2 Bogenminuten, Mond ca. 0,3 Grad). Fuer Anzeige voellig ausreichend.
  Zeit: d = Tage seit 1999-12-31 0:00 UT (aus der Unix-Zeit), Winkel in Grad.
*/
#include <math.h>
#include <stdint.h>

namespace astro {

static const double PI_D = 3.14159265358979323846;
static inline float rad(double deg) { return (float)(deg * (PI_D / 180.0)); }
static inline double wrap360(double a) { a = fmod(a, 360.0); return a < 0 ? a + 360.0 : a; }
static inline float wrap360f(float a) { a = fmodf(a, 360.0f); return a < 0 ? a + 360.0f : a; }

static inline double dayNumber(double unixT) { return unixT / 86400.0 - 10956.0; }
static inline float  obliq(double d) { return (float)(23.4393 - 3.563e-7 * d); }

// Kepler-Gleichung: exzentrische Anomalie (Radiant) aus mittlerer Anomalie M (Grad)
static float kepler(double Mdeg, float e) {
  float M = rad(wrap360(Mdeg));
  float E = M + e * sinf(M) * (1.0f + e * cosf(M));
  for (int i = 0; i < 4; i++) E -= (E - e * sinf(E) - M) / (1.0f - e * cosf(E));
  return E;
}

// ---- Sonne: ekliptikale Laenge (Grad) und Abstand (AE) ----
static void sunEcl(double d, float& lon, float& r) {
  const double w = 282.9404 + 4.70935e-5 * d, M = 356.0470 + 0.9856002585 * d;
  const float e = (float)(0.016709 - 1.151e-9 * d);
  const float E = kepler(M, e);
  const float xv = cosf(E) - e, yv = sqrtf(1.0f - e * e) * sinf(E);
  const float v = atan2f(yv, xv) * (180.0f / (float)PI_D);
  r = sqrtf(xv * xv + yv * yv);
  lon = wrap360f(v + (float)wrap360(w));
}

// ---- Mond: ekliptikale Laenge/Breite (Grad), Abstand (Erdradien) ----
static void moonEcl(double d, float& lon, float& lat, float& dist) {
  const double N = wrap360(125.1228 - 0.0529538083 * d), w = wrap360(318.0634 + 0.1643573223 * d);
  const double M = wrap360(115.3654 + 13.0649929509 * d);
  const double Ms = wrap360(356.0470 + 0.9856002585 * d), ws = 282.9404 + 4.70935e-5 * d;
  const float e = 0.0549f, a = 60.2666f, inc = rad(5.1454);
  const float E = kepler(M, e);
  const float xv = a * (cosf(E) - e), yv = a * sqrtf(1.0f - e * e) * sinf(E);
  const float v = atan2f(yv, xv), r = sqrtf(xv * xv + yv * yv);
  const float vw = v + rad(w), Nr = rad(N);
  const float xh = r * (cosf(Nr) * cosf(vw) - sinf(Nr) * sinf(vw) * cosf(inc));
  const float yh = r * (sinf(Nr) * cosf(vw) + cosf(Nr) * sinf(vw) * cosf(inc));
  const float zh = r * sinf(vw) * sinf(inc);
  float lo = atan2f(yh, xh) * (180.0f / (float)PI_D), la = atan2f(zh, sqrtf(xh * xh + yh * yh)) * (180.0f / (float)PI_D);
  const double Lm = wrap360(N + w + M), Ls = wrap360(ws + Ms);
  const float D = rad(wrap360(Lm - Ls)), F = rad(wrap360(Lm - N)), Mm = rad(M), Mso = rad(Ms);
  lo += -1.274f * sinf(Mm - 2 * D) + 0.658f * sinf(2 * D) - 0.186f * sinf(Mso) - 0.059f * sinf(2 * Mm - 2 * D)
        - 0.057f * sinf(Mm - 2 * D + Mso) + 0.053f * sinf(Mm + 2 * D) + 0.046f * sinf(2 * D - Mso)
        + 0.041f * sinf(Mm - Mso) - 0.035f * sinf(D) - 0.031f * sinf(Mm + Mso) - 0.015f * sinf(2 * F - 2 * D)
        + 0.011f * sinf(Mm - 4 * D);
  la += -0.173f * sinf(F - 2 * D) - 0.055f * sinf(Mm - F - 2 * D) - 0.046f * sinf(Mm + F - 2 * D)
        + 0.033f * sinf(F + 2 * D) + 0.017f * sinf(2 * Mm + F);
  dist = r - 0.58f * cosf(Mm - 2 * D) - 0.46f * cosf(2 * D);
  lon = wrap360f(lo); lat = la;
}

// ---- Planeten (0 Merkur, 1 Venus, 2 Mars, 3 Jupiter, 4 Saturn): geozentrische ekliptikale Laenge ----
static float planetLon(int k, double d) {
  static const double EL[5][12] = {
    {48.3313, 3.24587e-5, 7.0047, 5.0e-8, 29.1241, 1.01444e-5, 0.387098, 0.205635, 5.59e-10, 168.6562, 4.0923344368, 0},
    {76.6799, 2.46590e-5, 3.3946, 2.75e-8, 54.8910, 1.38374e-5, 0.723330, 0.006773, -1.302e-9, 48.0052, 1.6021302244, 0},
    {49.5574, 2.11081e-5, 1.8497, -1.78e-8, 286.5016, 2.92961e-5, 1.523688, 0.093405, 2.516e-9, 18.6021, 0.5240207766, 0},
    {100.4542, 2.76854e-5, 1.3030, -1.557e-7, 273.8777, 1.64505e-5, 5.20256, 0.048498, 4.469e-9, 19.8950, 0.0830853001, 0},
    {113.6634, 2.38980e-5, 2.4886, -1.081e-7, 339.3939, 2.97661e-5, 9.55475, 0.055546, -9.499e-9, 316.9670, 0.0334442282, 0}};
  const double* p = EL[k];
  const double N = wrap360(p[0] + p[1] * d), w = wrap360(p[4] + p[5] * d), M = wrap360(p[9] + p[10] * d);
  const float inc = rad(p[2] + p[3] * d), a = (float)p[6], e = (float)(p[7] + p[8] * d);
  const float E = kepler(M, e);
  const float xv = a * (cosf(E) - e), yv = a * sqrtf(1.0f - e * e) * sinf(E);
  const float v = atan2f(yv, xv), r = sqrtf(xv * xv + yv * yv);
  const float vw = v + rad(w), Nr = rad(N);
  float xh = r * (cosf(Nr) * cosf(vw) - sinf(Nr) * sinf(vw) * cosf(inc));
  float yh = r * (sinf(Nr) * cosf(vw) + cosf(Nr) * sinf(vw) * cosf(inc));
  float zh = r * sinf(vw) * sinf(inc);
  if (k >= 3) {                                           // Jupiter/Saturn: gegenseitige Stoerungen
    const float Mj = rad(wrap360(19.8950 + 0.0830853001 * d)), Msa = rad(wrap360(316.9670 + 0.0334442282 * d));
    float lo = atan2f(yh, xh) * (180.0f / (float)PI_D), la = atan2f(zh, sqrtf(xh * xh + yh * yh)) * (180.0f / (float)PI_D);
    if (k == 3) lo += -0.332f * sinf(2 * Mj - 5 * Msa - rad(67.6)) - 0.056f * sinf(2 * Mj - 2 * Msa + rad(21))
                      + 0.042f * sinf(3 * Mj - 5 * Msa + rad(21)) - 0.036f * sinf(Mj - 2 * Msa) + 0.022f * cosf(Mj - Msa)
                      + 0.023f * sinf(2 * Mj - 3 * Msa + rad(52)) - 0.016f * sinf(Mj - 5 * Msa - rad(69));
    else {
      lo += 0.812f * sinf(2 * Mj - 5 * Msa - rad(67.6)) - 0.229f * cosf(2 * Mj - 4 * Msa - rad(2))
            + 0.119f * sinf(Mj - 2 * Msa - rad(3)) + 0.046f * sinf(2 * Mj - 6 * Msa - rad(69)) + 0.014f * sinf(Mj - 3 * Msa + rad(32));
      la += -0.020f * cosf(2 * Mj - 4 * Msa - rad(2)) + 0.018f * sinf(2 * Mj - 6 * Msa - rad(49));
    }
    const float lr = rad(lo), br = rad(la), rr = r * cosf(br);
    xh = rr * cosf(lr); yh = rr * sinf(lr); zh = r * sinf(br);
  }
  float sl, sr;
  sunEcl(d, sl, sr);
  const float xs = sr * cosf(rad(sl)), ys = sr * sinf(rad(sl));
  return wrap360f(atan2f(yh + ys, xh + xs) * (180.0f / (float)PI_D));
}

// ---- Koordinaten ----
static void eclToEq(float lonDeg, float latDeg, double d, float& ra, float& dec) {
  const float l = rad(lonDeg), b = rad(latDeg), ec = rad(obliq(d));
  const float x = cosf(l) * cosf(b), y = sinf(l) * cosf(b) * cosf(ec) - sinf(b) * sinf(ec), z = sinf(l) * cosf(b) * sin(ec) + sinf(b) * cosf(ec);
  ra = wrap360f(atan2f(y, x) * (180.0f / (float)PI_D));
  dec = asinf(z) * (180.0f / (float)PI_D);
}
static void altAz(float raDeg, float decDeg, double d, float latDeg, float lonDeg, float& alt, float& az) {
  const double gmst = wrap360(280.46061837 + 360.98564736629 * (d - 1.5));
  const float H = rad(wrap360(gmst + lonDeg - raDeg)), la = rad(latDeg), de = rad(decDeg);
  const float sa = sinf(la) * sinf(de) + cosf(la) * cosf(de) * cosf(H);
  alt = asinf(sa) * (180.0f / (float)PI_D);
  const float y = -cosf(de) * sinf(H), x = sinf(de) * cosf(la) - cosf(de) * cosf(H) * sinf(la);
  az = wrap360f(atan2f(y, x) * (180.0f / (float)PI_D));
}

// Sonne: Hoehe/Azimut (Grad) mit Refraktion am Horizont (-0,833 Grad wird beim Auf-/Untergang beruecksichtigt)
static void sunAltAz(double unixT, float lat, float lon, float& alt, float& az) {
  const double d = dayNumber(unixT);
  float l, r, ra, dec;
  sunEcl(d, l, r);
  eclToEq(l, 0.0f, d, ra, dec);
  altAz(ra, dec, d, lat, lon, alt, az);
}
// Mond: Hoehe/Azimut (topozentrisch, Parallaxe beruecksichtigt)
static void moonAltAz(double unixT, float lat, float lon, float& alt, float& az) {
  const double d = dayNumber(unixT);
  float l, b, r, ra, dec;
  moonEcl(d, l, b, r);
  eclToEq(l, b, d, ra, dec);
  altAz(ra, dec, d, lat, lon, alt, az);
  alt -= (180.0f / (float)PI_D) * asinf(1.0f / r) * cosf(rad(alt));
}
// Mondphase: Elongation (0..360, 0 = Neumond, 180 = Vollmond) und beleuchteter Anteil 0..1
static void moonPhase(double unixT, float& elong, float& illum) {
  const double d = dayNumber(unixT);
  float sl, sr, ml, mb, md;
  sunEcl(d, sl, sr);
  moonEcl(d, ml, mb, md);
  elong = wrap360f(ml - sl);
  illum = (1.0f - cosf(rad(elong))) * 0.5f;
}
static float moonLon(double unixT) { float l, b, r; moonEcl(dayNumber(unixT), l, b, r); return l; }
static float sunLon(double unixT) { float l, r; sunEcl(dayNumber(unixT), l, r); return l; }

}  // namespace astro
