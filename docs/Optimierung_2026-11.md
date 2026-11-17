# Analyse und Optimierung der WetterUhr-Firmware (November 2026)

Stand: Firmware **2026.11.17-3**. Der Quellcode gilt für **ESP32-C3** und **CYD** gemeinsam, weil beide aus denselben
Dateien gebaut werden. RetroCYD ist nicht Teil dieser Analyse.

**Messgrundlage.** Alle Zahlen stammen aus einer von drei Quellen; geschätzte Benchmarks gibt es keine:

- **Compiler:** arduino-cli, ESP32-Core 3.3.12. Gemessen sind statischer RAM und Flash.
- **Protokoll der Uhr:** das `/log` von echter Hardware (CYD).
- **Messungen am PC:** Host-Tests mit den Original-Quelltexten.

Geschwindigkeit auf dem Gerät lässt sich nur dort messen. Ab dieser Version schreibt die Uhr dazu alle 5 Minuten eine
Zeile „Messung“ ins Protokoll.

## 1. Ausgangslage: gefundene Probleme

Die Bestandsaufnahme zeigt einen grundsätzlich sauberen, nicht blockierenden Aufbau:

- Die Hauptschleife arbeitet mit `millis()`.
- Die Designs zeichnen in einen 160×120-Puffer und senden nur geänderte Kacheln (32×16) zum Display.
- Große Konstanten liegen im Flash (PROGMEM).

Die eigentlichen Schwachstellen lagen bei **Speicher, Netz und Zeitlimits**. Die meisten davon haben die Protokolle der
Uhr sichtbar gemacht.

| # | Problem | Priorität | Nachweis | Behoben in |
|---|---|---|---|---|
| 1 | Bibliotheksfunktionen (`Stream::readStringUntil/readBytes`, Chunk-Köpfe im HTTPClient) warten **ohne Pause** bis zu 8 s auf Daten. Stockt die Verbindung, hungert der Netz-Task den Leerlauf aus, und der **Task-Watchdog** startet neu. | hoch | Absturz-Spur „Flugradar (Daten lesen), Watchdog: Kern 0 'net'“ | 2026.11.17 |
| 2 | Flugradar liest die **ganze Flugzeugliste** in ein JSON-Dokument. | hoch | Geräteprotokoll: min. freier Speicher 2,1–2,6 KB während des Radars; PC-Messung unten | 2026.11.17-3 |
| 3 | TLS-Abrufe scheitern an Speichermangel (Puffer der feinen Schrift 38 KB dauerhaft belegt, Radar-Verbindung 40 KB offen). | hoch | `TLS -32512 SSL - Memory allocation failed` | 2026.11.16-6 / -7 |
| 4 | Drei Netz-Tasks mit zusammen 30 KB Stack, die ohnehin nacheinander arbeiten. | mittel | freier Speicher 53,5 KB → 69,6 KB nach der Zusammenlegung (Status-Seite) | 2026.11.16-3 |
| 5 | Bildpuffer werden bei jedem Designwechsel freigegeben und neu geholt (19 KB / 38 KB) → Heap-Fragmentierung über Stunden. | mittel | Uhr nach langer Laufzeit nicht mehr erreichbar | 2026.11.16 |
| 6 | Browser-Seite in ~40 kleinen Stücken mit Nagle + verzögertem ACK → ~10 s Ladezeit, Bild steht so lange. | mittel | Meldung „ca. 10 s“ | 2026.11.17-2 |
| 7 | 11 Designs belegen eigene Partikel- und Sternfelder **dauerhaft**, obwohl nur eines läuft (zusammen ~24 KB statischer RAM für Design-Zustände). | mittel | `nm` auf dem ELF: `f_nye::ps` allein 7 840 B | 2026.11.17-3 |
| 8 | Zeitvergleich mit Überlauf in der Selbstheilung (`now - gNetOkAt` mit jüngerem Zeitstempel aus dem Netz-Task). | mittel | grundloses Neuverbinden im Protokoll | 2026.11.16-7 |
| 9 | Ton-Engine konfigurierte den LEDC-Zeitgeber bis zu 1000×/s neu (`ledcChangeFrequency`). | mittel | Knacken, Abstürze beim Abspielen | 2026.11.13-2 |
| 10 | In 9 Designs Zeitvergleiche der Form `now >= nextX` (nicht überlaufsicher, ~49,7 Tage). | niedrig | Code-Analyse; jeder Designwechsel setzt die Zeiten neu | offen (siehe 7.) |
| 11 | Diagnose kannte nur Maximalwerte (max. Bildzeit), keine Mittelwerte. Vorher/Nachher war auf dem Gerät nicht messbar. | niedrig | – | 2026.11.17-3 |

## 2. Durchgeführte Optimierungen (Überblick)

| Bereich | Änderung |
|---|---|
| Stabilität | Eigener Leser `NetStream`: wartet mit Pause, hat eine Gesamtfrist und entpackt „chunked“ selbst. Wetter, Ferien und Ortssuche ohne „chunked“ (HTTP/1.0). |
| Stabilität | Netz-Task mit Priorität 0, ohne Kernbindung; Watchdog-Spur (welcher Task lief auf Kern 0/1). |
| RAM (Laufzeit) | Flugradar Flugzeug für Flugzeug: konstanter, kleiner JSON-Speicher. |
| RAM (statisch) | Gemeinsamer Design-Arbeitsspeicher `fc::pool` (8 KB) für 11 Designs. |
| RAM (Laufzeit) | Ein Netz-Task statt drei (−18 KB Stack); Bildpuffer einmal beim Start; Laufschrift-Puffer fest. |
| RAM (Laufzeit) | Feine Schrift gibt ihren Puffer während TLS-Abrufen frei; Radar-Verbindung wird vor Wetter/Ferien geschlossen. |
| Reaktionszeit | Browser-Seite: `setNoDelay`, Stücke bis 5 KB, CSS/JS (16 KB) als eigene Dateien im Browser-Speicher. |
| Ton | Tonhöhe direkt im LEDC-Teiler-Register statt Neukonfiguration; Lautsprecher-Ausgang in Ruhe hochohmig. |
| Diagnose | Protokoll im RTC-Speicher (übersteht Neustarts), Absturz-Spur, „Messung“ alle 5 min. |

## 3. Die wichtigsten Verbesserungen

1. **Kein Watchdog-Neustart mehr durch hängende Verbindungen.** Das war die häufigste Absturzursache: Warteschleifen
   ohne Pause in der Bibliothek. Alle Lesestellen der Uhr warten jetzt mit Pause und haben eine Frist.
2. **Flugradar mit konstantem Speicherbedarf.** Vorher wuchs der Speicher mit der Zahl der Flugzeuge. Zusammen mit der
   offenen TLS-Verbindung blieb dann fast nichts übrig, und alles andere (Browser, Wetter) scheiterte.
3. **Rund 9,4 KB mehr dauerhaft freier Heap** durch den gemeinsamen Design-Speicher, dazu 18 KB durch den einen
   Netz-Task. Für die TLS-Abrufe (kurzzeitig 60–70 KB) ist das der entscheidende Spielraum.
4. **Browser-Seite schnell, das Bild steht nicht mehr.** Der Engpass war die TCP-Wartezeit je Stück, nicht die
   Rechenzeit.

## 4. Messwerte

### Compiler (statischer RAM und Flash)

| | 2026.11.13-4 (Ausgang) | 2026.11.17-3 | Änderung |
|---|---|---|---|
| ESP32-C3 statischer RAM | 80 848 B | 71 560 B | **−9 288 B** |
| ESP32-C3 Flash (Programm) | 1 772 023 B (90 %) | 1 782 415 B (90 %) | +10 392 B |
| CYD statischer RAM | 93 264 B | 84 008 B | **−9 256 B** |
| CYD Flash (Programm) | 1 661 890 B (84 %) | 1 671 726 B (85 %) | +9 836 B |

Der Flash ist gewachsen, weil Funktionen hinzugekommen sind: Protokoll im RTC-Speicher, Absturz-Spur,
Selbstheilung, `NetStream`, DNS-Absicherung, Messzeilen. Bezogen nur auf diesen letzten Schritt (2026.11.17-2 → -3):
RAM −9 432 B (C3) bzw. −9 408 B (CYD), Flash +4,3 KB bzw. +3,8 KB.

### Laufzeit-Speicher

| Messung | Wert | Quelle |
|---|---|---|
| Netz-Tasks: Stack | 30 720 B (3 Tasks) → 12 288 B (1 Task) | Quelltext |
| Freier Heap nach Zusammenlegung der Tasks | 53 548 B → 69 620 B | Status-Seite des Geräts |
| Freier Heap im Normalbetrieb (CYD, ohne Radar) | ~118 000 B, größter Block 36–41 KB | Protokoll des Geräts |
| Min. freier Heap während des Radars (vor 2026.11.17-3) | 2 144–2 568 B | Protokoll des Geräts |
| JSON-Speicherspitze Radar, 150 Flugzeuge | 65 263 B → 2 675 B | PC-Messung mit zählendem Allocator (ArduinoJson 7.4.2), synthetische Antwort im adsb.lol-Format (90 KB, 40 Felder je Flugzeug) |

Der neue Wert für den minimalen Heap während des Radars ist auf dem Gerät noch nicht gemessen. Er steht nach einer
Stunde Radar im Protokoll unter „Zustand … (min …)“.

### Reaktionszeit und Geschwindigkeit

| Messung | Wert | Quelle |
|---|---|---|
| Browser-Seite laden | ~10 s (gemeldet) → noch nicht nachgemessen | Rückmeldung |
| Max. Bildzeit (Themen-Designs, CYD) | 55–60 ms je 10-s-Fenster; Radar bis 140 ms | Protokoll des Geräts |
| Mittlere Bildzeit, Bilder/s, Kacheln/Bild, Schleifen/s | **neu**: Zeile „Messung“ alle 5 min | ab 2026.11.17-3 |

### Prüfung der Änderungen

- Beide Boards kompilieren ohne Fehler und ohne neue Warnungen.
- **Design-Speicher:** Jedes der 11 umgestellten Designs wurde am PC 400–600 Bilder lang gezeichnet. Der gemeinsame
  Speicher war dabei vorher mit 0x00, 0xA5 und 0xFF gefüllt. Alle Bilder waren bytegleich, ebenso alle 37
  Feiertags-Szenen. Damit liest kein Design etwas, das es nicht selbst eingestellt hat.
- **Flugradar-Leser** am PC: 150 Flugzeuge, leere Liste `[]`, Schlüssel `aircraft` mit Leerzeichen, fehlende Liste,
  abgeschnittene Antwort (wird sauber als Fehler erkannt).
- **`NetStream`** am PC: „chunked“ mit Erweiterung, feste Länge, stockende Verbindung (bricht nach der Frist ab).

## 5. Quellcode, Bibliotheken, Einstellungen

- Quellcode: `firmware/WetterUhr/` im Projekt, als Paket in `releases/2026.11.17-3/WetterUhr_2026.11.17-3_komplett.zip`.
- Boardpaket: **esp32 by Espressif Systems 3.3.12**.
- Bibliotheken:
  - **Adafruit GFX Library 1.13.0**
  - **Adafruit ST7735 and ST7789 Library 1.12.0**
  - **Adafruit ILI9341 1.7.0** (für das CYD)
  - **Adafruit BusIO 1.17.4**
  - **ArduinoJson 7.4.2**
- Arduino IDE:
  - **ESP32-C3:** *ESP32C3 Dev Module*, *USB CDC On Boot: Enabled*, **Partition Scheme: Minimal SPIFFS (1.9MB APP
    with OTA)**, bei Boot-Problemen *Flash Mode: DIO*.
  - **CYD:** *ESP32 Dev Module*, **Partition Scheme: Minimal SPIFFS (1.9MB APP with OTA)**.
  - Keine Änderung an GPIO-Belegung, Display-Ansteuerung oder Bedienung.

## 6. Bewusst nicht geändert

- **Display-Ausgabe:** Sie sendet bereits nur geänderte 32×16-Kacheln. Einen DMA-Transfer bieten die
  Adafruit-Bibliotheken nicht; ein Bibliothekswechsel (z. B. LovyanGFX) wäre ein Architekturwechsel.
- **Flash-Verbrauch:** Rund 1,2 MB entfallen auf WLAN, lwIP und mbedTLS aus dem Boardpaket. Der eigene Code ist
  verteilt; der größte Einzelposten ist `handleRoot()` mit 13,5 KB. Es gibt keinen großen Brocken, der ohne
  Funktionsverlust wegfallen könnte.
- **Priorität des Netz-Tasks auf dem C3:** Priorität 0 heißt, dass ein TLS-Handshake auf dem Einkern-C3 länger dauert,
  weil die Anzeige Vorrang hat. Das ist bewusst so gewählt: Der Watchdog ist wichtiger als ein paar Sekunden beim
  Wetterabruf.

## 7. Empfehlungen für weitere Optimierungen

1. **Zeitvergleiche in den Designs überlaufsicher machen** (Punkt 10). Einfaches Ersetzen durch
   `(int32_t)(now - t) >= 0` reicht nicht: Nie gesetzte Zeiten (0) würden nach 24,8 Tagen Dauerbetrieb falsch
   ausgewertet. Sauber wäre, dass jedes Design alle Zeitpunkte in `enter()` relativ zu `now` setzt. Vorher
   nachmessen, ob ein Design überhaupt so lange ohne Wechsel läuft.
2. **ISS:** Keep-Alive statt einer neuen TCP-Verbindung alle 5 s. Spart Rechenzeit; open-notify lehnt häufig ab, was
   ohnehin seltener abgefragt wird.
3. **Weitere Designs in den gemeinsamen Speicher** (z. B. Sonne/Mond mit 1,6 KB). Dort werden Werte aber schrittweise
   berechnet; das braucht vorher die gleiche Prüfung mit verschmutztem Speicher.
4. **Nach einem Tag Laufzeit die Zeilen „Messung“ und „Zustand“ aus `/log` auswerten.** Damit gibt es echte Zahlen
   für Bildzeit, Bilder/s und minimalen Heap im Radar-Betrieb.
