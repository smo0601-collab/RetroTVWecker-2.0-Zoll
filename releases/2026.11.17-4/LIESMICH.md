**Firmware 2026.11.17-4** (RetroCYD unverändert 4.0.3) – dauerhaftes Protokoll im Flash: mehrere Tage, übersteht Neustart, Absturz, Stromausfall und Updates.

### Neu
- Das Protokoll liegt jetzt zusätzlich im Flash (124 KB im sonst ungenutzten Bereich „spiffs“; der Sektor mit der
  Einstellungs-Sicherung bleibt unberührt). Bei rund 20 KB pro Tag reicht das für **etwa 5 Tage** – erst danach
  werden die ältesten Zeilen überschrieben. Neustart, Absturz, Stromausfall und Firmware-Updates löschen nichts.
- Jede Zeile mit Datum und Uhrzeit. Alle 10 Minuten eine Zustandszeile: Speicher, WLAN, Wetter, letzter
  erfolgreicher Abruf, Browser-Anfragen, Bilder/s, Bildzeit, Kacheln, Schleifen/s, Design, ISS-Aussetzer.
- Wiederholte Meldungen werden gezählt statt jedes Mal geschrieben. Kurze ISS-Aussetzer werden nur gezählt,
  Ausfälle ab 60 s mit Dauer protokolliert.
- `http://<uhr>/log` zeigt das ganze Protokoll (älteste Zeilen zuerst). Die bisherige ausführliche Ansicht der
  letzten zwei Sitzungen gibt es unter `http://<uhr>/log?rtc=1`.
- Gelöscht wird das Protokoll nur durch eine USB-Erstinstallation (die überschreibt den ganzen Flash).

### Einspielen
- CYD: `WetterUhr_2026.11.17-4_CYD_update.bin`
- ESP32-C3: `WetterUhr_2026.11.17-4_update.bin`

### Dateien
| Datei | Wofür |
|---|---|
| `WetterUhr_2026.11.17-4_CYD_update.bin` | CYD: Uhr-Update |
| `RetroCYD_4.0.3_update.bin` | CYD: Spiele (unverändert) |
| `WetterUhr_2026.11.17-4_CYD_mitSpielen_erstinstallation.bin` | CYD: Uhr + RetroCYD 4.0.3 per USB an `0x0` |
| `WetterUhr_2026.11.17-4_CYD_erstinstallation.bin` | CYD: nur Uhr per USB an `0x0` |
| `WetterUhr_2026.11.17-4_update.bin` / `_erstinstallation.bin` | ESP32-C3 |
| `WetterUhr_2026.11.17-4_komplett.zip`, `RetroCYD_4.0.3_komplett.zip` | Quellcode |

Am PC geprüft (Ringpuffer mit Neustart mittendrin), nicht auf echter Hardware getestet.
