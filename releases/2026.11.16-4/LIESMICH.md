**Firmware 2026.11.16-4** (RetroCYD unverändert 4.0.3) – Protokoll über Neustarts hinweg, Selbstheilung des Netzes.

### Gemeldet
Uhr im Browser nicht erreichbar, die Designs laufen weiter, Wetter „noch keine Daten“; erst ein Neustart hilft.
Nach dem Neustart stand die Uhrzeit kurz auf einem unsinnigen Datum (26.10.21212).

### Neu
- **Protokoll** `http://<uhr>/log` (auch über *Status → Absturz-Spur → Protokoll*): zeigt die **vorige** und die
  aktuelle Sitzung, weil es im RTC-Speicher liegt und Neustart wie Absturz übersteht. Darin: Start mit Grund und
  Systemzeit, WLAN, wann NTP die Zeit gesetzt hat, Fehler bei Wetter/Radar/ISS und alle 5 Minuten der Zustand
  (Speicher, WLAN, Uhrzeit, Wetter, letzter erfolgreicher Abruf, Anzahl Browser-Anfragen).
- **Selbstheilung:** Gilt das WLAN als verbunden, klappt aber 20 Minuten lang kein einziger Abruf, verbindet die
  Uhr das WLAN neu.
- *Absturz-Spur* steht jetzt immer im Status (auch „kein Absturz“).

### Einspielen
- CYD: `WetterUhr_2026.11.16-4_CYD_update.bin` (falls die Seite nicht lädt: vorher auf ein anderes Design tippen)
- ESP32-C3: `WetterUhr_2026.11.16-4_update.bin`

### Dateien
| Datei | Wofür |
|---|---|
| `WetterUhr_2026.11.16-4_CYD_update.bin` | CYD: Uhr-Update |
| `RetroCYD_4.0.3_update.bin` | CYD: Spiele (unverändert) |
| `WetterUhr_2026.11.16-4_CYD_mitSpielen_erstinstallation.bin` | CYD: Uhr + RetroCYD 4.0.3 per USB an `0x0` |
| `WetterUhr_2026.11.16-4_CYD_erstinstallation.bin` | CYD: nur Uhr per USB an `0x0` |
| `WetterUhr_2026.11.16-4_update.bin` / `_erstinstallation.bin` | ESP32-C3 |
| `WetterUhr_2026.11.16-4_komplett.zip`, `RetroCYD_4.0.3_komplett.zip` | Quellcode |

Nicht auf echter Hardware getestet.
