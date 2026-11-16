**Firmware 2026.11.16-2** (RetroCYD unverändert 4.0.3) – Browser-Seite auch bei Flugradar und ISS-Tracker erreichbar.

### Was war
Flugradar und ISS-Tracker holen ständig Daten aus dem Internet; das Radar hält dazu eine verschlüsselte Verbindung
offen (rund 40 KB) und braucht beim Auswerten weiteren Speicher. Kam eine Browser-Anfrage, fehlte dem Netz der
Speicher: die Seite kam nur tropfenweise an, die Uhr stand bis zu einer Minute still.

### Jetzt
Solange der Browser die Uhr benutzt (bis 6 s nach jeder Anfrage), pausieren Radar-, ISS- und Wetterabruf, und die
offene Radar-Verbindung wird geschlossen. Danach läuft alles normal weiter.

### Einspielen
- CYD: `WetterUhr_2026.11.16-2_CYD_update.bin` (falls die Seite nicht lädt: vorher auf ein anderes Design tippen)
- ESP32-C3: `WetterUhr_2026.11.16-2_update.bin`

### Dateien
| Datei | Wofür |
|---|---|
| `WetterUhr_2026.11.16-2_CYD_update.bin` | CYD: Uhr-Update |
| `RetroCYD_4.0.3_update.bin` | CYD: Spiele (unverändert) |
| `WetterUhr_2026.11.16-2_CYD_mitSpielen_erstinstallation.bin` | CYD: Uhr + RetroCYD 4.0.3 per USB an `0x0` |
| `WetterUhr_2026.11.16-2_CYD_erstinstallation.bin` | CYD: nur Uhr per USB an `0x0` |
| `WetterUhr_2026.11.16-2_update.bin` / `_erstinstallation.bin` | ESP32-C3 |
| `WetterUhr_2026.11.16-2_komplett.zip`, `RetroCYD_4.0.3_komplett.zip` | Quellcode |

Nicht auf echter Hardware getestet.
