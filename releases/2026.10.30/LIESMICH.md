**Firmware 2026.10.30** für den RetroTV-Wecker (ESP32-C3 SuperMini, 2,0-Zoll-Display ST7789).

### Dateien
| Datei | Wofür |
|---|---|
| `WetterUhr_2026.10.30_erstinstallation.bin` | **komplette Firmware für die Erstinstallation per USB** (Bootloader, Partitionstabelle mit Update-Bereich, Programm) – an Adresse `0x0` schreiben |
| `WetterUhr_2026.10.30_update.bin` | **Update-Datei** – im Browser unter *System → Firmware-Update über WLAN* einspielen |
| `WetterUhr_2026.10.30_komplett.zip` | kompletter Quellcode für die Arduino IDE |

### Erstinstallation (neue Uhr oder Umstieg von „Huge APP (No OTA)“)
Ohne Arduino IDE, direkt im Browser (Chrome oder Edge am PC):
1. ESP32-C3 per USB-C-Kabel anschließen.
2. <https://espressif.github.io/esptool-js/> öffnen, **Connect** klicken, den Port des ESP32-C3 wählen.
   Klappt das nicht: BOOT-Taste gedrückt halten, kurz RESET drücken, BOOT loslassen, dann noch einmal Connect.
3. Unter *Flash Address* `0x0` eintragen, die Datei `WetterUhr_2026.10.30_erstinstallation.bin` wählen, **Program** klicken.
4. Nach „Leaving...“ RESET drücken oder das Kabel kurz abziehen.
5. Die Uhr startet den Access-Point **Wetteruhr-Setup** (Passwort `wetteruhr1`) → `http://192.168.4.1` öffnen und einrichten.

Oder mit esptool in der Kommandozeile:
`esptool --chip esp32c3 write-flash 0x0 WetterUhr_2026.10.30_erstinstallation.bin`

**Achtung:** Die Erstinstallation schreibt den ganzen Speicher neu – gespeicherte Einstellungen (WLAN, Designs, Wecker)
werden dabei **gelöscht** und müssen neu eingegeben werden. Wer seine Einstellungen behalten will, lädt stattdessen den
Quellcode in der Arduino IDE einmal per USB hoch (*Partition Scheme: Minimal SPIFFS (1.9MB APP with OTA)*).

### Update (Uhr hat bereits eine Version mit WLAN-Update)
Browser → *System* → *Firmware-Update über WLAN* → `WetterUhr_2026.10.30_update.bin` wählen → *Update installieren*.
Die Einstellungen bleiben erhalten. Unter *Status → Firmware* steht danach `2026.10.30`.

### Neu in dieser Version
- **Firmware-Update über WLAN** im Browser (mit Prüfung der Datei, Neustart nur bei Erfolg) und über den Netzwerk-Port der Arduino IDE.
- Firmware-Version und Bauzeit unter *Status → Firmware*.

### Build
Gebaut mit arduino-cli, ESP32-Arduino-Core 3.3.12, Board *ESP32C3 Dev Module*, *USB CDC On Boot: Enabled*,
*Minimal SPIFFS (1.9MB APP with OTA)*, 4 MB Flash. Programmgröße 1,65 MB (84 % des Update-Bereichs).
Partitionen: nvs 0x9000, otadata 0xE000, app0 0x10000, app1 0x1F0000 (je 1920 KB), spiffs 0x3D0000, coredump 0x3F0000.

Alle Änderungen: [CHANGELOG](https://github.com/smo0601-collab/RetroTVWecker-2.0-Zoll/blob/main/firmware/WetterUhr/CHANGELOG.md)
