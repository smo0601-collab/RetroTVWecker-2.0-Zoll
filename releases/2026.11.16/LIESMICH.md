**Firmware 2026.11.16** (RetroCYD unverändert 4.0.3) – Uhr bleibt nach langer Laufzeit im Browser erreichbar.

### Was war
Nach längerer Laufzeit hing die Uhr beim Aufruf der Browser-Seite fast eine Minute, die Seite kam trotzdem nicht.
Sehr wahrscheinliche Ursache: Bei jedem Designwechsel wurden die Bildpuffer der Designs (19 KB, mit feiner Schrift
zusätzlich 38 KB) freigegeben und neu geholt – dazwischen die verschlüsselten Abrufe (Wetter, Radar). Mit der Zeit
zerstückelte der Speicher, für Netz und Browser-Seite fehlte ein großer Block. Jedes der ~15 Seitenstücke wartete
dann bis zu 10 s.

### Jetzt
- Die Bildpuffer werden einmal beim Start geholt und behalten, ebenso der Puffer der Laufschrift.
- Hängt die Verbindung, bricht die Uhr das Senden der Seite sofort ab (kein minutenlanger Stillstand mehr).
- Notbremse: Bleibt der größte freie Speicherblock 10 Minuten unter 16 KB, startet die Uhr in einem ruhigen Moment
  neu (nicht, während der Wecker klingelt).
- Status zeigt freien Speicher, größten Block und den Grund des letzten Neustarts.

### Einspielen
- CYD: Browser der Uhr → *System → Firmware-Update* → `WetterUhr_2026.11.16_CYD_update.bin`
  (Falls die Seite nicht lädt: Uhr kurz vom Strom trennen, dann gleich einspielen.)
- ESP32-C3: `WetterUhr_2026.11.16_update.bin`

### Dateien
| Datei | Wofür |
|---|---|
| `WetterUhr_2026.11.16_CYD_update.bin` | CYD: Uhr-Update |
| `RetroCYD_4.0.3_update.bin` | CYD: Spiele (unverändert) |
| `WetterUhr_2026.11.16_CYD_mitSpielen_erstinstallation.bin` | CYD: Uhr + RetroCYD 4.0.3 per USB an `0x0` |
| `WetterUhr_2026.11.16_CYD_erstinstallation.bin` | CYD: nur Uhr per USB an `0x0` |
| `WetterUhr_2026.11.16_update.bin` / `_erstinstallation.bin` | ESP32-C3 |
| `WetterUhr_2026.11.16_komplett.zip`, `RetroCYD_4.0.3_komplett.zip` | Quellcode |

Nicht auf echter Hardware getestet.
