**Firmware 2026.11.17-2** (RetroCYD unverändert 4.0.3) – Browser-Seite lädt schnell, das Bild stockt nicht mehr.

### Gemeldet
Beim Aufruf der Browser-Seite stockt das Bild, die Seite braucht etwa 10 Sekunden – bei jedem Design.

### Ursache
Die Seite ging in rund 40 kleinen Stücken heraus. Durch das Zusammenspiel von Nagle-Algorithmus und verzögerter
Bestätigung des Browsers wartete jedes Stück rund 200 ms. In dieser Zeit stand die Hauptschleife und damit das Bild.

### Jetzt
- Verzögerung aus, Stücke bis 5 KB.
- Formatvorlage und Programm der Seite (zusammen rund 16 KB) kommen als eigene Dateien und bleiben im
  Browser-Speicher. Nach einem Update lädt der Browser sie automatisch neu.

### Einspielen
- CYD: `WetterUhr_2026.11.17-2_CYD_update.bin`
- ESP32-C3: `WetterUhr_2026.11.17-2_update.bin`

### Dateien
| Datei | Wofür |
|---|---|
| `WetterUhr_2026.11.17-2_CYD_update.bin` | CYD: Uhr-Update |
| `RetroCYD_4.0.3_update.bin` | CYD: Spiele (unverändert) |
| `WetterUhr_2026.11.17-2_CYD_mitSpielen_erstinstallation.bin` | CYD: Uhr + RetroCYD 4.0.3 per USB an `0x0` |
| `WetterUhr_2026.11.17-2_CYD_erstinstallation.bin` | CYD: nur Uhr per USB an `0x0` |
| `WetterUhr_2026.11.17-2_update.bin` / `_erstinstallation.bin` | ESP32-C3 |
| `WetterUhr_2026.11.17-2_komplett.zip`, `RetroCYD_4.0.3_komplett.zip` | Quellcode |

Nicht auf echter Hardware getestet.
