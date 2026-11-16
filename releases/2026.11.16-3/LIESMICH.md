**Firmware 2026.11.16-3** (RetroCYD unverändert 4.0.3) – mehr freier Speicher, Absturz-Spur im Status.

### Änderungen
- Wetter, Kalender, Flugradar und ISS holen ihre Daten jetzt in **einem** Netz-Task statt in drei (vorher zusammen
  30 KB Stack; die Abrufe liefen ohnehin nacheinander). Das gibt rund **18 KB** Arbeitsspeicher frei – gemeldet waren
  nur noch 53 KB frei (32 KB am Stück) und ein Absturz beim ISS-Tracker.
- **Absturz-Spur:** Die Uhr schreibt laufend mit, was gerade passiert (Zeichnen mit Design-Nummer, Browser-Seite,
  Ton … / Wetter-, Radar-, ISS-Abruf), dazu den kleinsten freien Speicher. Nach einem Absturz steht das unter
  *Status → Absturz-Spur*.

### Einspielen
- CYD: `WetterUhr_2026.11.16-3_CYD_update.bin` (falls die Seite nicht lädt: vorher auf ein anderes Design tippen)
- ESP32-C3: `WetterUhr_2026.11.16-3_update.bin`

### Dateien
| Datei | Wofür |
|---|---|
| `WetterUhr_2026.11.16-3_CYD_update.bin` | CYD: Uhr-Update |
| `RetroCYD_4.0.3_update.bin` | CYD: Spiele (unverändert) |
| `WetterUhr_2026.11.16-3_CYD_mitSpielen_erstinstallation.bin` | CYD: Uhr + RetroCYD 4.0.3 per USB an `0x0` |
| `WetterUhr_2026.11.16-3_CYD_erstinstallation.bin` | CYD: nur Uhr per USB an `0x0` |
| `WetterUhr_2026.11.16-3_update.bin` / `_erstinstallation.bin` | ESP32-C3 |
| `WetterUhr_2026.11.16-3_komplett.zip`, `RetroCYD_4.0.3_komplett.zip` | Quellcode |

Nicht auf echter Hardware getestet.
