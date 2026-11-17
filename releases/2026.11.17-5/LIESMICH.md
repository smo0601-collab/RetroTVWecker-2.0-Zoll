**Firmware 2026.11.17-5** (RetroCYD unverändert 4.0.3) – SD-Karte am CYD: komplettes Protokoll, Wetterverlauf, Messwerte, Einstellungs-Sicherung.

### Neu (nur CYD)
Micro-SD-Karte (FAT32, bis 32 GB) in den Steckplatz auf der Rückseite. Die Uhr legt den Ordner **/wetteruhr** an:

| Datei | Inhalt |
|---|---|
| `log/2026-11-17.txt` | komplettes Protokoll je Tag, auch Routine-Meldungen – so lange, bis die Karte voll ist |
| `wetter_2026-11.csv` | jede Wetter-Aktualisierung: Temperatur, gefühlt, Feuchte, Wind, Wettercode, Tag/Nacht, Min/Max heute |
| `zustand_2026-11.csv` | alle 10 min: Speicher, WLAN, Netz, Browser-Anfragen, Bilder/s, Bildzeit, Kacheln, Schleifen/s, Design |
| `einstellungen.bin` | Sicherung bei jedem Speichern (wird genutzt, wenn NVS und Flash-Sicherung leer sind, z. B. nach einer USB-Erstinstallation) |

- CSV mit „;“ und Dezimalkomma – direkt in Excel oder LibreOffice öffnen.
- *Status → SD-Karte* zeigt Größe, Belegung und einen Link **Dateien** (`/sd`) zum Herunterladen.
- Ohne Karte oder wenn sie gezogen wird, läuft alles weiter. Die Uhr sucht die Karte nach 1, 2 und 5 Minuten und danach
  alle 10 Minuten erneut.
- Das Flash-Protokoll (rund 5 Tage) bleibt unabhängig davon bestehen.
- Hinweis: `einstellungen.bin` enthält auch das WLAN-Passwort – die Karte entsprechend aufbewahren.
- ESP32-C3: unverändert (kein Kartenleser).

### Einspielen
- CYD: `WetterUhr_2026.11.17-5_CYD_update.bin`
- ESP32-C3: `WetterUhr_2026.11.17-5_update.bin`

### Dateien
| Datei | Wofür |
|---|---|
| `WetterUhr_2026.11.17-5_CYD_update.bin` | CYD: Uhr-Update |
| `RetroCYD_4.0.3_update.bin` | CYD: Spiele (unverändert) |
| `WetterUhr_2026.11.17-5_CYD_mitSpielen_erstinstallation.bin` | CYD: Uhr + RetroCYD 4.0.3 per USB an `0x0` |
| `WetterUhr_2026.11.17-5_CYD_erstinstallation.bin` | CYD: nur Uhr per USB an `0x0` |
| `WetterUhr_2026.11.17-5_update.bin` / `_erstinstallation.bin` | ESP32-C3 |
| `WetterUhr_2026.11.17-5_komplett.zip`, `RetroCYD_4.0.3_komplett.zip` | Quellcode |

Nicht auf echter Hardware getestet.
