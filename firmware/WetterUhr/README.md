# Wetteruhr ESP32-C3 SuperMini + GMT020-02-7P

Ein Bildschirm mit Uhr und Wetter (Open-Meteo). Einstellungen per Browser.

**Status:** Der Code wird mit arduino-cli und dem ESP32-Core 3.3.12 fehlerfrei kompiliert (ca. 1,65 MB, passt in das
OTA-Partitionsschema). Auf echter Hardware ist nicht jede Funktion durchgetestet; Auffaelligkeiten bitte melden.

## Bildschirm (320 x 240, Querformat)

- Statusleiste: WLAN-Symbol, IP-Adresse (oder Einrichtungshinweis), Hinweis "Wetter veraltet"
- Grosse Uhrzeit HH:MM mit Sekunden, darunter Wochentag und Datum
- Aktuelles Wetter: Symbol, Temperatur, Zustand, gefuehlte Temperatur, Feuchte, Wind
- Vorhersage fuer heute und die naechsten zwei Tage

## Bedienung

Alles wird im Browser eingestellt. Optional: ein Touch-Taster (TTP223) zum Blaettern durch die Designs, fuer
Laufschrift und Standby, und ein aktiver Summer fuer Stundensignal und Wecker (siehe Verdrahtung).

## Einstellungen im Browser

Erreichbar unter der IP-Adresse aus der Statusleiste oder `http://wetteruhr.local` (Geraetename einstellbar).
Die Seite hat links eine Navigation mit Kategorien (auf dem Handy als Leiste oben):

| Kategorie | Inhalt |
|---|---|
| Status | WLAN, IP, Uhrzeit, Wetter, Design, Dimmung, Display, freier Speicher |
| Anzeige | Uhr-Design (Vorschaukarten), Akzentfarbe, Design-Wechsel, Extras (Silvester-Automatik, Weihnachtsschlitten, UFO), Sekunden, Ausrichtung |
| Helligkeit | Schieberegler, Display ein/aus (Standby), Zeitplan-Dimmung, Nachtmodus mit Testbild |
| WLAN | Netzwerkname mit Suche, Passwort |
| Summer & Wecker | Stundensignal, Nachtruhe, Touch-Klick, Summer testen, Wecker mit Wochentagen |
| Zeit & Wetter | Zeitzone, NTP an/aus, NTP-Server, Datum und Uhrzeit manuell, Koordinaten, Wetter-Intervall |
| System | Geraetename, Web-Passwort, Firmware-Update ueber WLAN, Werkseinstellungen |

Alle Felder gehoeren zu einem Formular: "Speichern" uebernimmt die Eingaben aller Kategorien auf einmal.
Design, Farbe und Schieberegler wirken sofort. Die zuletzt geoeffnete Kategorie merkt sich der Browser.
Aenderungen am WLAN, NTP-Server oder Geraetenamen starten das Geraet neu.

### Erste Einrichtung

1. Ohne gespeicherte WLAN-Daten startet der Access-Point **Wetteruhr-Setup**
   (Passwort aus `config.h`, Standard `wetteruhr1`, bitte aendern).
2. Mit dem Handy oder PC verbinden, im Browser `http://192.168.4.1` oeffnen.
3. WLAN eintragen, Zeitzone und Standort waehlen, speichern. Das Geraet startet neu.
4. Faellt das WLAN spaeter laenger als 60 s aus, oeffnet sich der Access-Point erneut und schliesst sich nach
   der Wiederverbindung.

### Sicherheit

Die Seite laeuft ueber unverschluesseltes HTTP. Ohne Web-Passwort kann jeder im selben Netzwerk sie bedienen.
Setze deshalb ein Web-Passwort (Benutzername `admin`). Das gespeicherte WLAN-Passwort wird nie wieder an den
Browser ausgegeben (leeres Feld = unveraendert).

## Uhr-Designs und Farben

Im Browser unter "Uhr-Design" gibt es Vorschaukarten. Ein Klick wirkt sofort auf dem Display und wird gespeichert.

| Nr. | Design | Beschreibung |
|---|---|---|
| 0 | Flip-Karten | zwei Karten fuer Stunden und Minuten |
| 1 | Digital + Balken | grosse Zeit mit Sekundenbalken |

Die uebrigen klassischen Designs (Digital klassisch/farbig/Duo, drei Analoguhren, Modern, Binaer) wurden entfernt.

Akzentfarben: Cyan, Gruen, Orange, Gelb, Rot, Pink, Blau, Weiss, Violett. Die Farbe wird auch fuer Sekunden,
Wochentag und Wetterzustand verwendet. Die Vorschaubilder im Browser sind vereinfachte Darstellungen,
das Display weicht im Detail ab.

Standard fuer den ersten Start: `DEFAULT_CLOCK_STYLE` und `DEFAULT_ACCENT` in `config.h`.

### Laufschrift

Browser > "Laufschrift": Text eingeben (bis 160 Zeichen), Umlaute und Sonderzeichen sind erlaubt. Die Tasten unter
dem Textfeld fuegen haeufige Zeichen ein (ä ö ü Ä Ö Ü ß € ° ♥ → ← ↑ ↓ • ± é è). Moeglich sind ausserdem µ ² £ ¥ « » ½ ¼ à ç ñ
und weitere westeuropaeische Buchstaben; unbekannte Zeichen erscheinen als "?".

- **Einmalig:** Der Text laeuft beim Speichern einmal ueber den Bildschirm. Der Knopf "Jetzt einmal abspielen" wiederholt das jederzeit.
- **Dauerschleife:** Der Text laeuft immer wieder, dazwischen die eingestellte Pause (0 = nahtlos).
- Position (oben, Mitte, unten), Groesse (16 oder 32 Pixel hoch), Farbe, Geschwindigkeit und dunkles Hintergrundband sind einstellbar.
- Die Schrift ist die eingebaute 5x7-Pixelschrift und liegt ueber jedem Design. Bei klassischen Designs wird der Bildschirm nach
  dem Durchlauf einmal neu aufgebaut (kurzes Aufblitzen), bei Themen-Designs nicht. Im Standby pausiert die Laufschrift.

### Automatischer Design-Wechsel

Im Browser unter "Anzeige" > "Automatischer Design-Wechsel": Haken setzen, Intervall waehlen (Sekunden, Minuten oder
Stunden; 10 s bis 24 h), die gewuenschten Designs ankreuzen (mindestens zwei) und optional "Zufaellige Reihenfolge".
Nach "Speichern" wechselt die Uhr im Takt durch die gewaehlten Designs, mit dem jeweiligen Uebergang der Themen-Designs.
Ein Klick auf eine Design-Karte zeigt das Design sofort und startet das Intervall neu. Im Standby pausiert der Timer.

**Feiertage automatisch:** Weihnachten, Silvester, Valentinstag, Halloween und Ostern kommen jeweils 10 Tage vor dem
Termin von selbst in den Wechsel und fallen nach dem Fest wieder heraus - ohne dass die eigene Liste geaendert wird.
Zeitfenster: Weihnachten und Weihnachtself 14.-26.12., Silvester 21.12.-1.1., Valentinstag 4.-14.2., Halloween 21.-31.10., Ostern
10 Tage vor Ostersonntag bis Ostermontag. Abschalten unter *Anzeige > Extras*. Die Status-Seite zeigt beim
Uhr-Design z. B. "(Wechsel aktiv + Halloween, naechster in 42 s)". Die Zahl der Tage steht in `config.h`
(`HOL_DAYS_BEFORE`).

### Themen-Designs (Pixel-Retro, Nr. 2-32)

Animierte Szenen mit eigenen, festen Farben (die Akzentfarbe wirkt hier nicht). Alle zeigen Uhrzeit,
Datum, Temperatur und Wetterdaten; Sekunden nur, wenn "Sekunden anzeigen" aktiv ist. Figuren, Namen und Symbole
sind eigene Erfindungen im Retro-Stil.

| Nr. | Design | Szene |
|---|---|---|
| 2 | Abenteuer | Nachtlandschaft mit Mond und Kiefern, Zeit in goldenen Zellen auf einer Steintafel, Wanderer mit Stab, Gluehwuermchen, Herzen zeigen das Wetter, Funkenregen zur vollen Minute |
| 3 | Fallende Bloecke | Ein Computerspieler legt Steine in einen Schacht (macht absichtlich Fehler und raeumt ab und zu auf). Links Zeit, Sekundenzellen, naechster Stein, Temperatur, Zeilenzaehler |
| 4 | Labyrinth | Maus im Ringgang sammelt Kruemel, springt ueber drei Katzen; Kaese in den Ecken macht Katzen zahm; Apfel zur vollen Minute, bunte Waende zur vollen Stunde |
| 5 | Huepfwelt | Ziffern in Bonus-Bloecken; ein runder Held springt bei jedem Zeitwechsel unter den passenden Block, sammelt Muenzen; Wolken, Gebuesch, Ziegelboden |
| 6 | Neon-Raster | Leuchtraster mit wandernden Lichtimpulsen, Punktmatrix-Zeit, orangefarbenes Lichtmotorrad umrundet den Rand (Sekunden), Lichtring zur vollen Minute, Scanlines |
| 7 | Asteroiden | Vektorlook: Sterne mit Parallaxe, trudelnde Brocken, Schiff schiesst lenkende Geschosse, Splitter, Leben-Symbole; Zeit aus Liniensegmenten |
| 8 | Code-Regen | Gruener Zeichenregen in drei Tiefenstufen, Uhr im freien Feld, Terminal mit Datum/Temperatur/Wind, Datenimpuls zur Minute, Scanbalken alle 20 s, Glitch zur Stunde |
| 9 | Zahnrad-Werk | Fabrikhalle mit Traegern und Leitern: ein Roboter rollt Zahnraeder im Zickzack nach unten, ein Monteur steigt Leitern und springt ueber Zahnraeder, BONUS-Blinken zur vollen Stunde |
| 10 | Ziegelbrecher | Fuenf bunte Steinreihen, Ball und Schlaeger spielen von selbst, Steine zerfallen in Funken, Uhr gross in Reihenfarben in der Mitte, Steine blinken zur Minute |
| 11 | Froschfurt | Ufer mit Uhr, Wasser mit Staemmen und Schildkroeten, Insel, drei Fahrspuren; ein Frosch huepft nach oben (mit gelegentlichen Fehlspruengen) |
| 12 | Raketenabwehr | Sternenhimmel und Pixelstadt, Raketenspuren werden von Abwehrstellungen abgefangen; Uhr in einer Kommandozentrale, Radarring zur Minute |
| 13 | Neonbrecher | Neon-Spielfeld, Uhr oben ueber dem Feld, Leuchtball, doppelte Bloecke, Kapsel macht den Schlaeger breiter |
| 14 | Pulsierendes Herz | Herz schlaegt im Doppelschlag (ca. 70/min) mit Ringen und aufsteigenden Mini-Herzen, EKG-Kurve, Datum links, Wetter rechts, Sekundenbalken; zur vollen Minute Blitz und Herzchen |
| 15 | Sinuskurve | Oszilloskop mit drei laufenden Wellen (Cyan, Magenta, gelbe Summe), Sekundenpunkt reitet auf der gelben Kurve; zur vollen Minute schlaegt die Amplitude aus |
| 16 | Badminton | Rot gegen Blau vor jubelndem Publikum, gezaehlt nach Badminton-Regeln (Satz bis 21 mit 2 Punkten Vorsprung, hoechstens 30, zwei Gewinnsaetze, fast immer ueber 3 Saetze). Clear, Drop, Smash im Sprung mit Geschwindigkeitsanzeige und Leuchtspur, Drive, Netzball; Punkte durch Gewinnschlag (Gegner hechtet vergeblich), Netz oder Aus, knappe Aus-Baelle per Hawk-Eye (kann das Urteil kippen). Momentum mit Serien und Aufholjagden, Rallye-Zaehler, SATZBALL/MATCHBALL/VERLAENGERUNG, Sieger jubelt, Konfetti zum Matchgewinn; zur vollen Minute Funken am Netz. Der Spielstand bleibt erhalten: nach einem Designwechsel geht das Match genau dort weiter, nach Neustart oder Stromausfall ab dem zuletzt gesicherten Stand (Sicherung im Flash nach jedem Punkt, hoechstens alle 8 s; Satz- und Seitenwechsel sofort). Erst wenn ein Sieger feststeht, beginnt ein neues Match |
| 17 | Testbild | Fernseh-Testbild (eigene Gestaltung): Raster, Farbbalken, Kreis, Graukeil, Uhrzeit im schwarzen Streifen, rollender Balken, Schnee und Stoerstreifen; auch fuer den Nachtmodus |
| 18 | Dackelwelpe | Schwarz-brauner Dackelwelpe jagt auf der Wiese einem Ball nach, setzt sich, schnueffelt, bellt zur vollen Minute "WUFF!"; Tag und Nacht |
| 19 | Flugradar | Radarschirm mit echten Flugzeugen im Umkreis (siehe unten); optional ab und zu ein UFO |
| 20 | Traumsand | Ein erfundener Wicht auf einer Wolke streut goldenen Sand, Sterne, Mondsichel, Sternschnuppe; Sandschauer zur vollen Minute |
| 21 | Aquarium | Korallenriff mit Farbverlauf, Lichtstrahlen und tanzenden Lichtmustern auf dem Sand, Korallen, Seeanemone, wogender Seetang, Seestern, Schatztruhe (klappt ab und zu auf). Clownfische (verstecken sich in der Anemone), Doktorfische, Segelflosser, Kugelfisch, Neon-Schwarm, Qualle, Krebs. Fuettern zur vollen und halben Stunde und per Knopf. Alle paar Minuten Besuch, abwechselnd: eine freundliche Seeschlange (die Fische fliehen, der Kugelfisch blaest sich auf, der Krebs versteckt sich) oder ein Forschungs-U-Boot mit Scheinwerfer, das anhaelt und "PING" macht. Knoepfe unter Anzeige > Extras: "Fische fuettern", "Besuch: Seeschlange / U-Boot" |
| 22 | Blockwelt | Scrollende Wuerfelwelt mit Baeumen, Sonne/Mond nach echter Uhrzeit |
| 23 | Familie | Vater, Mutter und drei Toechter winken abwechselnd, die Juengste huepft, zur vollen Minute steigen Herzen auf |
| 24 | ISS-Tracker | Weltkarte mit Tag/Nacht und der echten ISS-Position (Abruf alle 5 s von open-notify.org); optional Weihnachtsschlitten (siehe unten) |
| 25 | Sonne und Mond | Tagesdiagramm der Hoehe von Sonne und Mond, Auf-/Untergang, Mondphase; ohne Internet aus Standort und Uhrzeit gerechnet |
| 26 | Weihnachten | Verschneite Winternacht, geschmueckter Tannenbaum mit blinkenden Lichtern, Geschenke, Schneefall; Countdown bis Heiligabend 18 Uhr (Tage und Std:Min:Sek), der Adventskranz zuendet an jedem Adventssonntag eine Kerze mehr; am 24. ab 18 Uhr bis 26.12. "FROHE WEIHNACHTEN"; zur vollen Minute Lauflicht am Baum |
| 27 | Weihnachtself | Ein lustiger Elf in der Werkstatt holt Geschenke vom Foerderband und wirft sie in den Sack (Zaehler am Sack), rutscht ab und zu aus und sieht Sterne ("HOPPLA!"); zur vollen Minute huepft er und ruft "HO HO HO!", die Tage bis Weihnachten oder die Zahl der gepackten Geschenke |
| 28 | Halloween | Vollmond, Fledermaeuse, Spinne am Faden, Friedhof mit totem Baum, flackernder Kuerbis, schwebendes Gespenst; Countdown bis 31.10., am Tag selbst "HAPPY HALLOWEEN"; zur vollen Minute saust das Gespenst heran: "BUH!" |
| 29 | Ostern | Fruehlingswiese, Osterhase hoppelt und wackelt mit den Ohren, versteckte bemalte Eier, Schmetterlinge, Uhrzeit in Ostereier-Farben; Countdown bis Ostersonntag (echtes Osterdatum), an Ostersonntag und -montag "FROHE OSTERN"; zur vollen Minute schluepft im Nest ein Kueken: "PIEP!" |
| 30 | Valentinstag | Aufsteigende Herzen, zwei schnaebelnde Turteltauben auf einem Ast, Amors Pfeil fliegt durchs Bild; Countdown bis 14.02., am Tag selbst "ALLES LIEBE"; zur vollen Minute oeffnet sich ein Liebesbrief und ein grosses Herz zerplatzt |
| 31 | Silvester | Stadt-Silhouette mit Feuerwerk (Ring, Chrysantheme, goldene Weide, Knistersterne); Countdown bis Neujahr, am 31.12. als Std:Min:Sek, letzte Minute mit riesigen Sekunden, um Mitternacht Lichtblitz, "PROSIT NEUJAHR" und Dauerfeuerwerk, am 1.1. "FROHES NEUES JAHR" |
| 32 | Squishy Dumpling | Ein knautschiger Teigknoedel (eigene Figur) im Bambus-Daempfkorb: atmet, blinzelt, huepft mit Quetschen und Strecken, wird mit Essstaebchen angestupst ("SQUISH!") und wabbelt nach, wird rot und pufft Dampf ("HIHI!"), schaut sich um, schuettelt sich; nachts (22-6 Uhr) schlaeft er ("Zz"); zur vollen Minute grosser Sprung mit platschender Landung ("BOING!") |

Beim Wechsel von oder zu einem Themen-Design laeuft ein eigener Uebergangseffekt. Die Szene wird in einen
160x120-Puffer mit 256 Farben gezeichnet (ca. 19 KB RAM, nur waehrend ein Themen-Design aktiv ist), auf 320x240
verdoppelt und nur in geaenderten 32x16-Kacheln zum Display gesendet. Reicht der Speicher nicht, faellt die Uhr
auf Design 0 (Flip-Karten) zurueck.

**Kategorien:** Auf der Seite *Anzeige* filtern Schaltflaechen die Design-Karten. Im automatischen Wechsel sind die
Designs nach Kategorie gruppiert; ein Haken an der Kategorie waehlt alle ihre Designs. Kategorien: Klassische Uhren,
Spiele & Arcade, Tiere & Natur, Technik & Retro, Traum & Gefuehl, Heimat & Familie, Himmel & Weltraum, Feiertage.

**Umstellung von der alten Version:** Entfernt wurden die klassischen Designs ausser Flip-Karten und Digital + Balken
sowie Shiba-Inu-Welpe, Mayen, Tarnmuster, Maerchenschloss, Arena-Duell und Astrologie. Dadurch haben sich die Nummern
geaendert. Beim ersten Start rechnet die Uhr die gespeicherte Auswahl und die Wechsel-Liste automatisch um:
entfernte Designs fallen aus der Liste, ein entferntes ausgewaehltes Design wird durch ein aehnliches ersetzt
(z. B. Shiba -> Dackel, Maerchenschloss -> Traumsand, Astrologie -> Sonne und Mond, Analoguhren -> Flip-Karten).

### Feiertage und Extras

Browser > "Anzeige" > "Extras":

- **Feiertags-Designs automatisch im Design-Wechsel** (Standard an): siehe "Automatischer Design-Wechsel".
- **Silvester-Automatik** (Standard an): Am 31.12. von 23:30 bis 1.1. 00:30 zeigt die Uhr automatisch das Design
  "Silvester" mit Countdown und Feuerwerk, egal welches Design gewaehlt ist. Sie hat Vorrang vor dem Nachtmodus und
  dem automatischen Wechsel. Danach geht es mit dem gewaehlten Design bzw. der Liste weiter.
- **ISS-Tracker zeigt** die ISS, immer den Weihnachtsschlitten oder den Schlitten nur vom 1. bis 26.12.
  (Standard). Der Schlitten mit zwei Rentieren (eines mit blinkender roter Nase) fliegt immer dort, wo gerade
  Mitternacht ist, also mit 15 Grad pro Stunde nach Westen, und zieht eine Glitzerspur. Unten steht die
  Entfernung zu dir und wann er bei dir ist (deine Mitternacht). Dafuer braucht es kein Internet, nur die Uhrzeit.
- **Flugradar: UFO** (Standard an): Ab und zu (etwa alle 2 bis 5 Minuten, solange das Radar laeuft) taucht fuer ca.
  20 Sekunden ein "unbekanntes Objekt" auf. Es schwebt, springt ruckartig an neue Stellen, blinkt in Magenta und
  Gruen, die Infotafel zeigt "UFO!" mit unmoeglichen Werten, unten steht "+1 UFO". Nur ein Spass, keine echten Daten.
  Der Knopf **"UFO jetzt zeigen"** schaltet auf das Flugradar und laesst sofort eines auftauchen.

Die Countdowns rechnen mit der eingestellten Zeitzone (Sommer-/Winterzeit inklusive).

Eigenes Zifferblatt ergaenzen: Modul nach dem Muster `face_*.h` schreiben (`enter`, `draw`, `cover`), in `faces.h`
einbinden und **am Ende** von `FACES[]` eintragen, dann `FACE_COUNT` und `CLOCK_STYLE_COUNT` erhoehen sowie Name,
Kategorie und Vorschau in `webui.h` ergaenzen.

## Datum und Uhrzeit manuell

Browser > "Zeit & Wetter":

- **"Uhrzeit automatisch per NTP"**: Mit Haken holt die Uhr die Zeit aus dem Internet (Standard). Ohne Haken
  gibt es keinen NTP-Abgleich, die Zeit kommt nur aus dem Browser. Die Umstellung wirkt ohne Neustart.
- **"Datum und Uhrzeit manuell"**: Datum und Uhrzeit eintragen oder "Zeit vom Browser uebernehmen" druecken,
  dann **"Uhrzeit setzen"**. Dieser Knopf uebernimmt nur Datum und Uhrzeit, keine anderen Felder.
  Die Eingabe gilt in der eingestellten Zeitzone, Sommer- und Winterzeit werden automatisch erkannt.
- Mit NTP wird eine manuell gesetzte Zeit beim naechsten Abgleich (etwa stuendlich) wieder ueberschrieben.
- Die Uhr hat keine Pufferbatterie (RTC). Nach einem Stromausfall ist die Zeit weg und muss ohne NTP neu gesetzt
  werden. Ein Neustart ueber den Browser behaelt sie in der Regel.
- Die Status-Seite zeigt "(manuell gesetzt)" und die Zeitquelle.

## Touch-Taster (TTP223)

| TTP223 | ESP32-C3 SuperMini |
|---|---|
| VCC | 3V3 (nicht 5V) |
| GND | GND |
| I/O | GPIO 5 (`PIN_TOUCH` in `config.h`, ab Werk 5) |

Loetbruecken A und B offen lassen (Ausgang HIGH bei Beruehrung, nur solange beruehrt).
Bedienung: kurz tippen = naechstes Design aus der Wechsel-Liste, zweimal = Laufschrift, 1 s halten = Standby, im Standby weckt jede
Beruehrung, ein klingelnder Wecker wird abgestellt.

**Fehlersuche:** Die Status-Seite zeigt unter "Touch-Taster" den Pegel am Pin und wie oft eine Beruehrung erkannt
wurde (Seite beim Beruehren neu laden).
- Bleibt der Pegel bei Beruehrung auf LOW: Verdrahtung pruefen (I/O wirklich an GPIO 5? GND gemeinsam?). Die LED
  auf dem Modul muss bei Beruehrung leuchten, sonst bekommt das Modul keinen Strom.
- Steht er dauerhaft auf HIGH: Loetbruecke A ist geschlossen (Ausgang invertiert) oder das Modul hat sich beim
  Einschalten mit aufgelegtem Finger kalibriert. Strom aus und wieder an, ohne die Flaeche zu beruehren.
- Wird "erkannt" hochgezaehlt, aber nichts passiert: Nach einem Design-Wechsel ist der Touch 1,5 s gesperrt
  (`TOUCH_LOCK_MS`), nach einer Auswahl im Browser 2 s.

**Weiterblaettern:** Sind unter "Automatischer Design-Wechsel" mindestens zwei Designs angehakt, springt ein Tipp
zum naechsten Design dieser Liste (in Listenreihenfolge, auch bei zufaelligem Wechsel). Ist der Wechsel aktiv, laeuft
er danach normal weiter: das naechste automatische Umschalten kommt nach dem vollen Intervall ab dem Tipp. Ist der
Wechsel aus, bleibt das angetippte Design und wird gespeichert. Ohne Liste geht es durch alle Designs.

**Start und Speichern bei aktivem Wechsel:** Die Uhr startet direkt mit einem Design aus der Liste (dem angewaehlten,
wenn es in der Liste steht, sonst dem naechsten der Liste) und wechselt ab dem Start im eingestellten Intervall.
"Speichern" unterbricht den Wechsel nicht. Nur ein Klick auf eine Design-Karte zeigt dieses Design 2 Minuten lang,
danach geht es in der Liste weiter. Die Status-Seite zeigt, in wie vielen Sekunden der naechste Wechsel kommt.

## Summer und Wecker

Unterstuetzt wird ein **aktiver** Piezo-Summer fuer 3 V (mit eingebautem Tongeber, piept von selbst, sobald Spannung
anliegt). Ein passiver Summer ohne Tongeber bleibt stumm.

Anschluss an **GPIO 10** (`PIN_BUZZER` in `config.h`, -1 = nicht verbaut):

| Variante | Verdrahtung |
|---|---|
| Mit Transistor (empfohlen) | GPIO 10 -> 1 kOhm -> Basis NPN (BC547, S8050, 2N2222). Emitter -> GND. Kollektor -> Summer "-". Summer "+" (langes Bein) -> 3V3 |
| Direkt | Nur wenn der Summer laut Datenblatt hoechstens ca. 12 mA braucht: GPIO 10 -> Summer "+", Summer "-" -> GND |

Viele 3-V-Summer ziehen 20-30 mA, das ist zu viel fuer einen GPIO. Im Zweifel den Transistor nehmen. Bei
magnetischen Summern (mit Spule) eine Diode 1N4148 parallel zum Summer setzen, Kathode (Ring) an "+".
Nicht GPIO 2, 8 oder 9 verwenden (Boot-Pins). Schaltet die Ansteuerung invertiert (PNP), `BUZZER_ACTIVE_HIGH` auf 0.

Browser > "Summer & Wecker":

- **Stundensignal**: zur vollen Stunde zweimal kurz. Optional still waehrend der Zeitplan-Dimmung (Nachtruhe, Standard an).
- **Klick bei Beruehrung**: nur sichtbar, wenn ein Touch-Taster verbaut ist (`PIN_TOUCH`).
- **Summer testen**: kurzes Testmuster.
- **Wecker**: Weckzeit und Wochentage. Er piept bis zu 5 Minuten (`ALARM_MAX_MS`) und schaltet ein Display im
  Standby ein. Abstellen per Touch-Taster (jede Beruehrung) oder per Knopf "Wecker abstellen" im Browser
  (Status bzw. "Summer & Wecker"). Er braucht eine gueltige Uhrzeit (NTP oder manuell).

## Zeitplan-Dimmung

Im Browser unter "Zeitplan-Dimmung": Haken setzen, "Dimmen ab" und "Zurueck zur normalen Helligkeit um" waehlen,
dazu die Helligkeit waehrend der Dimmung (5-100 %). Beispiel: 22:00 bis 07:00 mit 15 %. Liegt "ab" spaeter als
"um", gilt das Fenster ueber Mitternacht. Nach "Speichern" wirkt es sofort, danach wechselt das Display automatisch.

- Waehrend des Fensters gilt der Dimmwert statt des Schiebereglers, ausserhalb der Schieberegler.
- Es wird eine gueltige Uhrzeit benoetigt (NTP). Ohne Uhrzeit bleibt die normale Helligkeit.
- Ohne Backlight-Umbau werden nur die Farben abgedunkelt (siehe Abschnitt Dimmung), mit Umbau per PWM.
- **Nachtmodus mit Testbild:** Haken "Nachtmodus: waehrend der Dimmung das Testbild zeigen". Im Fenster zeigt
  die Uhr dann das Testbild (Design 17) statt des gewaehlten Designs, auch wenn der automatische Design-Wechsel an
  ist. Am Ende des Fensters kehrt sie zum gewaehlten Design zurueck. Ein Tipp auf den Touch-Taster waehlt nachts
  das naechste Design fuer den Tag, das Testbild bleibt bis zum Fensterende. Standard: `DEFAULT_DIM_TESTCARD`.
- Standardwerte: `DEFAULT_DIM_*` in `config.h`.

## Dimmung: was geht und was nicht

Das Backlight des GMT020-02-7P ist fest verdrahtet, das Modul hat keinen BL-Pin.

- **Ohne Umbau (Standard):** Software-Dimmung. Alle Farben werden gleichmaessig abgedunkelt. Das senkt die
  Helligkeit der Anzeige, das Backlight selbst bleibt aber gleich hell und schwarze Flaechen leuchten weiter.
  Bei einem dunklen Design wirkt das trotzdem deutlich. Standby (per Browser) schaltet nur den Controller ab (Backlight bleibt an).
- **Mit Umbau: echte PWM-Dimmung und Backlight wirklich aus.** Dazu muss der Masseweg des Backlights
  (LED-Minus) ueber einen N-Kanal-MOSFET laufen, dessen Gate ein GPIO per PWM steuert:
  - N-MOSFET mit niedriger Gate-Schwelle (z. B. AO3400 oder IRLML6344), Gate-Vorwiderstand 100-220 Ohm,
    10 kOhm Pulldown Gate nach GND
  - LED-Minus von GND trennen und ueber Drain/Source des MOSFETs fuehren
  - Gate-Pin in `PIN_TFT_BL` in `config.h` eintragen (z. B. 10 oder das jetzt freie GPIO0)
  - Das Backlight zieht etwa 60 mA und darf nie direkt von einem GPIO versorgt werden.

  **Ich kann aus dem Foto nicht sicher erkennen, wo LED-Minus auf deiner Platine verlaeuft** und was Q1 ist.
  Fuehre den Umbau nur durch, wenn du die Leiterbahn eindeutig identifiziert hast (z. B. mit Multimeter im
  Durchgangsmodus und einer Lupe). Ein schaerferes Nahfoto der Platinenrueckseite hilft, das zu klaeren.

## Verdrahtung

Modul-Beschriftung (links nach rechts, laut Foto): CS, DC, RST, SDA, SCL, VCC, GND.

| Bauteil | Anschluss | ESP32-C3 SuperMini |
|---|---|---|
| TFT | SCL | GPIO4 |
| TFT | SDA | GPIO6 |
| TFT | CS | GPIO7 |
| TFT | DC | GPIO3 |
| TFT | RST | GPIO1 |
| TFT | VCC | 3V3 |
| TFT | GND | GND |
| Touch-Taster TTP223 (optional) | I/O (VCC an 3V3, GND an GND) | GPIO5 (`PIN_TOUCH`) |
| Aktiver Summer 3 V (optional) | ueber NPN-Transistor, siehe "Summer und Wecker" | GPIO10 (`PIN_BUZZER`) |
| (optional) MOSFET-Gate | - | `PIN_TFT_BL` (freier GPIO, z. B. GPIO0) |

Touch-Taster (TTP223, Jumper unveraendert): kurz tippen = naechstes Design (aus der Wechsel-Liste), doppelt tippen = Laufschrift, 1 s halten = Standby an/aus, im Standby weckt jede Beruehrung.
GPIO0 ist frei. Nicht belegen: GPIO2, GPIO8, GPIO9 (Strapping/Boot), GPIO20/21 (UART). Display-Kabel kurz halten.

## Installation (Arduino IDE 2.x)

1. **Boardpaket:** Boardverwalter-URL
   `https://espressif.github.io/arduino-esp32/package_esp32_index.json` unter *Datei > Voreinstellungen*
   eintragen, dann "esp32 by Espressif Systems" installieren.
2. **Bibliotheken** (Bibliotheksverwalter): *Adafruit GFX Library*, *Adafruit ST7735 and ST7789 Library*,
   *ArduinoJson* in **Version 7.x**. (`WebServer`, `Preferences`, `ESPmDNS`, `WiFi`, `HTTPClient` sind im Boardpaket enthalten.)
3. **Board:** *ESP32C3 Dev Module*, *USB CDC On Boot: Enabled*, **Partition Scheme: Minimal SPIFFS (1.9MB APP with OTA)**
   (noetig fuer Updates ueber WLAN, siehe unten), bei Boot-Problemen *Flash Mode: DIO*.
4. Bei Upload-Problemen: BOOT gedrueckt halten, RESET druecken, loslassen, dann hochladen.

## Update ueber WLAN (OTA)

Neue Firmware laesst sich ohne USB-Kabel einspielen. Die Einstellungen (WLAN, Designs, Wecker, Badminton-Stand ...)
bleiben erhalten.

**Einmalig vorher:** Das Partitionsschema muss Platz fuer ein Update haben. Mit dem frueher empfohlenen
*Huge APP (3MB No OTA)* geht es nicht; die Web-Seite zeigt dann unter *System* einen Hinweis. Einmal per USB mit
*Partition Scheme: Minimal SPIFFS (1.9MB APP with OTA)* hochladen. Die Einstellungen bleiben dabei
erhalten (gleicher NVS-Bereich). Meldet die Arduino IDE beim Kompilieren mehr als 100 % Programmspeicher, passt die
Firmware nicht in 1,9 MB.

**Weg 1 - im Browser:**
1. Arduino IDE: *Sketch > Kompilierte Binaerdatei exportieren* (Strg+Alt+S).
2. Im Sketch-Ordner unter `build/...` liegt `WetterUhr.ino.bin`. Nur diese Datei verwenden, nicht
   `...merged.bin`, `...bootloader.bin` oder `...partitions.bin`.
3. Browser > *System* > *Firmware-Update ueber WLAN*: Datei waehlen, "Update installieren".
4. Fortschritt im Browser und auf dem Display. Die Uhr prueft die Datei und startet nur bei Erfolg neu; bei einem
   Fehler (falsche Datei, zu gross, Verbindung abgebrochen) laeuft die alte Firmware einfach weiter.
5. Unter *Status* > *Firmware* stehen danach Version und Bauzeit der neuen Firmware.

**Weg 2 - direkt aus der Arduino IDE:** Uhr im selben WLAN, in der IDE unter *Werkzeuge > Port* den Netzwerk-Port
(Geraetename, z. B. `wetteruhr at 192.168.x.x`) waehlen und normal hochladen. Ist ein Web-Passwort gesetzt, fragt
die IDE danach. Abschalten mit `ARDUINO_OTA 0` in `config.h`.

**Neue Versionen:** Jede Version kommt als Komplettpaket und als Update-Paket mit nur den geaenderten Dateien.
Welche Dateien sich geaendert haben, steht in `CHANGELOG.md`. Fuer ein Update die Dateien aus dem Update-Paket in den
Sketch-Ordner kopieren (vorhandene ueberschreiben, entfernte loeschen), Binaerdatei exportieren und per Weg 1 oder 2
einspielen. Die Firmware selbst ist immer eine einzige Datei; das Update ersetzt das ganze Programm.

**Sicherheit:** Ohne Web-Passwort kann jeder im WLAN eine Firmware einspielen. Bitte ein Web-Passwort setzen.

## Inbetriebnahme in dieser Reihenfolge

1. Display, ESP32-C3 und optionale Teile verdrahten (siehe Verdrahtung).
2. `WetterUhr` einmal per USB hochladen (Partitionsschema mit OTA), Einrichtungsablauf wie oben.
3. Danach Updates per WLAN (siehe "Update ueber WLAN").

## Projektdateien

| Datei | Inhalt |
|---|---|
| `WetterUhr.ino` | Hauptprogramm: Anzeige, WLAN, Wetterabruf |
| `config.h` | Pins, Standardwerte, feste Optionen |
| `settings.h` | Dauerhafte Einstellungen (Flash) |
| `webui.h` | Browser-Einstellungsseite |
| `icons.h` | Farben, Akzentfarben, Wettersymbole, Wettercode-Texte |
| `types.h` | Gemeinsame Typen (Weather, Align) |
| `faces.h` | Themen-Designs: Verwaltung, Kachel-Ausgabe, Uebergaenge, Liste der Zifferblaetter |
| `faces_core.h` | Gemeinsame Bausteine: Pixelziffern, Sprites, Text, Zufall |
| `ticker.h` | Laufschrift: UTF-8 zu Pixelschrift, Scrollen, Zeichnen |
| `faces_fx.h` | Gemeinsame Effekte: Partikel, Ballbewegung, Arcade-Schrift |
| `buzzer.h` | Aktiver Piezo-Summer: Tonmuster, Wecker-Zustand |
| `CHANGELOG.md` | Versionen und geaenderte Dateien |
| `face_*.h`, `pet.h`, `astro.h`, `iss_map.h` | je ein Themen-Zifferblatt bzw. Hilfsdaten (Haustier-Szene, Astronomie, Weltkarte) |
| `face_xmas.h`, `face_elf.h`, `face_halloween.h`, `face_easter.h`, `face_valentine.h`, `face_newyear.h` | Feiertags-Designs (`face_dumpling.h`: Squishy Dumpling) |

## Funktionsweise

- **Nichts blockiert:** Wetterabruf laeuft in eigenem FreeRTOS-Task, WLAN-Suche asynchron. Die Hauptschleife
  bedient Anzeige und Webserver. Beim Seitenaufruf kann der Webserver kurz (Millisekunden) beschaeftigt sein.
- **Zeit:** SNTP, POSIX-Zeitzone mit automatischer Sommer-/Winterzeit. Bei WLAN-Ausfall laeuft die Uhr intern weiter.
- **Wetter:** Intervall einstellbar (Standard 15 min). Nach Fehlern erneuter Versuch nach 1 min. Nach drei verpassten
  Intervallen wird "Wetter veraltet" angezeigt und die Werte werden orange.
- **Speicher schonen:** Die Helligkeit wird erst 3 s nach der letzten Aenderung in den Flash geschrieben.
- **Flackerarm:** Es werden nur geaenderte Elemente neu gezeichnet. Software-Dimmung zeichnet einmal komplett neu.

## Bekannte Einschraenkungen

- Umlaute: Die grossen Schriften der klassischen Designs haben keine, Texte sind ASCII ("Gefuehlt"). Nur die Laufschrift beherrscht Umlaute. Das Gradzeichen wird gezeichnet.
- TLS ohne Zertifikatspruefung (`setInsecure()`): verschluesselt, aber der Server wird nicht verifiziert.
- Das Layout ist fuer Querformat (320 x 240) gebaut.
- HTTP-Seite ohne Verschluesselung, kein Schutz gegen Angriffe aus dem eigenen Netzwerk ausser dem Passwort.
- Zeitzonenliste ist eine Auswahl haeufiger Zonen; andere ueber den POSIX-String eintragen.

## Fehlersuche

| Symptom | Moegliche Ursache / Abhilfe |
|---|---|
| Display bleibt weiss | VCC/GND, Pinbelegung, RST pruefen |
| Rot und Blau vertauscht | `TFT_SWAP_RB 1` |
| Farben invertiert | `TFT_FORCE_INVERT` auf 0 oder 1 testen |
| Bildstoerungen | Kabel kuerzen, Masse sicher verbinden |
| Seite nicht erreichbar | IP in der Statusleiste pruefen, gleiches Netzwerk, `wetteruhr.local` geht nicht in jedem Netz |
| Passwort vergessen | Web-Passwort ueber "Werkseinstellungen" nicht erreichbar: Flash komplett loeschen (Arduino IDE: Tools > Erase All Flash) |
| Kein WLAN / instabil | 2,4 GHz, SSID/Passwort, `WIFI_REDUCE_TX_POWER 1` testen |
| Wetter "keine Daten" | Serielle Ausgabe lesen (HTTP-Code), Koordinaten pruefen |
| Dimmen aendert nur Farben | Erwartet ohne Backlight-Umbau, siehe Abschnitt Dimmung |
| Bootschleife | GPIO2/8/9 nicht belegen, Flash Mode DIO, Datenkabel verwenden |


## Flugradar (Design 19)

Zeigt echte Flugzeuge rund um den Standort (Breiten-/Laengengrad im Reiter "Wetter"; der Radius steht
direkt darunter, Standard 25 km). Norden ist oben, der Standort liegt in der Mitte.

- **Daten:** Offene ADS-B-Dienste, die der Reihe nach probiert werden: adsb.lol, airplanes.live, adsb.fi
  (nur private, nicht kommerzielle Nutzung; Quellenangabe: adsb.lol, airplanes.live, adsb.fi). Es wird kein Konto
  und kein Schluessel gebraucht. Bei Ausfall eines Dienstes springt die Uhr automatisch zum naechsten.
- **Abruf:** alle 10 Sekunden, aber nur solange das Design angezeigt wird (auch bei der automatischen Rotation).
  Zwischen zwei Abrufen werden die Positionen anhand von Kurs und Geschwindigkeit weitergerechnet, damit die Flugzeuge
  fluessig fliegen. Meldungen im Schirm: "LADE...", "KEIN WLAN", "KEINE DATEN", "KEINE FLUGZEUGE", "VERALTET".
- **Darstellung:** Pfeil = Flugzeug in Kursrichtung mit kurzer Spur; Farben: grau = am Boden, cyan unter 3000 ft,
  gruen unter 10000 ft, gelb unter 25000 ft, orange darueber. Das markierte Flugzeug (weisser Ring, Rufzeichen)
  wechselt alle fuenf Sekunden; rechts steht seine Infotafel: Rufzeichen, Typ, Hoehe (FL = Flugflaeche in 100 ft),
  Steigen/Sinken als Pfeil, Geschwindigkeit in Knoten und Entfernung in km.
- **Hinweise:** Nicht jedes Flugzeug sendet ADS-B (z. B. manche Militaermaschinen), und sehr tiefe Flugzeuge
  werden von Empfaengern in der Naehe manchmal nicht erfasst. Wichtig ist ein richtig eingetragener Standort.
  Die Daten sind eine Hobby-Anzeige und keine Flugsicherungs-Information.
