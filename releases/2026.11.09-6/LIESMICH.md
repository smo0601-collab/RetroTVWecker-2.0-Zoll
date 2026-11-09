**Firmware 2026.11.09-6** und **RetroCYD 3.6** – optimiert für das CYD 2,8 Zoll: schärfere Anzeige, Großanzeige für die Ferne, Spiele per Touch.

### Neu in der Uhr
- **Schärfer statt Doppelpixel:** Themen-Designs werden beim Vergrößern geglättet (keine Treppen an schrägen Kanten),
  Texte, Datum und Zahlen werden in voller Auflösung gezeichnet. Einstellbar unter *Anzeige → Darstellung*
  („Kanten glätten“, „Schrift in voller Auflösung“). CYD: an, ESP32-C3: aus.
- **Design 27 „Großanzeige“:** Uhrzeit als 7-Segment über die ganze Breite, Sekundenbalken, große Temperatur mit
  Wettersymbol, Datum und Wochentag – aus der Ferne lesbar.

### Neu in RetroCYD 3.6
- **Spiele ohne Controller per Touch spielbar.** Beim ersten Spiel 3 Kreuze antippen (Touch einrichten).
  Menü: Spiel antippen = auswählen, nochmal = starten. Im Spiel: Mitte = Aktion, außen = Richtung,
  Ecke oben rechts = Pause-Menü. Shooter feuern automatisch, Sunset Race gibt automatisch Gas.
  Spielende: antippen = nochmal, lange halten = Menü. Controller funktionieren weiter wie bisher.

### Einspielen (beides direkt im Browser der Uhr)
1. *System → Firmware-Update* → `RetroCYD_3.6_update.bin`
2. *System → Firmware-Update* → `WetterUhr_2026.11.09-6_CYD_update.bin` (läuft automatisch über RetroCYD, ca. ½ Minute)

Alternativ: `WetterUhr_2026.11.09-6_CYD_mitSpielen_erstinstallation.bin` per USB an `0x0` (löscht Einstellungen).

### Dateien
| Datei | Wofür |
|---|---|
| `WetterUhr_2026.11.09-6_CYD_update.bin` | CYD: Uhr-Update |
| `RetroCYD_3.6_update.bin` | CYD: Spiele-Update |
| `WetterUhr_2026.11.09-6_CYD_mitSpielen_erstinstallation.bin` | CYD: Uhr + RetroCYD 3.6 per USB an `0x0` |
| `WetterUhr_2026.11.09-6_CYD_erstinstallation.bin` | CYD: nur Uhr per USB an `0x0` |
| `WetterUhr_2026.11.09-6_update.bin` / `_erstinstallation.bin` | ESP32-C3 (Großanzeige; Glätten einschaltbar) |
| `WetterUhr_2026.11.09-6_komplett.zip`, `RetroCYD_3.6_komplett.zip` | Quellcode |

Alle Änderungen: [CHANGELOG](https://github.com/smo0601-collab/RetroTVWecker-2.0-Zoll/blob/main/firmware/WetterUhr/CHANGELOG.md)
