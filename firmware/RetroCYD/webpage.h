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
<p class="small"><b>Zusammen mit der Wetteruhr:</b> Hier wird die <b>Uhr</b> aktualisiert (Datei <b>WetterUhr_…_CYD_update.bin</b>). RetroCYD-Updates (<b>RetroCYD_…_update.bin</b>) im Browser der Uhr unter System → Firmware-Update einspielen.</p>
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
const VALS=['rot','bright','st7789','rumbleStr','snakeSpeed','ledBri'], CHK=['mirror','invert','rumbleOn','allowNew','ledOn'];
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
function upload(){
 const f=$('#fw').files[0]; if(!f){toast('Bitte .bin-Datei wählen');return;}
 const fd=new FormData(); fd.append('firmware',f,f.name);
 const x=new XMLHttpRequest(); x.open('POST','/update?size='+f.size);
 x.upload.onprogress=e=>{if(e.lengthComputable){const p=Math.round(e.loaded/e.total*100);$('#prog').style.width=p+'%';$('#upmsg').textContent='Übertrage: '+p+' %';}};
 x.onload=()=>{let r={};try{r=JSON.parse(x.responseText)}catch(e){}
  if(r.ok){$('#upmsg').textContent='Update erfolgreich – Konsole startet neu...';setTimeout(()=>location.reload(),12000);}
  else $('#upmsg').textContent='Update fehlgeschlagen: '+(r.err||x.status);};
 x.onerror=()=>$('#upmsg').textContent='Verbindung abgebrochen';
 $('#upmsg').textContent='Übertrage...'; x.send(fd);
}
function sdUpdate(){
 if(!confirm('Firmware von /retrocyd/update.bin installieren?'))return;
 $('#upmsg').textContent='Installiere von SD-Karte...';
 post('/api/sdupdate').then(r=>{ $('#upmsg').textContent=r.ok?'Update erfolgreich – Konsole startet neu...':'Fehler: '+(r.err||'unbekannt'); if(r.ok)setTimeout(()=>location.reload(),12000); });
}
load(); setInterval(load,3000);
</script></body></html>)HTML";
