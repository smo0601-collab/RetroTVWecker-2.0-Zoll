# Kurzanleitung WetterUhr / RetroCYD als PDF (reportlab)
from reportlab.lib.pagesizes import A4
from reportlab.lib.units import mm
from reportlab.lib import colors
from reportlab.lib.styles import ParagraphStyle
from reportlab.platypus import (BaseDocTemplate, PageTemplate, Frame, Paragraph, Spacer, Table, TableStyle,
                                KeepTogether)
import sys
OUT = sys.argv[1]
VER = sys.argv[2]

ACC = colors.HexColor('#1f6f5c'); ACC2 = colors.HexColor('#e8f2ef'); INK = colors.HexColor('#1d2327'); MUT = colors.HexColor('#5b6670')
base = ParagraphStyle('b', fontName='Helvetica', fontSize=8.8, leading=11.6, textColor=INK, spaceAfter=3)
small = ParagraphStyle('s', parent=base, fontSize=7.8, leading=10, textColor=MUT)
h1 = ParagraphStyle('h1', parent=base, fontName='Helvetica-Bold', fontSize=20, leading=24, textColor=ACC, spaceAfter=2)
sub = ParagraphStyle('sub', parent=base, fontSize=10, leading=13, textColor=MUT, spaceAfter=8)
h2 = ParagraphStyle('h2', parent=base, fontName='Helvetica-Bold', fontSize=11.5, leading=14, textColor=ACC, spaceBefore=8, spaceAfter=4, keepWithNext=1)
cell = ParagraphStyle('c', parent=base, fontSize=8.3, leading=10.6, spaceAfter=0)
cellb = ParagraphStyle('cb', parent=cell, fontName='Helvetica-Bold')
num = ParagraphStyle('n', parent=base, leftIndent=11, bulletIndent=0)

def P(t, s=base): return Paragraph(t, s)
def steps(items):
    return [Paragraph(t, num, bulletText=f'{i}.') for i, t in enumerate(items, 1)]
def bullets(items):
    return [Paragraph(t, num, bulletText='•') for t in items]
def table(rows, widths, head=True):
    data = [[Paragraph(c, cellb if (head and r == 0) else cell) for c in row] for r, row in enumerate(rows)]
    t = Table(data, colWidths=widths, hAlign='LEFT')
    st = [('VALIGN', (0, 0), (-1, -1), 'TOP'), ('TOPPADDING', (0, 0), (-1, -1), 3), ('BOTTOMPADDING', (0, 0), (-1, -1), 3),
          ('LEFTPADDING', (0, 0), (-1, -1), 4), ('RIGHTPADDING', (0, 0), (-1, -1), 4),
          ('LINEBELOW', (0, 0), (-1, -1), 0.4, colors.HexColor('#d5dde0'))]
    if head: st += [('BACKGROUND', (0, 0), (-1, 0), ACC2), ('LINEBELOW', (0, 0), (-1, 0), 0.8, ACC)]
    t.setStyle(TableStyle(st)); return t
def box(flow, color=ACC2):
    t = Table([[flow]], colWidths=[174 * mm]); t.setStyle(TableStyle([('BACKGROUND', (0, 0), (-1, -1), color),
        ('LEFTPADDING', (0, 0), (-1, -1), 7), ('RIGHTPADDING', (0, 0), (-1, -1), 7), ('TOPPADDING', (0, 0), (-1, -1), 5),
        ('BOTTOMPADDING', (0, 0), (-1, -1), 5)])); return t

W = 174 * mm
s = []
s.append(P('Wetteruhr &amp; RetroCYD', h1))
s.append(P(f'Kurzanleitung &middot; Firmware {VER} &middot; CYD 2,8&quot; (ESP32-2432S028R) und ESP32-C3 mit 2,0&quot;-Display', sub))
s.append(P('Uhr mit Wetter, 29 Designs, Kalender, Wecker und Kl&auml;ngen. Alles wird im Browser eingestellt. Auf dem CYD '
           'liegt daneben die Spielekonsole <b>RetroCYD</b> (20 Spiele). Ausf&uuml;hrlich: README im GitHub-Projekt '
           '<b>smo0601-collab/RetroTVWecker-2.0-Zoll</b>.'))

s.append(P('1 &nbsp;Firmware zum ersten Mal aufspielen (USB)', h2))
s += steps([
  'Chrome oder Edge: <b>espressif.github.io/esptool-js</b> &ouml;ffnen, Ger&auml;t per USB-<i>Daten</i>kabel anschlie&szlig;en, <b>Connect</b>.',
  'Adresse <b>0x0</b>, Datei w&auml;hlen, <b>Program</b>:<br/>'
  '&nbsp;&nbsp;CYD mit Spielen: <font face="Courier">WetterUhr_&lt;Version&gt;_CYD_mitSpielen_erstinstallation.bin</font><br/>'
  '&nbsp;&nbsp;CYD nur Uhr: <font face="Courier">WetterUhr_&lt;Version&gt;_CYD_erstinstallation.bin</font><br/>'
  '&nbsp;&nbsp;ESP32-C3: <font face="Courier">WetterUhr_&lt;Version&gt;_erstinstallation.bin</font>',
  'Klappt die Verbindung nicht: USB-Treiber f&uuml;r CH340 (CYD) installieren; beim C3 BOOT halten, RESET dr&uuml;cken, loslassen.'])
s.append(P('Die Dateien liegen auf GitHub unter <b>releases/&lt;Version&gt;</b> bzw. bei den Releases (Assets).', small))

s.append(P('2 &nbsp;Einrichten', h2))
s += steps([
  'Ohne WLAN-Daten &ouml;ffnet die Uhr das WLAN <b>Wetteruhr-Setup</b> (Passwort <b>wetteruhr1</b>). Damit verbinden.',
  'Im Browser <b>http://192.168.4.1</b> &ouml;ffnen, unter <b>WLAN</b> Netz suchen und Passwort eintragen, <b>Speichern</b>. Die Uhr startet neu.',
  'Danach ist sie unter der IP-Adresse aus der Statusleiste oder <b>http://wetteruhr.local</b> erreichbar.',
  '<b>Zeit &amp; Wetter</b>: Ort oder Postleitzahl suchen, Treffer antippen, Zeitzone pr&uuml;fen, <b>Speichern</b>.',
  '<b>System</b>: ein <b>Web-Passwort</b> setzen (Benutzer <i>admin</i>). Ohne Passwort kann jeder im WLAN die Uhr bedienen.'])
s.append(P('F&auml;llt das WLAN l&auml;nger als 60 s aus, &ouml;ffnet sich <i>Wetteruhr-Setup</i> wieder und schlie&szlig;t sich nach der Wiederverbindung.', small))

s.append(P('3 &nbsp;Bedienung am Ger&auml;t', h2))
s.append(table([
  ['Geste', 'CYD (ganze Bildfl&auml;che)', 'ESP32-C3 (Touch-Taster TTP223)'],
  ['kurz tippen', 'n&auml;chstes Design aus der Wechsel-Liste', 'n&auml;chstes Design'],
  ['doppelt tippen', 'Laufschrift', 'Laufschrift'],
  ['1 s halten', 'Men&uuml;: <b>Spiele</b>, <b>Standby</b>, <b>Zur&uuml;ck</b>', 'Standby an/aus'],
  ['im Standby', 'jede Ber&uuml;hrung weckt', 'jede Ber&uuml;hrung weckt'],
  ['Wecker klingelt', 'jede Ber&uuml;hrung stellt ihn ab', 'jede Ber&uuml;hrung stellt ihn ab'],
], [32 * mm, 71 * mm, 71 * mm]))
s.append(P('Resistiver Touch: mit dem Finger kurz etwas fester tippen. Nach dem Einschalten fragt das CYD <b>Uhr</b> oder <b>Spiele</b> (ohne Wahl nach 6 s die Uhr).', small))

s.append(P('4 &nbsp;Die Browser-Seite', h2))
s.append(table([
  ['Reiter', 'Was man dort findet'],
  ['Status', 'WLAN, Uhrzeit, Wetter, Design, Speicher, Firmware, <b>Letzter Neustart</b>, <b>Absturz-Spur</b>, Link zum <b>Protokoll</b>'],
  ['Anzeige', 'Design (Vorschaukarten), Farbe, automatischer Design-Wechsel, Extras, Sprache, Sekunden, Ausrichtung (180&deg;)'],
  ['Helligkeit', 'Helligkeit, Display an/aus, Zeitplan-Dimmung mit Nachtmodus, RGB-LED (CYD)'],
  ['Kalender', 'Bundesland, Feiertage, besondere Tage, Schulferien, Einblendungen und Laufschrift, Jahres&uuml;bersicht'],
  ['WLAN', 'Netz suchen, Passwort'],
  ['Summer &amp; Wecker', 'Wecker mit Wochentagen, Stundensignal; CYD: Lautst&auml;rke, Weckton, Stundengong, Klang-Effekte &ndash; jeder Klang mit Knopf zum Anh&ouml;ren'],
  ['Zeit &amp; Wetter', 'Ortssuche, Koordinaten, Zeitzone, Zeitserver, Uhrzeit von Hand, Wetter-Intervall'],
  ['System', 'Ger&auml;tename, Web-Passwort, <b>Firmware-Update</b>, Display-Anpassung, Spiele (RetroCYD), Werkseinstellungen'],
], [32 * mm, 142 * mm]))
s.append(P('&bdquo;Speichern&ldquo; &uuml;bernimmt alle Reiter auf einmal. Design, Farbe und Regler wirken sofort.', small))

s.append(P('5 &nbsp;Update &uuml;ber WLAN', h2))
s.append(P('Browser &rarr; <b>System &rarr; Firmware-Update</b> &rarr; Datei w&auml;hlen. Die Einstellungen bleiben erhalten. Die Uhr pr&uuml;ft die Datei und lehnt falsche ab.'))
s.append(table([
  ['Ger&auml;t', 'Datei', 'Hinweis'],
  ['CYD: Spiele', '<font face="Courier">RetroCYD_&lt;Version&gt;_update.bin</font>', 'bei neuer RetroCYD-Version <b>zuerst</b> einspielen'],
  ['CYD: Uhr', '<font face="Courier">WetterUhr_&lt;Version&gt;_CYD_update.bin</font>', 'mit Spielen: RetroCYD &uuml;bernimmt kurz (&bdquo;UHR-UPDATE&ldquo;), ca. 30 s, Seite offen lassen'],
  ['ESP32-C3', '<font face="Courier">WetterUhr_&lt;Version&gt;_update.bin</font>', 'Datei ohne &bdquo;CYD&ldquo; im Namen'],
], [24 * mm, 74 * mm, 76 * mm]))
s.append(P('Nie die <i>_erstinstallation.bin</i> &uuml;ber WLAN einspielen &ndash; die ist nur f&uuml;r USB an Adresse 0x0.', small))

s.append(P('6 &nbsp;Spiele (RetroCYD, nur CYD)', h2))
s += bullets([
  'Starten: in der Uhr 1 s halten &rarr; <b>Spiele</b>, oder beim Einschalten <b>Spiele</b> antippen.',
  '20 Spiele in 6 Kategorien: Klassiker, Shooter, Jump &amp; Run, Rennen, Denkspiele, Brett &amp; Karten. Steuerung per Touch oder Xbox-Controller (Bluetooth).',
  'In RetroCYD 1,5 s halten (Bildschirm oder BOOT): <b>ZUR UHR</b>, <b>SETUP / WLAN</b>, <b>ZUR&Uuml;CK</b>. Tippen = n&auml;chster Punkt, halten = w&auml;hlen.',
  'Ton, LED-Effekte, Highscores und Updates: im RetroCYD-Setup im Browser. W&auml;hrend gespielt wird, l&auml;uft die Uhr nicht (kein Wecker).'])

s.append(P('7 &nbsp;Ton (CYD)', h2))
s.append(P('Kleinen Lautsprecher (8 &Omega;, 0,5&ndash;2 W, Stecker JST 1,25 mm) an <b>SPEAK</b> stecken. Lautst&auml;rke, Weckton (9 Kl&auml;nge), Stundengong (6) und '
           'Klang-Effekte unter <b>Summer &amp; Wecker</b>. Brummt es in Ruhe: Haken &bdquo;Lautsprecher-Ausgang in Ruhe abschalten&ldquo; gesetzt lassen.'))

s.append(P('8 &nbsp;Wenn etwas nicht klappt', h2))
s.append(table([
  ['Problem', 'Abhilfe'],
  ['Seite nicht erreichbar', 'IP aus der Statusleiste nehmen, gleiches WLAN (2,4 GHz). Kurz ein anderes Design antippen; sonst Strom kurz trennen.'],
  ['Wetter &bdquo;noch keine Daten&ldquo;', 'Status &rarr; Wetter zeigt den letzten Fehler. Ort/Koordinaten pr&uuml;fen. Nach dem Start 1&ndash;2 Minuten warten.'],
  ['Uhr startet neu', 'Status &rarr; <b>Letzter Neustart</b> und <b>Absturz-Spur</b> ansehen; <b>Protokoll</b> unter <font face="Courier">http://&lt;IP&gt;/log</font> zeigt auch die Sitzung vor dem Neustart.'],
  ['Bild gedreht, gespiegelt, falsche Farben', 'CYD: System &rarr; Display-Anpassung. Nur auf dem Kopf: Anzeige &rarr; Ausrichtung &bdquo;180 Grad&ldquo;.'],
  ['Touch trifft nicht (CYD)', 'System &rarr; Spiele (RetroCYD) &rarr; <b>Touch kalibrieren</b>.'],
  ['Update abgelehnt', 'Passende Datei nehmen (CYD/C3, <i>_update.bin</i>); mit Spielen zuerst RetroCYD aktualisieren.'],
  ['Web-Passwort vergessen', 'Per USB neu aufspielen (Abschnitt 1); dabei gehen die Einstellungen verloren.'],
], [44 * mm, 130 * mm]))
s.append(Spacer(1, 6))
s.append(box(P('Fehler melden: Status-Seite (Letzter Neustart, Absturz-Spur, freier Speicher) und den Text von '
               '<font face="Courier">/log</font> mitschicken &ndash; damit l&auml;sst sich die Ursache meist direkt finden.', base)))

def deco(c, d):
    c.saveState()
    c.setFillColor(ACC); c.rect(0, A4[1] - 6 * mm, A4[0], 6 * mm, stroke=0, fill=1)
    c.setFont('Helvetica', 7.5); c.setFillColor(MUT)
    c.drawString(18 * mm, 10 * mm, f'Wetteruhr & RetroCYD - Kurzanleitung - Firmware {VER}')
    c.drawRightString(A4[0] - 18 * mm, 10 * mm, f'Seite {d.page}')
    c.restoreState()

doc = BaseDocTemplate(OUT, pagesize=A4, leftMargin=18 * mm, rightMargin=18 * mm, topMargin=16 * mm, bottomMargin=17 * mm,
                      title='Wetteruhr & RetroCYD - Kurzanleitung', author='smo0601-collab', subject=f'Firmware {VER}')
doc.addPageTemplates([PageTemplate(id='p', frames=[Frame(doc.leftMargin, doc.bottomMargin, doc.width, doc.height, id='f')], onPage=deco)])
doc.build(s)
