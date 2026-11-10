**Firmware 2026.11.11-3** und **RetroCYD 3.9.1** – Korrektur: eine richtige `*_update.bin` wurde fälschlich als
„Erstinstallations-Datei (mit Bootloader)“ abgelehnt.

### Was war falsch
Die neue Prüfung aus 2026.11.11-2 / RetroCYD 3.9 sollte nur den **Anfang** der Datei ansehen. Sie lief aber auch bei
den nächsten Übertragungs-Blöcken (das Update-Modul meldet erst nach 4 KB Fortschritt) – dort steht natürlich kein
Dateikopf, also kam immer die Fehlermeldung. Jetzt wird wirklich nur der erste Block geprüft.

### Einspielen – zuerst unter *Status → Firmware* nachsehen, welche Uhr-Version läuft

**Uhr zeigt 2026.11.11 (oder älter):**
1. Browser der Uhr → *System → Firmware-Update* → `RetroCYD_3.9.1_update.bin`
2. Danach → `WetterUhr_2026.11.11-3_CYD_update.bin`

**Uhr zeigt schon 2026.11.11-2:** Diese Version lehnt per WLAN jede Datei ab, deshalb einmal per USB
(z. B. mit dem ESP Web Flasher oder esptool):
- Einstellungen bleiben erhalten: `WetterUhr_2026.11.11-3_CYD_update.bin` an `0x10000` **und**
  `RetroCYD_3.9.1_update.bin` an `0x1F0000`
- oder alles neu (Einstellungen weg): `WetterUhr_2026.11.11-3_CYD_mitSpielen_erstinstallation.bin` an `0x0`

**ESP32-C3:** `WetterUhr_2026.11.11-3_update.bin` (bei 2026.11.11-2 auf dem C3 ebenfalls einmal per USB:
`_update.bin` an `0x10000` oder `_erstinstallation.bin` an `0x0`).

### Dateien
| Datei | Wofür |
|---|---|
| `WetterUhr_2026.11.11-3_CYD_update.bin` | CYD: Uhr-Update |
| `RetroCYD_3.9.1_update.bin` | CYD: Spiele-Update |
| `WetterUhr_2026.11.11-3_CYD_mitSpielen_erstinstallation.bin` | CYD: Uhr + RetroCYD 3.9.1 per USB an `0x0` |
| `WetterUhr_2026.11.11-3_CYD_erstinstallation.bin` | CYD: nur Uhr per USB an `0x0` |
| `WetterUhr_2026.11.11-3_update.bin` / `_erstinstallation.bin` | ESP32-C3 |
| `WetterUhr_2026.11.11-3_komplett.zip`, `RetroCYD_3.9.1_komplett.zip` | Quellcode |

Nicht auf echter Hardware getestet.
