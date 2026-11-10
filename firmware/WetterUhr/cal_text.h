#pragma once
/*
  cal_text.h - Texte fuer den Kalender (cal.h): Namen der Feier- und besonderen Tage (5 Sprachen, UTF-8)
  und lustige Sprueche fuer Laufschrift und Einblendung (Deutsch und Englisch; FR/ES/IT nehmen Englisch).
  Auf dem Display (Pixelschrift) werden Umlaute umgeschrieben (calAscii), die Laufschrift kann sie.
*/

// Reihenfolge = enum CalId in cal.h
static const char* const CAL_NAME[][5] = {
  // gesetzliche Feiertage
  {"Neujahr", "New Year's Day", "Nouvel An", "Año Nuevo", "Capodanno"},
  {"Heilige Drei Könige", "Epiphany", "Épiphanie", "Reyes Magos", "Epifania"},
  {"Frauentag", "Women's Day", "Journée de la femme", "Día de la Mujer", "Festa della donna"},
  {"Karfreitag", "Good Friday", "Vendredi saint", "Viernes Santo", "Venerdì santo"},
  {"Ostersonntag", "Easter Sunday", "Pâques", "Domingo de Pascua", "Pasqua"},
  {"Ostermontag", "Easter Monday", "Lundi de Pâques", "Lunes de Pascua", "Pasquetta"},
  {"Tag der Arbeit", "Labour Day", "Fête du Travail", "Día del Trabajo", "Festa del lavoro"},
  {"Christi Himmelfahrt", "Ascension Day", "Ascension", "Ascensión", "Ascensione"},
  {"Pfingstsonntag", "Whit Sunday", "Pentecôte", "Pentecostés", "Pentecoste"},
  {"Pfingstmontag", "Whit Monday", "Lundi de Pentecôte", "Lunes de Pentecostés", "Lunedì di Pentecoste"},
  {"Fronleichnam", "Corpus Christi", "Fête-Dieu", "Corpus Christi", "Corpus Domini"},
  {"Mariä Himmelfahrt", "Assumption Day", "Assomption", "Asunción", "Ferragosto"},
  {"Weltkindertag", "Children's Day", "Journée des enfants", "Día del Niño", "Giornata dei bambini"},
  {"Tag der Deutschen Einheit", "German Unity Day", "Unité allemande", "Unidad Alemana", "Unità tedesca"},
  {"Reformationstag", "Reformation Day", "Fête de la Réforme", "Día de la Reforma", "Festa della Riforma"},
  {"Allerheiligen", "All Saints' Day", "Toussaint", "Todos los Santos", "Ognissanti"},
  {"Buß- und Bettag", "Day of Repentance", "Jour de pénitence", "Día de penitencia", "Giorno di penitenza"},
  {"1. Weihnachtstag", "Christmas Day", "Noël", "Navidad", "Natale"},
  {"2. Weihnachtstag", "Boxing Day", "Lendemain de Noël", "San Esteban", "Santo Stefano"},
  // besondere Tage
  {"Valentinstag", "Valentine's Day", "Saint-Valentin", "San Valentín", "San Valentino"},
  {"Weiberfastnacht", "Women's Carnival", "Jeudi gras", "Jueves de Carnaval", "Giovedì grasso"},
  {"Rosenmontag", "Carnival Monday", "Lundi des roses", "Lunes de Carnaval", "Lunedì di Carnevale"},
  {"Faschingsdienstag", "Shrove Tuesday", "Mardi gras", "Martes de Carnaval", "Martedì grasso"},
  {"Muttertag", "Mother's Day", "Fête des mères", "Día de la Madre", "Festa della mamma"},
  {"Vatertag", "Father's Day", "Fête des pères", "Día del Padre", "Festa del papà"},
  {"Erntedankfest", "Harvest Festival", "Fête des récoltes", "Fiesta de la cosecha", "Festa del raccolto"},
  {"Halloween", "Halloween", "Halloween", "Halloween", "Halloween"},
  {"Sankt Martin", "St. Martin's Day", "Saint-Martin", "San Martín", "San Martino"},
  {"Nikolaus", "St. Nicholas' Day", "Saint-Nicolas", "San Nicolás", "San Nicola"},
  {"1. Advent", "1st Advent", "1er dimanche de l'Avent", "1.º domingo de Adviento", "1ª domenica di Avvento"},
  {"2. Advent", "2nd Advent", "2e dimanche de l'Avent", "2.º domingo de Adviento", "2ª domenica di Avvento"},
  {"3. Advent", "3rd Advent", "3e dimanche de l'Avent", "3.º domingo de Adviento", "3ª domenica di Avvento"},
  {"4. Advent", "4th Advent", "4e dimanche de l'Avent", "4.º domingo de Adviento", "4ª domenica di Avvento"},
  {"Heiligabend", "Christmas Eve", "Réveillon de Noël", "Nochebuena", "Vigilia di Natale"},
  {"Silvester", "New Year's Eve", "Saint-Sylvestre", "Nochevieja", "San Silvestro"},
  // Schule
  {"Ferienbeginn", "Holidays begin", "Début des vacances", "Empiezan las vacaciones", "Iniziano le vacanze"},
  {"Letzter Schultag", "Last school day", "Dernier jour d'école", "Último día de clase", "Ultimo giorno di scuola"},
};

// Je Tag zwei Sprueche (abwechselnd); [0] Deutsch, [1] Englisch
static const char* const CAL_SAY[][2][2] = {
  {{"Frohes neues Jahr! Gute Vorsätze? Ab morgen!", "Prosit Neujahr - 365 frische Tage liegen vor uns!"},
   {"Happy New Year! Resolutions start tomorrow!", "Cheers - 365 fresh days ahead!"}},
  {{"Die Sternsinger sind unterwegs: C+M+B - Christus segne dieses Haus!", "Kaspar, Melchior und Balthasar bringen Gold, Weihrauch und Myrrhe."},
   {"Star singers on tour: C+M+B - bless this house!", "Three kings bring gold, frankincense and myrrh."}},
  {{"Heute ist Frauentag - Blumen für alle Heldinnen des Alltags!", "Danke an alle starken Frauen!"},
   {"Women's Day - flowers for all everyday heroines!", "Thank you to all strong women!"}},
  {{"Karfreitag - heute gibt es traditionell Fisch.", "Stiller Feiertag: Zeit zum Durchatmen."},
   {"Good Friday - fish is on the menu today.", "A quiet holiday: time to breathe."}},
  {{"Frohe Ostern! Der Osterhase war fleißig - ab auf Eiersuche!", "Wer findet das goldene Ei?"},
   {"Happy Easter! The bunny was busy - go egg hunting!", "Who finds the golden egg?"}},
  {{"Ostermontag - noch Schokoeier übrig?", "Eierlauf und Osterspaziergang!"},
   {"Easter Monday - any chocolate eggs left?", "Egg race and Easter walk!"}},
  {{"Tag der Arbeit - und heute wird mal nicht gearbeitet!", "Der Maibaum steht - Tanz in den Mai!"},
   {"Labour Day - and today nobody works!", "The maypole is up - dance into May!"}},
  {{"Christi Himmelfahrt - ab in die Natur!", "Brückentag morgen? Lang lebe das lange Wochenende!"},
   {"Ascension Day - off into the countryside!", "Bridge day tomorrow? Long live the long weekend!"}},
  {{"Frohe Pfingsten! Der Pfingstochse ist geschmückt.", "Pfingsten: Zeit für einen Ausflug ins Grüne."},
   {"Happy Pentecost! The Whitsun ox is decorated.", "Whitsun: time for a trip to the countryside."}},
  {{"Pfingstmontag - noch ein freier Tag, herrlich!", "Pfingstmontag: Fahrrad raus!"},
   {"Whit Monday - one more day off, lovely!", "Whit Monday: get the bike out!"}},
  {{"Fronleichnam - Blumenteppiche und Prozessionen.", "Feiertag mitten in der Woche - wer nimmt den Brückentag?"},
   {"Corpus Christi - flower carpets and processions.", "A holiday mid-week - who takes the bridge day?"}},
  {{"Mariä Himmelfahrt - Zeit, Kräuterbuschen zu binden!", "Sommerfeiertag: ab an den See!"},
   {"Assumption Day - time to tie herb bouquets!", "Summer holiday: off to the lake!"}},
  {{"Weltkindertag - heute bestimmen die Kinder!", "Luftballons für alle Kinder!"},
   {"Children's Day - today the kids are in charge!", "Balloons for all children!"}},
  {{"Tag der Deutschen Einheit - Einigkeit und Recht und Freiheit!", "Seit 1990 wieder ein Land - Glückwunsch!"},
   {"German Unity Day - unity, justice and freedom!", "One country again since 1990 - congratulations!"}},
  {{"Reformationstag - Luther hämmert seine 95 Thesen an die Tür!", "1517 in Wittenberg: der Hammer schlägt Geschichte."},
   {"Reformation Day - Luther nails his 95 theses to the door!", "Wittenberg 1517: the hammer makes history."}},
  {{"Allerheiligen - Lichter für die Lieben.", "Ein stiller Tag zum Erinnern."},
   {"All Saints' Day - lights for our loved ones.", "A quiet day to remember."}},
  {{"Buß- und Bettag - einmal tief durchatmen.", "Ein Tag zum Nachdenken - und zum Ausschlafen."},
   {"Day of Repentance - take a deep breath.", "A day to reflect - and to sleep in."}},
  {{"Frohe Weihnachten! Gans im Ofen, Plätzchen auf dem Teller.", "Fröhliche Weihnacht überall!"},
   {"Merry Christmas! Goose in the oven, cookies on the plate.", "Merry Christmas everywhere!"}},
  {{"Zweiter Weihnachtstag - Reste essen ist auch ein Fest!", "Noch Platz für ein Plätzchen?"},
   {"Boxing Day - leftovers are a feast too!", "Still room for one more cookie?"}},
  {{"Happy Valentinstag - Amor hat gezielt!", "Rosen sind rot, Veilchen sind blau ..."},
   {"Happy Valentine's Day - Cupid took aim!", "Roses are red, violets are blue ..."}},
  {{"Weiberfastnacht - Krawatten in Gefahr!", "Alaaf! Heute regieren die Frauen."},
   {"Women's Carnival - ties beware!", "Alaaf! Today the women rule."}},
  {{"Helau und Alaaf! Kamelle!", "Rosenmontag - Konfetti überall!"},
   {"Helau and Alaaf! Throw the sweets!", "Carnival Monday - confetti everywhere!"}},
  {{"Faschingsdienstag - Krapfen essen, Narren feiern!", "Morgen ist alles vorbei - heute noch mal Helau!"},
   {"Shrove Tuesday - doughnuts and fools!", "Tomorrow it's over - one more Helau today!"}},
  {{"Alles Liebe zum Muttertag - danke, Mama!", "Blumen, Frühstück ans Bett und eine dicke Umarmung!"},
   {"Happy Mother's Day - thanks, Mum!", "Flowers, breakfast in bed and a big hug!"}},
  {{"Vatertag! Bollerwagen voll, Papa vorneweg.", "Alles Gute zum Vatertag - Grill an!"},
   {"Father's Day! Handcart packed, dad in front.", "Happy Father's Day - fire up the grill!"}},
  {{"Erntedank - danke für Kürbis, Korn und Äpfel!", "Die Ernte ist eingefahren!"},
   {"Harvest festival - thanks for pumpkins, grain and apples!", "The harvest is in!"}},
  {{"Süßes oder Saures!", "Buuuh! Die Kürbisse grinsen schon."},
   {"Trick or treat!", "Boooo! The pumpkins are grinning."}},
  {{"Sankt Martin - Laternen leuchten, die Martinsgans schnattert!", "Martin teilt seinen Mantel - und wir die Weckmänner."},
   {"St. Martin's Day - lanterns glow, the goose cackles!", "Martin shares his cloak - we share the sweet bread."}},
  {{"Nikolaus war da - Stiefel schon kontrolliert?", "Ho ho ho! Warst du auch brav?"},
   {"St. Nicholas came - checked your boots yet?", "Ho ho ho! Have you been good?"}},
  {{"Erster Advent - das erste Lichtlein brennt!", "Zeit für Plätzchen und Kerzenschein."},
   {"First Advent - the first candle is lit!", "Time for cookies and candlelight."}},
  {{"Zweiter Advent - zwei Kerzen brennen!", "Plätzchen backen, Wunschzettel schreiben!"},
   {"Second Advent - two candles burning!", "Bake cookies, write wish lists!"}},
  {{"Dritter Advent - drei Kerzen brennen!", "Glühwein und Weihnachtsmarkt!"},
   {"Third Advent - three candles burning!", "Mulled wine and Christmas market!"}},
  {{"Vierter Advent - bald ist Weihnachten!", "Alle vier Kerzen brennen - Geschenke schon eingepackt?"},
   {"Fourth Advent - Christmas is near!", "All four candles burn - presents wrapped yet?"}},
  {{"Heiligabend - gleich kommt das Christkind!", "Stille Nacht ... und dann Bescherung!"},
   {"Christmas Eve - Santa is on his way!", "Silent night ... then presents!"}},
  {{"Guten Rutsch! Bleigießen, Raketen und Sekt!", "Heute Nacht wird der Himmel bunt!"},
   {"Happy New Year's Eve! Rockets and bubbly!", "Tonight the sky turns colourful!"}},
  {{"Ferien! Schulranzen in die Ecke!", "Endlich frei - Ferien, wir kommen!"},
   {"Holidays! Throw the school bag in the corner!", "Finally free - here we come!"}},
  {{"Letzter Schultag - morgen beginnen die Ferien!", "Zeugnis abholen und ab in die Ferien!"},
   {"Last school day - holidays start tomorrow!", "Grab your report and off you go!"}},
};

// Ferien-Arten (aus dem Namen der OpenHolidays-Daten erkannt), 5 Sprachen
static const char* const FER_NAME[][5] = {
  {"Ferien", "Holidays", "Vacances", "Vacaciones", "Vacanze"},
  {"Winterferien", "Winter holidays", "Vacances d'hiver", "Vacaciones de invierno", "Vacanze invernali"},
  {"Osterferien", "Easter holidays", "Vacances de Pâques", "Vacaciones de Pascua", "Vacanze di Pasqua"},
  {"Pfingstferien", "Whitsun holidays", "Vacances de Pentecôte", "Vacaciones de Pentecostés", "Vacanze di Pentecoste"},
  {"Sommerferien", "Summer holidays", "Vacances d'été", "Vacaciones de verano", "Vacanze estive"},
  {"Herbstferien", "Autumn holidays", "Vacances d'automne", "Vacaciones de otoño", "Vacanze autunnali"},
  {"Weihnachtsferien", "Christmas holidays", "Vacances de Noël", "Vacaciones de Navidad", "Vacanze di Natale"},
  {"Frühjahrsferien", "Spring holidays", "Vacances de printemps", "Vacaciones de primavera", "Vacanze di primavera"},
};
// Schluesselwort im deutschen Namen -> Art
static const char* const FER_KEY[] = {"", "Winter", "Oster", "Pfingst", "Sommer", "Herbst", "Weihnacht", "Frühjahr"};

// Laufschrift-Bausteine (5 Sprachen)
static const char* const CAL_T_TODAY[5]   = {"Heute: %s", "Today: %s", "Aujourd'hui : %s", "Hoy: %s", "Oggi: %s"};
static const char* const CAL_T_TOMORROW[5] = {"Morgen ist %s", "Tomorrow is %s", "Demain : %s", "Mañana es %s", "Domani: %s"};
static const char* const CAL_T_DAYS[5]    = {"Noch %d Tage bis %s", "%d days until %s", "Encore %d jours avant : %s", "Faltan %d días para %s", "Mancano %d giorni: %s"};
static const char* const CAL_T_FERIN[5]   = {"Noch %d Tage bis zu den %s", "%d days until the %s", "Encore %d jours avant les %s", "Faltan %d días para las %s", "Mancano %d giorni alle %s"};
static const char* const CAL_T_FERNOW[5]  = {"%s: noch %d Tage frei!", "%s: %d more days off!", "%s : encore %d jours de libre !", "%s: ¡%d días más libres!", "%s: ancora %d giorni liberi!"};
static const char* const CAL_T_FERLAST[5] = {"%s: heute ist der letzte freie Tag", "%s: last day off today", "%s : dernier jour de libre", "%s: hoy es el último día libre", "%s: oggi è l'ultimo giorno libero"};
static const char* const CAL_T_TOMFER[5]  = {"Morgen beginnen die %s!", "The %s start tomorrow!", "Les %s commencent demain !", "¡Mañana empiezan las %s!", "Domani iniziano le %s!"};

// Fuer das Kalender-Design (klassische Schrift ohne Umlaute)
static const char* const CAL_MON[5][12] = {
  {"Januar", "Februar", "Maerz", "April", "Mai", "Juni", "Juli", "August", "September", "Oktober", "November", "Dezember"},
  {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"},
  {"Janvier", "Fevrier", "Mars", "Avril", "Mai", "Juin", "Juillet", "Aout", "Septembre", "Octobre", "Novembre", "Decembre"},
  {"Enero", "Febrero", "Marzo", "Abril", "Mayo", "Junio", "Julio", "Agosto", "Septiembre", "Octubre", "Noviembre", "Diciembre"},
  {"Gennaio", "Febbraio", "Marzo", "Aprile", "Maggio", "Giugno", "Luglio", "Agosto", "Settembre", "Ottobre", "Novembre", "Dicembre"},
};
static const char* const CAL_L_TODAY[5] = {"HEUTE", "TODAY", "AUJOURD'HUI", "HOY", "OGGI"};
static const char* const CAL_L_NEXT[5]  = {"DEMNAECHST", "COMING UP", "PROCHAINEMENT", "PROXIMAMENTE", "PROSSIMAMENTE"};
static const char* const CAL_L_FER[5]   = {"SCHULFERIEN", "SCHOOL HOLIDAYS", "VACANCES SCOLAIRES", "VACACIONES", "VACANZE"};
static const char* const CAL_L_IN[5]    = {"in %d Tagen", "in %d days", "dans %d jours", "en %d dias", "tra %d giorni"};
static const char* const CAL_L_TOM[5]   = {"morgen", "tomorrow", "demain", "manana", "domani"};
static const char* const CAL_L_LEFT[5]  = {"noch %d Tage", "%d days left", "encore %d jours", "quedan %d dias", "ancora %d giorni"};
static const char* const CAL_L_LAST[5]  = {"letzter Tag", "last day", "dernier jour", "ultimo dia", "ultimo giorno"};
static const char* const CAL_L_NOBL[5]  = {"Bundesland im Browser waehlen", "Choose state in browser", "Choisir le Land", "Elegir estado", "Scegli il Land"};
static const char* const CAL_L_NOFER[5] = {"keine Daten", "no data", "pas de donnees", "sin datos", "nessun dato"};
// Kurzform fuer das Kalender-Design (unter der Ueberschrift "SCHULFERIEN"), ohne Umlaute
static const char* const FER_SHORT[][5] = {
  {"Ferien", "Holidays", "Vacances", "Vacaciones", "Vacanze"},
  {"Winter", "Winter", "Hiver", "Invierno", "Inverno"},
  {"Ostern", "Easter", "Paques", "Pascua", "Pasqua"},
  {"Pfingsten", "Whitsun", "Pentecote", "Pentecostes", "Pentecoste"},
  {"Sommer", "Summer", "Ete", "Verano", "Estate"},
  {"Herbst", "Autumn", "Automne", "Otono", "Autunno"},
  {"Weihnachten", "Christmas", "Noel", "Navidad", "Natale"},
  {"Fruehjahr", "Spring", "Printemps", "Primavera", "Primavera"},
};
