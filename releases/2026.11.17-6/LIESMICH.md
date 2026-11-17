**Firmware 2026.11.17-6** (RetroCYD unverändert 4.0.3) – SD-Karte wieder entfernt, Stand wie 2026.11.17-4.

### Änderung
- Auf Wunsch zurückgenommen: die Auslagerung von Protokoll, Wetterverlauf, Messwerten und Einstellungs-Sicherung
  auf die SD-Karte aus 2026.11.17-5. Der Quellcode entspricht wieder 2026.11.17-4, nur die Versionsnummer ist neu.
- Das dauerhafte Protokoll im Flash (rund 5 Tage, `/log`) bleibt.
- Falls 2026.11.17-5 schon einen Ordner `/wetteruhr` auf einer Karte angelegt hat: wird nicht mehr beschrieben und kann gelöscht werden.

### Einspielen
- CYD: `WetterUhr_2026.11.17-6_CYD_update.bin`
- ESP32-C3: `WetterUhr_2026.11.17-6_update.bin`

### Dateien
| Datei | Wofür |
|---|---|
| `WetterUhr_2026.11.17-6_CYD_update.bin` | CYD: Uhr-Update |
| `RetroCYD_4.0.3_update.bin` | CYD: Spiele (unverändert) |
| `WetterUhr_2026.11.17-6_CYD_mitSpielen_erstinstallation.bin` | CYD: Uhr + RetroCYD 4.0.3 per USB an `0x0` |
| `WetterUhr_2026.11.17-6_CYD_erstinstallation.bin` | CYD: nur Uhr per USB an `0x0` |
| `WetterUhr_2026.11.17-6_update.bin` / `_erstinstallation.bin` | ESP32-C3 |
| `WetterUhr_2026.11.17-6_komplett.zip`, `RetroCYD_4.0.3_komplett.zip` | Quellcode |

Nicht auf echter Hardware getestet.
