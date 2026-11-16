**Firmware 2026.11.16-6** (RetroCYD unverändert 4.0.3) – Wetter lädt wieder: Speicher für die verschlüsselte Verbindung.

### Ursache (aus dem Protokoll der Uhr)
`Wetter: HTTP -1 (connection refused)`, DNS in Ordnung, kleinster freier Speicher **40 Bytes**. Der verschlüsselte
Abruf braucht kurzzeitig rund 60–70 KB. Seit 2026.11.16 war der Puffer der feinen Schrift (38 KB) dauerhaft belegt –
damit reichte der Speicher nie mehr.

### Jetzt
- Für die Dauer eines verschlüsselten Abrufs (Wetter, Schulferien, Ortssuche) gibt die Anzeige den Puffer der feinen
  Schrift frei und holt ihn danach wieder. Die Schrift ist dabei 1–2 Sekunden grob.
- Flugradar: feine Schrift aus (die offene Verbindung braucht den Speicher dauerhaft).
- Protokoll (`/log`): genauer TLS-Fehler beim Wetterabruf; die Diag-Zeilen füllen das Protokoll nicht mehr.

### Einspielen
- CYD: `WetterUhr_2026.11.16-6_CYD_update.bin`
- ESP32-C3: `WetterUhr_2026.11.16-6_update.bin`

### Dateien
| Datei | Wofür |
|---|---|
| `WetterUhr_2026.11.16-6_CYD_update.bin` | CYD: Uhr-Update |
| `RetroCYD_4.0.3_update.bin` | CYD: Spiele (unverändert) |
| `WetterUhr_2026.11.16-6_CYD_mitSpielen_erstinstallation.bin` | CYD: Uhr + RetroCYD 4.0.3 per USB an `0x0` |
| `WetterUhr_2026.11.16-6_CYD_erstinstallation.bin` | CYD: nur Uhr per USB an `0x0` |
| `WetterUhr_2026.11.16-6_update.bin` / `_erstinstallation.bin` | ESP32-C3 |
| `WetterUhr_2026.11.16-6_komplett.zip`, `RetroCYD_4.0.3_komplett.zip` | Quellcode |

Nicht auf echter Hardware getestet.
