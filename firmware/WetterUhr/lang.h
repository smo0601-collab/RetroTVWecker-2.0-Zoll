#pragma once
/*
  lang.h - Sprachen: Deutsch, Englisch, Franzoesisch, Spanisch, Italienisch

  Alle Texte, die auf dem Display oder der Browser-Seite erscheinen, stehen hier in einer Zeile pro Text
  in allen fuenf Sprachen: X(Kennung, Deutsch, Englisch, Franzoesisch, Spanisch, Italienisch).
  Abruf mit TR(Kennung); die aktive Sprache steht in gLang (wird aus den Einstellungen gesetzt).

  Display-Texte (D_...): Grossbuchstaben fuer die eingebaute 5x7-Pixelschrift, ohne Akzente.
  Ausnahme: "\xA5" ist das N mit Tilde (Zeichensatz CP437), z. B. "A\xA5O" = ANO mit Tilde.
  Klassische Designs (K_...): normale Schreibweise, nur ASCII (die grossen Schriften kennen keine Akzente).
  Browser-Texte (W_...) stehen in lang_web.h und sind UTF-8.

  Neue Sprache: LANG_COUNT erhoehen, in jeder X(...)-Zeile eine Spalte anhaengen, LANG_NAME ergaenzen.
*/
#include <stdint.h>
#include <stdio.h>

enum : uint8_t { L_DE, L_EN, L_FR, L_ES, L_IT, LANG_COUNT };
static const char* const LANG_NAME[LANG_COUNT] = {"Deutsch", "English", "Fran\xC3\xA7" "ais", "Espa\xC3\xB1ol", "Italiano"};
static const char* const LANG_CODE[LANG_COUNT] = {"de", "en", "fr", "es", "it"};
static uint8_t gLang = L_DE;

// ---------------------------------------------------------------------------------------------
// Display-Texte
// ---------------------------------------------------------------------------------------------
#define LANG_DISPLAY(X) \
  /* allgemein */ \
  X(D_DAY,          "TAG", "DAY", "JOUR", "DIA", "GIORNO") \
  X(D_DAYS,         "TAGE", "DAYS", "JOURS", "DIAS", "GIORNI") \
  X(D_WAIT_TIME,    "WARTE AUF ZEIT", "WAITING FOR TIME", "ATTENTE HEURE", "ESPERANDO HORA", "ATTESA ORARIO") \
  X(D_WAIT_DATA,    "WARTE AUF DATEN", "WAITING FOR DATA", "ATTENTE DONNEES", "ESPERANDO DATOS", "ATTESA DATI") \
  X(D_NO_WIFI,      "KEIN WLAN", "NO WIFI", "PAS DE WIFI", "SIN WIFI", "NO WIFI") \
  X(D_RESUME,       "FORTSETZUNG", "RESUMING", "REPRISE", "CONTINUAMOS", "SI RIPRENDE") \
  /* Weihnachten */ \
  X(D_TO_XMAS,      "BIS HEILIGABEND", "TO CHRISTMAS EVE", "JUSQU'A NOEL", "PARA NOCHEBUENA", "ALLA VIGILIA") \
  X(D_TODAY_IS,     "HEUTE IST", "TODAY IS", "AUJOURD'HUI", "HOY ES", "OGGI E'") \
  X(D_XMAS_EVE,     "HEILIGABEND", "CHRISTMAS EVE", "LE REVEILLON", "NOCHEBUENA", "LA VIGILIA") \
  X(D_TO_GIFTS,     "BIS ZUR BESCHERUNG", "UNTIL PRESENTS", "AVANT LES CADEAUX", "PARA LOS REGALOS", "AI REGALI") \
  X(D_MERRY,        "FROHE", "MERRY", "JOYEUX", "FELIZ", "BUON") \
  X(D_XMAS,         "WEIHNACHTEN", "CHRISTMAS", "NOEL", "NAVIDAD", "NATALE") \
  X(D_MERRY_XMAS,   "FROHE WEIHNACHTEN!", "MERRY CHRISTMAS!", "JOYEUX NOEL!", "FELIZ NAVIDAD!", "BUON NATALE!") \
  /* Weihnachtself */ \
  X(D_ELF_DAYS,     "NOCH %d TAGE!", "%d DAYS TO GO!", "ENCORE %d JOURS!", "FALTAN %d DIAS!", "ANCORA %d GIORNI!") \
  X(D_ELF_GIFTS,    "%d GESCHENKE!", "%d GIFTS!", "%d CADEAUX!", "%d REGALOS!", "%d REGALI!") \
  X(D_ELF_YAY,      "JUHUU!", "YAY!", "YOUPI!", "YUJU!", "EVVIVA!") \
  X(D_ELF_OOPS,     "HOPPLA!", "OOPS!", "OUPS!", "UY!", "OPS!") \
  /* Halloween */ \
  X(D_HAPPY_HAL,    "HAPPY", "HAPPY", "JOYEUX", "FELIZ", "BUON") \
  X(D_TO_HAL,       "BIS HALLOWEEN", "TO HALLOWEEN", "AVANT HALLOWEEN", "PARA HALLOWEEN", "A HALLOWEEN") \
  X(D_BOO,          "BUH!", "BOO!", "BOU!", "BU!", "BU!") \
  /* Ostern */ \
  X(D_HAPPY_EASTER, "FROHE", "HAPPY", "JOYEUSES", "FELICES", "BUONA") \
  X(D_EASTER,       "OSTERN", "EASTER", "PAQUES", "PASCUAS", "PASQUA") \
  X(D_TO_EASTER,    "BIS OSTERN", "TO EASTER", "AVANT PAQUES", "PARA PASCUA", "A PASQUA") \
  X(D_PEEP,         "PIEP!", "PEEP!", "PIOU!", "PIO!", "PIO!") \
  /* Valentinstag */ \
  X(D_VAL_L1,       "ALLES LIEBE", "HAPPY", "JOYEUSE", "FELIZ", "BUON") \
  X(D_VAL_L2,       "ZUM VALENTINSTAG", "VALENTINE'S DAY", "SAINT-VALENTIN", "SAN VALENTIN", "SAN VALENTINO") \
  X(D_TO_VAL,       "BIS ZUM VALENTINSTAG", "TO VALENTINE'S DAY", "AVANT LA ST-VALENTIN", "PARA SAN VALENTIN", "A SAN VALENTINO") \
  /* Silvester */ \
  X(D_NYE,          "SILVESTER", "NEW YEAR'S EVE", "SAINT-SYLVESTRE", "NOCHEVIEJA", "SAN SILVESTRO") \
  X(D_UNTIL_YEAR,   "BIS %d", "TO %d", "AVANT %d", "HASTA %d", "AL %d") \
  X(D_CHEERS,       "PROSIT", "HAPPY", "BONNE", "FELIZ", "BUON") \
  X(D_NEWYEAR_Y,    "NEUJAHR %d", "NEW YEAR %d", "ANNEE %d", "A\xA5O %d", "ANNO %d") \
  X(D_NY_L1,        "FROHES NEUES", "HAPPY NEW", "BONNE ANNEE", "FELIZ A\xA5O", "BUON ANNO") \
  X(D_NY_L2,        "JAHR %d", "YEAR %d", "%d", "NUEVO %d", "%d") \
  /* Badminton */ \
  X(D_RED,          "ROT", "RED", "ROUGE", "ROJO", "ROSSO") \
  X(D_BLUE,         "BLAU", "BLUE", "BLEU", "AZUL", "BLU") \
  X(D_SET_N,        "SATZ %u", "SET %u", "SET %u", "SET %u", "SET %u") \
  X(D_BREAK,        "PAUSE", "BREAK", "PAUSE", "DESCANSO", "PAUSA") \
  X(D_AT_11,        "BEI 11 PUNKTEN", "AT 11 POINTS", "A 11 POINTS", "A 11 PUNTOS", "A 11 PUNTI") \
  X(D_CHANGE_ENDS,  "SEITENWECHSEL", "CHANGE ENDS", "CHANGEMENT", "CAMBIO DE LADO", "CAMBIO CAMPO") \
  X(D_SET3_AT_11,   "SATZ 3 - BEI 11 PUNKTEN", "SET 3 - AT 11 POINTS", "SET 3 - A 11 POINTS", "SET 3 - A 11 PUNTOS", "SET 3 - A 11 PUNTI") \
  X(D_WINS,         "SIEG %s", "%s WINS", "VICTOIRE %s", "GANA %s", "VINCE %s") \
  X(D_SETS_SCORE,   "%u:%u SAETZE", "%u:%u SETS", "%u:%u SETS", "%u:%u SETS", "%u:%u SET") \
  X(D_SETPOINT,     "SATZBALL", "SET POINT", "BALLE DE SET", "PUNTO DE SET", "SET POINT") \
  X(D_MATCHPOINT,   "MATCHBALL", "MATCH POINT", "BALLE DE MATCH", "PUNTO DE PARTIDO", "MATCH POINT") \
  X(D_DEUCE,        "VERLAENGERUNG", "EXTRA POINTS", "PROLONGATION", "PROLONGACION", "AI VANTAGGI") \
  X(D_RALLY_N,      "RALLYE %u", "RALLY %u", "ECHANGE %u", "PELOTEO %u", "SCAMBIO %u") \
  X(D_DROPSHOT,     "STOPPBALL!", "DROP SHOT!", "AMORTI!", "DEJADA!", "SMORZATA!") \
  X(D_NETCORD,      "NETZROLLER!", "NET CORD!", "BANDE DU FILET!", "TOCA LA RED!", "NASTRO!") \
  X(D_POINT,        "PUNKT!", "POINT!", "POINT!", "PUNTO!", "PUNTO!") \
  X(D_WHAT_RALLY,   "WAS FUER EIN BALLWECHSEL!", "WHAT A RALLY!", "QUEL ECHANGE!", "QUE PELOTEO!", "CHE SCAMBIO!") \
  X(D_NET,          "NETZ!", "NET!", "FILET!", "RED!", "RETE!") \
  X(D_OUT,          "AUS!", "OUT!", "FAUTE!", "FUERA!", "FUORI!") \
  X(D_OUT_CHAL,     "AUS?  CHALLENGE!", "OUT?  CHALLENGE!", "FAUTE?  CHALLENGE!", "FUERA?  CHALLENGE!", "FUORI?  CHALLENGE!") \
  X(D_IN,           "IN!", "IN!", "BONNE!", "DENTRO!", "DENTRO!") \
  /* Squishy Dumpling, Hund */ \
  X(D_YAWN,         "GAEHN!", "YAWN!", "BAILLE!", "BOSTEZO!", "SBADIGLIO!") \
  X(D_GIGGLE,       "HIHI!", "HEHE!", "HIHI!", "JIJI!", "IHIH!") \
  X(D_WOOF,         "WUFF!", "WOOF!", "OUAF!", "GUAU!", "BAU!") \
  /* Flugradar */ \
  X(D_LOADING,      "LADE...", "LOADING...", "CHARGE...", "CARGANDO...", "CARICO...") \
  X(D_NO_DATA,      "KEINE DATEN", "NO DATA", "AUCUNE DONNEE", "SIN DATOS", "NESSUN DATO") \
  X(D_NO_PLANES,    "KEINE FLUGZEUGE", "NO AIRCRAFT", "AUCUN AVION", "SIN AVIONES", "NESSUN AEREO") \
  X(D_OUTDATED,     "VERALTET", "OUTDATED", "PERIME", "ANTIGUO", "VECCHIO") \
  X(D_N_PLANES,     "%d FLZ", "%d A/C", "%d AV.", "%d AV.", "%d AER.") \
  X(D_GROUND,       "BODEN", "GROUND", "SOL", "SUELO", "SUOLO") \
  X(D_WAIT1,        "WARTE", "WAIT", "ATTENTE", "ESPE-", "IN") \
  X(D_WAIT2,        "AUF", "FOR", "DES", "RANDO", "ATTESA") \
  X(D_WAIT3,        "DATEN", "DATA", "DONNEES", "DATOS", "DATI") \
  X(D_NOPL1,        "KEINE", "NO", "AUCUN", "SIN", "NESSUN") \
  X(D_NOPL2,        "FLUG-", "AIR-", "AVION", "AVIO-", "AEREO") \
  X(D_NOPL3,        "ZEUGE", "CRAFT", "", "NES", "") \
  X(D_UNKNOWN,      "UNBEK.", "UNKN.", "INCON.", "DESC.", "IGNOTO") \
  X(D_UFO_EX,       "UFO!", "UFO!", "OVNI!", "OVNI!", "UFO!") \
  X(D_UFO_Q,        "UFO?", "UFO?", "OVNI?", "OVNI?", "UFO?") \
  X(D_PLUS_UFO,     "+1 UFO", "+1 UFO", "+1 OVNI", "+1 OVNI", "+1 UFO") \
  /* ISS und Weihnachtsschlitten */ \
  X(D_ISS_LOAD,     "LADE ISS-DATEN", "LOADING ISS DATA", "CHARGEMENT ISS", "CARGANDO ISS", "CARICO DATI ISS") \
  X(D_DIST_KM,      "ENTF %d KM", "DIST %d KM", "DIST %d KM", "DIST %d KM", "DIST %d KM") \
  X(D_OVERHEAD_VIS, "UEBER DIR - SICHTBAR", "OVERHEAD - VISIBLE", "AU-DESSUS - VISIBLE", "ENCIMA - VISIBLE", "SOPRA DI TE - VISIBILE") \
  X(D_OVERHEAD,     "UEBER DIR", "OVERHEAD", "AU-DESSUS", "ENCIMA", "SOPRA DI TE") \
  X(D_OVERHEAD_EX,  "UEBER DIR!", "OVERHEAD!", "AU-DESSUS!", "ENCIMA!", "SOPRA DI TE!") \
  X(D_CREW,         "BESATZUNG %d", "CREW %d", "EQUIPAGE %d", "TRIPULACION %d", "EQUIPAGGIO %d") \
  X(D_DATA_OLD,     "DATEN VERALTET", "DATA OUTDATED", "DONNEES PERIMEES", "DATOS ANTIGUOS", "DATI VECCHI") \
  X(D_SANTA,        "WEIHNACHTSMANN", "SANTA CLAUS", "PERE NOEL", "PAPA NOEL", "BABBO NATALE") \
  X(D_SANTA_SOON,   "GLEICH BEI DIR", "ARRIVING SOON", "BIENTOT CHEZ TOI", "LLEGA PRONTO", "ARRIVA PRESTO") \
  X(D_SANTA_IN,     "BEI DIR IN %d:%02d", "WITH YOU IN %d:%02d", "ICI DANS %d:%02d", "CONTIGO EN %d:%02d", "DA TE TRA %d:%02d") \
  /* Raketenabwehr */ \
  X(D_SITREP,       "LAGE", "SITREP", "ETAT", "ESTADO", "STATO") \
  /* Sonne und Mond */ \
  X(D_SUN,          "SONNE", "SUN", "SOLEIL", "SOL", "SOLE") \
  X(D_MOON,         "MOND", "MOON", "LUNE", "LUNA", "LUNA") \
  X(D_RISE,         "AUF %s", "\x18 %s", "\x18 %s", "\x18 %s", "\x18 %s") \
  X(D_SET,          "UNT %s", "\x19 %s", "\x19 %s", "\x19 %s", "\x19 %s") \
  X(D_ALT,          "ALT %d", "ALT %d", "HAUT %d", "ALT %d", "ALT %d") \
  X(D_NEWMOON,      "NEUMOND", "NEW MOON", "NOUV. LUNE", "LUNA NUEVA", "LUNA NUOVA") \
  X(D_WAXING,       "ZUNEHMEND", "WAXING", "CROISSANT", "CRECIENTE", "CRESCENTE") \
  X(D_FIRSTQ,       "1. VIERTEL", "1ST QUARTER", "PREM. QUART.", "CUARTO CREC.", "PRIMO QUARTO") \
  X(D_FULLMOON,     "VOLLMOND", "FULL MOON", "PLEINE LUNE", "LUNA LLENA", "LUNA PIENA") \
  X(D_WANING,       "ABNEHMEND", "WANING", "DECROISSANT", "MENGUANTE", "CALANTE") \
  X(D_LASTQ,        "3. VIERTEL", "LAST QUARTER", "DERN. QUART.", "CUARTO MENG.", "ULT. QUARTO") \
  /* klassische Designs (normale Schreibweise, ASCII) */ \
  X(K_FEELS,        "Gefuehlt", "Feels", "Ressenti", "Sensac.", "Percep.") \
  X(K_HUMID,        "Feuchte", "Humid.", "Humid.", "Humed.", "Umid.") \
  X(K_WIND,         "Wind", "Wind", "Vent", "Viento", "Vento") \
  X(K_NO_WEATHER,   "Keine Wetterdaten", "No weather data", "Pas de meteo", "Sin datos meteo", "Nessun meteo") \
  X(K_WX_OLD,       "Wetter veraltet", "Weather outdated", "Meteo perimee", "Meteo antigua", "Meteo vecchio") \
  X(K_NO_WX,        "kein Wetter", "no weather", "pas de meteo", "sin meteo", "niente meteo") \
  X(K_NO_WIFI,      "Kein WLAN", "No WiFi", "Pas de WiFi", "Sin WiFi", "Nessun WiFi") \
  X(K_STARTING,     "Starte ...", "Starting ...", "Demarrage ...", "Iniciando ...", "Avvio ...") \
  X(K_FW_UPDATE,    "Firmware-Update", "Firmware update", "Mise a jour", "Actualizacion", "Aggiornamento") \
  X(K_OTA_IDE,      "Arduino IDE, bitte warten ...", "Arduino IDE, please wait ...", "Arduino IDE, patientez ...", "Arduino IDE, espere ...", "Arduino IDE, attendere ...") \
  X(K_OTA_DONE,     "Fertig - Neustart", "Done - restarting", "Termine - redemarrage", "Listo - reiniciando", "Fatto - riavvio") \
  X(K_OTA_RECV,     "Daten werden empfangen ...", "Receiving data ...", "Reception des donnees ...", "Recibiendo datos ...", "Ricezione dati ...") \
  /* Wetterzustand (WMO-Code), klassische Anzeige */ \
  X(K_WX_CLEAR,     "Klar", "Clear", "Degage", "Despejado", "Sereno") \
  X(K_WX_MCLEAR,    "Meist klar", "Mostly clear", "Peu nuageux", "Poco nuboso", "Poco nuvoloso") \
  X(K_WX_PCLOUD,    "Wolkig", "Partly cloudy", "Nuageux", "Nuboso", "Nuvoloso") \
  X(K_WX_OVERCAST,  "Bedeckt", "Overcast", "Couvert", "Cubierto", "Coperto") \
  X(K_WX_FOG,       "Nebel", "Fog", "Brouillard", "Niebla", "Nebbia") \
  X(K_WX_DRIZZLE,   "Nieselregen", "Drizzle", "Bruine", "Llovizna", "Pioggerella") \
  X(K_WX_FDRIZZLE,  "Eisniesel", "Frz. drizzle", "Bruine vergl.", "Llovizna hel.", "Piogger. gel.") \
  X(K_WX_LRAIN,     "Lt. Regen", "Light rain", "Pluie faible", "Lluvia debil", "Pioggia legg.") \
  X(K_WX_RAIN,      "Regen", "Rain", "Pluie", "Lluvia", "Pioggia") \
  X(K_WX_HRAIN,     "Starkregen", "Heavy rain", "Forte pluie", "Lluvia fuerte", "Pioggia forte") \
  X(K_WX_FRAIN,     "Eisregen", "Frz. rain", "Pluie vergl.", "Lluvia helada", "Pioggia gel.") \
  X(K_WX_LSNOW,     "Lt. Schnee", "Light snow", "Neige faible", "Nieve debil", "Neve legg.") \
  X(K_WX_SNOW,      "Schnee", "Snow", "Neige", "Nieve", "Neve") \
  X(K_WX_HSNOW,     "Starkschnee", "Heavy snow", "Forte neige", "Nevada fuerte", "Neve forte") \
  X(K_WX_GRAINS,    "Griesel", "Snow grains", "Neige en gr.", "Granizo fino", "Nevischio") \
  X(K_WX_LSHOWER,   "Lt. Schauer", "Lt. showers", "Averses fbl.", "Chubascos dbl", "Rovesci legg.") \
  X(K_WX_SHOWER,    "Schauer", "Showers", "Averses", "Chubascos", "Rovesci") \
  X(K_WX_HSHOWER,   "St. Schauer", "Hvy. showers", "Fortes aver.", "Chubasc. fuer", "Rovesci forti") \
  X(K_WX_SSHOWER,   "Schneefall", "Snow showers", "Averses neige", "Chubasc. nieve", "Rovesci neve") \
  X(K_WX_STORM,     "Gewitter", "Thunderstorm", "Orage", "Tormenta", "Temporale") \
  X(K_WX_HAIL,      "Gewit.+Hagel", "Storm+hail", "Orage+grele", "Torm.+granizo", "Temp.+grandine")

#include "lang_web.h"

// ---------------------------------------------------------------------------------------------
// Tabelle und Zugriff
// ---------------------------------------------------------------------------------------------
#define LANG_ENUM(id, de, en, fr, es, it) id,
#define LANG_ROW(id, de, en, fr, es, it) {de, en, fr, es, it},
enum : uint16_t { LANG_DISPLAY(LANG_ENUM) LANG_WEB(LANG_ENUM) TXT_COUNT };
static const char* const TXT[TXT_COUNT][LANG_COUNT] = { LANG_DISPLAY(LANG_ROW) LANG_WEB(LANG_ROW) };
#undef LANG_ENUM
#undef LANG_ROW

static inline const char* TR(uint16_t id) { return TXT[id][gLang < LANG_COUNT ? gLang : 0]; }

// Wochentage: Pixelschrift (gross, 2 Buchstaben bzw. voll) und klassische Designs (normal)
static const char* const WDS_L[LANG_COUNT][7] = {
  {"SO", "MO", "DI", "MI", "DO", "FR", "SA"},
  {"SU", "MO", "TU", "WE", "TH", "FR", "SA"},
  {"DI", "LU", "MA", "ME", "JE", "VE", "SA"},
  {"DO", "LU", "MA", "MI", "JU", "VI", "SA"},
  {"DO", "LU", "MA", "ME", "GI", "VE", "SA"},
};
static const char* const WDF_L[LANG_COUNT][7] = {
  {"SONNTAG", "MONTAG", "DIENSTAG", "MITTWOCH", "DONNERSTAG", "FREITAG", "SAMSTAG"},
  {"SUNDAY", "MONDAY", "TUESDAY", "WEDNESDAY", "THURSDAY", "FRIDAY", "SATURDAY"},
  {"DIMANCHE", "LUNDI", "MARDI", "MERCREDI", "JEUDI", "VENDREDI", "SAMEDI"},
  {"DOMINGO", "LUNES", "MARTES", "MIERCOLES", "JUEVES", "VIERNES", "SABADO"},
  {"DOMENICA", "LUNEDI", "MARTEDI", "MERCOLEDI", "GIOVEDI", "VENERDI", "SABATO"},
};
static const char* const WDK_L[LANG_COUNT][7] = {
  {"Sonntag", "Montag", "Dienstag", "Mittwoch", "Donnerstag", "Freitag", "Samstag"},
  {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"},
  {"Dimanche", "Lundi", "Mardi", "Mercredi", "Jeudi", "Vendredi", "Samedi"},
  {"Domingo", "Lunes", "Martes", "Miercoles", "Jueves", "Viernes", "Sabado"},
  {"Domenica", "Lunedi", "Martedi", "Mercoledi", "Giovedi", "Venerdi", "Sabato"},
};
static const char* const WDK2_L[LANG_COUNT][7] = {
  {"So", "Mo", "Di", "Mi", "Do", "Fr", "Sa"},
  {"Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"},
  {"Di", "Lu", "Ma", "Me", "Je", "Ve", "Sa"},
  {"Do", "Lu", "Ma", "Mi", "Ju", "Vi", "Sa"},
  {"Do", "Lu", "Ma", "Me", "Gi", "Ve", "Sa"},
};
#define LANG_IDX (gLang < LANG_COUNT ? gLang : 0)

// Datum: Deutsch "28.10.", sonst "28/10"; mit Jahr "28.10.2026" bzw. "28/10/2026"
static void langDate(char* b, size_t n, int d, int m) {
  if (gLang == L_DE) snprintf(b, n, "%02d.%02d.", d, m); else snprintf(b, n, "%02d/%02d", d, m);
}
static void langDateY(char* b, size_t n, int d, int m, int y) {
  if (gLang == L_DE) snprintf(b, n, "%02d.%02d.%04d", d, m, y); else snprintf(b, n, "%02d/%02d/%04d", d, m, y);
}
// Kurzer Wochentag (Pixelschrift) + Datum, z. B. "MI 28.10." / "WE 28/10"
static void langWdDate(char* b, size_t n, int wd, int d, int m) {
  char x[12];
  langDate(x, sizeof(x), d, m);
  snprintf(b, n, "%s %s", WDS_L[LANG_IDX][((wd % 7) + 7) % 7], x);
}
