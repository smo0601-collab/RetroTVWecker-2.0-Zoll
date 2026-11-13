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
  X(W_R_BOARD,    "Hardware", "Hardware", "Matériel", "Hardware", "Hardware") \
  X(W_R_PANEL,    "Display-Controller", "Display controller", "Contrôleur d'écran", "Controlador de pantalla", "Controller del display") \
  X(W_R_FW,       "Firmware", "Firmware", "Micrologiciel", "Firmware", "Firmware") \
  X(W_R_RESET,    "Letzter Neustart", "Last restart", "Dernier redémarrage", "Último reinicio", "Ultimo riavvio") \
  X(W_UPTIME,     "l&auml;uft seit", "running for", "en marche depuis", "en marcha desde hace", "attivo da") \
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
  X(W_HOL_CB,     "Feiertags-Designs automatisch in den Design-Wechsel aufnehmen (jeweils 10 Tage vorher bis zum Fest: Weihnachten, Silvester, Valentinstag, Halloween, Ostern)", \
                  "Add holiday designs to the rotation automatically (10 days before until the day: Christmas, New Year's Eve, Valentine's Day, Halloween, Easter)", \
                  "Ajouter automatiquement les designs de fête à la rotation (10 jours avant jusqu'au jour J : Noël, Saint-Sylvestre, Saint-Valentin, Halloween, Pâques)", \
                  "Añadir automáticamente los diseños festivos a la rotación (10 días antes hasta el día: Navidad, Nochevieja, San Valentín, Halloween, Pascua)", \
                  "Aggiungi automaticamente i design delle feste alla rotazione (da 10 giorni prima fino al giorno: Natale, San Silvestro, San Valentino, Halloween, Pasqua)") \
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
  X(W_FINE_SM,    "Kanten gl&auml;tten (Themen-Designs feiner statt Doppelpixel)", "Smooth edges (themed designs finer instead of double pixels)", "Lisser les contours (designs plus fins)", "Suavizar bordes (diseños más finos)", "Ammorbidisci i bordi (design più fini)") \
  X(W_FINE_TX,    "Schrift in voller Aufl&ouml;sung (braucht 38 KB Speicher)", "Text in full resolution (needs 38 KB memory)", "Texte en pleine résolution (38 Ko de mémoire)", "Texto a resolución completa (38 KB de memoria)", "Testo a piena risoluzione (38 KB di memoria)") \
  X(W_FINE_HINT,  "Die Themen-Designs werden in 160&times;120 gezeichnet und auf 320&times;240 vergr&ouml;&szlig;ert. Mit Gl&auml;ttung verschwinden Treppen an schr&auml;gen Kanten; Texte, Datum und Zahlen der Designs werden fein nachgezeichnet. F&uuml;r gro&szlig;e Uhrzeit aus der Ferne: Design &bdquo;Gro&szlig;anzeige&ldquo;.", \
                  "Themed designs are drawn at 160×120 and enlarged to 320×240. Smoothing removes staircases on slanted edges; texts, dates and numbers of the designs are redrawn finely. For a big time readable from afar: design “Big display”.", \
                  "Les designs sont dessinés en 160×120 puis agrandis en 320×240. Le lissage supprime les escaliers ; textes, dates et chiffres sont redessinés finement. Pour une grande heure lisible de loin : design « Grand affichage ».", \
                  "Los diseños se dibujan a 160×120 y se amplían a 320×240. El suavizado quita los escalones; textos, fechas y números se redibujan finos. Para una hora grande legible de lejos: diseño «Pantalla grande».", \
                  "I design sono disegnati a 160×120 e ingranditi a 320×240. L'ammorbidimento toglie le scalette; testi, date e numeri sono ridisegnati fini. Per un'ora grande leggibile da lontano: design «Display grande».") \
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
  X(W_H_LED,      "RGB-LED (R&uuml;ckseite)", "RGB LED (back)", "LED RVB (arri&egrave;re)", "LED RGB (trasera)", "LED RGB (retro)") \
  X(W_LED_MODE,   "Modus", "Mode", "Mode", "Modo", "Modalit&agrave;") \
  X(W_LM0,        "Aus", "Off", "&Eacute;teinte", "Apagado", "Spento") \
  X(W_LM1,        "Feste Farbe", "Fixed colour", "Couleur fixe", "Color fijo", "Colore fisso") \
  X(W_LM2,        "Wetter-Farbe", "Weather colour", "Couleur m&eacute;t&eacute;o", "Color del tiempo", "Colore meteo") \
  X(W_LM3,        "Temperatur-Farbe", "Temperature colour", "Couleur temp&eacute;rature", "Color de temperatura", "Colore temperatura") \
  X(W_LM4,        "Farbwechsel", "Colour cycle", "Cycle de couleurs", "Ciclo de colores", "Ciclo di colori") \
  X(W_LM5,        "Atmen (feste Farbe)", "Breathing (fixed colour)", "Respiration (couleur fixe)", "Respiraci&oacute;n (color fijo)", "Respiro (colore fisso)") \
  X(W_LM6,        "Sekundenpuls (feste Farbe)", "Seconds pulse (fixed colour)", "Pulsation des secondes (couleur fixe)", "Pulso de segundos (color fijo)", "Impulso dei secondi (colore fisso)") \
  X(W_LED_COL,    "Farbe", "Colour", "Couleur", "Color", "Colore") \
  X(W_LED_BRI,    "LED-Helligkeit: ", "LED brightness: ", "Luminosit&eacute; LED : ", "Brillo del LED: ", "Luminosit&agrave; LED: ") \
  X(W_LED_TIME,   "Wann", "When", "Quand", "Cu&aacute;ndo", "Quando") \
  X(W_LT0,        "immer", "always", "toujours", "siempre", "sempre") \
  X(W_LT1,        "nur nachts (Nachtlicht)", "only at night (night light)", "seulement la nuit (veilleuse)", "solo de noche (luz nocturna)", "solo di notte (luce notturna)") \
  X(W_LT2,        "nachts aus", "off at night", "&eacute;teinte la nuit", "apagado de noche", "spento di notte") \
  X(W_LED_HINTS,  "Hinweise (blinken zus&auml;tzlich)", "Alerts (flash on top)", "Alertes (clignotent en plus)", "Avisos (parpadean adem&aacute;s)", "Avvisi (lampeggiano in pi&ugrave;)") \
  X(W_LH0,        "Wecker klingelt (rot)", "Alarm ringing (red)", "R&eacute;veil sonne (rouge)", "Alarma sonando (rojo)", "Sveglia suona (rosso)") \
  X(W_LH1,        "kein WLAN (orange)", "no Wi-Fi (orange)", "pas de Wi-Fi (orange)", "sin WiFi (naranja)", "nessun Wi-Fi (arancione)") \
  X(W_LH2,        "Update l&auml;uft (blau)", "update running (blue)", "mise &agrave; jour en cours (bleu)", "actualizaci&oacute;n en curso (azul)", "aggiornamento in corso (blu)") \
  X(W_LH3,        "Gewitter (violett) und Frost (eisblau)", "thunderstorm (violet) and frost (ice blue)", "orage (violet) et gel (bleu glacier)", "tormenta (violeta) y helada (azul hielo)", "temporale (viola) e gelo (azzurro ghiaccio)") \
  X(W_LED_NOTE,   "&Auml;nderungen sind sofort f&uuml;r 8 Sekunden zu sehen, gespeichert wird mit &bdquo;Speichern&ldquo;. Wetter-Farbe: gelb Sonne, dunkelblau klare Nacht, grau Wolken, blau Regen, wei&szlig; Schnee, violett Gewitter. Temperatur: blau kalt &rarr; gr&uuml;n &rarr; gelb &rarr; rot hei&szlig;. &bdquo;Nachts&ldquo; ist das Dimm-Zeitfenster oben, ohne Zeitplan nach Sonnenstand. Im Standby bleibt die LED aus (au&szlig;er Wecker und Update).", \
                  "Changes show immediately for 8 seconds; &ldquo;Save&rdquo; stores them. Weather colour: yellow sun, dark blue clear night, grey clouds, blue rain, white snow, violet thunderstorm. Temperature: blue cold &rarr; green &rarr; yellow &rarr; red hot. &ldquo;Night&rdquo; is the dimming window above, without a schedule it follows the sun. In standby the LED stays off (except alarm and update).", \
                  "Les changements s'affichent tout de suite pendant 8 secondes ; &laquo; Enregistrer &raquo; les garde. Couleur m&eacute;t&eacute;o : jaune soleil, bleu fonc&eacute; nuit claire, gris nuages, bleu pluie, blanc neige, violet orage. Temp&eacute;rature : bleu froid &rarr; vert &rarr; jaune &rarr; rouge chaud. &laquo; La nuit &raquo; = la plage de gradation ci-dessus, sinon selon le soleil. En veille la LED reste &eacute;teinte (sauf r&eacute;veil et mise &agrave; jour).", \
                  "Los cambios se ven al momento durante 8 segundos; &laquo;Guardar&raquo; los conserva. Color del tiempo: amarillo sol, azul oscuro noche despejada, gris nubes, azul lluvia, blanco nieve, violeta tormenta. Temperatura: azul fr&iacute;o &rarr; verde &rarr; amarillo &rarr; rojo calor. &laquo;De noche&raquo; es la franja de atenuaci&oacute;n de arriba, sin horario seg&uacute;n el sol. En reposo el LED queda apagado (salvo alarma y actualizaci&oacute;n).", \
                  "Le modifiche si vedono subito per 8 secondi; &laquo;Salva&raquo; le memorizza. Colore meteo: giallo sole, blu scuro notte serena, grigio nuvole, blu pioggia, bianco neve, viola temporale. Temperatura: blu freddo &rarr; verde &rarr; giallo &rarr; rosso caldo. &laquo;Di notte&raquo; &egrave; la fascia di attenuazione sopra, senza orario secondo il sole. In standby il LED resta spento (tranne sveglia e aggiornamento).") \
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
  X(W_R_SPK,      "Lautsprecher", "Speaker", "Haut-parleur", "Altavoz", "Altoparlante") \
  X(W_SND_VOL,    "Lautst&auml;rke:", "Volume:", "Volume :", "Volumen:", "Volume:") \
  X(W_SND_HINT,   "&#9654; spielt einen Klang mit der Lautst&auml;rke des Reglers ab &ndash; auch ohne Speichern.", "&#9654; plays a sound at the slider volume &ndash; no need to save first.", \
                  "&#9654; joue le son au volume du curseur &ndash; sans enregistrer.", "&#9654; reproduce el sonido con el volumen del control, sin guardar.", "&#9654; riproduce il suono al volume del cursore, senza salvare.") \
  X(W_SND_IDLE,   "Lautsprecher-Ausgang in Ruhe abschalten (gegen St&ouml;rger&auml;usche)", "Switch speaker output off when silent (against noise)", \
                  "Couper la sortie haut-parleur au repos (contre les parasites)", "Apagar la salida del altavoz en reposo (contra ruidos)", "Spegni l'uscita altoparlante a riposo (contro i disturbi)") \
  X(W_HR_ON,      "Stundengong zur vollen Stunde", "Hourly chime on the hour", "Carillon à chaque heure pile", "Campanada a la hora en punto", "Rintocco allo scoccare dell'ora") \
  X(W_SND_QUIET,  "Ruhe w&auml;hrend der Zeitplan-Dimmung (kein Gong, keine Effekte)", "Quiet during scheduled dimming (no chime, no effects)", \
                  "Silence pendant l'atténuation programmée (ni carillon ni effets)", "Silencio durante la atenuación programada (sin campanada ni efectos)", "Silenzio durante l'attenuazione programmata (niente rintocchi né effetti)") \
  X(W_HRT0,       "Piep-Piep", "Beep-beep", "Bip-bip", "Bip-bip", "Bip-bip") \
  X(W_HRT1,       "Westminster", "Westminster", "Westminster", "Westminster", "Westminster") \
  X(W_HRT2,       "Kuckuck (z&auml;hlt die Stunde)", "Cuckoo (counts the hour)", "Coucou (compte l'heure)", "Cucú (cuenta la hora)", "Cucù (conta l'ora)") \
  X(W_HRT3,       "Glockenschlag (z&auml;hlt die Stunde)", "Bell strikes (counts the hour)", "Cloche (compte l'heure)", "Campanadas (cuentan la hora)", "Rintocchi (contano l'ora)") \
  X(W_HRT4,       "Arcade-M&uuml;nze", "Arcade coin", "Pièce d'arcade", "Moneda arcade", "Moneta arcade") \
  X(W_HRT5,       "Glockenspiel", "Glockenspiel", "Carillon", "Carillón", "Glockenspiel") \
  X(W_H_FX,       "Klang-Effekte", "Sound effects", "Effets sonores", "Efectos de sonido", "Effetti sonori") \
  X(W_FX_POP,     "Feiertags-Einblendung: Melodie passend zum Tag", "Holiday pop-up: a tune to match the day", "Animation de fête : mélodie adaptée au jour", \
                  "Animación de festivo: melodía según el día", "Animazione festiva: melodia adatta al giorno") \
  X(W_FXN0,       "Weihnachten", "Christmas", "Noël", "Navidad", "Natale") \
  X(W_FXN1,       "Silvester / Neujahr", "New Year", "Nouvel An", "Año Nuevo", "Capodanno") \
  X(W_FXN2,       "Karneval (Tusch)", "Carnival (fanfare)", "Carnaval (fanfare)", "Carnaval (fanfarria)", "Carnevale (fanfara)") \
  X(W_FXN3,       "Halloween", "Halloween", "Halloween", "Halloween", "Halloween") \
  X(W_FXN4,       "Valentinstag / Muttertag", "Valentine's / Mother's Day", "Saint-Valentin / Fête des mères", "San Valentín / Día de la Madre", "San Valentino / Festa della mamma") \
  X(W_FXN5,       "Ostern", "Easter", "Pâques", "Pascua", "Pasqua") \
  X(W_FXN6,       "Ferien", "School holidays", "Vacances scolaires", "Vacaciones escolares", "Vacanze scolastiche") \
  X(W_FXN7,       "Feiertag (Tr&ouml;te)", "Holiday (party horn)", "Jour férié (trompette)", "Festivo (matasuegras)", "Festività (trombetta)") \
  X(W_FX_NYE,     "Silvester: letzte 10 Sekunden ticken, um 0 Uhr Feuerwerk", "New Year's Eve: ticking for the last 10 seconds, fireworks at midnight", \
                  "Saint-Sylvestre : tic-tac les 10 dernières secondes, feu d'artifice à minuit", "Nochevieja: tictac los últimos 10 segundos, fuegos artificiales a medianoche", \
                  "San Silvestro: ticchettio negli ultimi 10 secondi, fuochi d'artificio a mezzanotte") \
  X(W_FX_UFO,     "UFO im Flugradar", "UFO on the flight radar", "OVNI sur le radar", "OVNI en el radar", "UFO sul radar") \
  X(W_FX_STYLE,   "Automatischer Designwechsel (kurzer Ton)", "Automatic design change (short tone)", "Changement automatique de design (bip court)", \
                  "Cambio automático de diseño (tono corto)", "Cambio automatico di design (tono breve)") \
  X(W_FX_WX,      "Wetterwechsel: Regen, Schnee oder Gewitter beginnt", "Weather change: rain, snow or a thunderstorm starts", "Changement de météo : pluie, neige ou orage", \
                  "Cambio de tiempo: empieza lluvia, nieve o tormenta", "Cambio meteo: inizia pioggia, neve o temporale") \
  X(W_WX_RAIN,    "Regen", "Rain", "Pluie", "Lluvia", "Pioggia") \
  X(W_WX_SNOW,    "Schnee", "Snow", "Neige", "Nieve", "Neve") \
  X(W_WX_STORM,   "Gewitter", "Thunderstorm", "Orage", "Tormenta", "Temporale") \
  X(W_FX_NOTE,    "Effekte schweigen im Standby, w&auml;hrend der Wecker klingelt und (wenn oben gew&auml;hlt) in der Nachtruhe. Silvester spielt immer, wenn eingeschaltet.", \
                  "Effects stay silent in standby, while the alarm rings and (if selected above) during quiet hours. New Year's Eve always plays when enabled.", \
                  "Les effets se taisent en veille, pendant le réveil et (si choisi plus haut) la nuit. La Saint-Sylvestre joue toujours si activée.", \
                  "Los efectos callan en reposo, mientras suena la alarma y (si se elige arriba) en horas de silencio. Nochevieja suena siempre si está activada.", \
                  "Gli effetti tacciono in standby, mentre suona la sveglia e (se scelto sopra) nelle ore di silenzio. San Silvestro suona sempre se attivo.") \
  X(W_SND_STOP,   "Klang stoppen", "Stop sound", "Arrêter le son", "Detener sonido", "Ferma suono") \
  X(W_AL_TONE,    "Weckton", "Alarm sound", "Sonnerie", "Sonido de alarma", "Suoneria") \
  X(W_ALT0,       "Piepser", "Beeper", "Bips", "Pitidos", "Bip") \
  X(W_ALT1,       "Klingel", "Bell ringer", "Sonnerie classique", "Timbre", "Campanello") \
  X(W_ALT2,       "Fanfare", "Fanfare", "Fanfare", "Fanfarria", "Fanfara") \
  X(W_ALT3,       "Kuckuck", "Cuckoo", "Coucou", "Cucú", "Cucù") \
  X(W_ALT4,       "Gong (Ding-Dong)", "Gong (ding-dong)", "Gong (ding-dong)", "Gong (din-don)", "Gong (din-don)") \
  X(W_ALT5,       "Morgenstimmung (Grieg)", "Morning Mood (Grieg)", "Au matin (Grieg)", "La mañana (Grieg)", "Il mattino (Grieg)") \
  X(W_ALT6,       "Arcade", "Arcade", "Arcade", "Arcade", "Arcade") \
  X(W_ALT7,       "Sirene", "Siren", "Sirène", "Sirena", "Sirena") \
  X(W_ALT8,       "Vogelgezwitscher", "Birdsong", "Chant d'oiseaux", "Canto de pájaros", "Cinguettio") \
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
  X(W_GEO,        "Ort suchen", "Find place", "Chercher un lieu", "Buscar lugar", "Cerca luogo") \
  X(W_GEO_BTN,    "Suchen", "Search", "Chercher", "Buscar", "Cerca") \
  X(W_GEO_PH,     "z.&thinsp;B. Berlin oder 10115", "e.g. London or SW1A", "p.&thinsp;ex. Paris ou 75001", "p.&thinsp;ej. Madrid o 28001", "es. Roma o 00118") \
  X(W_GEO_HINT,   "Ort oder Postleitzahl eingeben und Treffer antippen &ndash; Breiten- und L&auml;ngengrad werden eingetragen. Danach Speichern. Ortsdaten: Open-Meteo / GeoNames.", \
                  "Enter a place or postcode and tap a result &ndash; latitude and longitude are filled in. Then Save. Place data: Open-Meteo / GeoNames.", \
                  "Saisissez un lieu ou un code postal et touchez un résultat &ndash; latitude et longitude sont remplies. Ensuite Enregistrer. Données : Open-Meteo / GeoNames.", \
                  "Escribe un lugar o código postal y toca un resultado &ndash; se rellenan latitud y longitud. Después Guardar. Datos: Open-Meteo / GeoNames.", \
                  "Inserisci un luogo o CAP e tocca un risultato &ndash; latitudine e longitudine vengono compilate. Poi Salva. Dati: Open-Meteo / GeoNames.") \
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
  X(W_R_GAMES,    "Spiele (RetroCYD)", "Games (RetroCYD)", "Jeux (RetroCYD)", "Juegos (RetroCYD)", "Giochi (RetroCYD)") \
  X(W_R_NVS,      "Einstellungsspeicher (NVS)", "Settings storage (NVS)", "Mémoire des réglages (NVS)", "Memoria de ajustes (NVS)", "Memoria impostazioni (NVS)") \
  X(W_NVS_FMT,    "%u von %u Eintr&auml;gen belegt", "%u of %u entries used", "%u sur %u entrées utilisées", "%u de %u entradas usadas", "%u di %u voci usate") \
  X(W_NVS_RESTORED, " &ndash; Einstellungen wurden aus der Sicherung wiederhergestellt", " – settings were restored from the backup", " – réglages restaurés depuis la sauvegarde", " – ajustes restaurados desde la copia", " – impostazioni ripristinate dal backup") \
  X(W_H_GAMES,    "Spiele (RetroCYD)", "Games (RetroCYD)", "Jeux (RetroCYD)", "Juegos (RetroCYD)", "Giochi (RetroCYD)") \
  X(W_G_YES,      "installiert", "installed", "installés", "instalados", "installati") \
  X(W_G_NO,       "nicht installiert &ndash; Datei <b>RetroCYD_&hellip;_update.bin</b> unten bei Firmware-Update einspielen", "not installed – upload <b>RetroCYD_…_update.bin</b> below under firmware update", \
                  "non installés – charger <b>RetroCYD_…_update.bin</b> ci-dessous (mise à jour)", "no instalados – carga <b>RetroCYD_…_update.bin</b> abajo en actualización", "non installati – carica <b>RetroCYD_…_update.bin</b> qui sotto in aggiornamento") \
  X(W_G_START,    "Spiele starten", "Start games", "Lancer les jeux", "Iniciar juegos", "Avvia giochi") \
  X(W_G_STARTED,  "Das CYD startet jetzt RetroCYD. Zur&uuml;ck zur Uhr: im Spiele-Men&uuml; &bdquo;ZUR UHR&ldquo; oder lange auf den Bildschirm dr&uuml;cken.", \
                  "The CYD now starts RetroCYD. Back to the clock: “ZUR UHR” in the games menu or a long press on the screen.", \
                  "Le CYD lance RetroCYD. Retour à l'horloge : « ZUR UHR » dans le menu des jeux ou appui long sur l'écran.", \
                  "El CYD inicia RetroCYD. Volver al reloj: «ZUR UHR» en el menú de juegos o pulsación larga en la pantalla.", \
                  "Il CYD avvia RetroCYD. Ritorno all'orologio: «ZUR UHR» nel menu giochi o pressione lunga sullo schermo.") \
  X(W_TCAL,       "Touch kalibrieren", "Calibrate touch", "Calibrer le tactile", "Calibrar tactil", "Calibra touch") \
  X(W_G_HINT,     "Uhr und Spiele sind zwei Programme. Beim Einschalten w&auml;hlst du am Bildschirm &bdquo;Uhr&ldquo; oder &bdquo;Spiele&ldquo; (sonst startet nach 6 s die Uhr); langes Halten &ouml;ffnet ein Men&uuml; mit &bdquo;Spiele&ldquo; und &bdquo;Standby&ldquo;. <b>Updates</b> f&uuml;r Uhr und Spiele hier einspielen. F&uuml;r ein Uhr-Update wechselt die Uhr dabei kurz in RetroCYD (die Uhr kann sich nicht selbst &uuml;berschreiben) &ndash; die Seite erledigt das von allein. &bdquo;Touch kalibrieren&ldquo; zeigt drei Kreuze zum Antippen.", \
                  "Clock and games are two programs. At power-on you choose “Clock” or “Games” on the screen (otherwise the clock starts after 6 s); a long press opens a menu with “Games” and “Standby”. <b>Updates</b> for clock and games are installed here. For a clock update the clock briefly switches to RetroCYD (it cannot overwrite itself) – the page handles this by itself. “Calibrate touch” shows three crosses to tap.", \
                  "Horloge et jeux sont deux programmes. À l'allumage, choisissez « Horloge » ou « Jeux » à l'écran (sinon l'horloge démarre après 6 s) ; un appui long ouvre un menu avec « Jeux » et « Veille ». <b>Mises à jour</b> de l'horloge et des jeux ici. Pour l'horloge, elle passe brièvement par RetroCYD (elle ne peut pas s'écraser elle-même) – la page s'en charge. « Calibrer le tactile » affiche trois croix à toucher.", \
                  "Reloj y juegos son dos programas. Al encender eliges «Reloj» o «Juegos» en la pantalla (si no, el reloj arranca a los 6 s); una pulsación larga abre un menú con «Juegos» y «En espera». <b>Actualizaciones</b> del reloj y de los juegos aquí. Para actualizar el reloj, pasa un momento a RetroCYD (no puede sobrescribirse a sí mismo) – la página lo hace sola. «Calibrar tactil» muestra tres cruces para tocar.", \
                  "Orologio e giochi sono due programmi. All'accensione scegli «Orologio» o «Giochi» sullo schermo (altrimenti dopo 6 s parte l'orologio); una pressione lunga apre un menu con «Giochi» e «Standby». <b>Aggiornamenti</b> di orologio e giochi qui. Per aggiornare l'orologio, passa un attimo a RetroCYD (non può sovrascrivere se stesso) – la pagina lo fa da sola. «Calibra touch» mostra tre croci da toccare.") \
  X(W_E_CLOCK_IN_GAMES, "Die Spiele (RetroCYD) belegen den zweiten Programmbereich. Uhr-Updates bitte im Spiele-Modus einspielen: Spiele starten > SETUP / WLAN > Firmware-Update. RetroCYD-Dateien müssen \"RetroCYD\" im Namen haben.", \
                  "The games (RetroCYD) occupy the second program slot. Please install clock updates from the games: start games > SETUP / WLAN > firmware update. RetroCYD files must have \"RetroCYD\" in their name.", \
                  "Les jeux (RetroCYD) occupent la seconde zone. Installez la mise à jour de l'horloge depuis les jeux : lancer les jeux > SETUP / WLAN > mise à jour. Les fichiers RetroCYD doivent contenir \"RetroCYD\".", \
                  "Los juegos (RetroCYD) ocupan la segunda zona. Instala la actualización del reloj desde los juegos: iniciar juegos > SETUP / WLAN > actualización. Los archivos RetroCYD deben llevar \"RetroCYD\" en el nombre.", \
                  "I giochi (RetroCYD) occupano la seconda area. Installa l'aggiornamento dell'orologio dai giochi: avvia giochi > SETUP / WLAN > aggiornamento. I file RetroCYD devono avere \"RetroCYD\" nel nome.") \
  X(W_E_NOT_GAMES, "Die Datei ist kein RetroCYD für die Wetteruhr (Version mit \"ZUR UHR\" nötig).", "The file is not a RetroCYD build for the weather clock (version with \"ZUR UHR\" needed).", \
                  "Ce fichier n'est pas un RetroCYD pour l'horloge (version avec \"ZUR UHR\" requise).", "El archivo no es un RetroCYD para el reloj (se necesita la versión con \"ZUR UHR\").", "Il file non è un RetroCYD per l'orologio (serve la versione con \"ZUR UHR\").") \
  X(W_E_NOT_CLOCK, "Die Datei ist keine Wetteruhr-Firmware für das CYD.", "The file is not weather clock firmware for the CYD.", "Ce fichier n'est pas un micrologiciel d'horloge pour le CYD.", \
                  "El archivo no es un firmware del reloj para el CYD.", "Il file non è un firmware dell'orologio per il CYD.") \
  X(W_G_INSTALLED, "RetroCYD installiert. Die Uhr läuft weiter; Spiele über \"Spiele starten\" oder beim Einschalten.", "RetroCYD installed. The clock keeps running; games via \"Start games\" or at power-on.", \
                  "RetroCYD installé. L'horloge continue ; jeux via \"Lancer les jeux\" ou à l'allumage.", "RetroCYD instalado. El reloj sigue; juegos con \"Iniciar juegos\" o al encender.", "RetroCYD installato. L'orologio continua; giochi con \"Avvia giochi\" o all'accensione.") \
  X(W_H_PANEL,    "Display-Anpassung", "Display adjustment", "Réglage de l'écran", "Ajuste de pantalla", "Regolazione display") \
  X(W_PANEL_CTL,  "Display-Controller", "Display controller", "Contrôleur d'écran", "Controlador de pantalla", "Controller del display") \
  X(W_PANEL_AUTO, "Automatisch erkennen", "Detect automatically", "Détection automatique", "Detectar automáticamente", "Rileva automaticamente") \
  X(W_PF0,        "Achsen tauschen (Bild um 90&deg; gedreht und gespiegelt)", "Swap axes (picture rotated 90&deg; and mirrored)", "Échanger les axes (image tournée de 90&deg; et en miroir)", \
                  "Intercambiar ejes (imagen girada 90&deg; y reflejada)", "Scambia assi (immagine ruotata di 90&deg; e speculare)") \
  X(W_PF1,        "Links/rechts spiegeln", "Mirror left/right", "Miroir gauche/droite", "Reflejar izquierda/derecha", "Specchia sinistra/destra") \
  X(W_PF2,        "Oben/unten spiegeln", "Mirror top/bottom", "Miroir haut/bas", "Reflejar arriba/abajo", "Specchia alto/basso") \
  X(W_PF3,        "Rot und Blau tauschen", "Swap red and blue", "Échanger rouge et bleu", "Intercambiar rojo y azul", "Scambia rosso e blu") \
  X(W_PF4,        "Farben invertieren (Negativ)", "Invert colours (negative)", "Inverser les couleurs (négatif)", "Invertir colores (negativo)", "Inverti i colori (negativo)") \
  X(W_PANEL_HINT, "Falls das Bild gedreht, spiegelverkehrt oder in falschen Farben erscheint: zuerst den Controller umstellen (beim CYD gibt es ILI9341 und ST7789), dann einzelne H&auml;kchen setzen. Jede &Auml;nderung wirkt sofort nach &bdquo;Speichern&ldquo;. Ein Bild auf dem Kopf l&auml;sst sich auch unter Anzeige &gt; Ausrichtung drehen.", \
                  "If the picture is rotated, mirrored or in wrong colours: first switch the controller (the CYD comes with ILI9341 or ST7789), then tick single options. Every change applies right after “Save”. An upside-down picture can also be turned under Display &gt; Orientation.", \
                  "Si l'image est tournée, en miroir ou dans de mauvaises couleurs : changez d'abord de contrôleur (le CYD existe en ILI9341 et ST7789), puis cochez les options une par une. Chaque changement s'applique dès « Enregistrer ». Une image à l'envers se tourne aussi sous Affichage &gt; Orientation.", \
                  "Si la imagen aparece girada, reflejada o con colores erróneos: cambia primero el controlador (el CYD existe con ILI9341 y ST7789) y luego marca opciones una a una. Cada cambio se aplica al pulsar «Guardar». Una imagen al revés también se gira en Pantalla &gt; Orientación.", \
                  "Se l'immagine è ruotata, speculare o con colori sbagliati: cambia prima il controller (il CYD esiste con ILI9341 e ST7789), poi spunta le opzioni una alla volta. Ogni modifica vale subito dopo «Salva». Un'immagine capovolta si gira anche in Display &gt; Orientamento.") \
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
  X(J_RELAY,      "Die Uhr wechselt kurz in den Update-Modus (RetroCYD) und kommt danach mit der neuen Version zurück. Bitte warten ...", \
                  "The clock briefly switches to update mode (RetroCYD) and comes back with the new version. Please wait ...", \
                  "L'horloge passe brièvement en mode mise à jour (RetroCYD) puis revient avec la nouvelle version. Patientez ...", \
                  "El reloj pasa un momento al modo de actualización (RetroCYD) y vuelve con la nueva versión. Espera ...", \
                  "L'orologio passa un attimo in modalità aggiornamento (RetroCYD) e torna con la nuova versione. Attendere ...") \
  X(J_GEO_WAIT,   "Suche ...", "Searching ...", "Recherche ...", "Buscando ...", "Ricerca ...") \
  X(J_GEO_NONE,   "Nichts gefunden. Anders schreiben oder größeren Ort in der Nähe versuchen.", "Nothing found. Try another spelling or a larger place nearby.", \
                  "Rien trouvé. Essayez une autre orthographe ou un lieu plus grand à proximité.", "No se encontró nada. Prueba otra escritura o un lugar mayor cercano.", \
                  "Nessun risultato. Prova un'altra grafia o un luogo più grande vicino.") \
  X(J_GEO_ERR,    "Suche nicht möglich (keine Internetverbindung?). Koordinaten bitte von Hand eintragen.", "Search not possible (no internet?). Please enter the coordinates by hand.", \
                  "Recherche impossible (pas d'Internet ?). Saisissez les coordonnées à la main.", "Búsqueda imposible (¿sin Internet?). Escribe las coordenadas a mano.", \
                  "Ricerca impossibile (niente Internet?). Inserisci le coordinate a mano.") \
  X(J_GEO_OK,     "Übernommen: ", "Applied: ", "Repris : ", "Aplicado: ", "Applicato: ") \
  X(J_GEO_SAVE,   " &ndash; bitte Speichern.", " &ndash; please Save.", " &ndash; puis Enregistrer.", " &ndash; luego Guardar.", " &ndash; poi Salva.") \
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
  X(W_E_BOARD,    "Falsche Firmware für dieses Board (CYD-Dateien enthalten \"CYD\" im Namen)", "Wrong firmware for this board (CYD files have \"CYD\" in their name)", \
                  "Mauvais micrologiciel pour cette carte (les fichiers CYD contiennent \"CYD\")", "Firmware incorrecto para esta placa (los archivos CYD llevan \"CYD\" en el nombre)", \
                  "Firmware errato per questa scheda (i file CYD hanno \"CYD\" nel nome)") \
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
  X(W_CAT6,       "Himmel &amp; Weltraum", "Sky &amp; space", "Ciel &amp; espace", "Cielo y espacio", "Cielo e spazio") \
  X(W_CAT7,       "Feiertage", "Holidays", "Fêtes", "Fiestas", "Feste") \
  /* Designs (gleiche Reihenfolge wie die Design-Nummern) */ \
  X(W_ST0,        "Flip-Karten", "Flip cards", "Cartes à rabat", "Tarjetas flip", "Cartelle flip") \
  X(W_ST1,        "Digital + Balken", "Digital + bar", "Numérique + barre", "Digital + barra", "Digitale + barra") \
  X(W_ST2,        "Abenteuer", "Adventure", "Aventure", "Aventura", "Avventura") \
  X(W_ST3,        "Fallende Bl&ouml;cke", "Falling blocks", "Blocs qui tombent", "Bloques que caen", "Blocchi cadenti") \
  X(W_ST5,        "H&uuml;pfwelt", "Jump world", "Monde à sauts", "Mundo de saltos", "Mondo dei salti") \
  X(W_ST6,        "Neon-Raster", "Neon grid", "Grille néon", "Rejilla neón", "Griglia neon") \
  X(W_ST7,        "Asteroiden", "Asteroids", "Astéroïdes", "Asteroides", "Asteroidi") \
  X(W_ST8,        "Code-Regen", "Code rain", "Pluie de code", "Lluvia de código", "Pioggia di codice") \
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
  X(W_ST24,       "ISS-Tracker", "ISS tracker", "Suivi ISS", "Rastreador ISS", "Tracker ISS") \
  X(W_ST25,       "Sonne und Mond", "Sun and moon", "Soleil et lune", "Sol y luna", "Sole e luna") \
  X(W_ST26,       "Weihnachten", "Christmas", "Noël", "Navidad", "Natale") \
  X(W_ST28,       "Halloween", "Halloween", "Halloween", "Halloween", "Halloween") \
  X(W_ST29,       "Ostern", "Easter", "Pâques", "Pascua", "Pasqua") \
  X(W_ST30,       "Valentinstag", "Valentine's Day", "Saint-Valentin", "San Valentín", "San Valentino") \
  X(W_ST31,       "Silvester", "New Year's Eve", "Saint-Sylvestre", "Nochevieja", "San Silvestro") \
  X(W_ST27,       "Gro&szlig;anzeige", "Big display", "Grand affichage", "Pantalla grande", "Display grande") \
  X(W_ST_CAL,     "Kalender", "Calendar", "Calendrier", "Calendario", "Calendario") \
  X(W_TAB_CAL,    "Kalender", "Calendar", "Calendrier", "Calendario", "Calendario") \
  X(W_CAL_BL,     "Bundesland", "German state", "Land (Allemagne)", "Estado federado", "Land (Germania)") \
  X(W_CAL_NONE,   "keins (nur bundesweite Feiertage)", "none (nationwide holidays only)", "aucun (jours f&eacute;ri&eacute;s nationaux)", "ninguno (solo festivos nacionales)", "nessuno (solo festivit&agrave; nazionali)") \
  X(W_CAL_SHOW,   "Anzeigen", "Show", "Afficher", "Mostrar", "Mostra") \
  X(W_CAL_PUB,    "Gesetzliche Feiertage", "Public holidays", "Jours f&eacute;ri&eacute;s", "Festivos", "Festivit&agrave;") \
  X(W_CAL_SPEC,   "Besondere Tage (Valentinstag, Rosenmontag, Muttertag, Halloween, Nikolaus, Advent &hellip;)", "Special days (Valentine's, carnival, Mother's Day, Halloween, St. Nicholas, Advent &hellip;)", "Jours sp&eacute;ciaux (Saint-Valentin, carnaval, f&ecirc;te des m&egrave;res, Halloween, Saint-Nicolas, Avent &hellip;)", "D&iacute;as especiales (San Valent&iacute;n, carnaval, D&iacute;a de la Madre, Halloween, San Nicol&aacute;s, Adviento &hellip;)", "Giorni speciali (San Valentino, carnevale, festa della mamma, Halloween, San Nicola, Avvento &hellip;)") \
  X(W_CAL_SCHOOL, "Schulferien (aus dem Internet, OpenHolidays)", "School holidays (from the internet, OpenHolidays)", "Vacances scolaires (Internet, OpenHolidays)", "Vacaciones escolares (Internet, OpenHolidays)", "Vacanze scolastiche (Internet, OpenHolidays)") \
  X(W_CAL_POP,    "Am Feiertag regelm&auml;&szlig;ig ein lustiges Bild einblenden (20 Sekunden, 7&ndash;22 Uhr)", "Show a fun picture regularly on holidays (20 seconds, 7am&ndash;10pm)", "Afficher r&eacute;guli&egrave;rement une image amusante les jours de f&ecirc;te (20 s, 7h&ndash;22h)", "Mostrar una imagen divertida en festivos (20 s, 7&ndash;22 h)", "Mostrare un&rsquo;immagine divertente nei giorni di festa (20 s, 7&ndash;22)") \
  X(W_CAL_TICK,   "Countdown und Spr&uuml;che als Laufschrift", "Countdown and sayings as scrolling text", "Compte &agrave; rebours et dictons en texte d&eacute;filant", "Cuenta atr&aacute;s y frases en texto deslizante", "Conto alla rovescia e frasi come testo scorrevole") \
  X(W_CAL_POPMIN, "Einblenden alle", "Show every", "Afficher toutes les", "Mostrar cada", "Mostra ogni") \
  X(W_CAL_TICKMIN,"Laufschrift alle", "Scrolling text every", "Texte d&eacute;filant toutes les", "Texto cada", "Testo ogni") \
  X(W_CAL_MIN,    "Minuten", "minutes", "minutes", "minutos", "minuti") \
  X(W_CAL_TEST,   "Einblendung ansehen", "Preview picture", "Aper&ccedil;u de l'image", "Ver imagen", "Anteprima immagine") \
  X(W_CAL_TESTBTN,"Jetzt 20 Sekunden zeigen", "Show for 20 seconds now", "Afficher 20 secondes", "Mostrar 20 segundos", "Mostra per 20 secondi") \
  X(W_CAL_HINT,   "Das Design &bdquo;Kalender&ldquo; (Anzeige) zeigt das Monatsblatt. Feiertage werden in der Uhr berechnet, Schulferien kommen von openholidaysapi.org und werden gespeichert (Abruf einmal pro Woche). Ma&szlig;geblich sind die Angaben des Kultusministeriums.", \
                  "The &ldquo;Calendar&rdquo; design (Display) shows the month sheet. Holidays are calculated in the clock, school holidays come from openholidaysapi.org and are stored (weekly update). The education ministry's dates are authoritative.", \
                  "Le design &laquo; Calendrier &raquo; (Affichage) montre le mois. Les jours f&eacute;ri&eacute;s sont calcul&eacute;s dans l'horloge, les vacances viennent de openholidaysapi.org (mise &agrave; jour hebdomadaire).", \
                  "El dise&ntilde;o &laquo;Calendario&raquo; (Pantalla) muestra el mes. Los festivos se calculan en el reloj, las vacaciones vienen de openholidaysapi.org (cada semana).", \
                  "Il design &laquo;Calendario&raquo; (Display) mostra il mese. Le festivit&agrave; sono calcolate nell'orologio, le vacanze arrivano da openholidaysapi.org (ogni settimana).") \
  X(W_H_CALYEAR,  "Jahres&uuml;bersicht", "Year overview", "Aper&ccedil;u de l'ann&eacute;e", "Resumen del a&ntilde;o", "Panoramica dell'anno") \
  X(J_F_MERGED,   "Das ist eine Erstinstallations-Datei (mit Bootloader). Sie wird nur per USB eingespielt (Adresse 0x0). Hier bitte die Datei *_update.bin wählen.", \
                  "This is a first-install file (with bootloader). It can only be flashed via USB (address 0x0). Please choose the *_update.bin file here.", \
                  "Fichier de première installation (avec bootloader) : uniquement par USB (adresse 0x0). Choisissez ici le fichier *_update.bin.", \
                  "Es un archivo de primera instalación (con bootloader): solo por USB (dirección 0x0). Elige aquí el archivo *_update.bin.", \
                  "File di prima installazione (con bootloader): solo via USB (indirizzo 0x0). Scegli qui il file *_update.bin.") \
  X(J_F_BOARD,    "Diese Datei ist für ein anderes Board (CYD bzw. ESP32-C3) und wird nicht eingespielt.", "This file is for another board (CYD or ESP32-C3) and will not be installed.", \
                  "Ce fichier est pour une autre carte (CYD ou ESP32-C3).", "Este archivo es para otra placa (CYD o ESP32-C3).", "Questo file è per un'altra scheda (CYD o ESP32-C3).") \
  X(J_F_BAD,      "Das ist keine passende Firmware-Datei.", "This is not a suitable firmware file.", "Ce n'est pas un fichier de firmware adapté.", "No es un archivo de firmware adecuado.", "Non è un file firmware adatto.") \
  X(J_F_OLD,      "Datei nicht erkannt (zu alte Version?). Bitte eine aktuelle *_update.bin verwenden.", "File not recognised (too old?). Please use a current *_update.bin.", \
                  "Fichier non reconnu (trop ancien ?). Utilisez un *_update.bin actuel.", "Archivo no reconocido (¿demasiado antiguo?). Usa un *_update.bin actual.", "File non riconosciuto (troppo vecchio?). Usa un *_update.bin attuale.") \
  X(J_F_CHECK,    "Prüfe Datei ...", "Checking file ...", "Vérification ...", "Comprobando ...", "Verifica ...") \
  X(J_F_GAMES,    "Erkannt: RetroCYD (Spiele) - wird neben der Uhr installiert.", "Detected: RetroCYD (games) - installed next to the clock.", \
                  "Détecté : RetroCYD (jeux) - installé à côté de l'horloge.", "Detectado: RetroCYD (juegos) - se instala junto al reloj.", "Rilevato: RetroCYD (giochi) - installato accanto all'orologio.") \
  X(J_F_CLOCK,    "Erkannt: Uhr-Update.", "Detected: clock update.", "Détecté : mise à jour de l'horloge.", "Detectado: actualización del reloj.", "Rilevato: aggiornamento dell'orologio.") \
  X(J_F_GSTART,   "Spiele jetzt starten?", "Start the games now?", "Lancer les jeux maintenant ?", "¿Iniciar los juegos ahora?", "Avviare i giochi ora?") \
  X(W_E_MERGED,   "Erstinstallations-Datei (mit Bootloader) - nur per USB. Bitte *_update.bin verwenden.", "First-install file (with bootloader) - USB only. Please use *_update.bin.", \
                  "Fichier de première installation - USB uniquement. Utilisez *_update.bin.", "Archivo de primera instalación - solo USB. Usa *_update.bin.", "File di prima installazione - solo USB. Usa *_update.bin.") \
  X(J_CAL_PUB,    "Feiertag", "Public holiday", "Jour férié", "Festivo", "Festività") \
  X(J_CAL_SPEC,   "Besonderer Tag", "Special day", "Jour spécial", "Día especial", "Giorno speciale") \
  X(J_CAL_FER,    "Schulferien", "School holidays", "Vacances scolaires", "Vacaciones escolares", "Vacanze scolastiche") \
  X(J_CAL_TODAY,  "Heute", "Today", "Aujourd'hui", "Hoy", "Oggi") \
  X(J_CAL_FEROK,  "Schulferien geladen: ", "School holidays loaded: ", "Vacances chargées : ", "Vacaciones cargadas: ", "Vacanze caricate: ") \
  X(J_CAL_FERNO,  "Noch keine Schulferien geladen - kommt kurz nach dem Speichern (Internet nötig).", "No school holidays loaded yet - arrives shortly after saving (internet needed).", "Pas encore de vacances - arrivent après l'enregistrement (Internet nécessaire).", "Aún no hay vacaciones - llegan tras guardar (se necesita Internet).", "Nessuna vacanza ancora - arriva dopo il salvataggio (serve Internet).") \
  X(J_CAL_FERERR, "Letzter Abruf fehlgeschlagen: ", "Last download failed: ", "Dernier téléchargement échoué : ", "Última descarga fallida: ", "Ultimo download fallito: ")
