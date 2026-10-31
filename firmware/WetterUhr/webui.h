#pragma once
/*
  Einstellungsseite im Browser (WebServer, Port 80).

  Wird in WetterUhr.ino NACH den Definitionen von gSet, setBrightness(),
  applyTimezone(), getWeather(), timeValid(), staleMs() usw. eingebunden.

  Sicherheit: Ohne Web-Passwort ist die Seite im Heimnetz fuer jeden erreichbar.
  Setze daher ein Web-Passwort (Benutzername: admin). Das WLAN-Passwort wird
  nie an den Browser zurueckgegeben. HTTP ist unverschluesselt.
*/
#include <WebServer.h>
#include <ESPmDNS.h>

static WebServer server(80);
static bool      needReboot = false;
static uint32_t  rebootAt = 0;
static bool      mdnsStarted = false;

struct TzEntry { const char* name; const char* posix; };
static const TzEntry TZS[] = {
  {"Europa/Berlin, Paris, Rom (MEZ/MESZ)", "CET-1CEST,M3.5.0,M10.5.0/3"},
  {"Europa/London, Dublin",                "GMT0BST,M3.5.0/1,M10.5.0"},
  {"Europa/Lissabon",                      "WET0WEST,M3.5.0/1,M10.5.0"},
  {"Europa/Athen, Helsinki (OEZ)",         "EET-2EEST,M3.5.0/3,M10.5.0/4"},
  {"Europa/Moskau",                        "MSK-3"},
  {"USA Ostkueste (New York)",             "EST5EDT,M3.2.0,M11.1.0"},
  {"USA Zentral (Chicago)",                "CST6CDT,M3.2.0,M11.1.0"},
  {"USA Berge (Denver)",                   "MST7MDT,M3.2.0,M11.1.0"},
  {"USA Westkueste (Los Angeles)",         "PST8PDT,M3.2.0,M11.1.0"},
  {"Dubai",                                "<+04>-4"},
  {"Indien (Kolkata)",                     "IST-5:30"},
  {"Japan (Tokio)",                        "JST-9"},
  {"Australien Ost (Sydney)",              "AEST-10AEDT,M10.1.0,M4.1.0/3"},
  {"UTC",                                  "UTC0"},
};

// ---------- Hilfsfunktionen ----------
static String esc(const String& s) {
  String o;
  o.reserve(s.length() + 8);
  for (size_t i = 0; i < s.length(); i++) {
    char c = s[i];
    switch (c) {
      case '&':  o += F("&amp;");  break;
      case '<':  o += F("&lt;");   break;
      case '>':  o += F("&gt;");   break;
      case '"':  o += F("&quot;"); break;
      case '\'': o += F("&#39;");  break;
      default:   o += c;
    }
  }
  return o;
}

static String jsonEsc(const String& s) {
  String o;
  for (size_t i = 0; i < s.length(); i++) {
    char c = s[i];
    if (c == '"' || c == '\\') { o += '\\'; o += c; }
    else if ((uint8_t)c < 0x20) { /* Steuerzeichen weglassen */ }
    else o += c;
  }
  return o;
}

static String hhmm(uint16_t m) {
  char b[8];
  snprintf(b, sizeof(b), "%02u:%02u", (unsigned)(m / 60), (unsigned)(m % 60));
  return String(b);
}

static bool parseHhmm(const String& s, uint16_t& out) {
  int h = -1, m = -1;
  if (sscanf(s.c_str(), "%d:%d", &h, &m) != 2) return false;
  if (h < 0 || h > 23 || m < 0 || m > 59) return false;
  out = (uint16_t)(h * 60 + m);
  return true;
}

static bool authOk() {
  if (gSet.webPw[0] == 0) return true;
  if (server.authenticate("admin", gSet.webPw)) return true;
  server.requestAuthentication();
  return false;
}

static String cleanHost(String h) {
  h.toLowerCase();
  String o;
  for (size_t i = 0; i < h.length() && o.length() < 23; i++) {
    char c = h[i];
    if ((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '-') o += c;
  }
  return o;
}

// ---------- Seite ----------
static const char PAGE_HEAD[] PROGMEM = R"=====(<!doctype html><html lang="de"><head><meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1"><title>Wetteruhr</title>
<style>
body{font-family:system-ui,sans-serif;background:#0d1117;color:#e6edf3;margin:0}
h1{font-size:1.3rem} h2{font-size:1rem;color:#58a6ff;margin:1.4em 0 .4em}
label{display:block;margin:.7em 0 .2em;color:#9da7b3;font-size:.9rem}
input,select{width:100%;box-sizing:border-box;padding:.55em;background:#161b22;color:#e6edf3;border:1px solid #30363d;border-radius:6px;font-size:1rem}
input[type=checkbox]{width:auto;margin-right:.5em} input[type=range]{padding:0}
button{margin-top:1em;padding:.7em 1.2em;border:0;border-radius:6px;background:#238636;color:#fff;font-size:1rem}
button.sec{background:#30363d;margin-left:.5em} button.red{background:#8e1519}
.ok{background:#12351f;border:1px solid #238636;padding:.6em;border-radius:6px;margin:.6em 0}
.warn{background:#3a2a0a;border:1px solid #9e6a03;padding:.6em;border-radius:6px;margin:.6em 0;font-size:.9rem}
table{width:100%;font-size:.9rem} td{padding:.15em 0;vertical-align:top} td:first-child{color:#9da7b3;width:40%}
:root{--acc:#00c8ff}
.cards{display:grid;grid-template-columns:1fr 1fr;gap:8px;margin-top:.4em}
.card{display:block;border:2px solid #30363d;border-radius:10px;padding:5px;cursor:pointer;position:relative;font-size:.85rem;color:#9da7b3;text-align:center}
.chips{display:flex;flex-wrap:wrap;gap:6px;margin:.5em 0}.chips a{border:1px solid #30363d;border-radius:16px;padding:4px 12px;font-size:.85rem;color:#9da7b3;cursor:pointer}.chips a.on{border-color:var(--acc);color:#e6edf3}.cg{display:block;margin-top:.8em;font-weight:600}.card.sel{border-color:var(--acc);color:#e6edf3}
.card input,.sw input{position:absolute;opacity:0;pointer-events:none}
.card svg{display:block;margin-bottom:3px;border-radius:6px}
.sw{display:inline-block;position:relative;width:34px;height:34px;border-radius:50%;border:3px solid #0d1117;box-shadow:0 0 0 1px #30363d;margin:4px;cursor:pointer}
.sw.sel{box-shadow:0 0 0 3px #e6edf3}
small{color:#9da7b3}
.app{display:flex;gap:18px;max-width:900px;margin:0 auto;padding:14px}
nav{display:flex;flex-direction:column;gap:4px;width:170px;flex:none;position:sticky;top:14px;align-self:flex-start}
nav a{padding:.6em .9em;border-radius:8px;color:#9da7b3;cursor:pointer;white-space:nowrap;user-select:none}
nav a.on{background:#161b22;color:#e6edf3;box-shadow:inset 3px 0 var(--acc)}
main{flex:1;min-width:0}
main h1{margin-top:.2em}
.tab{display:none}.tab.on{display:block}
.hid{position:absolute;left:-9999px;width:1px;height:1px;padding:0;margin:0;opacity:0}
#savebar{margin-top:1.4em;border-top:1px solid #30363d;padding-top:.2em}
@media(max-width:640px){
  .app{flex-direction:column;gap:6px;padding:10px}
  nav{flex-direction:row;width:auto;overflow-x:auto;position:sticky;top:0;background:#0d1117;padding:6px 0;z-index:5;border-bottom:1px solid #30363d}
  nav a.on{box-shadow:inset 0 -3px var(--acc)}
}
.sm{margin:.15em 0;padding:.25em .55em;min-width:2.2em;font-size:.95rem;background:#30363d}</style></head><body>
)=====";

static const char PAGE_JS[] PROGMEM = R"=====(<script>
const $=id=>document.getElementById(id);
$('br').oninput=e=>{
  $('bv').textContent=e.target.value;
  clearTimeout(window.bt);
  window.bt=setTimeout(()=>fetch('/brightness',{method:'POST',
    headers:{'Content-Type':'application/x-www-form-urlencoded'},body:'v='+e.target.value}),150);
};
async function scan(){
  const b=$('sb'); b.disabled=true; b.textContent='Suche ...';
  try{
    for(let i=0;i<12;i++){
      const r=await (await fetch('/scan')).json();
      if(r.state==='done'){
        $('ssids').innerHTML=r.list.map(s=>'<option value="'+s.replace(/"/g,'&quot;')+'">').join('');
        b.textContent='Gefunden: '+r.list.length; b.disabled=false; return;
      }
      await new Promise(r=>setTimeout(r,1500));
    }
  }catch(e){}
  b.textContent='Netzwerke suchen'; b.disabled=false;
}
function paint(){
  const ac=document.querySelector('input[name=acc]:checked');
  if(ac) document.documentElement.style.setProperty('--acc',accHex[ac.value]);
  document.querySelectorAll('.card,.sw').forEach(c=>c.classList.toggle('sel',c.querySelector('input').checked));
}
function tkIns(ch){
  const t=document.querySelector('input[name=tk]'),a=t.selectionStart||t.value.length,b=t.selectionEnd||a;
  t.value=t.value.slice(0,a)+ch+t.value.slice(b); t.focus(); const p=a+ch.length; t.setSelectionRange(p,p);
}
function tkPlay(){
  const b=document.getElementById('tkplay'); b.disabled=true;
  fetch('/ticker',{method:'POST'}).finally(()=>setTimeout(()=>{b.disabled=false;},1500));
}
function look(){
  paint();
  const cs=document.querySelector('input[name=cs]:checked').value,
        ac=document.querySelector('input[name=acc]:checked').value;
  fetch('/look',{method:'POST',headers:{'Content-Type':'application/x-www-form-urlencoded'},
    body:'cs='+cs+'&acc='+ac});
}
document.querySelectorAll('input[name=cs],input[name=acc]').forEach(i=>i.onchange=look);
paint();
function show(t){
  document.querySelectorAll('.tab').forEach(e=>e.classList.toggle('on',e.id==='t-'+t));
  document.querySelectorAll('#nav a').forEach(a=>a.classList.toggle('on',a.dataset.t===t));
  $('savebar').style.display=(t==='status')?'none':'block';
  try{sessionStorage.setItem('tab',t);}catch(e){}
}
document.querySelectorAll('#nav a').forEach(a=>a.onclick=()=>show(a.dataset.t));
let t0='status';
try{t0=sessionStorage.getItem('tab')||t0;}catch(e){}
if(!document.getElementById('t-'+t0)) t0='status';
show(t0);
</script></body></html>
)=====";


// ---------- Uhr-Designs: Namen und Vorschaubilder (SVG, folgen der Akzentfarbe) ----------
static const char* const STYLE_NAME[CLOCK_STYLE_COUNT] = {
  "Flip-Karten", "Digital + Balken",                                                                       // 0-1 klassisch
  "Abenteuer", "Fallende Bloecke", "Labyrinth", "Huepfwelt", "Neon-Raster", "Asteroiden", "Code-Regen",    // 2-8
  "Zahnrad-Werk", "Ziegelbrecher", "Froschfurt", "Raketenabwehr", "Neonbrecher",                          // 9-13
  "Pulsierendes Herz", "Sinuskurve", "Badminton", "Testbild", "Dackelwelpe",                              // 14-18
  "Flugradar", "Traumsand", "Aquarium", "Blockwelt", "Familie", "ISS-Tracker", "Sonne und Mond",          // 19-25
  "Weihnachten", "Weihnachtself", "Halloween", "Ostern", "Valentinstag", "Silvester",                     // 26-31 Feiertage
  "Squishy Dumpling"                                                                                       // 32
};

#define CAT_COUNT 8
static const char* const CAT_NAME[CAT_COUNT] = {"Klassische Uhren", "Spiele &amp; Arcade", "Tiere &amp; Natur", "Technik &amp; Retro", "Traum &amp; Gef&uuml;hl", "Heimat &amp; Familie", "Himmel &amp; Weltraum", "Feiertage"};
static const uint8_t STYLE_CAT[CLOCK_STYLE_COUNT] = {
  0, 0,                                         // 0 Flip-Karten, 1 Digital + Balken
  1, 1, 1, 1, 3, 1, 3, 3, 1, 1, 1, 1,           // 2 Abenteuer .. 13 Neonbrecher (6 Neon-Raster, 8 Code-Regen, 9 Zahnrad-Werk = Technik)
  4, 3, 1, 3, 2,                                // 14 Herz, 15 Sinuskurve, 16 Badminton, 17 Testbild, 18 Dackel
  6, 4, 2, 1, 5, 6, 6,                          // 19 Flugradar, 20 Traumsand, 21 Aquarium, 22 Blockwelt, 23 Familie, 24 ISS, 25 Sonne und Mond
  7, 7, 7, 7, 7, 7,                             // 26-31 Feiertage
  4                                             // 32 Squishy Dumpling
};

#define SVG_OPEN "<svg viewBox='0 0 120 70' width='100%'><rect width='120' height='70' rx='6' fill='#000'/>"
static const char SVG_7[] PROGMEM = SVG_OPEN
  "<rect x='8' y='10' width='49' height='42' rx='5' fill='#222'/><rect x='63' y='10' width='49' height='42' rx='5' fill='#222'/>"
  "<g font-family='sans-serif' font-weight='bold' font-size='26' text-anchor='middle'><text x='32' y='42' fill='#eee'>12</text>"
  "<text x='87' y='42' style='fill:var(--acc)'>34</text></g>"
  "<rect x='8' y='30' width='49' height='1.5' fill='#000'/><rect x='63' y='30' width='49' height='1.5' fill='#000'/>"
  "<text x='60' y='64' text-anchor='middle' font-family='sans-serif' font-size='8' fill='#aaa'>Freitag, 23.10.2026</text></svg>";
static const char SVG_8[] PROGMEM = SVG_OPEN
  "<text x='60' y='40' text-anchor='middle' font-family='sans-serif' font-weight='bold' font-size='34' fill='#eee'>12:34</text>"
  "<rect x='10' y='48' width='100' height='5' rx='2.5' fill='#222'/><rect x='10' y='48' width='62' height='5' rx='2.5' style='fill:var(--acc)'/>"
  "<text x='60' y='65' text-anchor='middle' font-family='sans-serif' font-size='8' fill='#aaa'>Freitag, 23.10.2026</text></svg>";

// Themen-Designs (Pixel-Retro): feste Farben, unabhaengig von der Akzentfarbe
static const char SVG_10[] PROGMEM =
  "<svg viewBox='0 0 120 70' width='100%'><rect width='120' height='70' rx='6' fill='#0a0c26'/>"
  "<circle cx='104' cy='12' r='5' fill='#eeeed6'/><circle cx='106.5' cy='10.5' r='4' fill='#0a0c26'/>"
  "<path d='M0 48 Q30 36 60 46 T120 44 V70 H0Z' fill='#1e3f42'/><rect y='58' width='120' height='12' fill='#22542c'/>"
  "<path d='M14 50l5-14 5 14zM96 50l5-14 5 14zM64 50l4-11 4 11z' fill='#0e2e28'/>"
  "<rect x='14' y='8' width='92' height='26' rx='3' fill='#4a4c60' stroke='#787c96' stroke-width='2'/>"
  "<text x='60' y='28' text-anchor='middle' font-family='monospace' font-weight='bold' font-size='20' fill='#ffcc40'>12:34</text>"
  "<rect x='56' y='48' width='4' height='9' fill='#40b054'/><rect x='56' y='45' width='4' height='3' fill='#f4ccA4'/></svg>";
static const char SVG_11[] PROGMEM =
  "<svg viewBox='0 0 120 70' width='100%'><rect width='120' height='70' rx='6' fill='#121426'/>"
  "<rect x='74' y='0' width='46' height='70' fill='#0c0c18'/>"
  "<g fill='#e63232'><rect x='76' y='64' width='6' height='6'/><rect x='82' y='64' width='6' height='6'/><rect x='82' y='58' width='6' height='6'/></g>"
  "<g fill='#00dce6'><rect x='88' y='64' width='6' height='6'/><rect x='94' y='64' width='6' height='6'/><rect x='100' y='64' width='6' height='6'/><rect x='106' y='64' width='6' height='6'/></g>"
  "<g fill='#f0dc00'><rect x='100' y='58' width='6' height='6'/><rect x='106' y='58' width='6' height='6'/></g>"
  "<g fill='#aa3cdc'><rect x='94' y='58' width='6' height='6'/></g>"
  "<text x='6' y='24' font-family='monospace' font-weight='bold' font-size='17' fill='#3ce0e8'>12:34</text>"
  "<rect x='6' y='30' width='56' height='3' fill='#2a2e50'/><rect x='6' y='30' width='34' height='3' fill='#f0dc00'/>"
  "<rect x='6' y='42' width='16' height='16' fill='none' stroke='#5a64a0'/><rect x='10' y='48' width='8' height='4' fill='#fa961e'/></svg>";
static const char SVG_12[] PROGMEM =
  "<svg viewBox='0 0 120 70' width='100%'><rect width='120' height='70' rx='6' fill='#000'/>"
  "<rect x='8' y='8' width='104' height='54' rx='8' fill='none' stroke='#2846dc' stroke-width='2'/>"
  "<rect x='20' y='20' width='80' height='30' rx='4' fill='none' stroke='#2846dc' stroke-width='2'/>"
  "<text x='60' y='42' text-anchor='middle' font-family='monospace' font-weight='bold' font-size='20' fill='#ffeb78'>12:34</text>"
  "<g fill='#c8aa78'><rect x='30' y='13' width='2' height='2'/><rect x='44' y='13' width='2' height='2'/><rect x='58' y='13' width='2' height='2'/></g>"
  "<circle cx='76' cy='14' r='3.5' fill='#aaaab8'/><circle cx='95' cy='14' r='4' fill='#f08228'/><circle cx='14' cy='40' r='4' fill='#9646c8'/>"
  "<path d='M9 56l8 0 0-6z' fill='#ffd228'/></svg>";
static const char SVG_13[] PROGMEM =
  "<svg viewBox='0 0 120 70' width='100%'><rect width='120' height='70' rx='6' fill='#5c94fc'/>"
  "<g fill='#f0be3c' stroke='#96600a' stroke-width='1.5'><rect x='9' y='9' width='16' height='16'/><rect x='28' y='9' width='16' height='16'/>"
  "<rect x='76' y='9' width='16' height='16'/><rect x='95' y='9' width='16' height='16'/></g>"
  "<g font-family='monospace' font-weight='bold' font-size='13' fill='#5a2d00' text-anchor='middle'><text x='17' y='22'>1</text><text x='36' y='22'>2</text>"
  "<text x='84' y='22'>3</text><text x='103' y='22'>4</text></g>"
  "<ellipse cx='30' cy='40' rx='9' ry='4' fill='#f8f8fc'/><ellipse cx='88' cy='36' rx='9' ry='4' fill='#f8f8fc'/>"
  "<rect y='56' width='120' height='14' fill='#be5014'/><path d='M0 62H120M0 68H120' stroke='#6e2c00'/>"
  "<circle cx='60' cy='50' r='6' fill='#3cc88c'/><rect x='62' y='47' width='3' height='4' fill='#fff'/></svg>";
static const char SVG_14[] PROGMEM =
  "<svg viewBox='0 0 120 70' width='100%'><rect width='120' height='70' rx='6' fill='#000'/>"
  "<g stroke='#00506e' stroke-width='.8'><path d='M12 0V70M24 0V70M36 0V70M48 0V70M60 0V70M72 0V70M84 0V70M96 0V70M108 0V70"
  "M0 11H120M0 22H120M0 33H120M0 44H120M0 55H120'/></g>"
  "<path d='M24 11H48V33' fill='none' stroke='#8ff' stroke-width='1.6'/>"
  "<text x='60' y='42' text-anchor='middle' font-family='monospace' font-weight='bold' font-size='24' fill='#bff' stroke='#00a0d8' stroke-width='.8'>12:34</text>"
  "<rect x='3' y='3' width='114' height='64' fill='none' stroke='#ff9600' stroke-width='1' stroke-dasharray='28 400'/></svg>";
static const char SVG_15[] PROGMEM =
  "<svg viewBox='0 0 120 70' width='100%'><rect width='120' height='70' rx='6' fill='#000'/>"
  "<g fill='none' stroke='#aab0c4' stroke-width='1'><path d='M12 14l8-4 7 5-2 8-9 2-5-6zM92 48l9-3 6 6-3 8-8 1-6-6z'/><path d='M26 52l4-6 5 3-3 6z'/></g>"
  "<text x='60' y='42' text-anchor='middle' font-family='monospace' font-size='26' fill='none' stroke='#e6eeff' stroke-width='1'>12:34</text>"
  "<path d='M60 56l-3 8 3-2 3 2z' fill='none' stroke='#fff'/><path d='M60 54V44' stroke='#fff' stroke-dasharray='2 3'/></svg>";
static const char SVG_16[] PROGMEM =
  "<svg viewBox='0 0 120 70' width='100%'><rect width='120' height='70' rx='6' fill='#000'/>"
  "<g font-family='monospace' font-size='7' fill='#0a8a38'><text x='6' y='9'>7 A 3</text><text x='6' y='33'>K 0 X</text><text x='100' y='15'>9 N</text><text x='98' y='60'>Z 4 #</text><text x='8' y='62'>= H</text></g>"
  "<g font-family='monospace' font-size='7' fill='#46ff7a'><text x='7' y='17'>2</text><text x='101' y='23'>E</text><text x='99' y='68'>R</text></g>"
  "<rect x='20' y='14' width='80' height='24' fill='#000' stroke='#00a03c'/>"
  "<text x='60' y='33' text-anchor='middle' font-family='monospace' font-weight='bold' font-size='18' fill='#a8ffc0'>12:34</text>"
  "<rect x='20' y='42' width='80' height='16' fill='#000' stroke='#00a03c'/>"
  "<text x='25' y='51' font-family='monospace' font-size='6' fill='#00eb5f'>&gt; 12C  H63%</text><text x='25' y='56' font-family='monospace' font-size='6' fill='#00eb5f'>&gt; WIND 14_</text></svg>";

static const char SVG_17[] PROGMEM =
  "<svg viewBox='0 0 120 70' width='100%'><rect width='120' height='70' rx='6' fill='#000'/>"
  "<text x='60' y='22' text-anchor='middle' font-family='monospace' font-weight='bold' font-size='22' fill='#fff4d2' stroke='#6e1814' stroke-width='.8'>12:34</text>"
  "<g fill='#c83228'><rect x='0' y='28' width='105' height='2'/><rect x='15' y='44' width='105' height='2'/><rect x='0' y='60' width='105' height='2'/></g>"
  "<g stroke='#3c78ff' stroke-width='1' fill='none'><path d='M30 30V44M27 33h6M27 37h6M27 41h6M92 46V60M89 49h6M89 53h6M89 57h6'/></g>"
  "<rect x='4' y='14' width='12' height='14' fill='#7886b4'/><circle cx='10' cy='20' r='2.5' fill='#ff3c3c'/>"
  "<circle cx='60' cy='26' r='3' fill='#ff961e'/><circle cx='40' cy='42' r='3' fill='#ff961e'/><rect x='64' y='51' width='4' height='8' fill='#3c6ee6'/><rect x='64' y='48' width='4' height='3' fill='#ff961e'/></svg>";
static const char SVG_18[] PROGMEM =
  "<svg viewBox='0 0 120 70' width='100%'><rect width='120' height='70' rx='6' fill='#000'/>"
  "<g><rect x='3' y='4' width='114' height='4' fill='#e62828'/><rect x='3' y='10' width='114' height='4' fill='#fa8c1e'/><rect x='3' y='16' width='114' height='4' fill='#f0dc28'/>"
  "<rect x='3' y='22' width='114' height='4' fill='#32c846'/><rect x='3' y='28' width='114' height='4' fill='#3c6ef0'/></g>"
  "<rect x='45' y='4' width='30' height='28' fill='#000'/>"
  "<text x='60' y='52' text-anchor='middle' font-family='monospace' font-weight='bold' font-size='22' fill='#ffd23c'>12:34</text>"
  "<rect x='72' y='58' width='22' height='3' fill='#c8d2e6'/><rect x='50' y='40' width='3' height='3' fill='#fff'/></svg>";
static const char SVG_19[] PROGMEM =
  "<svg viewBox='0 0 120 70' width='100%'><rect width='120' height='70' rx='6' fill='#1e6e28'/>"
  "<rect x='20' y='2' width='80' height='20' fill='#0a2c16'/>"
  "<text x='60' y='19' text-anchor='middle' font-family='monospace' font-weight='bold' font-size='18' fill='#fff'>12:34</text>"
  "<rect y='26' width='120' height='16' fill='#1446aa'/><rect x='10' y='30' width='24' height='5' fill='#8c5622'/><rect x='70' y='36' width='24' height='5' fill='#8c5622'/>"
  "<rect y='42' width='120' height='4' fill='#1e6e28'/><rect y='46' width='120' height='16' fill='#32323e'/>"
  "<rect x='14' y='49' width='12' height='5' fill='#e63232'/><rect x='70' y='49' width='12' height='5' fill='#f0d228'/><rect x='40' y='56' width='12' height='5' fill='#28d2e6'/>"
  "<rect x='56' y='62' width='7' height='6' fill='#64eb46'/></svg>";
static const char SVG_20[] PROGMEM =
  "<svg viewBox='0 0 120 70' width='100%'><rect width='120' height='70' rx='6' fill='#000'/>"
  "<path d='M10 0L40 40M96 0L82 42' stroke='#ff4632' stroke-width='.8'/><path d='M60 60L58 32' stroke='#4682ff'/>"
  "<circle cx='58' cy='30' r='5' fill='#ff961e'/><circle cx='58' cy='30' r='3' fill='#ffe678'/>"
  "<rect x='16' y='14' width='88' height='30' fill='#000' stroke='#4682ff'/>"
  "<text x='60' y='39' text-anchor='middle' font-family='monospace' font-weight='bold' font-size='22' fill='#a0cdff'>12:34</text>"
  "<g fill='#283e82'><rect x='4' y='56' width='8' height='10'/><rect x='14' y='52' width='9' height='14'/><rect x='25' y='57' width='7' height='9'/><rect x='36' y='50' width='9' height='16'/><rect x='48' y='55' width='8' height='11'/>"
  "<rect x='60' y='52' width='9' height='14'/><rect x='72' y='57' width='8' height='9'/><rect x='84' y='51' width='9' height='15'/><rect x='96' y='56' width='8' height='10'/></g>"
  "<rect y='66' width='120' height='4' fill='#5a2c18'/></svg>";
static const char SVG_21[] PROGMEM =
  "<svg viewBox='0 0 120 70' width='100%'><rect width='120' height='70' rx='6' fill='#000'/>"
  "<rect x='1' y='1' width='118' height='68' fill='none' stroke='#00c8ff'/>"
  "<text x='60' y='19' text-anchor='middle' font-family='monospace' font-weight='bold' font-size='17' fill='#78f0ff'>12:34</text>"
  "<path d='M3 26H117' stroke='#ff3cd2'/>"
  "<g><rect x='5' y='31' width='20' height='4' fill='#00e6ff'/><rect x='28' y='31' width='20' height='4' fill='#00e6ff'/><rect x='51' y='31' width='20' height='4' fill='#00e6ff'/>"
  "<rect x='5' y='37' width='20' height='4' fill='#ff28c8'/><rect x='51' y='37' width='20' height='4' fill='#ff28c8'/><rect x='74' y='37' width='20' height='4' fill='#ff28c8'/>"
  "<rect x='28' y='43' width='20' height='4' fill='#ffe628'/><rect x='74' y='43' width='20' height='4' fill='#ffe628'/><rect x='97' y='43' width='16' height='4' fill='#3cff78'/></g>"
  "<rect x='46' y='62' width='28' height='3' fill='#78f0ff'/><rect x='62' y='54' width='3' height='3' fill='#fff'/></svg>";
static const char SVG_22[] PROGMEM =
  "<svg viewBox='0 0 120 70' width='100%'><rect width='120' height='70' rx='6' fill='#14061a'/>"
  "<text x='60' y='19' text-anchor='middle' font-family='monospace' font-weight='bold' font-size='17' fill='#ffecf2'>12:34</text>"
  "<circle cx='60' cy='30' r='12' fill='none' stroke='#82204a'/>"
  "<path d='M60 49C46 40 48 26 54 26C58 26 60 29 60 31C60 29 62 26 66 26C72 26 74 40 60 49Z' fill='#eb2850'/>"
  "<path d='M0 60H34L38 56L42 62L46 46L50 66L54 60H70L74 56L78 60H120' fill='none' stroke='#78ffa0' stroke-width='1.2'/>"
  "<rect x='8' y='66' width='70' height='2' fill='#c83c6e'/></svg>";
static const char SVG_23[] PROGMEM =
  "<svg viewBox='0 0 120 70' width='100%'><rect width='120' height='70' rx='6' fill='#030c10'/>"
  "<g fill='#0e2c34'><rect x='0' y='35' width='120' height='.6'/><rect x='60' y='6' width='.6' height='58'/></g>"
  "<path d='M0 35Q15 8 30 35T60 35T90 35T120 35' fill='none' stroke='#28e6ff' stroke-width='1.4'/>"
  "<path d='M0 35Q8 50 15 35T30 35T45 35T60 35T75 35T90 35T105 35T120 35' fill='none' stroke='#ff3cd2' stroke-width='1'/>"
  "<path d='M0 35Q12 22 24 36T52 36T84 28T120 38' fill='none' stroke='#ffe83c' stroke-width='1.6'/>"
  "<rect x='20' y='25' width='80' height='21' fill='#020a0e'/>"
  "<text x='60' y='43' text-anchor='middle' font-family='monospace' font-weight='bold' font-size='22' fill='#ebfffa'>12:34</text>"
  "<rect x='6' y='58' width='108' height='1' fill='#0e2c34'/><rect x='68' y='54' width='4' height='8' fill='#ff8c28'/></svg>";
static const char SVG_24[] PROGMEM =
  "<svg viewBox='0 0 120 70' width='100%'><rect width='120' height='70' rx='6' fill='#1c3a4a'/>"
  "<rect y='56' width='120' height='14' fill='#a8703a'/><path d='M0 56H120' stroke='#f0f0e6'/>"
  "<text x='60' y='22' text-anchor='middle' font-family='monospace' font-weight='bold' font-size='20' fill='#fff' stroke='#081420' stroke-width='.6'>12:34</text>"
  "<rect x='59' y='38' width='2' height='20' fill='#c8c8d2'/><rect x='52' y='38' width='16' height='2' fill='#fff'/>"
  "<g fill='#e03838'><circle cx='24' cy='40' r='3' fill='#f0c096'/><rect x='22' y='43' width='5' height='8'/></g><path d='M25 44L33 34' stroke='#ddd'/><circle cx='35' cy='32' r='3' fill='none' stroke='#ddd'/>"
  "<g fill='#3c78eb'><rect x='94' y='43' width='5' height='8'/></g><circle cx='96' cy='40' r='3' fill='#f0c096'/><path d='M95 44L87 34' stroke='#ddd'/><circle cx='85' cy='32' r='3' fill='none' stroke='#ddd'/>"
  "<path d='M38 36Q60 8 82 36' fill='none' stroke='#fff' stroke-dasharray='1 3'/><rect x='58' y='20' width='3' height='3' fill='#ff9628'/>"
  "<path d='M22 51v5M26 51v5M95 51v5M98 51v5' stroke='#eee'/></svg>";
static const char SVG_25[] PROGMEM =
  "<svg viewBox='0 0 120 70' width='100%'><rect width='120' height='70' rx='6' fill='#4a4a50'/>"
  "<path d='M0 0L120 70M120 0L0 70' stroke='#606068'/>"
  "<circle cx='60' cy='33' r='22' fill='none' stroke='#eee' stroke-width='1.5'/>"
  "<g><rect x='6' y='5' width='15' height='13' fill='#ebebeb'/><rect x='21' y='5' width='15' height='13' fill='#ebeb14'/><rect x='36' y='5' width='15' height='13' fill='#14ebeb'/>"
  "<rect x='51' y='5' width='15' height='13' fill='#14dc1e'/><rect x='66' y='5' width='15' height='13' fill='#eb1eeb'/><rect x='81' y='5' width='15' height='13' fill='#eb1e1e'/><rect x='96' y='5' width='15' height='13' fill='#1e1eeb'/></g>"
  "<rect x='10' y='24' width='100' height='22' fill='#060609' stroke='#fff'/>"
  "<text x='60' y='41' text-anchor='middle' font-family='monospace' font-weight='bold' font-size='17' fill='#f5f5eb'>12:34</text>"
  "<g><rect x='6' y='52' width='13' height='8' fill='#000'/><rect x='19' y='52' width='13' height='8' fill='#242424'/><rect x='32' y='52' width='13' height='8' fill='#484848'/><rect x='45' y='52' width='13' height='8' fill='#6c6c6c'/>"
  "<rect x='58' y='52' width='13' height='8' fill='#909090'/><rect x='71' y='52' width='13' height='8' fill='#b4b4b4'/><rect x='84' y='52' width='13' height='8' fill='#d8d8d8'/><rect x='97' y='52' width='13' height='8' fill='#fff'/></g>"
  "<path d='M60 62L56 68H64Z' fill='#fff'/></svg>";
static const char SVG_26[] PROGMEM =
  "<svg viewBox='0 0 120 70' width='100%'><rect width='120' height='70' rx='6' fill='#6eb6ff'/><rect y='36' width='120' height='34' fill='#56b046'/>"
  "<text x='60' y='25' text-anchor='middle' font-family='monospace' font-weight='bold' font-size='20' fill='#fff' stroke='#163470' stroke-width='.6'>12:34</text>"
  "<rect x='34' y='47' width='38' height='9' rx='4' fill='#221c1e'/><rect x='38' y='54' width='4' height='6' fill='#221c1e'/><rect x='46' y='54' width='4' height='6' fill='#221c1e'/><rect x='62' y='54' width='4' height='6' fill='#221c1e'/>"
  "<circle cx='78' cy='46' r='6' fill='#221c1e'/><rect x='80' y='46' width='11' height='6' rx='2' fill='#c47028'/><rect x='72' y='44' width='5' height='13' rx='2' fill='#221c1e'/>"
  "<path d='M34 50Q28 44 25 40' stroke='#221c1e' stroke-width='3' fill='none'/><circle cx='14' cy='57' r='4' fill='#e83c3c'/></svg>";
static const char SVG_30[] PROGMEM =
  "<svg viewBox='0 0 120 70' width='100%'><rect width='120' height='70' rx='6' fill='#000'/>"
  "<circle cx='42' cy='35' r='31' fill='#02140c' stroke='#1a7842'/><circle cx='42' cy='35' r='20' fill='none' stroke='#0c4226'/><circle cx='42' cy='35' r='10' fill='none' stroke='#0c4226'/>"
  "<path d='M42 4V66M11 35H73' stroke='#0c4226'/><path d='M42 35L66 22' stroke='#6eff9f' stroke-width='1.2'/>"
  "<path d='M30 18L36 27L26 25Z' fill='#ffe646'/><path d='M58 44L50 47L53 39Z' fill='#ff8c3c'/><path d='M24 46L33 44L30 52Z' fill='#5ae6ff'/><path d='M42 31v8M38 35h8' stroke='#fff'/>"
  "<text x='98' y='24' text-anchor='middle' font-family='monospace' font-weight='bold' font-size='20' fill='#f0ffec'>14</text>"
  "<text x='98' y='46' text-anchor='middle' font-family='monospace' font-weight='bold' font-size='20' fill='#f0ffec'>37</text>"
  "<path d='M80 52H116' stroke='#1a7842'/><text x='98' y='62' text-anchor='middle' font-family='monospace' font-size='8' fill='#ffbe3c'>DLH4BC</text></svg>";
static const char SVG_31[] PROGMEM =
  "<svg viewBox='0 0 120 70' width='100%'><rect width='120' height='70' rx='6' fill='#14143e'/>"
  "<circle cx='104' cy='12' r='7' fill='#ffecaa'/><circle cx='107' cy='10' r='6' fill='#14143e'/>"
  "<g fill='#fff4c8'><rect x='14' y='8' width='1.5' height='1.5'/><rect x='40' y='16' width='1.5' height='1.5'/><rect x='70' y='6' width='1.5' height='1.5'/><rect x='90' y='28' width='1.5' height='1.5'/></g>"
  "<text x='60' y='34' fill='#ffeebe' font-size='16' font-family='monospace' text-anchor='middle'>21:45</text>"
  "<ellipse cx='28' cy='60' rx='20' ry='7' fill='#c8cef0'/><rect x='22' y='42' width='10' height='14' fill='#7850c8'/><circle cx='27' cy='38' r='5' fill='#ffcdaa'/><path d='M22 37L32 37L26 24Z' fill='#7850c8'/><path d='M23 40L31 40L27 50Z' fill='#f0f0fa'/>"
  "<g fill='#ffdc6e'><rect x='40' y='44' width='1.5' height='1.5'/><rect x='46' y='50' width='1.5' height='1.5'/><rect x='52' y='56' width='1.5' height='1.5'/><rect x='58' y='61' width='1.5' height='1.5'/><rect x='50' y='46' width='1.5' height='1.5'/></g></svg>";

static const char SVG_33[] PROGMEM =
  "<svg viewBox='0 0 120 70' width='100%'><rect width='120' height='70' rx='6' fill='#0a3e6e'/>"
  "<text x='60' y='15' fill='#f0fcff' font-size='12' font-family='monospace' text-anchor='middle'>21:45</text>"
  "<rect y='58' width='120' height='12' fill='#d6be82'/><path d='M12 58q-3-12 2-20M30 58q4-10-1-16M96 58q-4-14 1-22' stroke='#28aa5a' stroke-width='2' fill='none'/>"
  "<ellipse cx='42' cy='34' rx='9' ry='5' fill='#ff8c28'/><path d='M33 34l-7-5v10z' fill='#fff0e6'/><ellipse cx='78' cy='44' rx='8' ry='4.5' fill='#3c96ff'/><path d='M86 44l7-5v10z' fill='#145ab8'/>"
  "<circle cx='60' cy='24' r='2' fill='none' stroke='#aadcff'/><circle cx='100' cy='36' r='1.5' fill='none' stroke='#aadcff'/></svg>";
static const char SVG_35[] PROGMEM =
  "<svg viewBox='0 0 120 70' width='100%'><rect width='120' height='70' rx='6' fill='#6ebeff'/>"
  "<text x='60' y='15' fill='#fff' font-size='12' font-family='monospace' text-anchor='middle'>21:45</text>"
  "<rect y='46' width='120' height='24' fill='#866040'/><rect y='46' width='120' height='5' fill='#5abe46'/><rect x='80' y='30' width='4' height='16' fill='#6e4c2c'/><rect x='72' y='22' width='20' height='9' fill='#368c36'/>"
  "<rect x='34' y='36' width='7' height='10' fill='#28aabe'/><rect x='34' y='28' width='7' height='7' fill='#e8be96'/><rect x='34' y='27' width='7' height='3' fill='#5a381e'/></svg>";
static const char SVG_36[] PROGMEM =
  "<svg viewBox='0 0 120 70' width='100%'><rect width='120' height='70' rx='6' fill='#78bef5'/><rect y='52' width='120' height='18' fill='#60ac4e'/>"
  "<rect x='34' y='3' width='52' height='13' fill='#ece2ce' stroke='#6e5030'/><text x='60' y='13' fill='#50464a' font-size='9' font-family='monospace' text-anchor='middle'>21:45</text>"
  "<rect x='14' y='28' width='11' height='24' fill='#3864be'/><circle cx='19' cy='24' r='5' fill='#ffd4b0'/><rect x='14' y='26' width='10' height='3' fill='#dEb860'/>"
  "<rect x='30' y='30' width='10' height='22' fill='#c83c46'/><circle cx='35' cy='26' r='5' fill='#ffd4b0'/><rect x='29' y='22' width='11' height='4' fill='#643c26'/>"
  "<rect x='46' y='30' width='9' height='22' fill='#28aaaa'/><circle cx='50' cy='26' r='5' fill='#ffd4b0'/><rect x='45' y='22' width='10' height='4' fill='#e8c070'/>"
  "<rect x='60' y='31' width='9' height='21' fill='#f0c83c'/><circle cx='64' cy='27' r='5' fill='#ffd4b0'/><rect x='59' y='23' width='10' height='4' fill='#f6e49e'/>"
  "<rect x='76' y='40' width='7' height='12' fill='#f082aa'/><circle cx='79' cy='36' r='4' fill='#ffd4b0'/><rect x='75' y='33' width='8' height='3' fill='#c49a54'/></svg>";

static const char SVG_38[] PROGMEM =
  "<svg viewBox='0 0 120 70' width='100%'><rect width='120' height='70' rx='6' fill='#06081a'/>"
  "<rect x='4' y='14' width='112' height='48' fill='#16549f'/><path d='M60 14H112V62H78Z' fill='#06123a'/>"
  "<path d='M10 28q10-8 20 0t10 10-8 14-12-4-10-20M54 22q12-6 26 2t14 12-6 14-26-6-8-22' fill='#48964c'/>"
  "<circle cx='52' cy='34' r='9' fill='none' stroke='#6ebefa'/><rect x='50' y='32' width='5' height='5' fill='#fff'/><path d='M20 56q30-40 70-34' stroke='#ffdc78' stroke-dasharray='2 3' fill='none'/>"
  "<text x='6' y='11' fill='#f4f8ff' font-size='9' font-family='monospace'>21:45</text></svg>";
static const char SVG_39[] PROGMEM =
  "<svg viewBox='0 0 120 70' width='100%'><rect width='120' height='70' rx='6' fill='#0a0e22'/>"
  "<text x='6' y='12' fill='#fff8e6' font-size='11' font-family='monospace'>21:45</text>"
  "<rect x='6' y='16' width='108' height='30' fill='#4682c8'/><rect x='6' y='34' width='108' height='12' fill='#0c102c'/>"
  "<path d='M6 44Q36 4 60 8T114 44' stroke='#ffd646' stroke-width='2' fill='none'/><path d='M6 44Q50 28 80 20T114 40' stroke='#d6e0f0' fill='none'/>"
  "<line x1='88' y1='16' x2='88' y2='46' stroke='#ff7878'/><circle cx='88' cy='42' r='3' fill='#ffd646'/>"
  "<text x='6' y='58' fill='#ffd646' font-size='7' font-family='monospace'>AUF 08:09</text><text x='6' y='66' fill='#fff' font-size='7' font-family='monospace'>UNT 18:21</text>"
  "<circle cx='100' cy='58' r='8' fill='#2c3246'/><path d='M100 50A8 8 0 0 1 100 66A5 8 0 0 0 100 50Z' fill='#ecebd6'/></svg>";

// Feiertage
static const char SVG_40[] PROGMEM =
  "<svg viewBox='0 0 120 70' width='100%'><rect width='120' height='70' rx='6' fill='#0e1840'/>"
  "<rect y='58' width='120' height='12' fill='#ecf2ff'/><path d='M19 8l-12 20h6l-9 16h7l-8 15h32l-8-15h7l-9-16h6z' fill='#167838'/>"
  "<rect x='16' y='58' width='6' height='5' fill='#6e4222'/><circle cx='19' cy='7' r='2.5' fill='#ffe250'/>"
  "<g fill='#ff4040'><circle cx='14' cy='30' r='1.3'/><circle cx='24' cy='42' r='1.3'/><circle cx='12' cy='52' r='1.3'/></g>"
  "<g fill='#50aaff'><circle cx='22' cy='24' r='1.3'/><circle cx='16' cy='44' r='1.3'/><circle cx='28' cy='54' r='1.3'/></g>"
  "<text x='80' y='22' text-anchor='middle' font-family='monospace' font-weight='bold' font-size='20' fill='#fff8e8'>12:34</text>"
  "<text x='80' y='42' text-anchor='middle' font-family='monospace' font-weight='bold' font-size='14' fill='#fff8e8'>57 <tspan font-size='9'>TAGE</tspan></text>"
  "<g fill='#c81e28'><rect x='62' y='48' width='4' height='9'/><rect x='72' y='48' width='4' height='9'/><rect x='82' y='48' width='4' height='9'/><rect x='92' y='48' width='4' height='9'/></g>"
  "<g fill='#ffd860'><rect x='63' y='44' width='2' height='3'/><rect x='73' y='44' width='2' height='3'/></g></svg>";
static const char SVG_41[] PROGMEM =
  "<svg viewBox='0 0 120 70' width='100%'><rect width='120' height='70' rx='6' fill='#704024'/>"
  "<rect y='58' width='120' height='12' fill='#a87442'/><rect x='4' y='6' width='26' height='18' fill='#121c4a' stroke='#462614' stroke-width='2'/>"
  "<text x='76' y='22' text-anchor='middle' font-family='monospace' font-weight='bold' font-size='20' fill='#fff0c8'>12:34</text>"
  "<rect x='50' y='44' width='70' height='5' fill='#32343c'/><rect x='78' y='37' width='8' height='7' fill='#dc3240'/><rect x='100' y='37' width='8' height='7' fill='#3c6ee6'/>"
  "<ellipse cx='14' cy='52' rx='12' ry='10' fill='#966838'/>"
  "<path d='M30 22l10 8h-12z' fill='#28aa46'/><rect x='30' y='29' width='12' height='2' fill='#dc2832'/><circle cx='36' cy='35' r='5' fill='#ffcea8'/>"
  "<rect x='31' y='40' width='10' height='12' fill='#28aa46'/><rect x='32' y='52' width='3' height='5' fill='#dc2832'/><rect x='37' y='52' width='3' height='5' fill='#dc2832'/></svg>";
static const char SVG_42[] PROGMEM =
  "<svg viewBox='0 0 120 70' width='100%'><rect width='120' height='70' rx='6' fill='#2a0e4a'/>"
  "<circle cx='16' cy='14' r='10' fill='#faf0c4'/><rect y='58' width='120' height='12' fill='#0a0412'/>"
  "<path d='M10 58l4-30 4 30zM14 36l-8-8M15 34l9-9' stroke='#06020c' stroke-width='2' fill='#06020c'/>"
  "<text x='76' y='22' text-anchor='middle' font-family='monospace' font-weight='bold' font-size='20' fill='#ff9620'>12:34</text>"
  "<ellipse cx='98' cy='54' rx='14' ry='10' fill='#ff8214'/><rect x='97' y='41' width='3' height='4' fill='#466420'/>"
  "<path d='M90 50l3-4 3 4zM100 50l3-4 3 4zM90 56h16l-3 3-2-2-3 3-3-3-2 2z' fill='#ffec6e'/>"
  "<path d='M56 52a7 7 0 0 1 14 0v10l-2-2-2 2-2-2-2 2-2-2-2 2z' fill='#f0f0fa'/><circle cx='60' cy='51' r='1.3'/><circle cx='66' cy='51' r='1.3'/>"
  "<path d='M30 34l4 2 4-2-2 3h-4z' fill='#000'/></svg>";
static const char SVG_43[] PROGMEM =
  "<svg viewBox='0 0 120 70' width='100%'><rect width='120' height='70' rx='6' fill='#96d0fc'/>"
  "<circle cx='12' cy='12' r='7' fill='#ffe65a'/><ellipse cx='60' cy='76' rx='80' ry='24' fill='#6ec85a'/>"
  "<g font-family='monospace' font-weight='bold' font-size='20' text-anchor='middle'><text x='60' y='24'><tspan fill='#f05a96'>1</tspan><tspan fill='#f5aa14'>2</tspan><tspan fill='#e64678'>:</tspan><tspan fill='#28aa5a'>3</tspan><tspan fill='#8c5aeb'>4</tspan></text></g>"
  "<ellipse cx='84' cy='56' rx='9' ry='6' fill='#c4a078'/><circle cx='93' cy='49' r='5' fill='#c4a078'/>"
  "<rect x='91' y='34' width='3' height='12' rx='1.5' fill='#c4a078'/><rect x='95' y='36' width='3' height='11' rx='1.5' fill='#c4a078'/>"
  "<ellipse cx='30' cy='58' rx='4' ry='5' fill='#ff788c'/><ellipse cx='42' cy='60' rx='4' ry='5' fill='#78aaff'/><ellipse cx='108' cy='60' rx='4' ry='5' fill='#ffd246'/>"
  "<path d='M26 58h8M38 60h8' stroke='#fff' stroke-width='1'/></svg>";
static const char SVG_44[] PROGMEM =
  "<svg viewBox='0 0 120 70' width='100%'><rect width='120' height='70' rx='6' fill='#aa3264'/>"
  "<path d='M0 2a2 2 0 0 1 4 0a2 2 0 0 1 4 0q0 3-4 6q-4-3-4-6z' fill='#ff5a78' transform='translate(8 30) scale(1.4)'/>"
  "<path d='M0 2a2 2 0 0 1 4 0a2 2 0 0 1 4 0q0 3-4 6q-4-3-4-6z' fill='#ffd2e2' transform='translate(100 8) scale(1.2)'/>"
  "<path d='M0 2a2 2 0 0 1 4 0a2 2 0 0 1 4 0q0 3-4 6q-4-3-4-6z' fill='#ff8cb0' transform='translate(30 52) scale(1)'/>"
  "<text x='60' y='24' text-anchor='middle' font-family='monospace' font-weight='bold' font-size='20' fill='#fff'>12:34</text>"
  "<text x='60' y='42' text-anchor='middle' font-family='monospace' font-weight='bold' font-size='12' fill='#ffecf4'>109 TAGE</text>"
  "<rect x='86' y='50' width='26' height='15' fill='#fff4e8'/><path d='M86 50l13 8 13-8' fill='none' stroke='#dcc8be'/>"
  "<path d='M0 2a2 2 0 0 1 4 0a2 2 0 0 1 4 0q0 3-4 6q-4-3-4-6z' fill='#dc1e3c' transform='translate(96 54) scale(.8)'/></svg>";
static const char SVG_45[] PROGMEM =
  "<svg viewBox='0 0 120 70' width='100%'><rect width='120' height='70' rx='6' fill='#060a26'/>"
  "<g stroke-width='1.2'><path d='M22 20l-8-6M22 20l8-6M22 20l-10 2M22 20l10 2M22 20l-6 8M22 20l6 8M22 20v-10' stroke='#ff5064'/>"
  "<path d='M96 16l-7-5M96 16l7-5M96 16l-9 2M96 16l9 2M96 16l-5 7M96 16l5 7M96 16v-9' stroke='#5ae6ff'/></g>"
  "<text x='60' y='34' text-anchor='middle' font-family='monospace' font-weight='bold' font-size='20' fill='#fff6dc'>12:34</text>"
  "<text x='60' y='46' text-anchor='middle' font-family='monospace' font-weight='bold' font-size='9' fill='#ffe080'>BIS 2027</text>"
  "<path d='M0 70V58h8v-6h8v8h8v-12h8v10h8v-6h8v4h8v-14h8v12h8v-8h8v6h8v-10h8v8h8v-4h8v12z' fill='#0c0c18' transform='translate(0 9)'/>"
  "<g fill='#ffd26e' transform='translate(0 6)'><rect x='19' y='60' width='1.5' height='1.5'/><rect x='51' y='62' width='1.5' height='1.5'/><rect x='67' y='56' width='1.5' height='1.5'/><rect x='99' y='62' width='1.5' height='1.5'/></g></svg>";

static const char SVG_46[] PROGMEM =
  "<svg viewBox='0 0 120 70' width='100%'><rect width='120' height='70' rx='6' fill='#ffe4ce'/>"
  "<text x='60' y='19' text-anchor='middle' font-family='monospace' font-weight='bold' font-size='17' fill='#784632'>12:34</text>"
  "<path d='M38 60q0-34 22-34t22 34z' fill='#fcf8f0' stroke='#c8baa6'/><path d='M60 26l-6 6M60 26l6 6M60 26v7' stroke='#c8baa6'/>"
  "<circle cx='52' cy='45' r='1.8' fill='#32221e'/><circle cx='68' cy='45' r='1.8' fill='#32221e'/>"
  "<ellipse cx='48' cy='50' rx='3' ry='1.5' fill='#ff96a0'/><ellipse cx='72' cy='50' rx='3' ry='1.5' fill='#ff96a0'/>"
  "<path d='M57 50l1.5 1.5 1.5-1.5 1.5 1.5 1.5-1.5' fill='none' stroke='#c84650'/>"
  "<rect x='26' y='56' width='68' height='14' fill='#c49652'/><path d='M26 60h68M26 64h68M26 68h68' stroke='#a07438'/>"
  "<rect x='26' y='56' width='68' height='2' fill='#deb670'/></svg>";

static const char* const STYLE_SVG[CLOCK_STYLE_COUNT] = {SVG_7, SVG_8,
  SVG_10, SVG_11, SVG_12, SVG_13, SVG_14, SVG_15, SVG_16, SVG_17, SVG_18, SVG_19, SVG_20, SVG_21, SVG_22, SVG_23, SVG_24, SVG_25, SVG_26,
  SVG_30, SVG_31, SVG_33, SVG_35, SVG_36, SVG_38, SVG_39, SVG_40, SVG_41, SVG_42, SVG_43, SVG_44, SVG_45, SVG_46};

static String autoInfo() {
  const uint32_t now = millis();
  if ((int32_t)(now - gAutoHold) < 0)
    return String(" (Wechsel aktiv, pausiert nach Auswahl noch ") + ((gAutoHold - now) / 1000UL) + " s)";
  const uint32_t iv = gSet.autoSec * 1000UL, el = now - gStyleSince;
  String h;
  const uint64_t hm = holidayMask() & ~gSet.autoMask;
  for (int i = 0; i < CLOCK_STYLE_COUNT; i++)
    if (hm & (1ULL << i)) { h += h.length() ? ", " : " + "; h += STYLE_NAME[i]; }
  return String(" (Wechsel aktiv") + h + ", naechster in " + ((el < iv ? iv - el : 0) / 1000UL) + " s)";
}

static void handleRoot() {
  if (!authOk()) return;
  // Seite wird in Stuecken gesendet (chunked): kein grosser RAM-Block noetig, die Seite kann nicht mehr abgeschnitten werden
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html; charset=utf-8", "");
  String p;
  p.reserve(4096);
  auto fl = [&]() { if (p.length() > 1200) { server.sendContent(p); p = ""; } };
  p += FPSTR(PAGE_HEAD);

  // ---- Navigation (Kategorien) ----
  p += F("<div class=app><nav id=nav>"
         "<a data-t=status>Status</a><a data-t=display>Anzeige</a><a data-t=bright>Helligkeit</a><a data-t=ticker>Laufschrift</a><a data-t=sound>Summer &amp; Wecker</a>"
         "<a data-t=net>WLAN</a><a data-t=time>Zeit &amp; Wetter</a><a data-t=sys>System</a></nav><main>"
         "<h1>Wetteruhr</h1>");
  if (server.hasArg("saved")) p += F("<div class=ok>Gespeichert.</div>");
  if (gSet.webPw[0] == 0)
    p += F("<div class=warn>Kein Web-Passwort gesetzt: Jeder im Netzwerk kann diese Seite &ouml;ffnen. "
           "Bitte unter &bdquo;System&ldquo; ein Passwort vergeben.</div>");

  // Unsichtbarer Standard-Button: Enter in einem Feld soll "Speichern" ausloesen, nicht Standby/Reset
  p += F("<form method=post action=/save><button type=submit class=hid tabindex=-1 aria-hidden=true></button>");

  // ================= Status =================
  auto row = [&](const char* k, const String& v) {
    p += F("<tr><td>"); p += k; p += F("</td><td>"); p += esc(v); p += F("</td></tr>");
  };
  fl();
  p += F("<section class=tab id=t-status><h2>Status</h2><table>");
  const bool wf = (WiFi.status() == WL_CONNECTED);
  row("WLAN", wf ? (String(gSet.ssid) + "  (" + String(WiFi.RSSI()) + " dBm)") : String("nicht verbunden"));
  row("IP-Adresse", wf ? WiFi.localIP().toString() : String("-"));
  if (apActive) row("Einrichtungs-AP", String(AP_SSID) + "  " + WiFi.softAPIP().toString());
  if (timeValid()) {
    time_t n = time(nullptr); struct tm t; localtime_r(&n, &t);
    char tb[32]; strftime(tb, sizeof(tb), "%d.%m.%Y %H:%M:%S", &t);
    row("Uhrzeit", String(tb) + (gTimeManual ? "  (manuell gesetzt)" : ""));
  } else row("Uhrzeit", gSet.ntpOn ? "noch nicht synchronisiert" : "nicht gesetzt (NTP aus, bitte unter Zeit & Wetter eingeben)");
  row("Zeitquelle", gSet.ntpOn ? (String("NTP (") + gSet.ntp + ")") : String("nur manuell"));
  Weather w; bool hv = getWeather(w);
  if (hv) {
    uint32_t am = (millis() - w.fetchedMs) / 60000UL;
    row("Wetter", String(w.temp, 1) + " C, aktualisiert vor " + String(am) + " min");
  } else row("Wetter", "noch keine Daten");
  row("Uhr-Design", String(STYLE_NAME[gStyle < CLOCK_STYLE_COUNT ? gStyle : 0]) + (nyeNow() ? " (Silvester-Automatik)" : (nightTv() ? " (Nachtmodus)" : (autoActive() ? autoInfo() : String("")))) + ", " + ACCENT_NAME[gSet.accent < ACCENT_COUNT ? gSet.accent : 0]);
  row("Zeitplan-Dimmung", gSet.dimOn ? (String(gDimActive ? "aktiv: " : "wartet: ") + String(gSet.dimLevel) + " % von " + hhmm(gSet.dimStart) + " bis " + hhmm(gSet.dimEnd) + (gSet.dimTv ? ", mit Testbild" : "")) : String("aus"));
  row("Dimmung", HW_DIM ? "Hardware (PWM am Backlight)" : "Software (Farben abdunkeln, Backlight bleibt an)");
  row("Display", standby ? "Standby (aus)" : "an");
#if PIN_TOUCH >= 0
  {
    const bool hi = digitalRead(PIN_TOUCH) == HIGH;
    String v = String("GPIO ") + PIN_TOUCH + ", Pegel jetzt " + (hi ? "HIGH" : "LOW") +
               (hi == (TOUCH_ACTIVE_HIGH != 0) ? " (beruehrt)" : " (frei)") +
               ", erkannt: " + gTouchCount + "x";
    if (gTouchCount) v += String(", zuletzt vor ") + ((millis() - gTouchLastMs) / 1000UL) + " s";
    row("Touch-Taster", v);
  }
#else
  row("Touch-Taster", "nicht aktiviert (PIN_TOUCH = -1 in config.h)");
#endif
  if (!bz::present()) row("Summer", "nicht verbaut (PIN_BUZZER = -1)");
  else if (gAlarmRinging) row("Wecker", "klingelt");
  else if (gSet.alOn) {
    String d;
    for (int i = 0; i < 7; i++) if (gSet.alDays & (1 << i)) { if (d.length()) d += ' '; d += WEEKDAY2[(i + 1) % 7]; }
    row("Wecker", hhmm(gSet.alTime) + (d.length() ? String("  ") + d : String("  (kein Tag gewaehlt)")));
  } else row("Wecker", "aus");
  row("Freier Speicher", String(ESP.getFreeHeap()) + " Bytes");
  row("Firmware", String(FW_VERSION) + "  (gebaut " + __DATE__ + " " + __TIME__ + ")");
  p += F("</table>");
  if (gAlarmRinging)
    p += F("<button type=submit class=red formaction=/alarmoff formmethod=post formnovalidate>Wecker abstellen</button>");
  p += F("</section>");

  // ================= Anzeige =================
  fl();
  p += F("<section class=tab id=t-display><h2>Uhr-Design</h2><div class=chips id=cf><a class=on data-c=-1>Alle</a>");
  for (int k = 0; k < CAT_COUNT; k++) { p += F("<a data-c="); p += String(k); p += F(">"); p += CAT_NAME[k]; p += F("</a>"); }
  p += F("</div><div class=cards id=cards>");
  for (int i = 0; i < CLOCK_STYLE_COUNT; i++) {
    fl();
    p += F("<label class=card data-c="); p += String(STYLE_CAT[i]); p += F("><input type=radio name=cs value=");
    p += String(i);
    p += (gSet.clockStyle == i) ? F(" checked>") : F(">");
    p += FPSTR(STYLE_SVG[i]);
    p += STYLE_NAME[i];
    p += F("</label>");
  }
  p += F("</div>");
  p += F("<h2>Akzentfarbe</h2><div>");
  for (int i = 0; i < ACCENT_COUNT; i++) {
    char hx[8];
    snprintf(hx, sizeof(hx), "#%02x%02x%02x", ACCENT_RGB[i][0], ACCENT_RGB[i][1], ACCENT_RGB[i][2]);
    p += F("<label class=sw title=\""); p += ACCENT_NAME[i]; p += F("\" style=\"background:"); p += hx;
    p += F("\"><input type=radio name=acc value="); p += String(i);
    p += (gSet.accent == i) ? F(" checked></label>") : F("></label>");
  }
  p += F("</div><small>Design und Farbe wirken sofort auf dem Display. Alle anderen Felder &uuml;bernimmst du mit &bdquo;Speichern&ldquo;.</small>");
  p += F("<script>const accHex=[");
  for (int i = 0; i < ACCENT_COUNT; i++) {
    char hx[12];
    snprintf(hx, sizeof(hx), "%s\"#%02x%02x%02x\"", i ? "," : "", ACCENT_RGB[i][0], ACCENT_RGB[i][1], ACCENT_RGB[i][2]);
    p += hx;
  }
  p += F("];</script>");

  { // Automatischer Wechsel
    uint32_t sec = gSet.autoSec, unit = 1, val = sec;
    if (sec % 3600 == 0) { unit = 3600; val = sec / 3600; }
    else if (sec % 60 == 0) { unit = 60; val = sec / 60; }
    p += F("<h2>Automatischer Design-Wechsel</h2><label><input type=checkbox name=auto value=1");
    p += gSet.autoOn ? F(" checked") : F("");
    p += F(">Designs nach Zeitplan wechseln</label><label>Wechsel alle</label><div class=row><input type=number name=aint min=1 max=86400 style='width:6em' value=");
    p += String(val);
    p += F("><select name=aunit>");
    p += F("<option value=1");    p += unit == 1    ? F(" selected") : F(""); p += F(">Sekunden</option>");
    p += F("<option value=60");   p += unit == 60   ? F(" selected") : F(""); p += F(">Minuten</option>");
    p += F("<option value=3600"); p += unit == 3600 ? F(" selected") : F(""); p += F(">Stunden</option></select></div>");
    p += F("<label><input type=checkbox name=arnd value=1");
    p += gSet.autoRnd ? F(" checked") : F("");
    p += F(">Zuf&auml;llige Reihenfolge</label><label>Diese Designs verwenden (mindestens zwei) &ndash; Kategorie anhaken w&auml;hlt alle darin:</label>");
    for (int k = 0; k < CAT_COUNT; k++) {
      p += F("<label class=cg><input type=checkbox class=cgb data-c="); p += String(k); p += F("> "); p += CAT_NAME[k]; p += F("</label>");
      p += F("<div class=cards style='grid-template-columns:1fr 1fr 1fr'>");
      for (int i = 0; i < CLOCK_STYLE_COUNT; i++) {
        fl();
        if (STYLE_CAT[i] != k) continue;
        p += F("<label style='font-weight:normal'><input type=checkbox class=am data-c="); p += String(k); p += F(" name=am"); p += String(i); p += F(" value=1");
        p += (gSet.autoMask >> i) & 1 ? F(" checked") : F("");
        p += F("> "); p += STYLE_NAME[i]; p += F("</label>");
      }
      p += F("</div>");
    }
    p += F("<p><a href='#' onclick=\"document.querySelectorAll('.am').forEach(c=>c.checked=true);grpSync();return false\">alle</a> &middot; "
           "<a href='#' onclick=\"document.querySelectorAll('.am').forEach(c=>c.checked=false);grpSync();return false\">keine</a></p>"
           "<script>function grpSync(){document.querySelectorAll('.cgb').forEach(g=>{const m=[...document.querySelectorAll('.am[data-c=\"'+g.dataset.c+'\"]')];const n=m.filter(x=>x.checked).length;g.checked=n===m.length;g.indeterminate=n>0&&n<m.length;});}"
           "document.querySelectorAll('.cgb').forEach(g=>g.onchange=()=>{document.querySelectorAll('.am[data-c=\"'+g.dataset.c+'\"]').forEach(x=>x.checked=g.checked);grpSync();});"
           "document.querySelectorAll('.am').forEach(x=>x.onchange=grpSync);grpSync();"
           "document.querySelectorAll('#cf a').forEach(a=>a.onclick=()=>{document.querySelectorAll('#cf a').forEach(z=>z.classList.toggle('on',z===a));const c=a.dataset.c;document.querySelectorAll('#cards .card').forEach(k=>k.style.display=(c==='-1'||k.dataset.c===c)?'':'none');});</script>"
           "<small>Gilt, sobald &bdquo;Speichern&ldquo; gedr&uuml;ckt wird. Ein Klick auf eine Design-Karte oben zeigt es sofort an; der Wechsel l&auml;uft danach weiter. Der Touch-Taster bl&auml;ttert in dieser Liste weiter (ohne Liste durch alle Designs); danach l&auml;uft der Wechsel normal mit vollem Intervall weiter.</small>");
  }

  // Extras: Silvester-Automatik, Weihnachtsschlitten, UFO
  p += F("<h2>Extras</h2><label><input type=checkbox name=holauto value=1");
  p += gSet.holAuto ? F(" checked") : F("");
  p += F(">Feiertags-Designs automatisch in den Design-Wechsel aufnehmen (jeweils 10 Tage vorher bis zum Fest: "
         "Weihnachten mit Weihnachtself, Silvester, Valentinstag, Halloween, Ostern)</label>"
         "<small>Wirkt, wenn der automatische Design-Wechsel an ist. Die eigene Liste bleibt unver&auml;ndert; "
         "nach dem Fest f&auml;llt das Design von selbst wieder heraus.</small>");
  p += F("<label><input type=checkbox name=nyeauto value=1");
  p += gSet.nyeAuto ? F(" checked") : F("");
  p += F(">Silvester-Automatik: am 31.12. von 23:30 bis 00:30 Feuerwerk (Design &bdquo;Silvester&ldquo;)</label>"
         "<label>ISS-Tracker zeigt</label><select name=sled>");
  {
    static const char* const SL[3] = {"die ISS", "immer den Weihnachtsschlitten", "den Weihnachtsschlitten vom 1. bis 26.12."};
    for (int i = 0; i < 3; i++) {
      p += F("<option value="); p += String(i); p += (gSet.sleigh == i) ? F(" selected>") : F(">"); p += SL[i]; p += F("</option>");
    }
  }
  p += F("</select><label><input type=checkbox name=ufo value=1");
  p += gSet.ufoOn ? F(" checked") : F("");
  p += F(">Flugradar: ab und zu ein UFO erkennen (nur Spa&szlig;)</label>"
         "<button type=submit class=sec formaction=/ufo formmethod=post formnovalidate>UFO jetzt zeigen</button>"
         "<small>Der Knopf schaltet auf das Flugradar und l&auml;sst sofort ein UFO auftauchen.</small>"
         "<label>Aquarium</label><button type=submit class=sec formaction=/feed formmethod=post formnovalidate>Fische f&uuml;ttern</button>"
         "<button type=submit class=sec formaction=/visit formmethod=post formnovalidate>Besuch: Seeschlange / U-Boot</button>"
         "<small>Beide Kn&ouml;pfe schalten auf das Aquarium. Der Besuch kommt sonst von selbst alle paar Minuten, abwechselnd Seeschlange und U-Boot.</small>");

  p += F("<h2>Darstellung</h2><label><input type=checkbox name=sec value=1");
  p += gSet.seconds ? F(" checked") : F("");
  p += F(">Sekunden anzeigen</label><label>Ausrichtung</label><select name=rot><option value=1");
  p += (gSet.rotation == 1) ? F(" selected") : F("");
  p += F(">Normal</option><option value=3");
  p += (gSet.rotation == 3) ? F(" selected") : F("");
  p += F(">180&deg; gedreht</option></select></section>");

  // ================= Helligkeit =================
  fl();
  p += F("<section class=tab id=t-bright><h2>Helligkeit</h2><label>Helligkeit: <span id=bv>");
  p += String(gSet.bright);
  p += F("</span> %</label><input id=br name=b type=range min=5 max=100 value=");
  p += String(gSet.bright);
  p += F(">");
  if (!HW_DIM)
    p += F("<small>Ohne Umbau des Displays werden nur die Farben abgedunkelt; "
           "das Backlight selbst bleibt unver&auml;ndert hell.</small>");
  p += F("<h2>Display ein/aus</h2><button type=submit formaction=/standby formmethod=post formnovalidate name=v value=");
  p += standby ? F("0>Display einschalten") : F("1>Display ausschalten (Standby)");
  p += F("</button>");

  p += F("<h2>Zeitplan-Dimmung</h2><label><input type=checkbox name=dim value=1");
  p += gSet.dimOn ? F(" checked") : F("");
  p += F(">Display zu bestimmten Zeiten dimmen</label><label>Dimmen ab</label><input type=time name=dims value=\"");
  p += hhmm(gSet.dimStart);
  p += F("\"><label>Zur&uuml;ck zur normalen Helligkeit um</label><input type=time name=dime value=\"");
  p += hhmm(gSet.dimEnd);
  p += F("\"><label>Helligkeit w&auml;hrend der Dimmung (5&ndash;100 %)</label><input name=diml type=number min=5 max=100 value=");
  p += String(gSet.dimLevel);
  p += F("><label><input type=checkbox name=dimtv value=1");
  p += gSet.dimTv ? F(" checked") : F("");
  p += F(">Nachtmodus: w&auml;hrend der Dimmung das Testbild zeigen</label>");
  p += F("<small>Ben&ouml;tigt eine g&uuml;ltige Uhrzeit. Liegt &bdquo;ab&ldquo; sp&auml;ter als &bdquo;um&ldquo;, gilt das Fenster &uuml;ber Mitternacht (z.&nbsp;B. 22:00 bis 07:00). "
         "W&auml;hrend der Dimmung gilt dieser Wert statt des Schiebereglers. Mit Nachtmodus erscheint im Fenster das Testbild "
         "statt des gew&auml;hlten Designs (auch bei Design-Wechsel), danach kehrt die Uhr zum gew&auml;hlten Design zur&uuml;ck. Wirkt erst nach &bdquo;Speichern&ldquo;.</small></section>");

  // ================= Laufschrift =================
  fl();
  p += F("<section class=tab id=t-ticker><h2>Laufschrift</h2><label><input type=checkbox name=tkon value=1");
  p += gSet.tickOn ? F(" checked") : F("");
  p += F(">Laufschrift aktiv</label><label>Text (Umlaute und Sonderzeichen erlaubt, bis 160 Zeichen)</label><input name=tk maxlength=160 value=\"");
  p += esc(gSet.tick);
  p += F("\"><div style='margin:.4em 0'>");
  static const char* const SC[] = {"&auml;","&ouml;","&uuml;","&Auml;","&Ouml;","&Uuml;","&szlig;","&euro;","&deg;","&hearts;","&rarr;","&larr;","&uarr;","&darr;","&bull;","&plusmn;","&eacute;","&egrave;"};
  for (unsigned i = 0; i < sizeof(SC) / sizeof(SC[0]); i++) {
    p += F("<button type=button class=sm onclick=\"tkIns(this.textContent)\">"); p += SC[i]; p += F("</button> ");
  }
  p += F("</div><label>Ablauf</label><select name=tkmode><option value=0");
  p += gSet.tickLoop ? F("") : F(" selected");
  p += F(">Einmalig (beim Speichern)</option><option value=1");
  p += gSet.tickLoop ? F(" selected") : F("");
  p += F(">Dauerschleife</option></select><label>Pause zwischen den Durchl&auml;ufen (Sekunden, nur Schleife)</label><input type=number name=tkpause min=0 max=3600 value=");
  p += String(gSet.tickPause);
  p += F("><label>Position</label><select name=tkpos>");
  static const char* const PN[] = {"Oben", "Mitte", "Unten"};
  for (int i = 0; i < 3; i++) { p += F("<option value="); p += String(i); p += gSet.tickPos == i ? F(" selected>") : F(">"); p += PN[i]; p += F("</option>"); }
  p += F("</select><label>Schriftgr&ouml;&szlig;e</label><select name=tkbig><option value=0");
  p += gSet.tickBig ? F("") : F(" selected");
  p += F(">Mittel</option><option value=1");
  p += gSet.tickBig ? F(" selected") : F("");
  p += F(">Gro&szlig;</option></select><label>Farbe</label><select name=tkcol>");
  for (int i = 0; i <= ACCENT_COUNT; i++) {
    p += F("<option value="); p += String(i); p += gSet.tickColor == i ? F(" selected>") : F(">");
    p += i < ACCENT_COUNT ? ACCENT_NAME[i] : "Weiss"; p += F("</option>");
  }
  p += F("</select><label>Geschwindigkeit: <span id=tsv>");
  p += String(gSet.tickSpeed);
  p += F("</span></label><input type=range name=tkspd min=1 max=10 value=");
  p += String(gSet.tickSpeed);
  p += F(" oninput=\"document.getElementById('tsv').textContent=this.value\"><label><input type=checkbox name=tkbg value=1");
  p += gSet.tickBg ? F(" checked") : F("");
  p += F(">Dunkles Hintergrundband (bei klassischen Designs immer an)</label>"
         "<p><button type=button id=tkplay onclick=\"tkPlay()\">Jetzt einmal abspielen</button></p>"
         "<small>Der Knopf spielt den <b>gespeicherten</b> Text einmal ab. &Auml;nderungen erst mit &bdquo;Speichern&ldquo; &uuml;bernehmen. "
         "Die Laufschrift liegt &uuml;ber jedem Design und pausiert im Standby.</small></section>");

  // ================= Summer & Wecker =================
  fl();
  p += F("<section class=tab id=t-sound><h2>Summer</h2>");
  if (!bz::present())
    p += F("<small><b>Kein Summer konfiguriert.</b> In <code>config.h</code> <code>PIN_BUZZER</code> eintragen "
           "(z.&nbsp;B. 10) und neu aufspielen. Die Einstellungen werden trotzdem gespeichert.</small>");
  p += F("<label><input type=checkbox name=bzhr value=1");
  p += gSet.bzHour ? F(" checked") : F("");
  p += F(">Stundensignal (zur vollen Stunde zweimal kurz)</label><label><input type=checkbox name=bzq value=1");
  p += gSet.bzQuiet ? F(" checked") : F("");
  p += F(">Kein Stundensignal w&auml;hrend der Zeitplan-Dimmung (Nachtruhe)</label>");
#if PIN_TOUCH >= 0
  p += F("<label><input type=checkbox name=bztc value=1");
  p += gSet.bzTouch ? F(" checked") : F("");
  p += F(">Klick bei Ber&uuml;hrung des Touch-Tasters</label>");
#endif
  p += F("<button type=submit class=sec formaction=/beep formmethod=post formnovalidate>Summer testen</button>");

  p += F("<h2>Wecker</h2><label><input type=checkbox name=alon value=1");
  p += gSet.alOn ? F(" checked") : F("");
  p += F(">Wecker aktiv</label><label>Weckzeit</label><input type=time name=alt value=\"");
  p += hhmm(gSet.alTime);
  p += F("\"><label>Tage</label><div class=chips style='flex-wrap:wrap'>");
  {
    static const char* const DN[7] = {"Mo", "Di", "Mi", "Do", "Fr", "Sa", "So"};
    for (int i = 0; i < 7; i++) {
      p += F("<label style='display:inline-flex;align-items:center;gap:.3em;margin-right:.8em'><input type=checkbox name=ald");
      p += String(i);
      p += F(" value=1");
      p += (gSet.alDays & (1 << i)) ? F(" checked>") : F(">");
      p += DN[i];
      p += F("</label>");
    }
  }
  p += F("</div>");
  if (gAlarmRinging)
    p += F("<button type=submit class=red formaction=/alarmoff formmethod=post formnovalidate>Wecker abstellen</button>");
  p += F("<small>Der Wecker piept bis zu 5 Minuten und schaltet ein Display im Standby ein. Abstellen: Touch-Taster "
         "ber&uuml;hren oder hier bzw. unter Status &bdquo;Wecker abstellen&ldquo;. Ben&ouml;tigt eine g&uuml;ltige Uhrzeit "
         "(NTP oder manuell). Wirkt erst nach &bdquo;Speichern&ldquo;.</small></section>");

  // ================= WLAN =================
  fl();
  p += F("<section class=tab id=t-net><h2>WLAN</h2><label>Netzwerkname (SSID)</label><input name=ssid list=ssids maxlength=32 value=\"");
  p += esc(gSet.ssid);
  p += F("\"><datalist id=ssids></datalist><button type=button class=sec id=sb onclick=scan()>Netzwerke suchen</button>"
         "<label>Passwort</label><input name=pass type=password maxlength=64 placeholder=\"(unver&auml;ndert lassen: leer)\" autocomplete=new-password>"
         "<small>Ein Wechsel des WLANs startet das Ger&auml;t neu.</small></section>");

  // ================= Zeit & Wetter =================
  fl();
  p += F("<section class=tab id=t-time><h2>Zeit</h2><label>Zeitzone</label><select name=tz>");
  bool found = false;
  for (const TzEntry& z : TZS) {
    bool sel = (strcmp(gSet.tz, z.posix) == 0);
    found |= sel;
    p += F("<option value=\""); p += esc(z.posix); p += sel ? F("\" selected>") : F("\">");
    p += esc(z.name); p += F("</option>");
  }
  p += F("</select><label>Eigene Zeitzone (POSIX-String, &uuml;berschreibt die Auswahl)</label>"
         "<input name=tzc maxlength=63 placeholder=\"z.B. CET-1CEST,M3.5.0,M10.5.0/3\" value=\"");
  p += found ? String("") : esc(gSet.tz);
  p += F("\"><label><input type=checkbox name=ntpon value=1");
  p += gSet.ntpOn ? F(" checked") : F("");
  p += F(">Uhrzeit automatisch per NTP (Internet)</label><label>NTP-Server</label><input name=ntp maxlength=47 value=\"");
  p += esc(gSet.ntp);
  p += F("\"><small>Ein anderer NTP-Server wird nach einem Neustart genutzt. Ohne Haken holt die Uhr keine Zeit aus dem Internet; "
         "dann Datum und Uhrzeit unten von Hand setzen.</small>");

  {
    char dv[12] = "", tv[10] = "";
    if (timeValid()) {
      time_t n = time(nullptr); struct tm t; localtime_r(&n, &t);
      strftime(dv, sizeof(dv), "%Y-%m-%d", &t);
      strftime(tv, sizeof(tv), "%H:%M:%S", &t);
    }
    p += F("<h2>Datum und Uhrzeit manuell</h2><label>Datum</label><input type=date name=mdate min=2024-01-01 max=2099-12-31 value=\"");
    p += dv;
    p += F("\"><label>Uhrzeit</label><input type=time name=mtime step=1 value=\"");
    p += tv;
    p += F("\"><div style='margin:.5em 0'>"
           "<button type=button class=sec onclick=\"var d=new Date(),z=n=>('0'+n).slice(-2),f=this.form;"
           "f.mdate.value=d.getFullYear()+'-'+z(d.getMonth()+1)+'-'+z(d.getDate());"
           "f.mtime.value=z(d.getHours())+':'+z(d.getMinutes())+':'+z(d.getSeconds())\">Zeit vom Browser &uuml;bernehmen</button> "
           "<button type=submit formaction=/settime formmethod=post formnovalidate>Uhrzeit setzen</button></div>"
           "<small>Gilt in der oben gew&auml;hlten Zeitzone, Sommer-/Winterzeit wird automatisch erkannt. "
           "&bdquo;Uhrzeit setzen&ldquo; &uuml;bernimmt nur Datum und Uhrzeit, keine anderen Felder. "
           "Mit NTP wird die Zeit beim n&auml;chsten Abgleich (ca. st&uuml;ndlich) wieder &uuml;berschrieben. "
           "Die Uhr hat keine Pufferbatterie: Nach Stromausfall muss die Zeit ohne NTP neu gesetzt werden "
           "(ein Neustart &uuml;ber den Browser beh&auml;lt sie meist).</small>");
  }

  p += F("<h2>Wetter</h2><label>Breitengrad</label><input name=lat inputmode=decimal value=\"");
  p += String(gSet.lat, 4);
  p += F("\"><label>L&auml;ngengrad</label><input name=lon inputmode=decimal value=\"");
  p += String(gSet.lon, 4);
  p += F("\"><label>Aktualisierung (Minuten, 5&ndash;120)</label><input name=upd type=number min=5 max=120 value=");
  p += String(gSet.updMin);
  p += F("><h2>Flugradar</h2><label>Radius in km (5&ndash;100)</label><input name=rkm type=number min=5 max=100 value=");
  p += String(gSet.radarKm);
  p += F("><small>Das Design &bdquo;Flugradar&ldquo; zeigt echte Flugzeuge rund um den Standort (Breiten- und L&auml;ngengrad von oben). "
         "Daten: offene ADS-B-Dienste adsb.lol, airplanes.live und adsb.fi, nur f&uuml;r private Nutzung. "
         "Abgerufen wird nur, solange das Design angezeigt wird.</small></section>");

  // ================= System =================
  fl();
  p += F("<section class=tab id=t-sys><h2>Ger&auml;t</h2><label>Ger&auml;tename (erreichbar als <i>name</i>.local)</label><input name=host maxlength=23 value=\"");
  p += esc(gSet.host);
  p += F("\"><h2>Zugang</h2><label>Web-Passwort (Benutzer: admin; leer = unver&auml;ndert)</label>"
         "<input name=webpw type=password maxlength=32 autocomplete=new-password>"
         "<label><input type=checkbox name=rmpw value=1>Web-Passwort entfernen</label>"
         "<h2>Firmware-Update &uuml;ber WLAN</h2>");
  p += F("<label>Installiert: <b>");
  p += FW_VERSION;
  p += F("</b> (gebaut "); p += __DATE__; p += ' '; p += __TIME__; p += F(")</label>");
  if (!otaPartition()) {
    p += F("<div class=warn><b>Update &uuml;ber WLAN nicht m&ouml;glich:</b> Das Partitionsschema hat keinen Platz f&uuml;r ein Update "
           "(z.&nbsp;B. &bdquo;Huge APP (No OTA)&ldquo;). Einmal per USB mit <i>Partition Scheme: Minimal SPIFFS (1.9MB APP with OTA)</i> "
           "hochladen, danach geht es per WLAN. Die Einstellungen bleiben erhalten.</div>");
  } else {
    p += F("<label>Firmware-Datei (.bin, h&ouml;chstens ");
    p += String((unsigned long)(otaMaxSize() / 1024));
    p += F(" KB)</label><input type=file id=fw accept=.bin>"
           "<button type=button id=fwb onclick=fwUp()>Update installieren</button>"
           "<div id=fwp style='margin-top:.6em'></div>"
           "<small>In der Arduino IDE: <i>Sketch &gt; Kompilierte Bin&auml;rdatei exportieren</i>, dann die Datei "
           "<b>WetterUhr.ino.bin</b> aus dem Sketch-Ordner (Unterordner <i>build</i>) w&auml;hlen &ndash; nicht die "
           "&bdquo;merged&ldquo;-, &bdquo;bootloader&ldquo;- oder &bdquo;partitions&ldquo;-Datei. Die Uhr pr&uuml;ft die Datei, "
           "schreibt sie in den freien Bereich und startet nur bei Erfolg neu; sonst l&auml;uft die alte Firmware weiter. "
           "Einstellungen bleiben erhalten. Alternativ in der Arduino IDE als Port den Netzwerk-Port &bdquo;");
    p += esc(gSet.host);
    p += F("&ldquo; w&auml;hlen und normal hochladen (Passwort = Web-Passwort).</small>"
           "<script>function fwUp(){const f=$('fw').files[0],o=$('fwp'),b=$('fwb');"
           "if(!f){o.textContent='Bitte zuerst eine .bin-Datei w\u00e4hlen.';return;}"
           "if(!/\\.bin$/i.test(f.name)||/merged|bootloader|partitions/i.test(f.name)){o.textContent='Falsche Datei: bitte WetterUhr.ino.bin w\u00e4hlen.';return;}"
           "if(!confirm('Firmware \u201e'+f.name+'\u201c installieren? Die Uhr startet danach neu.'))return;"
           "b.disabled=true;const x=new XMLHttpRequest();x.open('POST','/update?size='+f.size);"
           "x.upload.onprogress=e=>{if(e.lengthComputable)o.textContent='\u00dcbertragen: '+Math.round(e.loaded*100/e.total)+' %';};"
           "x.onload=()=>{if(x.status==200){o.textContent='Fertig. Die Uhr startet neu, Seite l\u00e4dt in 20 s neu ...';setTimeout(()=>location.href='/',20000);}"
           "else{o.textContent='Fehler: '+x.responseText;b.disabled=false;}};"
           "x.onerror=()=>{o.textContent='Verbindung abgebrochen.';b.disabled=false;};"
           "const d=new FormData();d.append('firmware',f,f.name);x.send(d);}</script>");
  }
  p += F("<h2>Zur&uuml;cksetzen</h2><button type=submit class=red formaction=/reset formmethod=post formnovalidate "
         "onclick=\"return confirm('Alle Einstellungen l&ouml;schen?')\">Werkseinstellungen</button></section>");

  p += F("<div id=savebar><button type=submit>Speichern</button></div></form></main></div>");
  p += FPSTR(PAGE_JS);
  server.sendContent(p);
  server.sendContent("");               // Ende der Antwort
}

static void handleSave() {
  if (!authOk()) return;
  Settings n = gSet;
  String s;

  s = server.arg("ssid"); s.trim();
  if (s.length() <= 32) strlcpy(n.ssid, s.c_str(), sizeof(n.ssid));

  s = server.arg("pass");
  if (s.length() > 0 && s.length() <= 64) strlcpy(n.pass, s.c_str(), sizeof(n.pass));

  String tzc = server.arg("tzc"); tzc.trim();
  String tzs = server.arg("tz");  tzs.trim();
  String tz = tzc.length() ? tzc : tzs;
  if (tz.length() >= 1 && tz.length() <= 63) strlcpy(n.tz, tz.c_str(), sizeof(n.tz));

  s = server.arg("ntp"); s.trim();
  if (s.length() >= 1 && s.length() <= 47) strlcpy(n.ntp, s.c_str(), sizeof(n.ntp));

  s = server.arg("lat"); s.trim(); s.replace(',', '.');
  if (s.length()) { float v = s.toFloat(); if (v >= -90.0f && v <= 90.0f) n.lat = v; }
  s = server.arg("lon"); s.trim(); s.replace(',', '.');
  if (s.length()) { float v = s.toFloat(); if (v >= -180.0f && v <= 180.0f) n.lon = v; }

  int upd = server.arg("upd").toInt();
  if (upd >= 5 && upd <= 120) n.updMin = (uint16_t)upd;
  int rkm = server.arg("rkm").toInt();
  if (rkm >= 5 && rkm <= 100) n.radarKm = (uint8_t)rkm;

  int b = server.arg("b").toInt();
  if (b >= 5 && b <= 100) n.bright = (uint8_t)b;

  if (server.hasArg("cs"))  { int v = server.arg("cs").toInt();  if (v >= 0 && v < CLOCK_STYLE_COUNT) n.clockStyle = (uint8_t)v; }
  if (server.hasArg("acc")) { int v = server.arg("acc").toInt(); if (v >= 0 && v < ACCENT_COUNT)      n.accent = (uint8_t)v; }

  n.tickOn   = server.hasArg("tkon");
  n.tickLoop = server.arg("tkmode") == "1";
  n.tickBg   = server.hasArg("tkbg");
  { int v = server.arg("tkpos").toInt();   if (v >= 0 && v <= 2)    n.tickPos = (uint8_t)v; }
  { int v = server.arg("tkbig").toInt();   if (v == 0 || v == 1)    n.tickBig = (uint8_t)v; }
  { int v = server.arg("tkcol").toInt();   if (v >= 0 && v <= 9)    n.tickColor = (uint8_t)v; }
  { int v = server.arg("tkspd").toInt();   if (v >= 1 && v <= 10)   n.tickSpeed = (uint8_t)v; }
  { String v = server.arg("tkpause"); if (v.length()) { long x = v.toInt(); if (x >= 0 && x <= 3600) n.tickPause = (uint16_t)x; } }
  if (server.hasArg("tk")) {
    String t = server.arg("tk"); t.trim();
    if (t.length() <= 160) strlcpy(n.tick, t.c_str(), sizeof(n.tick));
  }
  n.autoOn  = server.hasArg("auto");
  n.autoRnd = server.hasArg("arnd");
  {
    uint64_t m = 0;
    for (int i = 0; i < CLOCK_STYLE_COUNT; i++) if (server.hasArg(String("am") + i)) m |= (1ULL << i);
    n.autoMask = m;
    long v = server.arg("aint").toInt(), u = server.arg("aunit").toInt();
    if (u != 1 && u != 60 && u != 3600) u = 60;
    if (v >= 1 && v * u >= 10 && v * u <= 86400) n.autoSec = (uint32_t)(v * u);
  }

  n.dimOn = server.hasArg("dim");
  n.dimTv = server.hasArg("dimtv");
  n.ntpOn   = server.hasArg("ntpon");
  n.bzHour  = server.hasArg("bzhr");
  n.bzQuiet = server.hasArg("bzq");
#if PIN_TOUCH >= 0
  n.bzTouch = server.hasArg("bztc");
#endif
  n.alOn    = server.hasArg("alon");
  n.nyeAuto = server.hasArg("nyeauto");
  n.holAuto = server.hasArg("holauto");
  n.ufoOn   = server.hasArg("ufo");
  { int v = server.arg("sled").toInt(); if (v >= 0 && v <= 2) n.sleigh = (uint8_t)v; }
  { uint16_t v; if (parseHhmm(server.arg("alt"), v)) n.alTime = v; }
  { uint8_t m = 0; for (int i = 0; i < 7; i++) if (server.hasArg(String("ald") + i)) m |= (uint8_t)(1 << i); n.alDays = m; }
  { uint16_t v; if (parseHhmm(server.arg("dims"), v)) n.dimStart = v; if (parseHhmm(server.arg("dime"), v)) n.dimEnd = v; }
  { int v = server.arg("diml").toInt(); if (v >= 5 && v <= 100) n.dimLevel = (uint8_t)v; }

  n.seconds  = server.hasArg("sec");
  n.rotation = (server.arg("rot").toInt() == 3) ? 3 : 1;

  String h = cleanHost(server.arg("host"));
  if (h.length() >= 1) strlcpy(n.host, h.c_str(), sizeof(n.host));

  String wp = server.arg("webpw");
  if (server.hasArg("rmpw")) n.webPw[0] = 0;
  else if (wp.length() > 0 && wp.length() <= 32) strlcpy(n.webPw, wp.c_str(), sizeof(n.webPw));

  const bool reboot = strcmp(n.ssid, gSet.ssid) || strcmp(n.pass, gSet.pass) ||
                      strcmp(n.ntp, gSet.ntp)   || strcmp(n.host, gSet.host);
  const bool rotChanged = (n.rotation != gSet.rotation);
  const bool brChanged  = (n.bright != gSet.bright);
  const bool ntpChanged = (n.ntpOn != gSet.ntpOn);

  const bool csChanged = (n.clockStyle != gSet.clockStyle);
  gSet = n;
  settingsSave();
  gHolAt = 0;                                         // Feiertags-Fenster neu berechnen
  if (ntpChanged) {                     // NTP ein/aus ohne Neustart
    if (!gSet.ntpOn) ntpStop();
    ntpStarted = false;                 // wifiLoop startet NTP bei Bedarf neu
  }
  if (csChanged || !autoActive()) styleManual();   // Design geaendert oder kein Wechsel: gewaehltes Design zeigen
  else if (!inList(gStyle)) {                       // Wechsel aktiv: weiterlaufen, nur bei Bedarf in die Liste springen
    gStyle = (uint8_t)nextStyle(gStyle); gStyleSince = millis(); needFull = true;
  }
  tickerRestart();

  applyTimezone();
  dimTick(millis(), true);
  themeApply();
  if (rotChanged) tft.setRotation(gSet.rotation);
  if (brChanged)  setBrightness(gSet.bright, false);
  gRefreshNow = true;   // Wetter mit neuen Koordinaten sofort neu abrufen
  needFull = true;

  if (reboot) {
    server.send(200, "text/html; charset=utf-8",
      F("<!doctype html><meta charset=utf-8><meta name=viewport content='width=device-width,initial-scale=1'>"
        "<body style='font-family:sans-serif;background:#0d1117;color:#e6edf3;padding:20px'>"
        "<h2>Gespeichert</h2><p>Das Ger&auml;t startet neu, um die Netzwerk&auml;nderung zu &uuml;bernehmen. "
        "Danach ist die Seite unter der neuen IP-Adresse (steht auf dem Display) erreichbar.</p></body>"));
    needReboot = true;
    rebootAt = millis() + 1500;
  } else {
    server.sendHeader("Location", "/?saved=1");
    server.send(303);
  }
}

static void handleBrightness() {
  if (!authOk()) return;
  int v = server.arg("v").toInt();
  if (v < 5 || v > 100) { server.send(400, "text/plain", "ungueltig"); return; }
  setBrightness((uint8_t)v, true);
  server.send(204);
}

static void handleLook() {
  if (!authOk()) return;
  int cs = server.arg("cs").toInt();
  int ac = server.arg("acc").toInt();
  if (cs < 0 || cs >= CLOCK_STYLE_COUNT || ac < 0 || ac >= ACCENT_COUNT) {
    server.send(400, "text/plain", "ungueltig");
    return;
  }
  gSet.clockStyle = (uint8_t)cs;
  gDbgLook = cs;
  gSet.accent = (uint8_t)ac;
  styleManual();
  gDbgSrc = 'W';
  themeApply();
  needFull = true;                       // komplett neu zeichnen
  setDirty = true; setDirtyAt = millis();  // nach 3 s Ruhe im Flash speichern
  server.send(204);
}

static void handleTicker() {
  if (!authOk()) return;
  if (gSet.tick[0] && !standby) gTickNow = true;
  server.send(204);
}

static void handleStandby() {
  if (!authOk()) return;
  if (server.arg("v") == "1") { if (!standby) enterStandby(); }
  else if (standby) startWake();
  server.sendHeader("Location", "/?saved=1");
  server.send(303);
}

static void handleScan() {
  if (!authOk()) return;
  int n = WiFi.scanComplete();
  if (n == WIFI_SCAN_FAILED) {            // noch kein Scan gestartet
    WiFi.scanNetworks(true);              // asynchron, blockiert nicht
    server.send(200, "application/json", F("{\"state\":\"running\"}"));
    return;
  }
  if (n == WIFI_SCAN_RUNNING) {
    server.send(200, "application/json", F("{\"state\":\"running\"}"));
    return;
  }
  String j = F("{\"state\":\"done\",\"list\":[");
  bool first = true;
  for (int i = 0; i < n && i < 20; i++) {
    String name = WiFi.SSID(i);
    if (name.length() == 0) continue;
    if (!first) j += ',';
    first = false;
    j += '"'; j += jsonEsc(name); j += '"';
  }
  j += F("]}");
  WiFi.scanDelete();
  server.send(200, "application/json", j);
}

static void handleReset() {
  if (!authOk()) return;
  settingsFactoryReset();
  server.send(200, "text/html; charset=utf-8",
    F("<!doctype html><meta charset=utf-8><body style='font-family:sans-serif;background:#0d1117;color:#e6edf3;padding:20px'>"
      "<h2>Zur&uuml;ckgesetzt</h2><p>Das Ger&auml;t startet neu und &ouml;ffnet den Einrichtungs-Access-Point.</p></body>"));
  needReboot = true;
  rebootAt = millis() + 1500;
}

static void handleSetTime() {
  if (!authOk()) return;
  int y = 0, mo = 0, d = 0, h = 0, mi = 0, s = 0;
  const String ds = server.arg("mdate"), ts = server.arg("mtime");
  const bool okD = sscanf(ds.c_str(), "%d-%d-%d", &y, &mo, &d) == 3;
  const int nt = sscanf(ts.c_str(), "%d:%d:%d", &h, &mi, &s);
  if (!okD || nt < 2 || !setLocalTime(y, mo, d, h, mi, nt == 3 ? s : 0)) {
    server.send(400, "text/plain; charset=utf-8", "Ungueltiges Datum oder Uhrzeit");
    return;
  }
  LOGF("Uhrzeit manuell gesetzt: %s %s\n", ds.c_str(), ts.c_str());
  dimTick(millis(), true);
  resetCaches();
  needFull = true;
  server.sendHeader("Location", "/?saved=1");
  server.send(303);
}

static void handleBeep() {
  if (!authOk()) return;
  if (!gAlarmRinging) bz::play(BZ_PAT(bz::P_TEST));
  server.sendHeader("Location", "/?saved=1");
  server.send(303);
}

static void handleAlarmOff() {
  if (!authOk()) return;
  alarmStop();
  server.sendHeader("Location", "/?saved=1");
  server.send(303);
}

static void handleUfo() {
  if (!authOk()) return;
  gSet.clockStyle = RADAR_STYLE;          // Flugradar anzeigen
  styleManual();
  gDbgSrc = 'W';
  f_radar::kick = true;                   // UFO sofort
  setDirty = true; setDirtyAt = millis();
  server.sendHeader("Location", "/?saved=1");
  server.send(303);
}

static void showAqua() {
  gSet.clockStyle = AQUA_STYLE;
  styleManual();
  gDbgSrc = 'W';
  setDirty = true; setDirtyAt = millis();
  server.sendHeader("Location", "/?saved=1");
  server.send(303);
}
static void handleFeed()  { if (!authOk()) return; f_aqua::requestFeed();  showAqua(); }
static void handleVisit() { if (!authOk()) return; f_aqua::requestVisit(); showAqua(); }

// ---- Firmware-Update per Browser (POST /update, multipart) ----
static String gOtaErr;
static size_t gOtaSize = 0;

static void handleUpdateUpload() {
  HTTPUpload& up = server.upload();
  if (up.status == UPLOAD_FILE_START) {
    gOtaErr = "";
    if (gSet.webPw[0] && !server.authenticate("admin", gSet.webPw)) { gOtaErr = "Nicht angemeldet"; return; }
    if (!otaPartition()) { gOtaErr = "Partitionsschema ohne OTA-Bereich (siehe Hinweis)"; return; }
    gOtaSize = server.hasArg("size") ? (size_t)server.arg("size").toInt() : 0;
    if (gOtaSize > otaMaxSize()) {
      gOtaErr = String("Datei zu gross (") + (unsigned long)(gOtaSize / 1024) + " KB, Platz " + (unsigned long)(otaMaxSize() / 1024) + " KB)";
      return;
    }
    LOGF("Update: %s, %u Bytes\n", up.filename.c_str(), (unsigned)gOtaSize);
    gOtaActive = true;
    otaScreen(0, "Daten werden empfangen ...");
    if (!Update.begin(gOtaSize ? gOtaSize : UPDATE_SIZE_UNKNOWN, U_FLASH)) { gOtaErr = Update.errorString(); otaResume(); }
  } else if (up.status == UPLOAD_FILE_WRITE) {
    if (gOtaErr.length()) return;
    if (Update.write(up.buf, up.currentSize) != up.currentSize) {
      gOtaErr = String("Schreibfehler: ") + Update.errorString();
      Update.abort();
      otaResume();
      return;
    }
    if (gOtaSize) otaScreen((int)((uint64_t)Update.progress() * 100 / gOtaSize), nullptr);
  } else if (up.status == UPLOAD_FILE_END) {
    if (gOtaErr.length()) return;
    if (!Update.end(true)) { gOtaErr = String("Pruefung fehlgeschlagen: ") + Update.errorString(); otaResume(); }
    else { otaScreen(100, "Fertig - Neustart"); LOGF("Update ok: %u Bytes\n", (unsigned)up.totalSize); }
  } else if (up.status == UPLOAD_FILE_ABORTED) {
    Update.abort();
    gOtaErr = "Uebertragung abgebrochen";
    otaResume();
  }
}

static void handleUpdateDone() {
  if (!authOk()) return;
  if (gOtaErr.length() || !Update.isFinished()) {
    server.send(500, "text/plain; charset=utf-8", gOtaErr.length() ? gOtaErr : String("Keine vollstaendige Datei empfangen"));
    if (gOtaActive) otaResume();
    return;
  }
  server.send(200, "text/plain", "OK");
  needReboot = true;
  rebootAt = millis() + 1500;
}

static void handleNotFound() {
  server.send(404, "text/plain", "Nicht gefunden");
}

static void webBegin() {
  server.on("/", HTTP_GET, handleRoot);
  server.on("/diag", HTTP_GET, []() {
    char b[420];
    snprintf(b, sizeof(b), "Schleifenluecke max %lu ms\nBildzeit max %lu us\nFreier Heap %u\n"
             "WLAN-Teil max %lu us\nWeb-Teil max %lu us\nZeichnen max %lu us\ndelay(2) max %lu us\nDesign gerade %d, im Browser gewaehlt %d, zuletzt gewechselt durch %c, Wechsel-Automatik %s\n",
             (unsigned long)gDiagGapLast, (unsigned long)gDiagFrameLast, (unsigned)ESP.getFreeHeap(),
             (unsigned long)gDiagSecLast[0], (unsigned long)gDiagSecLast[1], (unsigned long)gDiagSecLast[2], (unsigned long)gDiagSecLast[3], (int)gStyle, gDbgLook, gDbgSrc, gSet.autoOn ? "AN" : "aus");
    server.send(200, "text/plain", b);
  });
  server.on("/save", HTTP_POST, handleSave);
  server.on("/brightness", HTTP_POST, handleBrightness);
  server.on("/scan", HTTP_GET, handleScan);
  server.on("/standby", HTTP_POST, handleStandby);
  server.on("/look", HTTP_POST, handleLook);
  server.on("/ticker", HTTP_POST, handleTicker);
  server.on("/reset", HTTP_POST, handleReset);
  server.on("/settime", HTTP_POST, handleSetTime);
  server.on("/beep", HTTP_POST, handleBeep);
  server.on("/alarmoff", HTTP_POST, handleAlarmOff);
  server.on("/ufo", HTTP_POST, handleUfo);
  server.on("/feed", HTTP_POST, handleFeed);
  server.on("/visit", HTTP_POST, handleVisit);
  server.on("/update", HTTP_POST, handleUpdateDone, handleUpdateUpload);
  server.onNotFound(handleNotFound);
  server.begin();
}

static void webLoop(uint32_t now) {
  server.handleClient();
  if (!mdnsStarted && WiFi.status() == WL_CONNECTED) {
    if (MDNS.begin(gSet.host)) {
      MDNS.addService("http", "tcp", 80);
#if ARDUINO_OTA
      MDNS.enableArduino(3232, gSet.webPw[0] != 0);   // Netzwerk-Port in der Arduino IDE
#endif
      mdnsStarted = true;
    }
  }
  if (needReboot && (int32_t)(now - rebootAt) >= 0) ESP.restart();
}
