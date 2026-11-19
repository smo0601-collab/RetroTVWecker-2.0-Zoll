**Firmware 2026.11.19-1** (RetroCYD unverändert 4.0.3) – Display im Zeitfenster ausschalten, Netz-Task auf Kern 0.

### Neu
- **Display ganz aus statt dimmen:** Browser > Anzeige > Zeitplan-Dimmung, Häkchen „Display im Zeitfenster ganz ausschalten (statt dimmen)“.
  - Zu Beginn des Fensters geht das Display aus, am Ende wieder an.
  - Eine Berührung, der Browser oder der Wecker schalten es zwischendurch ein. Es bleibt dann bis zum nächsten Fenster an.
  - Wetter, Wecker und Browser laufen währenddessen weiter.

### Aus dem Protokoll von 2026.11.18-1
- **Bild-Hänger von rund 0,64 s:** Sie fielen immer in einen TLS-Verbindungsaufbau, entweder beim Wetter (Netz-Schritt 11) oder beim Flugradar (Netz-Schritt 24). Der Netz-Task läuft jetzt fest auf Kern 0, wie das WLAN; Kern 1 bleibt der Anzeige. Ob das die Hänger beseitigt, zeigt das nächste Protokoll.
- **Speicher-Engpässe** (Tiefstand 3972 und 988 Bytes): Sie entstanden im WLAN-Treiber (Task „wifi“), nicht in der Uhr selbst. Die Meldung nennt jetzt zusätzlich den freien Speicher im Moment der Meldung.

### Einspielen
- CYD: `WetterUhr_2026.11.19-1_CYD_update.bin`
- ESP32-C3: `WetterUhr_2026.11.19-1_update.bin`

### Dateien
| Datei | Wofür |
|---|---|
| `WetterUhr_2026.11.19-1_CYD_update.bin` | CYD: Uhr-Update |
| `RetroCYD_4.0.3_update.bin` | CYD: Spiele (unverändert) |
| `WetterUhr_2026.11.19-1_CYD_mitSpielen_erstinstallation.bin` | CYD: Uhr + RetroCYD 4.0.3 per USB an `0x0` |
| `WetterUhr_2026.11.19-1_CYD_erstinstallation.bin` | CYD: nur Uhr per USB an `0x0` |
| `WetterUhr_2026.11.19-1_update.bin` / `_erstinstallation.bin` | ESP32-C3 |
| `WetterUhr_2026.11.19-1_komplett.zip`, `RetroCYD_4.0.3_komplett.zip` | Quellcode |

Nicht auf echter Hardware getestet.
