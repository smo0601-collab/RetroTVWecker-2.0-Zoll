#pragma once
/*
  cal.h - Kalender: gesetzliche Feiertage je Bundesland, besondere Tage, Schulferien

  - Feiertage und besondere Tage werden selbst berechnet (Ostern, Advent, Muttertag, Buss- und Bettag ...),
    funktionieren also auch ohne Internet.
  - Schulferien kommen von OpenHolidays (openholidaysapi.org, frei, ohne Schluessel) und werden im Flash
    gespeichert (NVS "calfer"). Abruf: beim ersten Mal, nach Wechsel des Bundeslands und einmal pro Woche.
  - Fuer das Display: Namen ohne Umlaute (ascii()), fuer Laufschrift und Browser in UTF-8.

  Einstellungen (settings.h): calBl (0 = kein Bundesland, 1..16), calFlags (CF_*), calPop (Minuten zwischen
  den Einblendungen am Feiertag), calTick (Minuten zwischen den Kalender-Laufschriften).
*/
#include "cal_text.h"

namespace cal {

enum CalId : uint8_t {
  CD_NEUJAHR, CD_DREIKOENIG, CD_FRAUENTAG, CD_KARFREITAG, CD_OSTERSO, CD_OSTERMO, CD_MAI, CD_HIMMELFAHRT,
  CD_PFINGSTSO, CD_PFINGSTMO, CD_FRONLEICHNAM, CD_MARIA, CD_KINDERTAG, CD_EINHEIT, CD_REFORMATION,
  CD_ALLERHEILIGEN, CD_BUSSTAG, CD_WEIH1, CD_WEIH2,
  CD_VALENTIN, CD_WEIBER, CD_ROSENMO, CD_FASCHING, CD_MUTTERTAG, CD_VATERTAG, CD_ERNTEDANK, CD_HALLOWEEN,
  CD_MARTIN, CD_NIKOLAUS, CD_ADVENT1, CD_ADVENT2, CD_ADVENT3, CD_ADVENT4, CD_HEILIGABEND, CD_SILVESTER,
  CD_FERIENSTART, CD_LASTSCHOOL,
  CD_COUNT
};
static_assert(sizeof(CAL_NAME) / sizeof(CAL_NAME[0]) == CD_COUNT, "CAL_NAME passt nicht zu CalId");
static_assert(sizeof(CAL_SAY) / sizeof(CAL_SAY[0]) == CD_COUNT, "CAL_SAY passt nicht zu CalId");
static const uint8_t CD_FIRST_SPECIAL = CD_VALENTIN, CD_FIRST_SCHOOL = CD_FERIENSTART;

// Einstellungs-Bits (gSet.calFlags)
enum : uint8_t { CF_PUB = 1, CF_SPEC = 2, CF_SCHOOL = 4, CF_POP = 8, CF_TICK = 16 };
static const uint8_t CF_DEFAULT = CF_PUB | CF_SPEC | CF_SCHOOL | CF_POP | CF_TICK;

// Bundeslaender (Index 1..16; 0 = keins -> nur bundesweite Feiertage)
static const char* const BL_CODE[17] = {"", "BW", "BY", "BE", "BB", "HB", "HH", "HE", "MV", "NI", "NW", "RP", "SL", "SN", "ST", "SH", "TH"};
static const char* const BL_NAME[17] = {"-", "Baden-Württemberg", "Bayern", "Berlin", "Brandenburg", "Bremen", "Hamburg", "Hessen",
                                        "Mecklenburg-Vorpommern", "Niedersachsen", "Nordrhein-Westfalen", "Rheinland-Pfalz",
                                        "Saarland", "Sachsen", "Sachsen-Anhalt", "Schleswig-Holstein", "Thüringen"};
enum { BW = 1, BY, BE, BB, HB, HH, HE, MV, NI, NW, RP, SL, SN, ST, SH, TH };
#define CAL_B(x) (uint16_t)(1u << ((x) - 1))
static const uint16_t ALL = 0xFFFF;

// ---------------- Datum ----------------
// Tage seit 1970-01-01 (proleptischer gregorianischer Kalender)
static int32_t dnum(int y, int m, int d) {
  y -= m <= 2;
  const int32_t era = (y >= 0 ? y : y - 399) / 400;
  const int32_t yoe = y - era * 400;
  const int32_t doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;
  const int32_t doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
  return era * 146097 + doe - 719468;
}
static void civil(int32_t z, int& y, int& m, int& d) {
  z += 719468;
  const int32_t era = (z >= 0 ? z : z - 146096) / 146097;
  const int32_t doe = z - era * 146097;
  const int32_t yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
  const int32_t doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
  const int32_t mp = (5 * doy + 2) / 153;
  d = doy - (153 * mp + 2) / 5 + 1;
  m = mp < 10 ? mp + 3 : mp - 9;
  y = yoe + era * 400 + (m <= 2);
}
static int wday(int32_t dn) { return (int)(((dn % 7) + 11) % 7); }   // 0 = Sonntag (1.1.1970 war Donnerstag)
static int32_t easter(int y) {
  const int a = y % 19, b = y / 100, c = y % 100, dd = b / 4, e = b % 4, f = (b + 8) / 25, g = (b - f + 1) / 3;
  const int h = (19 * a + b - dd - g + 15) % 30, i = c / 4, k = c % 4, l = (32 + 2 * e + 2 * i - h - k) % 7;
  const int mm = (a + 11 * h + 22 * l) / 451;
  return dnum(y, (h + l - 7 * mm + 114) / 31, ((h + l - 7 * mm + 114) % 31) + 1);
}
static int32_t todayDn() {
  time_t n = time(nullptr);
  struct tm t;
  localtime_r(&n, &t);
  return dnum(t.tm_year + 1900, t.tm_mon + 1, t.tm_mday);
}

// ---------------- Feiertage eines Jahres ----------------
struct Day { int32_t dn; uint8_t id; };

static uint8_t calBl() { return gSet.calBl <= 16 ? gSet.calBl : 0; }
static bool inState(uint16_t mask) {
  if (mask == ALL) return true;
  const uint8_t bl = calBl();
  return bl && (mask & CAL_B(bl));
}

// Alle Feier- und besonderen Tage des Jahres y (nach Einstellung gefiltert), sortiert. Rueckgabe: Anzahl.
static int build(int y, Day* out, int max, uint8_t flags) {
  int n = 0;
  auto add = [&](int32_t dn, uint8_t id) {
    if (n >= max) return;
    const bool pub = id < CD_FIRST_SPECIAL;
    if (pub && !(flags & CF_PUB)) return;
    if (!pub && !(flags & CF_SPEC)) return;
    out[n++] = {dn, id};
  };
  const int32_t E = easter(y);
  auto pubIf = [&](int32_t dn, uint8_t id, uint16_t mask) { if (inState(mask)) add(dn, id); };
  pubIf(dnum(y, 1, 1), CD_NEUJAHR, ALL);
  pubIf(dnum(y, 1, 6), CD_DREIKOENIG, CAL_B(BW) | CAL_B(BY) | CAL_B(ST));
  pubIf(dnum(y, 3, 8), CD_FRAUENTAG, CAL_B(BE) | CAL_B(MV));
  pubIf(E - 2, CD_KARFREITAG, ALL);
  pubIf(E, CD_OSTERSO, CAL_B(BB));
  pubIf(E + 1, CD_OSTERMO, ALL);
  pubIf(dnum(y, 5, 1), CD_MAI, ALL);
  pubIf(E + 39, CD_HIMMELFAHRT, ALL);
  pubIf(E + 49, CD_PFINGSTSO, CAL_B(BB));
  pubIf(E + 50, CD_PFINGSTMO, ALL);
  pubIf(E + 60, CD_FRONLEICHNAM, CAL_B(BW) | CAL_B(BY) | CAL_B(HE) | CAL_B(NW) | CAL_B(RP) | CAL_B(SL));
  pubIf(dnum(y, 8, 15), CD_MARIA, CAL_B(SL) | CAL_B(BY));      // Bayern: in Gemeinden mit katholischer Mehrheit
  pubIf(dnum(y, 9, 20), CD_KINDERTAG, CAL_B(TH));
  pubIf(dnum(y, 10, 3), CD_EINHEIT, ALL);
  pubIf(dnum(y, 10, 31), CD_REFORMATION, CAL_B(BB) | CAL_B(HB) | CAL_B(HH) | CAL_B(MV) | CAL_B(NI) | CAL_B(SN) | CAL_B(ST) | CAL_B(SH) | CAL_B(TH));
  pubIf(dnum(y, 11, 1), CD_ALLERHEILIGEN, CAL_B(BW) | CAL_B(BY) | CAL_B(NW) | CAL_B(RP) | CAL_B(SL));
  {                                                   // Buss- und Bettag: Mittwoch vor dem 23. November
    int32_t d = dnum(y, 11, 22);
    while (wday(d) != 3) d--;
    pubIf(d, CD_BUSSTAG, CAL_B(SN));
  }
  pubIf(dnum(y, 12, 25), CD_WEIH1, ALL);
  pubIf(dnum(y, 12, 26), CD_WEIH2, ALL);
  // besondere Tage (ueberall)
  add(dnum(y, 2, 14), CD_VALENTIN);
  add(E - 52, CD_WEIBER);
  add(E - 48, CD_ROSENMO);
  add(E - 47, CD_FASCHING);
  {
    int32_t d = dnum(y, 5, 1);                        // Muttertag: 2. Sonntag im Mai
    while (wday(d) != 0) d++;
    add(d + 7, CD_MUTTERTAG);
  }
  add(E + 39, CD_VATERTAG);
  {
    int32_t d = dnum(y, 10, 1);                       // Erntedank: 1. Sonntag im Oktober
    while (wday(d) != 0) d++;
    add(d, CD_ERNTEDANK);
  }
  add(dnum(y, 10, 31), CD_HALLOWEEN);
  add(dnum(y, 11, 11), CD_MARTIN);
  {
    int32_t a4 = dnum(y, 12, 24);                     // 4. Advent: letzter Sonntag vor dem 25.12.
    while (wday(a4) != 0) a4--;
    add(a4 - 21, CD_ADVENT1);
    add(a4 - 14, CD_ADVENT2);
    add(dnum(y, 12, 6), CD_NIKOLAUS);
    add(a4 - 7, CD_ADVENT3);
    add(a4, CD_ADVENT4);
  }
  add(dnum(y, 12, 24), CD_HEILIGABEND);
  add(dnum(y, 12, 31), CD_SILVESTER);
  // sortieren (Einfuegesortierung, wenige Eintraege)
  for (int i = 1; i < n; i++) {
    const Day v = out[i];
    int j = i - 1;
    while (j >= 0 && out[j].dn > v.dn) { out[j + 1] = out[j]; j--; }
    out[j + 1] = v;
  }
  return n;
}

// ---------------- Schulferien ----------------
struct Fer { int32_t a, b; uint8_t type; char name[19]; };   // a..b einschliesslich (Tagesnummern)
static const int FER_MAX = 28;
static Fer     fer[FER_MAX];
static uint8_t ferN = 0, ferBl = 0;
static uint32_t ferFetched = 0;          // Unix-Zeit des letzten erfolgreichen Abrufs
static uint16_t ferVer = 0;              // steigt bei jeder Aenderung (Anzeige neu aufbauen)
static uint32_t ferTriedMs = 0;          // letzter Versuch (millis)
static char     ferErr[40] = "";

struct Blob { uint8_t ver, bl, n, _r; uint32_t fetched; Fer f[FER_MAX]; };

static void load() {
  Preferences p;
  if (!p.begin("wuhr", true)) return;
  Blob* b = (Blob*)malloc(sizeof(Blob));
  if (b) {
    const size_t got = p.getBytes("calfer", b, sizeof(Blob));
    if (got >= 8 && b->ver == 1 && b->n <= FER_MAX && got >= 8 + b->n * sizeof(Fer)) {
      ferN = b->n; ferBl = b->bl; ferFetched = b->fetched;
      memcpy(fer, b->f, ferN * sizeof(Fer));
      ferVer++;
    }
    free(b);
  }
  p.end();
}
static void save() {
  Preferences p;
  if (!p.begin("wuhr", false)) return;
  Blob* b = (Blob*)malloc(sizeof(Blob));
  if (b) {
    memset(b, 0, sizeof(Blob));
    b->ver = 1; b->bl = ferBl; b->n = ferN; b->fetched = ferFetched;
    memcpy(b->f, fer, ferN * sizeof(Fer));
    p.putBytes("calfer", b, 8 + ferN * sizeof(Fer));
    free(b);
  }
  p.end();
}

static bool parseDate(const char* s, int32_t& dn) {
  int y, m, d;
  if (!s || sscanf(s, "%d-%d-%d", &y, &m, &d) != 3 || m < 1 || m > 12 || d < 1 || d > 31) return false;
  dn = dnum(y, m, d);
  return true;
}
static uint8_t ferType(const char* name) {
  for (uint8_t i = 1; i < sizeof(FER_KEY) / sizeof(FER_KEY[0]); i++) if (strstr(name, FER_KEY[i])) return i;
  return 0;
}

// Abruf (aus dem Wetter-Task, mit gNetMutex). true = neue Daten gespeichert.
static bool fetch() {
  const uint8_t bl = calBl();
  if (!bl) return false;
#ifdef CAL_HOST
  return false;
#else
  time_t now = time(nullptr);
  struct tm t;
  localtime_r(&now, &t);
  const int y = t.tm_year + 1900;
  char url[220];
  snprintf(url, sizeof(url),
           "https://openholidaysapi.org/SchoolHolidays?countryIsoCode=DE&subdivisionCode=DE-%s&languageIsoCode=DE"
           "&validFrom=%d-12-01&validTo=%d-12-31", BL_CODE[bl], y - 1, y + 1);
  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;
  http.setConnectTimeout(10000);
  http.setTimeout(10000);
  http.setUserAgent("WetterUhr");
  if (!http.begin(client, url)) { strlcpy(ferErr, "begin", sizeof(ferErr)); return false; }
  http.addHeader("accept", "text/json");
  const int code = http.GET();
  if (code != HTTP_CODE_OK) { snprintf(ferErr, sizeof(ferErr), "HTTP %d", code); http.end(); return false; }
  const String body = http.getString();
  http.end();
  JsonDocument filter;
  filter[0]["startDate"] = true; filter[0]["endDate"] = true; filter[0]["type"] = true; filter[0]["name"][0]["text"] = true;
  JsonDocument doc;
  if (deserializeJson(doc, body, DeserializationOption::Filter(filter))) { strlcpy(ferErr, "JSON", sizeof(ferErr)); return false; }
  JsonArray arr = doc.as<JsonArray>();
  if (arr.isNull()) { strlcpy(ferErr, "leer", sizeof(ferErr)); return false; }
  Fer tmp[FER_MAX];
  int n = 0;
  for (JsonObject o : arr) {
    const char* ty = o["type"] | "School";
    if (strcmp(ty, "School") != 0) continue;
    Fer f;
    memset(&f, 0, sizeof(f));
    if (!parseDate(o["startDate"] | "", f.a) || !parseDate(o["endDate"] | "", f.b) || f.b < f.a) continue;
    const char* nm = o["name"][0]["text"] | "Ferien";
    strlcpy(f.name, nm, sizeof(f.name));
    f.type = ferType(nm);
    if (n < FER_MAX) tmp[n++] = f;
  }
  for (int i = 1; i < n; i++) {                       // nach Beginn sortieren
    const Fer v = tmp[i];
    int j = i - 1;
    while (j >= 0 && tmp[j].a > v.a) { tmp[j + 1] = tmp[j]; j--; }
    tmp[j + 1] = v;
  }
  memcpy(fer, tmp, n * sizeof(Fer));
  ferN = n; ferBl = bl; ferFetched = (uint32_t)now; ferErr[0] = 0;
  ferVer++;
  save();
#if DEBUG_SERIAL
  Serial.printf("Ferien %s: %d Eintraege\n", BL_CODE[bl], n);
#endif
  return true;
#endif
}

// im Wetter-Task aufrufen (einmal pro Sekunde): Abruf, wenn noetig
static void netTick(bool timeOk) {
  if (!timeOk || !(gSet.calFlags & CF_SCHOOL) || !calBl()) return;
  const uint32_t ms = millis();
  const uint32_t now = (uint32_t)time(nullptr);
  const bool need = ferBl != calBl() || !ferFetched || now - ferFetched > 7UL * 86400UL;
  if (!need) return;
  if (ferTriedMs && ms - ferTriedMs < (ferBl != calBl() ? 60000UL : 3600000UL)) return;   // bei Fehler spaeter wieder
  ferTriedMs = ms ? ms : 1;
  fetch();
}

static bool ferOk() { return (gSet.calFlags & CF_SCHOOL) && calBl() && ferBl == calBl() && ferN > 0; }
static int ferAt(int32_t dn) {                         // Ferien, in denen dn liegt (-1 = keine)
  if (!ferOk()) return -1;
  for (int i = 0; i < ferN; i++) if (dn >= fer[i].a && dn <= fer[i].b) return i;
  return -1;
}
static int ferNext(int32_t dn) {                       // naechste Ferien, die nach dn beginnen
  if (!ferOk()) return -1;
  for (int i = 0; i < ferN; i++) if (fer[i].a > dn) return i;
  return -1;
}
static const char* ferName(int i) {
  if (i < 0 || i >= ferN) return "";
  return fer[i].type ? FER_NAME[fer[i].type][LANG_IDX] : fer[i].name;
}
// Letzter Schultag vor den Ferien i: Werktag vor dem Ferienbeginn
static int32_t lastSchool(int i) {
  int32_t d = fer[i].a - 1;
  while (wday(d) == 0 || wday(d) == 6) d--;
  return d;
}

// ---------------- Heute / naechste Termine ----------------
struct Today { int32_t dn; uint8_t n; uint8_t ids[5]; };
static Today td{-1, 0, {0}};
static uint8_t tdFlags = 0, tdBl = 0;
static uint16_t tdVer = 0xFFFF;

static void computeToday(int32_t dn) {
  int y, m, d;
  civil(dn, y, m, d);
  Day days[48];
  const int n = build(y, days, 48, gSet.calFlags);
  td = {dn, 0, {0}};
  for (int i = 0; i < n; i++) if (days[i].dn == dn && td.n < 5) td.ids[td.n++] = days[i].id;
  if (ferOk()) {
    for (int i = 0; i < ferN; i++) {
      if (fer[i].a == dn && td.n < 5) td.ids[td.n++] = CD_FERIENSTART;
      if (lastSchool(i) == dn && td.n < 5 && ferAt(dn) < 0) td.ids[td.n++] = CD_LASTSCHOOL;
    }
  }
  tdFlags = gSet.calFlags; tdBl = gSet.calBl; tdVer = ferVer;
}
static const Today& today(int32_t dn) {
  if (dn != td.dn || tdFlags != gSet.calFlags || tdBl != gSet.calBl || tdVer != ferVer) computeToday(dn);
  return td;
}
static bool isPublic(uint8_t id) { return id < CD_FIRST_SPECIAL; }

// naechster Feier-/besonderer Tag nach dn (innerhalb eines Jahres). pubOnly: nur gesetzliche
static bool next(int32_t dn, Day& out, bool pubOnly) {
  int y, m, d;
  civil(dn, y, m, d);
  Day days[48];
  for (int yy = y; yy <= y + 1; yy++) {
    const int n = build(yy, days, 48, gSet.calFlags);
    for (int i = 0; i < n; i++)
      if (days[i].dn > dn && (!pubOnly || isPublic(days[i].id))) { out = days[i]; return true; }
  }
  return false;
}

static const char* name(uint8_t id) { return id < CD_COUNT ? CAL_NAME[id][LANG_IDX] : ""; }
static const char* say(uint8_t id, int k) { return id < CD_COUNT ? CAL_SAY[id][LANG_IDX == 0 ? 0 : 1][k & 1] : ""; }

// UTF-8 -> ASCII fuer die Pixelschrift (ae, oe, ue, ss, Akzente weg)
static void ascii(char* dst, const char* src, size_t n) {
  size_t o = 0;
  auto put = [&](const char* s) { while (*s && o + 1 < n) dst[o++] = *s++; };
  const uint8_t* p = (const uint8_t*)src;
  while (*p && o + 1 < n) {
    uint32_t cp = *p++;
    if (cp >= 0xC0) {
      int extra = cp >= 0xF0 ? 3 : (cp >= 0xE0 ? 2 : 1);
      cp &= (0x3F >> extra);
      while (extra-- && (*p & 0xC0) == 0x80) cp = (cp << 6) | (*p++ & 0x3F);
    }
    if (cp < 0x80) { dst[o++] = (char)cp; continue; }
    switch (cp) {
      case 0xE4: put("ae"); break; case 0xF6: put("oe"); break; case 0xFC: put("ue"); break;
      case 0xC4: put("Ae"); break; case 0xD6: put("Oe"); break; case 0xDC: put("Ue"); break;
      case 0xDF: put("ss"); break;
      case 0xE0: case 0xE1: case 0xE2: case 0xAA: put("a"); break;
      case 0xE8: case 0xE9: case 0xEA: case 0xEB: put("e"); break;
      case 0xC9: put("E"); break;
      case 0xEC: case 0xED: case 0xEE: case 0xEF: put("i"); break;
      case 0xF2: case 0xF3: case 0xF4: case 0xBA: put("o"); break;
      case 0xF9: case 0xFA: case 0xFB: put("u"); break;
      case 0xF1: put("n"); break; case 0xE7: put("c"); break;
      case 0x2019: put("'"); break; case 0x2013: case 0x2014: put("-"); break;
      default: break;
    }
  }
  dst[o] = 0;
}

// ---------------- Laufschrift ----------------
// Text fuer die Kalender-Laufschrift (UTF-8). k = laufende Nummer (wechselt die Sprueche). false = nichts zu sagen
static bool tickerText(char* out, size_t n, int k) {
  const int32_t dn = todayDn();
  const Today& t = today(dn);
  const int L = LANG_IDX;
  out[0] = 0;
  auto app = [&](const char* s) {
    if (!s || !*s) return;
    if (out[0]) strlcat(out, "   +++   ", n);
    strlcat(out, s, n);
  };
  char b[170];
  if (t.n) {                                            // heute
    const uint8_t id = t.ids[k % t.n];
    snprintf(b, sizeof(b), CAL_T_TODAY[L], name(id));
    app(b);
    app(say(id, k / t.n));
  } else {
    const Today saveT = td;                             // morgen? (eigene Berechnung, heute bleibt gemerkt)
    computeToday(dn + 1);
    const Today tm = td;
    td = saveT;
    for (int i = 0; i < tm.n; i++) {
      if (tm.ids[i] == CD_FERIENSTART) {
        const int f = ferAt(dn + 1);
        snprintf(b, sizeof(b), CAL_T_TOMFER[L], ferName(f));
        app(b);
      } else if (tm.ids[i] != CD_LASTSCHOOL) {
        snprintf(b, sizeof(b), CAL_T_TOMORROW[L], name(tm.ids[i]));
        app(b);
        break;
      }
    }
  }
  // Ferien
  const int fNow = ferAt(dn);
  if (fNow >= 0) {
    const int left = fer[fNow].b - dn;
    if (left == 0) snprintf(b, sizeof(b), CAL_T_FERLAST[L], ferName(fNow));
    else snprintf(b, sizeof(b), CAL_T_FERNOW[L], ferName(fNow), left);
    app(b);
  } else {
    const int f = ferNext(dn);
    if (f >= 0 && fer[f].a - dn > 1 && fer[f].a - dn <= 60) {
      snprintf(b, sizeof(b), CAL_T_FERIN[L], (int)(fer[f].a - dn), ferName(f));
      app(b);
    }
  }
  // naechster gesetzlicher Feiertag (wenn nicht heute/morgen)
  Day nx;
  if (next(dn, nx, true) && nx.dn - dn > 1 && nx.dn - dn <= 45) {
    snprintf(b, sizeof(b), CAL_T_DAYS[L], (int)(nx.dn - dn), name(nx.id));
    app(b);
  }
  return out[0] != 0;
}

// ---------------- Einblendung am Feiertag ----------------
static const uint32_t POP_MS = 20000;
static uint32_t popForceUntil = 0;
static uint8_t  popForceId = 0;
static void popTest(uint8_t id) { popForceId = id < CD_COUNT ? id : 0; popForceUntil = millis() + POP_MS; if (!popForceUntil) popForceUntil = 1; }

// Soll gerade eine Einblendung laufen? id = welches Bild
static bool popWant(uint8_t& id, bool dimActive) {
  const uint32_t ms = millis();
  if (popForceUntil) {
    if ((int32_t)(popForceUntil - ms) > 0) { id = popForceId; return true; }
    popForceUntil = 0;
  }
  if (!(gSet.calFlags & CF_POP) || dimActive) return false;
  time_t n = time(nullptr);
  struct tm t;
  localtime_r(&n, &t);
  if (t.tm_year < 120 || t.tm_hour < 7 || t.tm_hour > 21) return false;
  const Today& tt = today(dnum(t.tm_year + 1900, t.tm_mon + 1, t.tm_mday));
  if (!tt.n) return false;
  const int every = gSet.calPop >= 5 ? gSet.calPop : 15;
  const int mod = t.tm_hour * 60 + t.tm_min;
  if (mod % every != 0 || t.tm_sec >= (int)(POP_MS / 1000)) return false;
  id = tt.ids[(mod / every) % tt.n];
  return true;
}

// ---------------- Browser: Jahresuebersicht als JSON ----------------
static void json(String& j, int y) {
  j = "{\"y\":"; j += y;
  j += ",\"bl\":"; j += calBl();
  j += ",\"days\":[";
  Day days[48];
  const int n = build(y, days, 48, gSet.calFlags | CF_PUB | CF_SPEC);
  for (int i = 0; i < n; i++) {
    int yy, m, d;
    civil(days[i].dn, yy, m, d);
    char b[96];
    snprintf(b, sizeof(b), "%s[%d,%d,%d,\"", i ? "," : "", m, d, isPublic(days[i].id) ? 1 : 2);
    j += b; j += name(days[i].id); j += "\"]";
  }
  j += "],\"fer\":[";
  bool first = true;
  if (ferOk())
    for (int i = 0; i < ferN; i++) {
      int y1, m1, d1, y2, m2, d2;
      civil(fer[i].a, y1, m1, d1);
      civil(fer[i].b, y2, m2, d2);
      if (y2 < y || y1 > y) continue;
      char b[64];
      snprintf(b, sizeof(b), "%s[\"%04d-%02d-%02d\",\"%04d-%02d-%02d\",\"", first ? "" : ",", y1, m1, d1, y2, m2, d2);
      j += b; j += ferName(i); j += "\"]";
      first = false;
    }
  j += "],\"ferN\":"; j += ferBl == calBl() ? ferN : 0;
  j += ",\"fetched\":"; j += ferBl == calBl() ? ferFetched : 0;
  j += ",\"err\":\""; j += ferErr; j += "\"}";
}

}  // namespace cal
