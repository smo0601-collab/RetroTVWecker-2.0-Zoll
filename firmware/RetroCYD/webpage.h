// Web-Oberflaeche von RetroCYD (eigene Datei, damit die Arduino IDE
// den HTML-/JavaScript-Text nicht als C++-Code fehlinterpretiert)
#pragma once
#include <Arduino.h>

const char PAGE[] PROGMEM = R"HTML(<!doctype html>
<html lang="de"><head><meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>RetroCYD</title>
<style>
:root{--bg:#0b0b1a;--card:#16163a;--fg:#e8e8f0;--dim:#8a8ab0;--acc:#ffd400;--ok:#2edc5a}
*{box-sizing:border-box}body{margin:0;background:var(--bg);color:var(--fg);font:15px system-ui,sans-serif}
header{padding:18px 12px 12px;text-align:center;background:linear-gradient(90deg,#f04040,#ff8c00,#ffd400,#2edc5a,#00dce6,#aa3ce6) top/100% 5px no-repeat}
h1{margin:4px 0;color:var(--acc);font-family:monospace;letter-spacing:4px}
main{max-width:640px;margin:auto;padding:0 12px 40px}
section{background:var(--card);border-radius:12px;padding:14px 16px;margin:12px 0}
h2{margin:0 0 8px;font-size:16px;color:#00dce6}
label{display:flex;justify-content:space-between;align-items:center;gap:12px;margin:10px 0}
select,input[type=text],input[type=password]{background:var(--bg);color:var(--fg);border:1px solid #33335a;border-radius:6px;padding:8px;flex:1;max-width:60%;min-width:0;font-size:15px}
input[type=range]{flex:1;max-width:60%}input[type=checkbox]{width:22px;height:22px}
button{background:#2858ff;color:#fff;border:0;border-radius:6px;padding:9px 13px;font-size:14px;cursor:pointer;margin:6px 6px 0 0}
button.warn{background:#a02828}button.sec{background:#33335a}
.small{color:var(--dim);font-size:13px;margin:6px 0}
.pad{display:flex;justify-content:space-between;align-items:center;flex-wrap:wrap;padding:8px 0;border-top:1px solid #26264a}
.bar{height:12px;background:var(--bg);border-radius:6px;overflow:hidden;margin-top:10px}.bar div{height:100%;width:0;background:var(--ok);transition:width .2s}
#toast{position:fixed;bottom:18px;left:50%;transform:translateX(-50%);background:#000d;padding:10px 18px;border-radius:8px;display:none}
</style></head><body>
<header><h1>RETRO CYD</h1><div class="small" id="info">verbinde...</div></header>
<main>
<section><h2>Anzeige</h2>
<label>Ausrichtung<select id="rot"><option value="1">Quer</option><option value="3">Quer (180°)</option><option value="0">Hochkant</option><option value="2">Hochkant (180°)</option></select></label>
<label>Spiegeln<input type="checkbox" id="mirror"></label>
<label>Farben invertieren<input type="checkbox" id="invert"></label>
<label>Helligkeit<input type="range" id="bright" min="10" max="255"></label>
<label>Display-Chip<select id="st7789"><option value="0">ILI9341 (1x USB)</option><option value="1">ST7789 (2x USB)</option></select></label>
<p class="small">Änderungen wirken sofort. Ein Wechsel des Display-Chips startet die Konsole neu.</p></section>

<section><h2>Xbox-Controller</h2><div id="pads"></div>
<label>Neue Controller zulassen<input type="checkbox" id="allowNew"></label>
<p class="small">Neuen Controller koppeln: Knopf unten drücken, dann am Controller die Kopplungstaste (oben neben USB) halten, bis das Xbox-Logo schnell blinkt. Während der 60 Sekunden kann diese Seite kurz langsamer reagieren.</p>
<p class="small" id="pairinfo"></p>
<button onclick="bt('pair')">Neuen Controller koppeln (60 s)</button><button class="warn" onclick="bt('forget')">Alle Kopplungen löschen</button></section>

<section><h2>Spiele &amp; Highscores</h2>
<label>Vibration<input type="checkbox" id="rumbleOn"></label>
<label>Vibrationsstärke<input type="range" id="rumbleStr" min="20" max="255"></label>
<label>LED-Effekte (Rückseite)<input type="checkbox" id="ledOn"></label>
<label>LED-Helligkeit<input type="range" id="ledBri" min="1" max="100"></label>
<label>Spiel-Sounds (Lautsprecher)<input type="checkbox" id="sndOn"></label>
<label>Lautstärke<input type="range" id="sndVol" min="5" max="100"></label>
<label>Klang anhören<select id="sndSel"></select></label>
<button class="sec" onclick="post('/api/sndtest',{id:$('#sndSel').value,v:$('#sndVol').value})">&#9654; Anhören</button><button class="sec" onclick="post('/api/sndtest',{id:-1})">Stopp</button>
<p class="small">Kleiner Lautsprecher (8 Ω, 0,5–2 W) am Anschluss „SPEAK“. Jede Vibration klingt mit, wichtige Ereignisse haben eigene Klänge. &#9654; spielt mit der Stellung des Reglers, auch ohne Speichern.</p>
<label>Snake-Tempo<select id="snakeSpeed"><option value="0">Gemütlich</option><option value="1">Normal</option><option value="2">Schnell</option></select></label>
<div class="small" id="hs"></div>
<button class="warn" onclick="if(confirm('Alle Highscores löschen?'))post('/api/hsreset').then(()=>{toast('Gelöscht');load()})">Highscores zurücksetzen</button></section>

<section><h2>WLAN</h2>
<p class="small" id="wifiinfo"></p>
<label>Heimnetz (SSID)<input type="text" id="ssid" autocomplete="off"></label>
<label>WLAN-Passwort<input type="password" id="pass" placeholder="unverändert"></label>
<label>Setup-WLAN-Passwort<input type="text" id="apPass" placeholder="leer = offen"></label>
<label>WLAN immer an<input type="checkbox" id="wifiAlways"></label>
<p class="small">Ohne „immer an“ ist das WLAN nur im Setup-Modus aktiv. Das hält die Controller-Verbindung beim Spielen schnell, denn WLAN und Bluetooth teilen sich eine Antenne.</p>
<button onclick="saveWifi()">Speichern &amp; neu starten</button><button class="sec" onclick="$('#ssid').value='';$('#pass').value='';saveWifi()">Heimnetz vergessen</button></section>

<section><h2>Firmware-Update</h2>
<p class="small">Arduino IDE → Sketch → „Kompilierte Binärdatei exportieren“. Dann <b>RetroCYD.ino.bin</b> hier hochladen (nicht die *.merged.bin).</p>
<p class="small"><b>Zusammen mit der Wetteruhr:</b> Die Datei wird am Inhalt erkannt. Ein Uhr-Update (<b>WetterUhr_…_CYD_update.bin</b>) wird hier eingespielt; bei einem RetroCYD-Update (<b>RetroCYD_…_update.bin</b>) startet die Seite die Uhr und schickt die Datei dorthin. Erstinstallations-Dateien und Dateien für den ESP32-C3 werden abgelehnt.</p>
<input type="file" id="fw" accept=".bin"><br>
<button onclick="upload()">Hochladen &amp; installieren</button><button class="sec" onclick="sdUpdate()">Update von SD-Karte</button>
<div class="bar"><div id="prog"></div></div><p class="small" id="upmsg"></p></section>

<section><h2>System</h2><p class="small" id="sys"></p>
<button class="sec" onclick="post('/api/reboot');toast('Neustart...')">Neu starten</button></section>
</main><div id="toast"></div>
<script>
const $=s=>document.querySelector(s);
const post=(u,d)=>fetch(u,{method:'POST',headers:{'Content-Type':'application/x-www-form-urlencoded'},body:new URLSearchParams(d||{})}).then(r=>r.json()).catch(()=>({}));
const esc=t=>String(t).replace(/[&<>"]/g,c=>({'&':'&amp;','<':'&lt;','>':'&gt;','"':'&quot;'}[c]));
function toast(t){const e=$('#toast');e.textContent=t;e.style.display='block';clearTimeout(e._t);e._t=setTimeout(()=>e.style.display='none',2500);}
const VALS=['rot','bright','st7789','rumbleStr','snakeSpeed','ledBri','sndVol'], CHK=['mirror','invert','rumbleOn','allowNew','ledOn','sndOn'];
fetch('/api/sndlist').then(r=>r.json()).then(a=>{$('#sndSel').innerHTML=a.map((n,i)=>`<option value="${i}">${esc(n)}</option>`).join('');}).catch(()=>{});
let first=true;
async function load(){
 let s; try{s=await (await fetch('/api/state')).json();}catch(e){$('#info').textContent='keine Verbindung';return;}
 $('#info').textContent='Firmware '+s.fw+' · '+s.ip;
 if(first){first=false;
  VALS.forEach(k=>$('#'+k).value=s[k]); CHK.forEach(k=>$('#'+k).checked=s[k]);
  $('#wifiAlways').checked=s.wifiAlways; $('#ssid').value=s.ssid; $('#apPass').value=s.apPass;}
 $('#wifiinfo').textContent=s.ap?'Aktuell: eigenes WLAN „'+s.apName+'“':'Verbunden mit „'+s.ssid+'“';
 $('#hs').innerHTML='<table style="width:100%;border-collapse:collapse">'+s.games.map((g,i)=>`<tr style="border-top:1px solid #26264a"><td style="padding:5px 0"><b style="color:#e8e8f0">${esc(g.n)}</b><br><span>${esc(g.d)}</span></td><td style="text-align:right;color:#ffd400;font-weight:bold">${s.hs[i]}</td></tr>`).join('')+'</table>';
 $('#pairinfo').innerHTML=s.pairing?'<b style="color:#ffd400">Kopplung aktiv – noch '+s.pairing+' s</b>':'';
 $('#pads').innerHTML=s.pads.length?s.pads.map(p=>`<div class="pad"><span>🎮 ${esc(p.model)}${p.bat?' · Akku '+p.bat+' %':''}</span><span><button class="sec" onclick="bt('rumble',${p.i})">Vibrieren</button><button class="warn" onclick="bt('disconnect',${p.i})">Trennen</button></span></div>`).join(''):'<p class="small">Kein Controller verbunden.</p>';
 $('#sys').textContent='Freier Speicher: '+Math.round(s.heap/1024)+' KB · SD-Karte: '+(s.sd?'OK':'nicht gefunden');
}
function saveSettings(){
 const d={}; VALS.forEach(k=>d[k]=$('#'+k).value); CHK.forEach(k=>d[k]=$('#'+k).checked?1:0);
 post('/api/settings',d).then(r=>{ if(r.restart){toast('Neustart...');setTimeout(()=>location.reload(),7000);} else toast('Gespeichert'); });
}
[...VALS,...CHK].forEach(k=>$('#'+k).addEventListener('change',saveSettings));
function bt(a,i){ if(a=='forget'&&!confirm('Alle gespeicherten Controller-Kopplungen löschen?'))return;
 post('/api/bt',{action:a,idx:i??0}).then(()=>{toast('OK');setTimeout(load,600);}); }
function saveWifi(){
 post('/api/wifi',{ssid:$('#ssid').value,pass:$('#pass').value,apPass:$('#apPass').value,always:$('#wifiAlways').checked?1:0})
 .then(r=>{ if(r.err){toast(r.err);return;} toast('Gespeichert – Neustart...'); });
}
// Datei am Inhalt pruefen: echte Programmdatei (kein Bootloader/Erstinstallation), Chip ESP32, Uhr oder RetroCYD.
// Die Kennungen stehen um 1 verschoben hier (sonst wuerden sie im Programm selbst stecken).
async function fwKind(f){const b=new Uint8Array(await f.arrayBuffer()),n=b.length;if(n<4096)return'bad';
 if(b[0]!=0xE9)return(b[0]==0xFF&&n>0x9000&&b[0x1000]==0xE9)?'merged':'bad';
 if(!(b[32]==0x32&&b[33]==0x54&&b[34]==0xCD&&b[35]==0xAB))return'merged';
 if((b[12]|b[13]<<8)!=0)return'board';
 const has=e=>{const m=[...e].map(c=>c.charCodeAt(0)-1),l=m.length;o:for(let i=0;i<=n-l;i++){if(b[i]!=m[0])continue;for(let k=1;k<l;k++)if(b[i+k]!=m[k])continue o;return true;}return false;};
 if(has('SFUSPDZE.BQQ.JE.2'))return'games';if(has('XFUUFSVIS.BQQ.JE.2'))return'clock';return'old';}
function send(url,f,done){
 const fd=new FormData(); fd.append('firmware',f,f.name);
 const x=new XMLHttpRequest(); x.open('POST',url);
 x.upload.onprogress=e=>{if(e.lengthComputable){const p=Math.round(e.loaded/e.total*100);$('#prog').style.width=p+'%';$('#upmsg').textContent='Übertrage: '+p+' %';}};
 x.onload=()=>done(x);
 x.onerror=()=>$('#upmsg').textContent='Verbindung abgebrochen';
 $('#prog').style.width='0%'; $('#upmsg').textContent='Übertrage...'; x.send(fd);
}
async function upload(){
 const f=$('#fw').files[0]; if(!f){toast('Bitte .bin-Datei wählen');return;}
 const m=$('#upmsg'); m.textContent='Prüfe Datei...';
 let k; try{k=await fwKind(f);}catch(e){k='bad';}
 if(k=='merged'){m.textContent='Das ist eine Erstinstallations-Datei (mit Bootloader) – nur per USB an 0x0. Hier bitte *_update.bin wählen.';return;}
 if(k=='board'){m.textContent='Diese Datei ist für den ESP32-C3, nicht für das CYD.';return;}
 if(k=='old'){m.textContent='Datei nicht erkannt (zu alte Version?). Bitte eine aktuelle *_update.bin verwenden.';return;}
 if(k=='bad'){m.textContent='Das ist keine passende Firmware-Datei.';return;}
 let st={};try{st=await (await fetch('/api/state',{cache:'no-store'})).json();}catch(e){}
 if(k=='games'&&st.clock){                       // RetroCYD-Update: das spielt die Uhr ein -> Uhr starten, Datei dorthin
  if(!confirm(f.name+'\nErkannt: RetroCYD-Update.\nDie Uhr wird gestartet und spielt es ein. Weiter?'))return;
  m.textContent='Starte die Uhr ...';
  try{await fetch('/api/toclock',{method:'POST'});}catch(e){}
  const t0=Date.now();
  const poll=()=>{if(Date.now()-t0>120000){m.textContent='Uhr meldet sich nicht – bitte im Browser der Uhr einspielen.';return;}
   fetch('/fwprep?name=retrocyd',{cache:'no-store'}).then(r=>r.text()).then(x=>{
    if(x=='direct'||x=='relay'){m.textContent='Uhr bereit – übertrage RetroCYD ...';
     send('/update?size='+f.size+'&app=games',f,x=>{if(x.status==200){$('#prog').style.width='100%';m.textContent='RetroCYD aktualisiert. Spiele starten ...';fetch('/games',{method:'POST'}).catch(()=>{});setTimeout(()=>location.reload(),15000);}
      else m.textContent='Update fehlgeschlagen: '+x.responseText;});}
    else setTimeout(poll,2000);}).catch(()=>setTimeout(poll,2000));};
  setTimeout(poll,5000);
  return;
 }
 const what=k=='clock'?(st.clock?'Uhr-Update (Wetteruhr)':'Wetteruhr neben RetroCYD installieren'):'RetroCYD-Update';
 if(!confirm(f.name+'\nErkannt: '+what+'. Weiter?'))return;
 send('/update?size='+f.size+'&app='+k,f,x=>{let r={};try{r=JSON.parse(x.responseText)}catch(e){}
  if(r.ok){m.textContent=k=='clock'?'Uhr aktualisiert – startet ...':'Update erfolgreich – Konsole startet neu...';setTimeout(()=>location.reload(),12000);}
  else m.textContent='Update fehlgeschlagen: '+(r.err||x.status);});
}
function sdUpdate(){
 if(!confirm('Firmware von /retrocyd/update.bin installieren?'))return;
 $('#upmsg').textContent='Installiere von SD-Karte...';
 post('/api/sdupdate').then(r=>{ $('#upmsg').textContent=r.ok?'Update erfolgreich – Konsole startet neu...':'Fehler: '+(r.err||'unbekannt'); if(r.ok)setTimeout(()=>location.reload(),12000); });
}
load(); setInterval(load,3000);
</script></body></html>)HTML";
