**Firmware 2026.11.10-3** und **RetroCYD 3.8** – die RGB-LED auf der Rückseite des CYD lässt sich steuern.

### Uhr: Browser → Helligkeit → RGB-LED
- **Modus:** aus, feste Farbe, Wetter-Farbe, Temperatur-Farbe, Farbwechsel, Atmen, Sekundenpuls.
- **Farbe** (Farbwähler) und **Helligkeit**.
- **Wann:** immer, nur nachts (Nachtlicht), nachts aus. „Nachts“ = Dimm-Zeitfenster, ohne Zeitplan nach Sonnenstand.
- **Hinweise** (blinken zusätzlich, einzeln abschaltbar): Wecker rot, kein WLAN orange, Update blau,
  Gewitter violett, Frost eisblau.
- Änderungen sind sofort 8 Sekunden lang zu sehen; gespeichert wird mit *Speichern*. Im Standby bleibt die LED aus
  (außer Wecker und Update). Ab Werk ist die LED wie bisher aus und blinkt nur beim Wecker.

### Spiele (RetroCYD 3.8)
- Im Menü leuchtet die Farbe der Kategorie.
- Jede Vibration (Treffer, Apfel, Absturz …) blitzt mit – auch ohne Controller.
- Regenbogen bei neuem Rekord, gelöstem Rätsel, 2048 und Black Jack.
- Startampel bei Sunset Race, Spielerfarbe bei Vier gewinnt und Paare finden (2 Spieler),
  orange Tankwarnung bei Spurwechsel, grün/rot für Gewinn/Verlust bei Siebzehn und vier.
- Ein/aus und Helligkeit in der RetroCYD-Weboberfläche (Setup → Einstellungen).

### Einspielen (beides im Browser der Uhr)
1. *System → Firmware-Update* → `RetroCYD_3.8_update.bin`
2. *System → Firmware-Update* → `WetterUhr_2026.11.10-3_CYD_update.bin`

Alternativ: `WetterUhr_2026.11.10-3_CYD_mitSpielen_erstinstallation.bin` per USB an `0x0` (löscht Einstellungen).
Für den ESP32-C3 (ohne LED) ändert sich nur die Versionsnummer.

### Dateien
| Datei | Wofür |
|---|---|
| `WetterUhr_2026.11.10-3_CYD_update.bin` | CYD: Uhr-Update |
| `RetroCYD_3.8_update.bin` | CYD: Spiele-Update |
| `WetterUhr_2026.11.10-3_CYD_mitSpielen_erstinstallation.bin` | CYD: Uhr + RetroCYD 3.8 per USB an `0x0` |
| `WetterUhr_2026.11.10-3_CYD_erstinstallation.bin` | CYD: nur Uhr per USB an `0x0` |
| `WetterUhr_2026.11.10-3_update.bin` / `_erstinstallation.bin` | ESP32-C3 |
| `WetterUhr_2026.11.10-3_komplett.zip`, `RetroCYD_3.8_komplett.zip` | Quellcode |

Alle Änderungen: [CHANGELOG](https://github.com/smo0601-collab/RetroTVWecker-2.0-Zoll/blob/main/firmware/WetterUhr/CHANGELOG.md)
