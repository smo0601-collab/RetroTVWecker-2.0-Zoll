#pragma once
/*
  lang_web.h - Texte der Browser-Seite in fuenf Sprachen (UTF-8, HTML erlaubt).
  Spalten: Kennung, Deutsch, Englisch, Franzoesisch, Spanisch, Italienisch. Wird von lang.h eingebunden.
*/
#define LANG_WEB(X) \
  /* Navigation */ \
  X(W_TITLE,      "Wetteruhr", "Weather clock", "Horloge météo", "Reloj meteorológico", "Orologio meteo") \
  X(W_STATUS,     "Status", "Status", "État", "Estado", "Stato") \
  X(W_DISPLAY,    "Anzeige", "Display", "Affichage", "Pantalla", "Display") \
  X(W_BRIGHT,     "Helligkeit", "Brightness", "Luminosité", "Brillo", "Luminosità") \
  X(W_TICKER,     "Laufschrift", "Ticker", "Texte défilant", "Texto en movimiento", "Testo scorrevole") \
  X(W_SOUND,      "Summer &amp; Wecker", "Buzzer &amp; alarm", "Buzzer &amp; réveil", "Zumbador y alarma", "Cicalino e sveglia") \
  X(W_WIFI,       "WLAN", "Wi-Fi", "Wi-Fi", "Wi-Fi", "Wi-Fi") \
  X(W_TIMEWX,     "Zeit &amp; Wetter", "Time &amp; weather", "Heure &amp; météo", "Hora y tiempo", "Ora e meteo") \
  X(W_SYSTEM,     "System", "System", "Système", "Sistema", "Sistema") \
  X(W_SAVED,      "Gespeichert.", "Saved.", "Enregistré.", "Guardado.", "Salvato.") \
  X(W_NOPW,       "Kein Web-Passwort gesetzt: Jeder im Netzwerk kann diese Seite &ouml;ffnen. Bitte unter &bdquo;System&ldquo; ein Passwort vergeben.", \
                  "No web password set: anyone on the network can open this page. Please set a password under “System”.", \
                  "Aucun mot de passe web : tout le monde sur le réseau peut ouvrir cette page. Définissez un mot de passe sous « Système ».", \
                  "No hay contraseña web: cualquiera en la red puede abrir esta página. Define una contraseña en «Sistema».", \
                  "Nessuna password web: chiunque nella rete può aprire questa pagina. Imposta una password in «Sistema».") \
  X(W_SAVE,       "Speichern", "Save", "Enregistrer", "Guardar", "Salva") \
  /* Status */ \
  X(W_NOT_CONN,   "nicht verbunden", "not connected", "non connecté", "no conectado", "non connesso") \
  X(W_R_IP,       "IP-Adresse", "IP address", "Adresse IP", "Dirección IP", "Indirizzo IP") \
  X(W_R_AP,       "Einrichtungs-AP", "Setup access point", "Point d'accès de config.", "Punto de acceso de config.", "Access point di config.") \
  X(W_R_TIME,     "Uhrzeit", "Time", "Heure", "Hora", "Ora") \
  X(W_MANUAL_SET, "  (manuell gesetzt)", "  (set manually)", "  (réglée à la main)", "  (ajustada a mano)", "  (impostata a mano)") \
  X(W_NOT_SYNC,   "noch nicht synchronisiert", "not synchronised yet", "pas encore synchronisée", "aún no sincronizada", "non ancora sincronizzata") \
  X(W_NOT_SETT,   "nicht gesetzt (NTP aus, bitte unter Zeit &amp; Wetter eingeben)", "not set (NTP off, please enter it under Time &amp; weather)", \
                  "non réglée (NTP désactivé, à saisir sous Heure &amp; météo)", "sin ajustar (NTP apagado, introdúcela en Hora y tiempo)", "non impostata (NTP spento, inseriscila in Ora e meteo)") \
  X(W_R_TSRC,     "Zeitquelle", "Time source", "Source de l'heure", "Fuente de hora", "Fonte dell'ora") \
  X(W_ONLY_MAN,   "nur manuell", "manual only", "manuelle uniquement", "solo manual", "solo manuale") \
  X(W_R_WX,       "Wetter", "Weather", "Météo", "Tiempo", "Meteo") \
  X(W_WX_FMT,     "%.1f &deg;C, aktualisiert vor %lu min", "%.1f &deg;C, updated %lu min ago", "%.1f &deg;C, mise à jour il y a %lu min", "%.1f &deg;C, actualizado hace %lu min", "%.1f &deg;C, aggiornato %lu min fa") \
  X(W_NO_DATA,    "noch keine Daten", "no data yet", "pas encore de données", "aún sin datos", "ancora nessun dato") \
  X(W_R_DESIGN,   "Uhr-Design", "Clock design", "Design de l'horloge", "Diseño del reloj", "Design dell'orologio") \
  X(W_NYE_S,      " (Silvester-Automatik)", " (New Year's Eve automatic)", " (automatique Saint-Sylvestre)", " (automático Nochevieja)", " (automatico San Silvestro)") \
  X(W_NIGHT_S,    " (Nachtmodus)", " (night mode)", " (mode nuit)", " (modo noche)", " (modalità notte)") \
  X(W_AUTO_PAUSE, " (Wechsel aktiv, pausiert nach Auswahl noch %lu s)", " (rotation on, paused after selection for %lu s)", " (rotation active, en pause encore %lu s après le choix)", \
                  " (rotación activa, en pausa tras la selección %lu s)", " (rotazione attiva, in pausa dopo la scelta ancora %lu s)") \
  X(W_AUTO_ON,    " (Wechsel aktiv", " (rotation on", " (rotation active", " (rotación activa", " (rotazione attiva") \
  X(W_AUTO_NEXT,  ", n&auml;chster in %lu s)", ", next in %lu s)", ", suivant dans %lu s)", ", siguiente en %lu s)", ", prossimo tra %lu s)") \
  X(W_R_DIMSCH,   "Zeitplan-Dimmung", "Scheduled dimming", "Atténuation programmée", "Atenuación programada", "Attenuazione programmata") \
  X(W_DIM_ACT,    "aktiv: ", "active: ", "active : ", "activa: ", "attiva: ") \
  X(W_DIM_WAIT,   "wartet: ", "waiting: ", "en attente : ", "en espera: ", "in attesa: ") \
  X(W_DIM_FMT,    "%u %% von %s bis %s", "%u %% from %s to %s", "%u %% de %s à %s", "%u %% de %s a %s", "%u %% dalle %s alle %s") \
  X(W_WITH_TV,    ", mit Testbild", ", with test card", ", avec mire", ", con carta de ajuste", ", con monoscopio") \
  X(W_OFF,        "aus", "off", "désactivé", "apagado", "spento") \
  X(W_ON,         "an", "on", "allumé", "encendido", "acceso") \
  X(W_R_DIM,      "Dimmung", "Dimming", "Atténuation", "Atenuación", "Attenuazione") \
  X(W_DIM_HW,     "Hardware (PWM am Backlight)", "Hardware (PWM on the backlight)", "Matérielle (PWM du rétroéclairage)", "Hardware (PWM en la retroiluminación)", "Hardware (PWM sulla retroilluminazione)") \
  X(W_DIM_SW,     "Software (Farben abdunkeln, Backlight bleibt an)", "Software (colours dimmed, backlight stays on)", "Logicielle (couleurs assombries, rétroéclairage allumé)", \
                  "Software (colores atenuados, la retroiluminación sigue encendida)", "Software (colori scuriti, retroilluminazione accesa)") \
  X(W_R_DISP,     "Display", "Display", "Écran", "Pantalla", "Display") \
  X(W_STANDBY_S,  "Standby (aus)", "Standby (off)", "Veille (éteint)", "En espera (apagada)", "Standby (spento)") \
  X(W_R_TOUCH,    "Touch-Taster", "Touch button", "Bouton tactile", "Botón táctil", "Tasto touch") \
  X(W_T_LEVEL,    ", Pegel jetzt ", ", level now ", ", niveau actuel ", ", nivel ahora ", ", livello ora ") \
  X(W_TOUCHED,    " (ber&uuml;hrt)", " (touched)", " (touché)", " (tocado)", " (toccato)") \
  X(W_FREE,       " (frei)", " (released)", " (libre)", " (libre)", " (libero)") \
  X(W_DETECTED,   ", erkannt: ", ", detected: ", ", détecté : ", ", detectado: ", ", rilevato: ") \
  X(W_LAST_AGO,   ", zuletzt vor %lu s", ", last %lu s ago", ", dernier il y a %lu s", ", último hace %lu s", ", ultimo %lu s fa") \
  X(W_TOUCH_OFF,  "nicht aktiviert (PIN_TOUCH = -1 in config.h)", "not enabled (PIN_TOUCH = -1 in config.h)", "non activé (PIN_TOUCH = -1 dans config.h)", \
                  "no activado (PIN_TOUCH = -1 en config.h)", "non attivato (PIN_TOUCH = -1 in config.h)") \
  X(W_R_BUZZ,     "Summer", "Buzzer", "Buzzer", "Zumbador", "Cicalino") \
  X(W_BZ_NONE,    "nicht verbaut (PIN_BUZZER = -1)", "not fitted (PIN_BUZZER = -1)", "non installé (PIN_BUZZER = -1)", "no instalado (PIN_BUZZER = -1)", "non montato (PIN_BUZZER = -1)") \
  X(W_R_ALARM,    "Wecker", "Alarm", "Réveil", "Alarma", "Sveglia") \
  X(W_RINGING,    "klingelt", "ringing", "sonne", "sonando", "suona") \
  X(W_NO_DAY,     "  (kein Tag gew&auml;hlt)", "  (no day selected)", "  (aucun jour choisi)", "  (ningún día elegido)", "  (nessun giorno scelto)") \
  X(W_R_HEAP,     "Freier Speicher", "Free memory", "Mémoire libre", "Memoria libre", "Memoria libera") \
  X(W_R_FW,       "Firmware", "Firmware", "Micrologiciel", "Firmware", "Firmware") \
  X(W_BUILT,      "gebaut", "built", "compilé", "compilado", "compilato") \
  X(W_AL_STOP,    "Wecker abstellen", "Stop alarm", "Arrêter le réveil", "Parar la alarma", "Ferma la sveglia") \
  X(W_R_LANG,     "Sprache", "Language", "Langue", "Idioma", "Lingua") \
  /* Anzeige */ \
  X(W_ALL,        "Alle", "All", "Tous", "Todos", "Tutti") \
  X(W_H_ACCENT,   "Akzentfarbe", "Accent colour", "Couleur d'accent", "Color de acento", "Colore d'accento") \
  X(W_LOOK_HINT,  "Design und Farbe wirken sofort auf dem Display. Alle anderen Felder &uuml;bernimmst du mit &bdquo;Speichern&ldquo;.", \
                  "Design and colour apply to the display immediately. All other fields take effect with “Save”.", \
                  "Le design et la couleur s'appliquent tout de suite. Les autres champs sont pris en compte avec « Enregistrer ».", \
                  "El diseño y el color se aplican al instante. Los demás campos se aplican con «Guardar».", \
                  "Design e colore si applicano subito. Gli altri campi si applicano con «Salva».") \
  X(W_H_AUTO,     "Automatischer Design-Wechsel", "Automatic design rotation", "Rotation automatique des designs", "Rotación automática de diseños", "Rotazione automatica dei design") \
  X(W_AUTO_CB,    "Designs nach Zeitplan wechseln", "Rotate designs on a timer", "Changer de design à intervalle régulier", "Cambiar de diseño cada cierto tiempo", "Cambia design a intervalli") \
  X(W_AUTO_EVERY, "Wechsel alle", "Change every", "Changer toutes les", "Cambiar cada", "Cambia ogni") \
  X(W_SECONDS,    "Sekunden", "seconds", "secondes", "segundos", "secondi") \
  X(W_MINUTES,    "Minuten", "minutes", "minutes", "minutos", "minuti") \
  X(W_HOURS,      "Stunden", "hours", "heures", "horas", "ore") \
  X(W_RANDOM,     "Zuf&auml;llige Reihenfolge", "Random order", "Ordre aléatoire", "Orden aleatorio", "Ordine casuale") \
  X(W_USE_THESE,  "Diese Designs verwenden (mindestens zwei) &ndash; Kategorie anhaken w&auml;hlt alle darin:", \
                  "Use these designs (at least two) – ticking a category selects all of it:", \
                  "Utiliser ces designs (au moins deux) – cocher une catégorie la sélectionne entière :", \
                  "Usar estos diseños (al menos dos) – marcar una categoría la selecciona entera:", \
                  "Usa questi design (almeno due) – spuntare una categoria la seleziona tutta:") \
  X(W_ALL_L,      "alle", "all", "tous", "todos", "tutti") \
  X(W_NONE_L,     "keine", "none", "aucun", "ninguno", "nessuno") \
  X(W_AUTO_HINT,  "Gilt, sobald &bdquo;Speichern&ldquo; gedr&uuml;ckt wird. Ein Klick auf eine Design-Karte oben zeigt es sofort an; der Wechsel l&auml;uft danach weiter. Der Touch-Taster bl&auml;ttert in dieser Liste weiter (ohne Liste durch alle Designs); danach l&auml;uft der Wechsel normal mit vollem Intervall weiter.", \
                  "Takes effect after “Save”. Clicking a design card above shows it at once; the rotation then carries on. The touch button steps through this list (without a list: through all designs); afterwards the rotation continues with a full interval.", \
                  "Pris en compte après « Enregistrer ». Un clic sur une carte ci-dessus l'affiche tout de suite ; la rotation continue ensuite. Le bouton tactile parcourt cette liste (sans liste : tous les designs) ; ensuite la rotation reprend avec un intervalle complet.", \
                  "Se aplica al pulsar «Guardar». Un clic en una tarjeta de arriba la muestra al instante; después la rotación continúa. El botón táctil avanza por esta lista (sin lista: por todos los diseños); después la rotación sigue con el intervalo completo.", \
                  "Vale dopo «Salva». Un clic su una scheda qui sopra la mostra subito; poi la rotazione continua. Il tasto touch scorre questa lista (senza lista: tutti i design); poi la rotazione riprende con l'intervallo completo.") \
  X(W_H_EXTRAS,   "Extras", "Extras", "Extras", "Extras", "Extra") \
  X(W_HOL_CB,     "Feiertags-Designs automatisch in den Design-Wechsel aufnehmen (jeweils 10 Tage vorher bis zum Fest: Weihnachten mit Weihnachtself, Silvester, Valentinstag, Halloween, Ostern)", \
                  "Add holiday designs to the rotation automatically (10 days before until the day: Christmas with Christmas elf, New Year's Eve, Valentine's Day, Halloween, Easter)", \
                  "Ajouter automatiquement les designs de fête à la rotation (10 jours avant jusqu'au jour J : Noël avec le lutin, Saint-Sylvestre, Saint-Valentin, Halloween, Pâques)", \
                  "Añadir automáticamente los diseños festivos a la rotación (10 días antes hasta el día: Navidad con el elfo, Nochevieja, San Valentín, Halloween, Pascua)", \
                  "Aggiungi automaticamente i design delle feste alla rotazione (da 10 giorni prima fino al giorno: Natale con l'elfo, San Silvestro, San Valentino, Halloween, Pasqua)") \
  X(W_HOL_HINT,   "Wirkt, wenn der automatische Design-Wechsel an ist. Die eigene Liste bleibt unver&auml;ndert; nach dem Fest f&auml;llt das Design von selbst wieder heraus.", \
                  "Works when the automatic rotation is on. Your own list stays unchanged; after the holiday the design drops out by itself.", \
                  "Actif si la rotation automatique est activée. Votre liste reste inchangée ; après la fête le design disparaît tout seul.", \
                  "Funciona si la rotación automática está activa. Tu lista no cambia; después de la fiesta el diseño sale solo.", \
                  "Funziona se la rotazione automatica è attiva. La tua lista resta invariata; dopo la festa il design esce da solo.") \
  X(W_NYE_CB,     "Silvester-Automatik: am 31.12. von 23:30 bis 00:30 Feuerwerk (Design &bdquo;Silvester&ldquo;)", \
                  "New Year's Eve automatic: fireworks on 31 Dec from 23:30 to 00:30 (design “New Year's Eve”)", \
                  "Automatique Saint-Sylvestre : feu d'artifice le 31/12 de 23h30 à 00h30 (design « Saint-Sylvestre »)", \
                  "Automático Nochevieja: fuegos artificiales el 31/12 de 23:30 a 00:30 (diseño «Nochevieja»)", \
                  "Automatico San Silvestro: fuochi d'artificio il 31/12 dalle 23:30 alle 00:30 (design «San Silvestro»)") \
  X(W_ISS_SHOWS,  "ISS-Tracker zeigt", "ISS tracker shows", "Le suivi ISS montre", "El rastreador ISS muestra", "Il tracker ISS mostra") \
  X(W_SL0,        "die ISS", "the ISS", "l'ISS", "la ISS", "la ISS") \
  X(W_SL1,        "immer den Weihnachtsschlitten", "always Santa's sleigh", "toujours le traîneau du Père Noël", "siempre el trineo de Papá Noel", "sempre la slitta di Babbo Natale") \
  X(W_SL2,        "den Weihnachtsschlitten vom 1. bis 26.12.", "Santa's sleigh from 1 to 26 Dec", "le traîneau du 1er au 26/12", "el trineo del 1 al 26/12", "la slitta dal 1 al 26/12") \
  X(W_UFO_CB,     "Flugradar: ab und zu ein UFO erkennen (nur Spa&szlig;)", "Flight radar: spot a UFO now and then (just for fun)", "Radar aérien : repérer de temps en temps un OVNI (pour rire)", \
                  "Radar de vuelos: detectar de vez en cuando un OVNI (de broma)", "Radar voli: ogni tanto un UFO (per gioco)") \
  X(W_UFO_BTN,    "UFO jetzt zeigen", "Show UFO now", "Montrer un OVNI", "Mostrar OVNI ahora", "Mostra UFO ora") \
  X(W_UFO_HINT,   "Der Knopf schaltet auf das Flugradar und l&auml;sst sofort ein UFO auftauchen.", "The button switches to the flight radar and makes a UFO appear at once.", \
                  "Le bouton passe au radar aérien et fait apparaître un OVNI aussitôt.", "El botón cambia al radar de vuelos y hace aparecer un OVNI enseguida.", \
                  "Il pulsante passa al radar voli e fa comparire subito un UFO.") \
  X(W_AQUA,       "Aquarium", "Aquarium", "Aquarium", "Acuario", "Acquario") \
  X(W_FEED,       "Fische f&uuml;ttern", "Feed the fish", "Nourrir les poissons", "Dar de comer a los peces", "Dai da mangiare ai pesci") \
  X(W_VISIT,      "Besuch: Seeschlange / U-Boot", "Visitor: sea serpent / submarine", "Visite : serpent de mer / sous-marin", "Visita: serpiente marina / submarino", "Visita: serpente marino / sottomarino") \
  X(W_AQUA_HINT,  "Beide Kn&ouml;pfe schalten auf das Aquarium. Der Besuch kommt sonst von selbst alle paar Minuten, abwechselnd Seeschlange und U-Boot.", \
                  "Both buttons switch to the aquarium. Otherwise a visitor comes by itself every few minutes, alternating sea serpent and submarine.", \
                  "Les deux boutons passent à l'aquarium. Sinon une visite arrive d'elle-même toutes les quelques minutes, serpent de mer et sous-marin en alternance.", \
                  "Ambos botones cambian al acuario. Si no, la visita llega sola cada pocos minutos, alternando serpiente marina y submarino.", \
                  "Entrambi i pulsanti passano all'acquario. Altrimenti la visita arriva da sola ogni pochi minuti, alternando serpente marino e sottomarino.") \
  X(W_H_LOOK,     "Darstellung", "Appearance", "Présentation", "Presentación", "Aspetto") \
  X(W_SHOW_SEC,   "Sekunden anzeigen", "Show seconds", "Afficher les secondes", "Mostrar segundos", "Mostra i secondi") \
  X(W_ORIENT,     "Ausrichtung", "Orientation", "Orientation", "Orientación", "Orientamento") \
  X(W_NORMAL,     "Normal", "Normal", "Normale", "Normal", "Normale") \
  X(W_ROT180,     "180&deg; gedreht", "Rotated 180&deg;", "Tourné de 180&deg;", "Girado 180&deg;", "Ruotato di 180&deg;") \
  X(W_LANG_HINT,  "Gilt f&uuml;r Display und diese Seite, sofort nach &bdquo;Speichern&ldquo;.", "Applies to the display and this page right after “Save”.", \
                  "S'applique à l'écran et à cette page dès « Enregistrer ».", "Se aplica a la pantalla y a esta página nada más pulsar «Guardar».", \
                  "Vale per il display e per questa pagina subito dopo «Salva».") \
  /* Helligkeit */ \
  X(W_BRIGHT_LBL, "Helligkeit: ", "Brightness: ", "Luminosité : ", "Brillo: ", "Luminosità: ") \
  X(W_SWDIM_HINT, "Ohne Umbau des Displays werden nur die Farben abgedunkelt; das Backlight selbst bleibt unver&auml;ndert hell.", \
                  "Without modifying the display only the colours are dimmed; the backlight itself stays at full brightness.", \
                  "Sans modification de l'écran seules les couleurs sont assombries ; le rétroéclairage reste à pleine luminosité.", \
                  "Sin modificar la pantalla solo se oscurecen los colores; la retroiluminación sigue al máximo.", \
                  "Senza modificare il display si scuriscono solo i colori; la retroilluminazione resta al massimo.") \
  X(W_H_ONOFF,    "Display ein/aus", "Display on/off", "Écran allumé/éteint", "Pantalla encendida/apagada", "Display acceso/spento") \
  X(W_DISP_ON,    "Display einschalten", "Turn display on", "Allumer l'écran", "Encender la pantalla", "Accendi il display") \
  X(W_DISP_OFF,   "Display ausschalten (Standby)", "Turn display off (standby)", "Éteindre l'écran (veille)", "Apagar la pantalla (en espera)", "Spegni il display (standby)") \
  X(W_DIM_CB,     "Display zu bestimmten Zeiten dimmen", "Dim the display at set times", "Atténuer l'écran à certaines heures", "Atenuar la pantalla a ciertas horas", "Attenua il display a orari stabiliti") \
  X(W_DIM_FROM,   "Dimmen ab", "Dim from", "Atténuer à partir de", "Atenuar desde", "Attenua dalle") \
  X(W_DIM_BACK,   "Zur&uuml;ck zur normalen Helligkeit um", "Back to normal brightness at", "Retour à la luminosité normale à", "Volver al brillo normal a las", "Torna alla luminosità normale alle") \
  X(W_DIM_LEVEL,  "Helligkeit w&auml;hrend der Dimmung (5&ndash;100 %)", "Brightness while dimmed (5–100 %)", "Luminosité pendant l'atténuation (5–100 %)", "Brillo durante la atenuación (5–100 %)", "Luminosità durante l'attenuazione (5–100 %)") \
  X(W_DIMTV_CB,   "Nachtmodus: w&auml;hrend der Dimmung das Testbild zeigen", "Night mode: show the test card while dimmed", "Mode nuit : afficher la mire pendant l'atténuation", \
                  "Modo noche: mostrar la carta de ajuste durante la atenuación", "Modalità notte: mostra il monoscopio durante l'attenuazione") \
  X(W_DIM_HINT,   "Ben&ouml;tigt eine g&uuml;ltige Uhrzeit. Liegt &bdquo;ab&ldquo; sp&auml;ter als &bdquo;um&ldquo;, gilt das Fenster &uuml;ber Mitternacht (z.&nbsp;B. 22:00 bis 07:00). W&auml;hrend der Dimmung gilt dieser Wert statt des Schiebereglers. Mit Nachtmodus erscheint im Fenster das Testbild statt des gew&auml;hlten Designs (auch bei Design-Wechsel), danach kehrt die Uhr zum gew&auml;hlten Design zur&uuml;ck. Wirkt erst nach &bdquo;Speichern&ldquo;.", \
                  "Needs a valid time. If “from” is later than “back”, the window spans midnight (e.g. 22:00 to 07:00). While dimmed this value replaces the slider. With night mode the test card replaces the chosen design during the window (also with rotation); afterwards the clock returns to the chosen design. Takes effect after “Save”.", \
                  "Nécessite une heure valide. Si le début est plus tard que la fin, la plage passe minuit (p. ex. 22:00 à 07:00). Pendant l'atténuation cette valeur remplace le curseur. En mode nuit la mire remplace le design choisi (aussi en rotation), puis l'horloge revient au design choisi. Pris en compte après « Enregistrer ».", \
                  "Necesita una hora válida. Si el inicio es más tarde que el final, el intervalo cruza la medianoche (p. ej. 22:00 a 07:00). Durante la atenuación este valor sustituye al deslizador. Con el modo noche la carta de ajuste sustituye al diseño elegido (también en rotación); después el reloj vuelve al diseño elegido. Se aplica tras «Guardar».", \
                  "Serve un orario valido. Se l'inizio è dopo la fine, la fascia passa la mezzanotte (es. 22:00–07:00). Durante l'attenuazione questo valore sostituisce il cursore. In modalità notte il monoscopio sostituisce il design scelto (anche in rotazione), poi l'orologio torna al design scelto. Vale dopo «Salva».") \
  /* Laufschrift */ \
  X(W_TK_ON,      "Laufschrift aktiv", "Ticker on", "Texte défilant actif", "Texto en movimiento activo", "Testo scorrevole attivo") \
  X(W_TK_TEXT,    "Text (Umlaute und Sonderzeichen erlaubt, bis 160 Zeichen)", "Text (accents and special characters allowed, up to 160 characters)", "Texte (accents et caractères spéciaux permis, 160 caractères max.)", \
                  "Texto (acentos y caracteres especiales permitidos, hasta 160 caracteres)", "Testo (accenti e caratteri speciali ammessi, fino a 160 caratteri)") \
  X(W_TK_FLOW,    "Ablauf", "Mode", "Déroulement", "Modo", "Modalità") \
  X(W_TK_ONCE,    "Einmalig (beim Speichern)", "Once (when saving)", "Une fois (à l'enregistrement)", "Una vez (al guardar)", "Una volta (al salvataggio)") \
  X(W_TK_LOOP,    "Dauerschleife", "Continuous loop", "En boucle", "En bucle", "In ciclo continuo") \
  X(W_TK_PAUSE,   "Pause zwischen den Durchl&auml;ufen (Sekunden, nur Schleife)", "Pause between runs (seconds, loop only)", "Pause entre deux passages (secondes, boucle seulement)", \
                  "Pausa entre pasadas (segundos, solo en bucle)", "Pausa tra i passaggi (secondi, solo ciclo)") \
  X(W_TK_POS,     "Position", "Position", "Position", "Posición", "Posizione") \
  X(W_TOP,        "Oben", "Top", "En haut", "Arriba", "In alto") \
  X(W_MID,        "Mitte", "Middle", "Au milieu", "En medio", "Al centro") \
  X(W_BOTTOM,     "Unten", "Bottom", "En bas", "Abajo", "In basso") \
  X(W_TK_SIZE,    "Schriftgr&ouml;&szlig;e", "Font size", "Taille du texte", "Tamaño de letra", "Dimensione testo") \
  X(W_MEDIUM,     "Mittel", "Medium", "Moyenne", "Mediano", "Media") \
  X(W_LARGE,      "Gro&szlig;", "Large", "Grande", "Grande", "Grande") \
  X(W_COLOR,      "Farbe", "Colour", "Couleur", "Color", "Colore") \
  X(W_WHITE,      "Weiss", "White", "Blanc", "Blanco", "Bianco") \
  X(W_SPEED,      "Geschwindigkeit: ", "Speed: ", "Vitesse : ", "Velocidad: ", "Velocità: ") \
  X(W_TK_BG,      "Dunkles Hintergrundband (bei klassischen Designs immer an)", "Dark background band (always on with classic designs)", "Bande de fond sombre (toujours active sur les designs classiques)", \
                  "Banda de fondo oscura (siempre activa en los diseños clásicos)", "Banda di sfondo scura (sempre attiva nei design classici)") \
  X(W_TK_PLAY,    "Jetzt einmal abspielen", "Play once now", "Lire une fois maintenant", "Reproducir una vez ahora", "Riproduci una volta ora") \
  X(W_TK_HINT,    "Der Knopf spielt den <b>gespeicherten</b> Text einmal ab. &Auml;nderungen erst mit &bdquo;Speichern&ldquo; &uuml;bernehmen. Die Laufschrift liegt &uuml;ber jedem Design und pausiert im Standby.", \
                  "The button plays the <b>saved</b> text once. Apply changes with “Save” first. The ticker runs over every design and pauses in standby.", \
                  "Le bouton lit une fois le texte <b>enregistré</b>. Validez d'abord les changements avec « Enregistrer ». Le texte défile par-dessus chaque design et s'arrête en veille.", \
                  "El botón reproduce una vez el texto <b>guardado</b>. Aplica antes los cambios con «Guardar». El texto pasa por encima de cada diseño y se detiene en espera.", \
                  "Il pulsante riproduce una volta il testo <b>salvato</b>. Applica prima le modifiche con «Salva». Il testo scorre sopra ogni design e si ferma in standby.") \
  /* Summer und Wecker */ \
  X(W_BZ_NOCFG,   "<b>Kein Summer konfiguriert.</b> In <code>config.h</code> <code>PIN_BUZZER</code> eintragen (z.&nbsp;B. 10) und neu aufspielen. Die Einstellungen werden trotzdem gespeichert.", \
                  "<b>No buzzer configured.</b> Set <code>PIN_BUZZER</code> in <code>config.h</code> (e.g. 10) and upload again. The settings are saved anyway.", \
                  "<b>Aucun buzzer configuré.</b> Indiquez <code>PIN_BUZZER</code> dans <code>config.h</code> (p. ex. 10) et téléversez à nouveau. Les réglages sont tout de même enregistrés.", \
                  "<b>No hay zumbador configurado.</b> Indica <code>PIN_BUZZER</code> en <code>config.h</code> (p. ej. 10) y vuelve a cargar. Los ajustes se guardan igualmente.", \
                  "<b>Nessun cicalino configurato.</b> Imposta <code>PIN_BUZZER</code> in <code>config.h</code> (es. 10) e ricarica. Le impostazioni vengono salvate comunque.") \
  X(W_BZ_HOUR,    "Stundensignal (zur vollen Stunde zweimal kurz)", "Hourly chime (two short beeps on the hour)", "Signal horaire (deux bips à chaque heure pile)", "Señal horaria (dos pitidos cortos a la hora en punto)", "Segnale orario (due bip brevi allo scoccare dell'ora)") \
  X(W_BZ_QUIET,   "Kein Stundensignal w&auml;hrend der Zeitplan-Dimmung (Nachtruhe)", "No hourly chime during scheduled dimming (quiet hours)", "Pas de signal horaire pendant l'atténuation programmée (nuit)", \
                  "Sin señal horaria durante la atenuación programada (descanso)", "Nessun segnale orario durante l'attenuazione programmata (riposo)") \
  X(W_BZ_TOUCH,   "Klick bei Ber&uuml;hrung des Touch-Tasters", "Click when the touch button is touched", "Clic au toucher du bouton tactile", "Clic al tocar el botón táctil", "Clic al tocco del tasto touch") \
  X(W_BZ_TEST,    "Summer testen", "Test buzzer", "Tester le buzzer", "Probar el zumbador", "Prova il cicalino") \
  X(W_AL_ON,      "Wecker aktiv", "Alarm on", "Réveil actif", "Alarma activa", "Sveglia attiva") \
  X(W_AL_TIME,    "Weckzeit", "Alarm time", "Heure du réveil", "Hora de la alarma", "Orario sveglia") \
  X(W_AL_DAYS,    "Tage", "Days", "Jours", "Días", "Giorni") \
  X(W_AL_HINT,    "Der Wecker piept bis zu 5 Minuten und schaltet ein Display im Standby ein. Abstellen: Touch-Taster ber&uuml;hren oder hier bzw. unter Status &bdquo;Wecker abstellen&ldquo;. Ben&ouml;tigt eine g&uuml;ltige Uhrzeit (NTP oder manuell). Wirkt erst nach &bdquo;Speichern&ldquo;.", \
                  "The alarm beeps for up to 5 minutes and wakes a display in standby. To stop: touch the touch button or use “Stop alarm” here or under Status. Needs a valid time (NTP or manual). Takes effect after “Save”.", \
                  "Le réveil sonne jusqu'à 5 minutes et rallume un écran en veille. Pour l'arrêter : toucher le bouton tactile ou « Arrêter le réveil » ici ou sous État. Nécessite une heure valide (NTP ou manuelle). Pris en compte après « Enregistrer ».", \
                  "La alarma pita hasta 5 minutos y enciende la pantalla en espera. Para pararla: tocar el botón táctil o «Parar la alarma» aquí o en Estado. Necesita una hora válida (NTP o manual). Se aplica tras «Guardar».", \
                  "La sveglia suona fino a 5 minuti e riaccende il display in standby. Per fermarla: tocca il tasto touch o «Ferma la sveglia» qui o in Stato. Serve un orario valido (NTP o manuale). Vale dopo «Salva».") \
  /* WLAN */ \
  X(W_SSID,       "Netzwerkname (SSID)", "Network name (SSID)", "Nom du réseau (SSID)", "Nombre de la red (SSID)", "Nome della rete (SSID)") \
  X(W_SCAN,       "Netzwerke suchen", "Search networks", "Chercher les réseaux", "Buscar redes", "Cerca reti") \
  X(W_SCANNING,   "Suche ...", "Searching ...", "Recherche ...", "Buscando ...", "Ricerca ...") \
  X(W_FOUND,      "Gefunden: ", "Found: ", "Trouvés : ", "Encontradas: ", "Trovate: ") \
  X(W_PASS,       "Passwort", "Password", "Mot de passe", "Contraseña", "Password") \
  X(W_PASS_PH,    "(unverändert lassen: leer)", "(leave empty to keep)", "(vide = inchangé)", "(vacío = sin cambios)", "(vuoto = invariato)") \
  X(W_WIFI_HINT,  "Ein Wechsel des WLANs startet das Ger&auml;t neu.", "Changing the Wi-Fi restarts the device.", "Changer de Wi-Fi redémarre l'appareil.", "Cambiar de Wi-Fi reinicia el dispositivo.", "Cambiare Wi-Fi riavvia il dispositivo.") \
  /* Zeit und Wetter */ \
  X(W_H_TIME,     "Zeit", "Time", "Heure", "Hora", "Ora") \
  X(W_TZ,         "Zeitzone", "Time zone", "Fuseau horaire", "Zona horaria", "Fuso orario") \
  X(W_TZC,        "Eigene Zeitzone (POSIX-String, &uuml;berschreibt die Auswahl)", "Custom time zone (POSIX string, overrides the selection)", "Fuseau personnalisé (chaîne POSIX, remplace le choix)", \
                  "Zona horaria propia (cadena POSIX, sustituye la selección)", "Fuso personalizzato (stringa POSIX, sostituisce la scelta)") \
  X(W_EG,         "z.B.", "e.g.", "p. ex.", "p. ej.", "es.") \
  X(W_NTP_CB,     "Uhrzeit automatisch per NTP (Internet)", "Set time automatically via NTP (internet)", "Heure automatique par NTP (Internet)", "Hora automática por NTP (Internet)", "Ora automatica via NTP (Internet)") \
  X(W_NTP_SRV,    "NTP-Server", "NTP server", "Serveur NTP", "Servidor NTP", "Server NTP") \
  X(W_NTP_HINT,   "Ein anderer NTP-Server wird nach einem Neustart genutzt. Ohne Haken holt die Uhr keine Zeit aus dem Internet; dann Datum und Uhrzeit unten von Hand setzen.", \
                  "A different NTP server is used after a restart. Unticked, the clock fetches no time from the internet; then set date and time by hand below.", \
                  "Un autre serveur NTP est utilisé après un redémarrage. Sans coche, l'horloge ne prend pas l'heure sur Internet ; réglez alors date et heure ci-dessous.", \
                  "Otro servidor NTP se usa tras reiniciar. Sin marcar, el reloj no toma la hora de Internet; entonces ajusta fecha y hora abajo a mano.", \
                  "Un altro server NTP viene usato dopo un riavvio. Senza spunta l'orologio non prende l'ora da Internet; imposta allora data e ora qui sotto a mano.") \
  X(W_H_MANUAL,   "Datum und Uhrzeit manuell", "Set date and time manually", "Date et heure manuelles", "Fecha y hora manuales", "Data e ora manuali") \
  X(W_DATE,       "Datum", "Date", "Date", "Fecha", "Data") \
  X(W_BROWSER_T,  "Zeit vom Browser &uuml;bernehmen", "Take time from browser", "Prendre l'heure du navigateur", "Tomar la hora del navegador", "Prendi l'ora dal browser") \
  X(W_SET_TIME,   "Uhrzeit setzen", "Set time", "Régler l'heure", "Ajustar la hora", "Imposta l'ora") \
  X(W_MAN_HINT,   "Gilt in der oben gew&auml;hlten Zeitzone, Sommer-/Winterzeit wird automatisch erkannt. &bdquo;Uhrzeit setzen&ldquo; &uuml;bernimmt nur Datum und Uhrzeit, keine anderen Felder. Mit NTP wird die Zeit beim n&auml;chsten Abgleich (ca. st&uuml;ndlich) wieder &uuml;berschrieben. Die Uhr hat keine Pufferbatterie: Nach Stromausfall muss die Zeit ohne NTP neu gesetzt werden (ein Neustart &uuml;ber den Browser beh&auml;lt sie meist).", \
                  "Uses the time zone chosen above; daylight saving time is detected automatically. “Set time” only applies date and time, no other fields. With NTP the time is overwritten at the next sync (about hourly). The clock has no backup battery: after a power cut the time must be set again without NTP (a restart from the browser usually keeps it).", \
                  "Utilise le fuseau choisi ci-dessus ; l'heure d'été est détectée automatiquement. « Régler l'heure » ne prend que la date et l'heure. Avec NTP l'heure est réécrite à la synchronisation suivante (environ toutes les heures). L'horloge n'a pas de pile : après une coupure de courant il faut régler l'heure à nouveau sans NTP (un redémarrage depuis le navigateur la conserve en général).", \
                  "Usa la zona horaria elegida arriba; el horario de verano se detecta solo. «Ajustar la hora» solo aplica fecha y hora. Con NTP la hora se sobrescribe en la siguiente sincronización (aprox. cada hora). El reloj no tiene pila: tras un corte de luz hay que volver a ajustar la hora sin NTP (un reinicio desde el navegador suele conservarla).", \
                  "Usa il fuso scelto sopra; l'ora legale viene riconosciuta da sola. «Imposta l'ora» applica solo data e ora. Con NTP l'ora viene sovrascritta alla sincronizzazione successiva (circa ogni ora). L'orologio non ha batteria tampone: dopo un blackout senza NTP l'ora va reimpostata (un riavvio dal browser di solito la mantiene).") \
  X(W_LAT,        "Breitengrad", "Latitude", "Latitude", "Latitud", "Latitudine") \
  X(W_LON,        "L&auml;ngengrad", "Longitude", "Longitude", "Longitud", "Longitudine") \
  X(W_UPD,        "Aktualisierung (Minuten, 5&ndash;120)", "Update interval (minutes, 5–120)", "Mise à jour (minutes, 5–120)", "Actualización (minutos, 5–120)", "Aggiornamento (minuti, 5–120)") \
  X(W_H_RADAR,    "Flugradar", "Flight radar", "Radar aérien", "Radar de vuelos", "Radar voli") \
  X(W_RADIUS,     "Radius in km (5&ndash;100)", "Radius in km (5–100)", "Rayon en km (5–100)", "Radio en km (5–100)", "Raggio in km (5–100)") \
  X(W_RADAR_HINT, "Das Design &bdquo;Flugradar&ldquo; zeigt echte Flugzeuge rund um den Standort (Breiten- und L&auml;ngengrad von oben). Daten: offene ADS-B-Dienste adsb.lol, airplanes.live und adsb.fi, nur f&uuml;r private Nutzung. Abgerufen wird nur, solange das Design angezeigt wird.", \
                  "The “Flight radar” design shows real aircraft around the location (latitude and longitude above). Data: open ADS-B services adsb.lol, airplanes.live and adsb.fi, for private use only. Data is only fetched while the design is shown.", \
                  "Le design « Radar aérien » montre les vrais avions autour du lieu (latitude et longitude ci-dessus). Données : services ADS-B ouverts adsb.lol, airplanes.live et adsb.fi, usage privé uniquement. Rien n'est chargé tant que le design n'est pas affiché.", \
                  "El diseño «Radar de vuelos» muestra aviones reales alrededor de la ubicación (latitud y longitud de arriba). Datos: servicios ADS-B abiertos adsb.lol, airplanes.live y adsb.fi, solo para uso privado. Solo se consulta mientras se muestra el diseño.", \
                  "Il design «Radar voli» mostra aerei reali intorno alla posizione (latitudine e longitudine sopra). Dati: servizi ADS-B aperti adsb.lol, airplanes.live e adsb.fi, solo per uso privato. I dati si scaricano solo mentre il design è visibile.") \
  /* System */ \
  X(W_H_DEVICE,   "Ger&auml;t", "Device", "Appareil", "Dispositivo", "Dispositivo") \
  X(W_HOST_LBL,   "Ger&auml;tename (erreichbar als <i>name</i>.local)", "Device name (reachable as <i>name</i>.local)", "Nom de l'appareil (accessible via <i>nom</i>.local)", \
                  "Nombre del dispositivo (accesible como <i>nombre</i>.local)", "Nome del dispositivo (raggiungibile come <i>nome</i>.local)") \
  X(W_H_ACCESS,   "Zugang", "Access", "Accès", "Acceso", "Accesso") \
  X(W_WEBPW,      "Web-Passwort (Benutzer: admin; leer = unver&auml;ndert)", "Web password (user: admin; empty = unchanged)", "Mot de passe web (utilisateur : admin ; vide = inchangé)", \
                  "Contraseña web (usuario: admin; vacío = sin cambios)", "Password web (utente: admin; vuoto = invariata)") \
  X(W_RMPW,       "Web-Passwort entfernen", "Remove web password", "Supprimer le mot de passe web", "Quitar la contraseña web", "Rimuovi la password web") \
  X(W_H_FW,       "Firmware-Update &uuml;ber WLAN", "Firmware update over Wi-Fi", "Mise à jour du micrologiciel par Wi-Fi", "Actualizar el firmware por Wi-Fi", "Aggiornamento firmware via Wi-Fi") \
  X(W_INSTALLED,  "Installiert: ", "Installed: ", "Installé : ", "Instalado: ", "Installato: ") \
  X(W_NO_OTA,     "<b>Update &uuml;ber WLAN nicht m&ouml;glich:</b> Das Partitionsschema hat keinen Platz f&uuml;r ein Update (z.&nbsp;B. &bdquo;Huge APP (No OTA)&ldquo;). Einmal per USB mit <i>Partition Scheme: Minimal SPIFFS (1.9MB APP with OTA)</i> hochladen, danach geht es per WLAN. Die Einstellungen bleiben erhalten.", \
                  "<b>Update over Wi-Fi not possible:</b> the partition scheme has no room for an update (e.g. “Huge APP (No OTA)”). Upload once via USB with <i>Partition Scheme: Minimal SPIFFS (1.9MB APP with OTA)</i>, after that it works over Wi-Fi. Settings are kept.", \
                  "<b>Mise à jour par Wi-Fi impossible :</b> le schéma de partitions n'a pas de place (p. ex. « Huge APP (No OTA) »). Téléversez une fois par USB avec <i>Partition Scheme: Minimal SPIFFS (1.9MB APP with OTA)</i>, ensuite cela marche par Wi-Fi. Les réglages sont conservés.", \
                  "<b>No se puede actualizar por Wi-Fi:</b> el esquema de particiones no tiene espacio (p. ej. «Huge APP (No OTA)»). Carga una vez por USB con <i>Partition Scheme: Minimal SPIFFS (1.9MB APP with OTA)</i>; después funciona por Wi-Fi. Los ajustes se conservan.", \
                  "<b>Aggiornamento via Wi-Fi impossibile:</b> lo schema delle partizioni non ha spazio (es. «Huge APP (No OTA)»). Carica una volta via USB con <i>Partition Scheme: Minimal SPIFFS (1.9MB APP with OTA)</i>, poi funziona via Wi-Fi. Le impostazioni restano.") \
  X(W_FW_FILE,    "Firmware-Datei (.bin, h&ouml;chstens %lu KB)", "Firmware file (.bin, at most %lu KB)", "Fichier du micrologiciel (.bin, %lu Ko max.)", "Archivo de firmware (.bin, máx. %lu KB)", "File firmware (.bin, al massimo %lu KB)") \
  X(W_FW_INST,    "Update installieren", "Install update", "Installer la mise à jour", "Instalar actualización", "Installa aggiornamento") \
  X(W_FW_HINT1,   "Die Datei <b>&hellip;_update.bin</b> aus dem Release nehmen oder in der Arduino IDE:<i>Sketch &gt; Kompilierte Bin&auml;rdatei exportieren</i>, dann die Datei <b>WetterUhr.ino.bin</b> aus dem Sketch-Ordner (Unterordner <i>build</i>) w&auml;hlen &ndash; nicht die &bdquo;merged&ldquo;-, &bdquo;bootloader&ldquo;- oder &bdquo;partitions&ldquo;-Datei. Die Uhr pr&uuml;ft die Datei, schreibt sie in den freien Bereich und startet nur bei Erfolg neu; sonst l&auml;uft die alte Firmware weiter. Einstellungen bleiben erhalten. Alternativ in der Arduino IDE als Port den Netzwerk-Port &bdquo;", \
                  "Use the release file <b>…_update.bin</b>, or in the Arduino IDE: <i>Sketch &gt; Export Compiled Binary</i>, then pick <b>WetterUhr.ino.bin</b> from the sketch folder (subfolder <i>build</i>) – not the “merged”, “bootloader” or “partitions” file. The clock checks the file, writes it to the free slot and only restarts on success; otherwise the old firmware keeps running. Settings are kept. Alternatively choose the network port “", \
                  "Utilisez le fichier <b>…_update.bin</b> de la release, ou dans l'Arduino IDE : <i>Croquis &gt; Exporter les binaires compilées</i>, puis choisissez <b>WetterUhr.ino.bin</b> dans le dossier du croquis (sous-dossier <i>build</i>) – pas le fichier « merged », « bootloader » ou « partitions ». L'horloge vérifie le fichier, l'écrit dans la zone libre et ne redémarre qu'en cas de succès ; sinon l'ancien micrologiciel continue. Les réglages sont conservés. Autre possibilité : choisir dans l'Arduino IDE le port réseau « ", \
                  "Usa el archivo <b>…_update.bin</b> de la versión, o en el Arduino IDE: <i>Programa &gt; Exportar binarios compilados</i> y elige <b>WetterUhr.ino.bin</b> de la carpeta del sketch (subcarpeta <i>build</i>) – no el archivo «merged», «bootloader» ni «partitions». El reloj comprueba el archivo, lo escribe en la zona libre y solo reinicia si todo va bien; si no, sigue el firmware anterior. Los ajustes se conservan. También puedes elegir en el Arduino IDE el puerto de red «", \
                  "Usa il file <b>…_update.bin</b> della release, oppure nell'Arduino IDE: <i>Sketch &gt; Esporta sketch compilato</i> e scegli <b>WetterUhr.ino.bin</b> dalla cartella dello sketch (sottocartella <i>build</i>) – non il file «merged», «bootloader» o «partitions». L'orologio controlla il file, lo scrive nell'area libera e si riavvia solo se va tutto bene; altrimenti resta il firmware vecchio. Le impostazioni restano. In alternativa scegli nell'Arduino IDE la porta di rete «") \
  X(W_FW_HINT2,   "&ldquo; w&auml;hlen und normal hochladen (Passwort = Web-Passwort).", "” in the Arduino IDE and upload as usual (password = web password).", " » et téléversez normalement (mot de passe = mot de passe web).", \
                  "» y carga como siempre (contraseña = contraseña web).", "» e carica come sempre (password = password web).") \
  X(W_H_RESET,    "Zur&uuml;cksetzen", "Reset", "Réinitialiser", "Restablecer", "Ripristino") \
  X(W_RESET_Q,    "Alle Einstellungen löschen?", "Erase all settings?", "Effacer tous les réglages ?", "¿Borrar todos los ajustes?", "Cancellare tutte le impostazioni?") \
  X(W_FACTORY,    "Werkseinstellungen", "Factory reset", "Réglages d'usine", "Valores de fábrica", "Impostazioni di fabbrica") \
  /* Texte fuer JavaScript (ohne HTML-Entities) */ \
  X(J_PICK,       "Bitte zuerst eine .bin-Datei wählen.", "Please choose a .bin file first.", "Choisissez d'abord un fichier .bin.", "Elige primero un archivo .bin.", "Scegli prima un file .bin.") \
  X(J_WRONG,      "Falsche Datei: bitte WetterUhr.ino.bin bzw. …_update.bin wählen.", "Wrong file: please choose WetterUhr.ino.bin or …_update.bin.", "Mauvais fichier : choisissez WetterUhr.ino.bin ou …_update.bin.", \
                  "Archivo incorrecto: elige WetterUhr.ino.bin o …_update.bin.", "File errato: scegli WetterUhr.ino.bin o …_update.bin.") \
  X(J_CONFIRM,    "Firmware installieren? Die Uhr startet danach neu.", "Install firmware? The clock will restart afterwards.", "Installer le micrologiciel ? L'horloge redémarrera ensuite.", \
                  "¿Instalar el firmware? El reloj se reiniciará después.", "Installare il firmware? L'orologio si riavvierà.") \
  X(J_SENT,       "Übertragen: ", "Uploaded: ", "Transféré : ", "Enviado: ", "Trasferito: ") \
  X(J_DONE,       "Fertig. Die Uhr startet neu, Seite lädt in 20 s neu ...", "Done. The clock is restarting, the page reloads in 20 s ...", "Terminé. L'horloge redémarre, la page se recharge dans 20 s ...", \
                  "Listo. El reloj se reinicia, la página se recarga en 20 s ...", "Fatto. L'orologio si riavvia, la pagina si ricarica tra 20 s ...") \
  X(J_ERR,        "Fehler: ", "Error: ", "Erreur : ", "Error: ", "Errore: ") \
  X(J_LOST,       "Verbindung abgebrochen.", "Connection lost.", "Connexion interrompue.", "Conexión interrumpida.", "Connessione interrotta.") \
  /* Antworten */ \
  X(W_PG_SAVED,   "<h2>Gespeichert</h2><p>Das Ger&auml;t startet neu, um die Netzwerk&auml;nderung zu &uuml;bernehmen. Danach ist die Seite unter der neuen IP-Adresse (steht auf dem Display) erreichbar.</p>", \
                  "<h2>Saved</h2><p>The device restarts to apply the network change. Afterwards the page is reachable at the new IP address (shown on the display).</p>", \
                  "<h2>Enregistré</h2><p>L'appareil redémarre pour appliquer le changement de réseau. La page sera ensuite accessible à la nouvelle adresse IP (affichée à l'écran).</p>", \
                  "<h2>Guardado</h2><p>El dispositivo se reinicia para aplicar el cambio de red. Después la página estará en la nueva dirección IP (aparece en la pantalla).</p>", \
                  "<h2>Salvato</h2><p>Il dispositivo si riavvia per applicare la modifica di rete. Poi la pagina sarà raggiungibile al nuovo indirizzo IP (indicato sul display).</p>") \
  X(W_PG_RESET,   "<h2>Zur&uuml;ckgesetzt</h2><p>Das Ger&auml;t startet neu und &ouml;ffnet den Einrichtungs-Access-Point.</p>", \
                  "<h2>Reset done</h2><p>The device restarts and opens the setup access point.</p>", \
                  "<h2>Réinitialisé</h2><p>L'appareil redémarre et ouvre le point d'accès de configuration.</p>", \
                  "<h2>Restablecido</h2><p>El dispositivo se reinicia y abre el punto de acceso de configuración.</p>", \
                  "<h2>Ripristinato</h2><p>Il dispositivo si riavvia e apre l'access point di configurazione.</p>") \
  X(W_BAD_TIME,   "Ungültiges Datum oder Uhrzeit", "Invalid date or time", "Date ou heure invalide", "Fecha u hora no válida", "Data o ora non valida") \
  X(W_E_LOGIN,    "Nicht angemeldet", "Not logged in", "Non connecté", "Sin iniciar sesión", "Accesso non effettuato") \
  X(W_E_NOPART,   "Partitionsschema ohne OTA-Bereich (siehe Hinweis)", "Partition scheme without OTA slot (see note)", "Schéma de partitions sans zone OTA (voir la remarque)", \
                  "Esquema de particiones sin zona OTA (ver nota)", "Schema partizioni senza area OTA (vedi nota)") \
  X(W_E_BIG,      "Datei zu groß (%lu KB, Platz %lu KB)", "File too large (%lu KB, space %lu KB)", "Fichier trop gros (%lu Ko, place %lu Ko)", "Archivo demasiado grande (%lu KB, espacio %lu KB)", "File troppo grande (%lu KB, spazio %lu KB)") \
  X(W_E_WRITE,    "Schreibfehler: ", "Write error: ", "Erreur d'écriture : ", "Error de escritura: ", "Errore di scrittura: ") \
  X(W_E_CHECK,    "Prüfung fehlgeschlagen: ", "Verification failed: ", "Vérification échouée : ", "Verificación fallida: ", "Verifica fallita: ") \
  X(W_E_ABORT,    "Übertragung abgebrochen", "Upload aborted", "Transfert interrompu", "Transferencia interrumpida", "Trasferimento interrotto") \
  X(W_E_INCOMP,   "Keine vollständige Datei empfangen", "No complete file received", "Aucun fichier complet reçu", "No se recibió un archivo completo", "Nessun file completo ricevuto") \
  X(W_INVALID,    "ungültig", "invalid", "invalide", "no válido", "non valido") \
  X(W_NOT_FOUND,  "Nicht gefunden", "Not found", "Introuvable", "No encontrado", "Non trovato") \
  /* Akzentfarben */ \
  X(W_AC0,        "Cyan", "Cyan", "Cyan", "Cian", "Ciano") \
  X(W_AC1,        "Gr&uuml;n", "Green", "Vert", "Verde", "Verde") \
  X(W_AC2,        "Orange", "Orange", "Orange", "Naranja", "Arancione") \
  X(W_AC3,        "Gelb", "Yellow", "Jaune", "Amarillo", "Giallo") \
  X(W_AC4,        "Rot", "Red", "Rouge", "Rojo", "Rosso") \
  X(W_AC5,        "Pink", "Pink", "Rose", "Rosa", "Rosa") \
  X(W_AC6,        "Blau", "Blue", "Bleu", "Azul", "Blu") \
  X(W_AC7,        "Wei&szlig;", "White", "Blanc", "Blanco", "Bianco") \
  X(W_AC8,        "Violett", "Violet", "Violet", "Violeta", "Viola") \
  /* Kategorien */ \
  X(W_CAT0,       "Klassische Uhren", "Classic clocks", "Horloges classiques", "Relojes clásicos", "Orologi classici") \
  X(W_CAT1,       "Spiele &amp; Arcade", "Games &amp; arcade", "Jeux &amp; arcade", "Juegos y arcade", "Giochi e arcade") \
  X(W_CAT2,       "Tiere &amp; Natur", "Animals &amp; nature", "Animaux &amp; nature", "Animales y naturaleza", "Animali e natura") \
  X(W_CAT3,       "Technik &amp; Retro", "Tech &amp; retro", "Technique &amp; rétro", "Técnica y retro", "Tecnica e retrò") \
  X(W_CAT4,       "Traum &amp; Gef&uuml;hl", "Dreams &amp; feelings", "Rêve &amp; émotion", "Sueños y emociones", "Sogni ed emozioni") \
  X(W_CAT5,       "Heimat &amp; Familie", "Home &amp; family", "Foyer &amp; famille", "Hogar y familia", "Casa e famiglia") \
  X(W_CAT6,       "Himmel &amp; Weltraum", "Sky &amp; space", "Ciel &amp; espace", "Cielo y espacio", "Cielo e spazio") \
  X(W_CAT7,       "Feiertage", "Holidays", "Fêtes", "Fiestas", "Feste") \
  /* Designs (gleiche Reihenfolge wie die Design-Nummern) */ \
  X(W_ST0,        "Flip-Karten", "Flip cards", "Cartes à rabat", "Tarjetas flip", "Cartelle flip") \
  X(W_ST1,        "Digital + Balken", "Digital + bar", "Numérique + barre", "Digital + barra", "Digitale + barra") \
  X(W_ST2,        "Abenteuer", "Adventure", "Aventure", "Aventura", "Avventura") \
  X(W_ST3,        "Fallende Bl&ouml;cke", "Falling blocks", "Blocs qui tombent", "Bloques que caen", "Blocchi cadenti") \
  X(W_ST4,        "Labyrinth", "Maze", "Labyrinthe", "Laberinto", "Labirinto") \
  X(W_ST5,        "H&uuml;pfwelt", "Jump world", "Monde à sauts", "Mundo de saltos", "Mondo dei salti") \
  X(W_ST6,        "Neon-Raster", "Neon grid", "Grille néon", "Rejilla neón", "Griglia neon") \
  X(W_ST7,        "Asteroiden", "Asteroids", "Astéroïdes", "Asteroides", "Asteroidi") \
  X(W_ST8,        "Code-Regen", "Code rain", "Pluie de code", "Lluvia de código", "Pioggia di codice") \
  X(W_ST9,        "Zahnrad-Werk", "Gearworks", "Engrenages", "Engranajes", "Ingranaggi") \
  X(W_ST10,       "Ziegelbrecher", "Brick breaker", "Casse-briques", "Rompeladrillos", "Spaccamattoni") \
  X(W_ST11,       "Froschfurt", "Frog crossing", "Gué des grenouilles", "Cruce de ranas", "Guado delle rane") \
  X(W_ST12,       "Raketenabwehr", "Missile defence", "Défense antimissile", "Defensa antimisiles", "Difesa missilistica") \
  X(W_ST13,       "Neonbrecher", "Neon breaker", "Casse-néon", "Rompeneón", "Spacca-neon") \
  X(W_ST14,       "Pulsierendes Herz", "Beating heart", "Cœur battant", "Corazón latiente", "Cuore pulsante") \
  X(W_ST15,       "Sinuskurve", "Sine wave", "Sinusoïde", "Sinusoide", "Sinusoide") \
  X(W_ST16,       "Badminton", "Badminton", "Badminton", "Bádminton", "Badminton") \
  X(W_ST17,       "Testbild", "Test card", "Mire", "Carta de ajuste", "Monoscopio") \
  X(W_ST18,       "Dackelwelpe", "Dachshund puppy", "Chiot teckel", "Cachorro teckel", "Cucciolo bassotto") \
  X(W_ST19,       "Flugradar", "Flight radar", "Radar aérien", "Radar de vuelos", "Radar voli") \
  X(W_ST20,       "Traumsand", "Dream sand", "Sable des rêves", "Arena de sueños", "Sabbia dei sogni") \
  X(W_ST21,       "Aquarium", "Aquarium", "Aquarium", "Acuario", "Acquario") \
  X(W_ST22,       "Blockwelt", "Block world", "Monde de blocs", "Mundo de bloques", "Mondo a blocchi") \
  X(W_ST23,       "Familie", "Family", "Famille", "Familia", "Famiglia") \
  X(W_ST24,       "ISS-Tracker", "ISS tracker", "Suivi ISS", "Rastreador ISS", "Tracker ISS") \
  X(W_ST25,       "Sonne und Mond", "Sun and moon", "Soleil et lune", "Sol y luna", "Sole e luna") \
  X(W_ST26,       "Weihnachten", "Christmas", "Noël", "Navidad", "Natale") \
  X(W_ST27,       "Weihnachtself", "Christmas elf", "Lutin de Noël", "Elfo navideño", "Elfo di Natale") \
  X(W_ST28,       "Halloween", "Halloween", "Halloween", "Halloween", "Halloween") \
  X(W_ST29,       "Ostern", "Easter", "Pâques", "Pascua", "Pasqua") \
  X(W_ST30,       "Valentinstag", "Valentine's Day", "Saint-Valentin", "San Valentín", "San Valentino") \
  X(W_ST31,       "Silvester", "New Year's Eve", "Saint-Sylvestre", "Nochevieja", "San Silvestro") \
  X(W_ST32,       "Squishy Dumpling", "Squishy dumpling", "Raviolo tout mou", "Dumpling blandito", "Raviolo morbidoso")
