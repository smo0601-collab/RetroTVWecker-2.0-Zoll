# Aenderungen

Jede Version nennt die geaenderten Dateien. Fuer ein Update genuegt es, genau diese Dateien im Sketch-Ordner
zu ersetzen (neue hinzufuegen, entfernte loeschen), die Binaerdatei zu exportieren und per WLAN einzuspielen
(README, "Update ueber WLAN"). Die installierte Version steht im Browser unter Status > Firmware.

## 2026.10.31-3 - Sprachen

- Sprache waehlbar unter Anzeige > Darstellung: Deutsch, English, Francais, Espanol, Italiano.
- Uebersetzt sind das Display (alle Designs, Wochentage, Datum, Wettertexte, Feiertags-Gruesse, Badminton-Ansagen,
  Flugradar, ISS, Sonne und Mond, Update-Anzeige) und die komplette Browser-Seite inkl. Design-Namen.
- Datum ausserhalb von Deutsch als `31/10`; lange Ueberschriften werden automatisch kleiner, damit sie passen.
- ISS-Tracker: "Lade ISS-Daten" ueberdeckt nicht mehr das Datum.

Geaenderte Dateien: `lang.h` (neu), `lang_web.h` (neu), `WetterUhr.ino`, `webui.h`, `settings.h`, `config.h`, `icons.h`,
`faces_core.h`, `faces_fx.h`, `pet.h`, alle `face_*.h` mit Text oder Datum, `README.md`, `CHANGELOG.md`

## 2026.10.31-2 - Weihnachtself im Feiertags-Wechsel

- Der Weihnachtself kommt zusammen mit Weihnachten vom 14. bis 26.12. automatisch in den Design-Wechsel.

Geaenderte Dateien: `WetterUhr.ino`, `faces.h`, `webui.h`, `config.h`, `README.md`, `CHANGELOG.md`

## 2026.10.31 - Feiertage automatisch im Design-Wechsel

- Weihnachten, Silvester, Valentinstag, Halloween und Ostern kommen jeweils 10 Tage vor dem Termin automatisch in den
  Design-Wechsel und fallen nach dem Fest wieder heraus (Weihnachten 14.-26.12., Silvester 21.12.-1.1.,
  Valentinstag 4.-14.2., Halloween 21.-31.10., Ostern 10 Tage vor Ostersonntag bis Ostermontag).
- Die eigene Liste bleibt unveraendert; abschaltbar unter Anzeige > Extras. Die Status-Seite zeigt, welche
  Feiertags-Designs gerade dazugekommen sind.

Geaenderte Dateien: `WetterUhr.ino`, `webui.h`, `settings.h`, `config.h`, `faces.h`, `README.md`, `CHANGELOG.md`

## 2026.10.30 - Update ueber WLAN

- Firmware-Update im Browser (System > Firmware-Update ueber WLAN): Fortschritt im Browser und auf dem Display,
  die Datei wird geprueft, Neustart nur bei Erfolg, sonst laeuft die alte Firmware weiter.
- Update direkt aus der Arduino IDE ueber den Netzwerk-Port (Passwort = Web-Passwort), abschaltbar mit `ARDUINO_OTA 0`.
- Firmware-Version und Bauzeit unter Status und System (`FW_VERSION` in `config.h`).
- Hinweis auf der Web-Seite, wenn das Partitionsschema kein Update erlaubt.
- Empfohlenes Partitionsschema jetzt *Minimal SPIFFS (1.9MB APP with OTA)*.

Geaenderte Dateien: `WetterUhr.ino`, `webui.h`, `config.h`, `README.md`, `CHANGELOG.md` (neu)

**Einmalig:** Wer bisher *Huge APP (No OTA)* verwendet, laedt diese Version einmal per USB mit dem neuen
Partitionsschema hoch. Danach gehen alle weiteren Updates per WLAN.

## 2026.10.29 - Aquarium, Badminton-Spielstand

- Aquarium neu gestaltet (Korallenriff, neue Fischarten, Schwarm, Qualle), Besuch von Seeschlange oder U-Boot,
  Knoepfe "Fische fuettern" und "Besuch" unter Anzeige > Extras.
- Badminton: spannenderer Spielverlauf ueber drei Saetze; Spielstand bleibt bei Designwechsel und Neustart erhalten.

Dateien: `face_aqua.h`, `face_badminton.h`, `faces.h`, `webui.h`, `README.md`

## 2026.10.28

- Squishy Dumpling, Feiertags-Designs (Weihnachten, Elf, Halloween, Ostern, Valentinstag, Silvester), UFO im
  Flugradar, Weihnachtsschlitten im ISS-Tracker, Silvester-Automatik; Designs entfernt und Nummern umgerechnet;
  Nachtmodus mit Testbild, manuelle Uhrzeit, Summer und Wecker, Touch-Taster.
