**Firmware 2026.11.13** und **RetroCYD 4.0** – Klänge für den Lautsprecher am CYD, alles im Browser zum Anhören.

Dafür einen kleinen Lautsprecher (8 Ω, 0,5–2 W, Stecker JST 1,25 mm 2-polig) an den Anschluss **„SPEAK“** stecken.

### Wetteruhr (CYD) – Browser → *Summer & Wecker*
- **Lautstärke** (Regler). Neben jedem Klang ein **▶** zum Anhören – mit der Reglerstellung, ohne Speichern; „Klang stoppen“.
- **Weckton:** Piepser, Klingel, Fanfare, Kuckuck, Gong, Morgenstimmung (Grieg), Arcade, Sirene, Vogelgezwitscher.
- **Stundengong:** Piep-Piep, Westminster, Kuckuck und Glockenschlag (zählen die Stunde), Arcade-Münze, Glockenspiel.
- **Klang-Effekte** (einzeln abschaltbar):
  - Feiertags-Einblendung mit passender Melodie (Weihnachten, Neujahr, Karneval-Tusch, Halloween, Valentinstag,
    Ostern, Ferien, sonst Tröte; an stillen Feiertagen nichts)
  - Silvester: die letzten 10 Sekunden ticken, um 0 Uhr Feuerwerk
  - UFO im Flugradar
  - automatischer Designwechsel (Standard aus)
  - Wetterwechsel: Regen, Schnee oder Gewitter beginnt
- Effekte schweigen im Standby, während der Wecker klingelt und (wenn gewählt) während der Nachtruhe-Dimmung.
- ESP32-C3 mit aktivem Summer: unverändert.

### RetroCYD 4.0 – Spiel-Sounds
- Alle 20 Spiele klingen: Schüsse, Laser, Sprünge, Münzen, Power-ups, Reihe weg, Ball an Schläger/Wand/Stein,
  Karten, Paar gefunden, Startampel, Motor beim Rennen, Hupe, Platsch, Explosionen …
- Jede Vibration klingt automatisch mit (je nach Stärke), eigene Klänge haben Vorrang.
- Menü: Klick beim Blättern, Auswählen, Zurück, Spielstart, Pause, Game Over, Rekord-Fanfare.
- Web-Oberfläche → *Spiele & Highscores*: Sounds an/aus, Lautstärke, alle 42 Klänge zum Anhören.
- Nebenbei behoben: der Ton teilte sich bisher einen Zeitgeber mit der blauen LED.

### Einspielen
1. Browser der Uhr → *System → Firmware-Update* → `RetroCYD_4.0_update.bin`
2. Danach → `WetterUhr_2026.11.13_CYD_update.bin` (ESP32-C3: `WetterUhr_2026.11.13_update.bin`)

### Dateien
| Datei | Wofür |
|---|---|
| `WetterUhr_2026.11.13_CYD_update.bin` | CYD: Uhr-Update |
| `RetroCYD_4.0_update.bin` | CYD: Spiele-Update |
| `WetterUhr_2026.11.13_CYD_mitSpielen_erstinstallation.bin` | CYD: Uhr + RetroCYD 4.0 per USB an `0x0` |
| `WetterUhr_2026.11.13_CYD_erstinstallation.bin` | CYD: nur Uhr per USB an `0x0` |
| `WetterUhr_2026.11.13_update.bin` / `_erstinstallation.bin` | ESP32-C3 |
| `WetterUhr_2026.11.13_komplett.zip`, `RetroCYD_4.0_komplett.zip` | Quellcode |

Nicht auf echter Hardware getestet.
