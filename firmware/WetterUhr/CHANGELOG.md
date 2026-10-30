# Aenderungen

Jede Version nennt die geaenderten Dateien. Fuer ein Update genuegt es, genau diese Dateien im Sketch-Ordner
zu ersetzen (neue hinzufuegen, entfernte loeschen), die Binaerdatei zu exportieren und per WLAN einzuspielen
(README, "Update ueber WLAN"). Die installierte Version steht im Browser unter Status > Firmware.

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
