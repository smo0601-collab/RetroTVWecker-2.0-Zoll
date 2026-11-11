**Firmware 2026.11.11-4** (RetroCYD unverändert 3.9.1) – Korrektur: der automatische Design-Wechsel blieb stehen.

### Was war falsch
An Feier- und besonderen Tagen (z. B. am 11.11.) erscheint alle 15 Minuten kurz die Feiertags-Einblendung. Danach
sprang die Uhr bisher auf das gespeicherte Design zurück und startete den Wechsel-Timer neu – so kam der
automatische Wechsel nie zum nächsten Design.

### Jetzt
- Die Einblendung hält Design und Wechsel-Timer nur an, danach geht es mit dem vorherigen Design weiter.
- Tippen aufs Display während der Einblendung beendet sie.
- Ein im Browser gewähltes Design gilt nach der Einblendung.

### Einspielen
- CYD: Browser der Uhr → *System → Firmware-Update* → `WetterUhr_2026.11.11-4_CYD_update.bin`
  (RetroCYD 3.9.1 muss schon drauf sein – siehe Release 2026.11.11-3)
- ESP32-C3: `WetterUhr_2026.11.11-4_update.bin`

### Dateien
| Datei | Wofür |
|---|---|
| `WetterUhr_2026.11.11-4_CYD_update.bin` | CYD: Uhr-Update |
| `RetroCYD_3.9.1_update.bin` | CYD: Spiele (unverändert) |
| `WetterUhr_2026.11.11-4_CYD_mitSpielen_erstinstallation.bin` | CYD: Uhr + RetroCYD 3.9.1 per USB an `0x0` |
| `WetterUhr_2026.11.11-4_CYD_erstinstallation.bin` | CYD: nur Uhr per USB an `0x0` |
| `WetterUhr_2026.11.11-4_update.bin` / `_erstinstallation.bin` | ESP32-C3 |
| `WetterUhr_2026.11.11-4_komplett.zip`, `RetroCYD_3.9.1_komplett.zip` | Quellcode |

Nicht auf echter Hardware getestet.
