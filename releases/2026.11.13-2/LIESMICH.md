**Firmware 2026.11.13-2** und **RetroCYD 4.0.1** – Korrektur: Absturz und Knacken beim Abspielen von Klängen.

### Was war falsch
Für jede neue Tonhöhe wurde der LEDC-Zeitgeber über `ledcChangeFrequency` komplett neu eingerichtet und
zurückgesetzt – bei Gleittönen und Rauschen bis zu 1000-mal pro Sekunde. Das knackte hörbar und hat sehr
wahrscheinlich die Abstürze ausgelöst (auch die beim Wetterabruf: ein Wetterwechsel oder das UFO spielt einen Klang).

### Jetzt
- Die Tonhöhe wird nur noch über den Teiler des eigenen Zeitgebers direkt umgestellt – kein Treiberaufruf, kein Zurücksetzen.
- Ton-Task mit mehr Speicher und niedrigerem Vorrang.
- Höchstens 35 % Tastverhältnis: weniger Spitzenstrom für den Verstärker.
- **Status → „Letzter Neustart“** zeigt den Grund des letzten Neustarts (z. B. *PANIC* = Absturz,
  *BROWNOUT* = Spannungseinbruch) und die Laufzeit.

### Einspielen
1. Browser der Uhr → *System → Firmware-Update* → `RetroCYD_4.0.1_update.bin`
2. Danach → `WetterUhr_2026.11.13-2_CYD_update.bin` (ESP32-C3: `WetterUhr_2026.11.13-2_update.bin`)

Startet die Uhr dabei schon neu, bevor das Update fertig ist: Klang-Effekte unter *Summer & Wecker* kurz
ausschalten, dann einspielen.

### Dateien
| Datei | Wofür |
|---|---|
| `WetterUhr_2026.11.13-2_CYD_update.bin` | CYD: Uhr-Update |
| `RetroCYD_4.0.1_update.bin` | CYD: Spiele-Update |
| `WetterUhr_2026.11.13-2_CYD_mitSpielen_erstinstallation.bin` | CYD: Uhr + RetroCYD 4.0.1 per USB an `0x0` |
| `WetterUhr_2026.11.13-2_CYD_erstinstallation.bin` | CYD: nur Uhr per USB an `0x0` |
| `WetterUhr_2026.11.13-2_update.bin` / `_erstinstallation.bin` | ESP32-C3 |
| `WetterUhr_2026.11.13-2_komplett.zip`, `RetroCYD_4.0.1_komplett.zip` | Quellcode |

Nicht auf echter Hardware getestet.
