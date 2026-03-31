import network
import time
import uasyncio as asyncio
import ujson
import ntptime
import machine

import os


# OLED
from machine import Pin, I2C
import ssd1306
import time

# Configurer la communication I2C
gnd = Pin(32, Pin.OUT)
plus = Pin(33, Pin.OUT)
plus.value(1)
gnd.value(0)

time.sleep(1) 

i2c = I2C(0, sda=Pin(26), scl=Pin(25))
oled = ssd1306.SSD1306_I2C(128, 64, i2c)

# Effacer l'écran à nouveau en le remplissant de noir
oled.fill(0)
oled.show()
time.sleep(1)  # Attendre encore une seconde

# Afficher du texte sur l'écran OLED
oled.text('OPEN BEKEN', 0, 0)  # Afficher "Hello," à la position (0, 0)
oled.text('Arnaud', 0, 16)  # Afficher "sunfounder.com" à la position (0, 16)
oled.show()

# gros caractères
import framebuf

def big_text(oled, text, x, y, scale=2):
    w = 8 * len(text)
    h = 8
    buf = bytearray(w * h // 8)
    fb = framebuf.FrameBuffer(buf, w, h, framebuf.MONO_HLSB)
    fb.text(text, 0, 0, 1)
    for i in range(w):
        for j in range(h):
            if fb.pixel(i, j):
                oled.fill_rect(x + i * scale, y + j * scale, scale, scale, 1)

# --- Paramètres réseau ESP32 ---
ESP32_IP = "192.168.1.101"
NETMASK = "255.255.255.0"
GATEWAY = "192.168.0.1"
DNS = "8.8.8.8"

# --- IP fixe de la prise ---
PRISE_IP = "192.168.1.198"

# --- Fichier de sauvegarde ---
FICHIER_SVG = "sauvegarde_solaire.json"

# --- Variables globales ---
derniere_conso = {}
etat_prise = "OFF"
etat_led_bleue = "OFF"
historique_5min = []
dernier_total_kwh_5min = None
temps_depart_5min = time.time()
derniere_sauvegarde = time.time()
conso_hier = 0.0
total_kwh_debut_jour = 0.0

# --- Pic solaire ---
max_power_today = 0
heure_pic = "--:--"
max_power_record = 0.0
datetime_record = "--/-- --:--"
jour_actuel = time.localtime()[2]  # jour du mois

# --- Dashboard HTML + JS (graphe inclus) ---
dashboard_html = """<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Dashboard Solaire 24h</title>
<style>
body {font-family:'Segoe UI',Tahoma,sans-serif;background:linear-gradient(135deg,#1a1a2e 0%,#16213e 100%);color:#fff;text-align:center;margin:0;padding:20px;}
.cards-row {display:flex;flex-wrap:wrap;justify-content:center;gap:20px;}
.card {background:rgba(22,33,62,0.95);border-radius:20px;padding:25px;flex:1;min-width:320px;max-width:380px;}
.data {font-size:22px;color:#000;font-weight:bold;background:linear-gradient(45deg,#f1c40f,#f39c12);padding:12px;border-radius:10px;margin:10px 0;display:flex;justify-content:space-between;}
.data-small {font-size:14px;color:#fff;background:rgba(44,62,80,0.8);padding:10px;border-radius:8px;margin:6px 0;display:flex;justify-content:space-between;}
.data-green {background:linear-gradient(45deg,#27ae60,#2ecc71);color:#fff;font-size:18px;padding:12px;border-radius:10px;margin:10px 0;display:flex;justify-content:space-between;font-weight:bold;}
.btn {padding:15px 30px;margin:10px 5px;font-size:16px;color:#fff;text-decoration:none;border-radius:12px;}
.btn-on {background:linear-gradient(45deg,#4cd137,#44bd32);}
.btn-off {background:linear-gradient(45deg,#e84118,#c23616);}
.btn-led-on {background:linear-gradient(45deg,#0984e3,#3498db);}
</style>
</head>
<body>

<h1>☀️ Solaire LSPA8 - 24h (5min)</h1>

<div class="cards-row">

<!-- Commandes -->
<div class="card">
<h2>🔧 Commandes</h2>
<h3>Prise 220V <span id="etat_prise">OFF</span></h3>
<a href="/on" class="btn btn-on">ALLUMER</a>
<a href="/off" class="btn btn-off">ÉTEINDRE</a>
<h3>LED Wifi <span id="etat_led">OFF</span></h3>
<a href="/ledon" class="btn btn-led-on">ALLUMER</a>
<a href="/ledoff" class="btn btn-off">ÉTEINDRE</a>
</div>

<!-- Production Live -->
<div class="card">
<h2>⚡ Production Live</h2>
<div class="data"><span>Puissance</span><span id="power">0 W</span></div>
<div class="data-small"><span>Tension</span><span id="voltage">0 V</span></div>
<div class="data-small"><span>Courant</span><span id="current">0 A</span></div>
<div class="data-small"><span>App. Power</span><span id="apparent_power">0 VA</span></div>
<div class="data-small"><span>P Réactive</span><span id="reactive_power">0 VAr</span></div>
<div class="data-small"><span>Facteur</span><span id="factor">0</span></div>
<div class="data-small"><span>E totale</span><span id="energy_total">0 Wh</span></div>
</div>

<!-- Énergie 24h -->
<div class="card">
<h2>🌱 Énergie + 📈 24h</h2>
<div class="data-green"><span>Aujourd'hui</span><span id="today">0 Wh</span></div>
<div class="data-green"><span>Hier</span><span id="yesterday">0 Wh</span></div>
<div class="data-green"><span>Total</span><span id="total">0 Wh</span></div>
</div>

<!-- Pic solaire -->
    <div class="card">
        <h2>🌟 Pic solaire aujourd'hui</h2>
        <div class="data"><span>Max Puissance</span><span id="max_power">0 W</span></div>
        <div class="data"><span>Heure du pic (raz à minuit) </span><span id="time_peak">--:--</span></div>
    </div>

<!-- Fin de la première rangée -->
</div>

<!-- Deuxième rangée : Graphe + Record -->
<div class="cards-row" style="margin-top:30px;">
    <!-- Graphe 24h (à gauche) -->
    <div id="graphe_container"></div>

    <!-- Relevé si déconnexion (à droite) -->
    <div class="card">
        <h2>🏆 Historique </h2>
        <div class="data"><span>Maximum</span><span id="max_record">0 W</span></div>
        <div class="data"><span>Horodatage</span><span id="time_record">--/-- --:--</span></div>
    </div>
</div>

<script>
// --- Envoi automatique de l'heure du téléphone/PC vers l'ESP32 ---
(function syncTime(){
    try {
        let n = new Date();
        fetch(`/set_time?y=${n.getFullYear()}&mo=${n.getMonth()+1}&d=${n.getDate()}&h=${n.getHours()}&m=${n.getMinutes()}&s=${n.getSeconds()}`).catch(e=>console.log(e));
    } catch(e) {}
})();

async function updateData(){
    try{
        const resp = await fetch('/status');
        const data = await resp.json();

        // Production Live
        document.getElementById('power').textContent = data.power.toFixed(0)+' W';
        document.getElementById('voltage').textContent = data.voltage.toFixed(1)+' V';
        document.getElementById('current').textContent = data.current.toFixed(3)+' A';
        document.getElementById('apparent_power').textContent = data.apparent_power.toFixed(0)+' VA';
        document.getElementById('reactive_power').textContent = data.reactive_power.toFixed(0)+' VAr';
        document.getElementById('factor').textContent = data.factor.toFixed(2);
        document.getElementById('energy_total').textContent = data.energy_total.toFixed(2)+' kWh';

        // Énergie 24h
        let hist = data.historique || [];
        // On utilise directement les valeurs envoyées par le serveur pour plus de précision
        document.getElementById('today').textContent = (data.today || 0).toFixed(3)+' Wh';
        document.getElementById('yesterday').textContent = (data.yesterday || 0).toFixed(3)+' Wh';
        document.getElementById('total').textContent = (data.total || 0).toFixed(3)+' Wh';
        document.getElementById('energy_total').textContent = (data.energy_total || 0).toFixed(3)+' Wh';

        // États
        document.getElementById('etat_prise').textContent = data.etat_prise;
        document.getElementById('etat_led').textContent = data.etat_led;

        // Pic solaire
        document.getElementById('max_power').textContent = (data.max_power_today || 0).toFixed(0) + ' W';
        document.getElementById('time_peak').textContent = data.heure_pic || '--:--';

        // Record Historique
        document.getElementById('max_record').textContent = (data.max_power_record || 0).toFixed(0) + ' W';
        document.getElementById('time_record').textContent = data.datetime_record || '--/-- --:--';

        // Graphe (existant)
        if(hist.length>0){
            let largeur=680, hauteur=300, marge=60;
            let max_conso=Math.max(...hist), min_conso=Math.min(...hist);
            let ecart=Math.max(max_conso-min_conso,0.001);
            let svg='<svg width="'+largeur+'" height="'+hauteur+'" viewBox="0 0 '+largeur+' '+hauteur+'">';
            svg+='<rect x="0" y="0" width="'+largeur+'" height="'+hauteur+'" rx="12" fill="none" stroke="#2c3e50" stroke-width="3"/>';
            svg+='<text x="20" y="30" font-size="24" fill="#f39c12">☀️</text>';
            svg+='<rect x="'+marge+'" y="'+marge+'" width="'+(largeur-2*marge)+'" height="'+(hauteur-2*marge)+'" rx="10" fill="none" stroke="#2c3e50" stroke-width="2"/>';
            for(let i=0;i<=5;i++){
                let y=marge+i*(hauteur-2*marge)/5;
                let val=(max_conso-i*ecart/5).toFixed(2);
                svg+='<line x1="'+marge+'" y1="'+y+'" x2="'+(largeur-marge)+'" y2="'+y+'" stroke="#ccc" stroke-width="0.5" stroke-dasharray="4,4"/>';
                svg+='<text x="'+(marge-5)+'" y="'+(y+4)+'" text-anchor="end" fill="#a4b0be" font-size="12">'+val+'</text>';
            }
            svg+='<line x1="'+marge+'" y1="'+(hauteur-marge)+'" x2="'+(largeur-marge)+'" y2="'+(hauteur-marge)+'" stroke="#34495e" stroke-width="2"/>';
            svg+='<line x1="'+marge+'" y1="'+marge+'" x2="'+marge+'" y2="'+(hauteur-marge)+'" stroke="#34495e" stroke-width="2"/>';
            let points = hist.map((c,i)=>{
                let x = marge + (largeur-2*marge)*i/Math.max(1,hist.length-1);
                let y = hauteur-marge-(hauteur-2*marge)*(c-min_conso)/ecart;
                return [x,y];
            });
            let path = "M "+points.map(p=>p.join(",")).join(" L ");
            svg+='<path d="'+path+'" fill="none" stroke="#f1c40f" stroke-width="4" stroke-linecap="round" stroke-linejoin="round"/>';
            svg+='<path d="'+path+' Z" fill="#f1c40f" fill-opacity="0.2"/>';
            points.forEach(p=>{svg+='<circle cx="'+p[0]+'" cy="'+p[1]+'" r="4" fill="#f1c40f" stroke="#fff" stroke-width="1.5"/>';});
            let dateFin = new Date();
            let dateDebut = new Date(dateFin.getTime() - ((hist.length>0?hist.length-1:0)*5*60000));
            let strHeureDebut = dateDebut.getHours().toString().padStart(2,'0')+':'+dateDebut.getMinutes().toString().padStart(2,'0');
            let strHeureFin = dateFin.getHours().toString().padStart(2,'0')+':'+dateFin.getMinutes().toString().padStart(2,'0');
            svg+='<text x="'+(marge+10)+'" y="'+(hauteur-10)+'" fill="#7f8fa6" font-size="12">'+strHeureDebut+'</text>';
            svg+='<text x="'+(largeur-marge-10)+'" y="'+(hauteur-10)+'" text-anchor="end" fill="#7f8fa6" font-size="12">'+strHeureFin+' (Actuel)</text>';
            svg+='<text x="'+(largeur/2)+'" y="25" text-anchor="middle" fill="#f1c40f" font-size="18" font-weight="bold">Consommation 24h</text>';
            svg+='</svg>';
            document.getElementById("graphe_container").innerHTML=svg;
        }

    }catch(e){console.log("Erreur updateData:",e);}
}
setInterval(updateData,3000);
updateData();
</script>

</body>
</html>
"""

# --- Gestion Sauvegarde / Chargement ---
def charger_donnees():
    global historique_5min, dernier_total_kwh_5min
    global max_power_today, heure_pic, jour_actuel
    global conso_hier, total_kwh_debut_jour
    try:
        with open(FICHIER_SVG, "r") as f:
            data = ujson.loads(f.read())
            historique_5min = data.get("historique", [])
            dernier_total_kwh_5min = data.get("dernier_total", None)
            
            # Forcer la conversion numérique pour éviter tout bug issu d'anciennes sauvegardes
            try:
                max_power_today = float(data.get("max_power_today", 0))
            except:
                max_power_today = 0.0
                
            heure_pic = data.get("heure_pic", "--:--")
            
            try:
                max_power_record = float(data.get("max_power_record", 0.0))
            except:
                max_power_record = 0.0
                
            datetime_record = data.get("datetime_record", "--/-- --:--")
            
            try:
                jour_actuel = int(data.get("jour_actuel", 0))
            except:
                jour_actuel = 0
            
            conso_hier = float(data.get("conso_hier", 0.0))
            total_kwh_debut_jour = float(data.get("total_kwh_debut_jour", 0.0))

            # --- Synchronisation de sécurité au premier démarrage du nouveau code ---
            if max_power_today > max_power_record:
                max_power_record = max_power_today
                if datetime_record == "--/-- --:--":
                    datetime_record = f"Auj. {heure_pic}"
                
            print("✅ Données historiques chargées avec succès.")
    except Exception as e:
        print("ℹ️ Aucune donnée de sauvegarde trouvée ou fichier corrompu. Démarrage à zéro.", e)

def sauvegarder_donnees():
    try:
        data = {
            "historique": historique_5min,
            "dernier_total": dernier_total_kwh_5min,
            "max_power_today": max_power_today,
            "heure_pic": heure_pic,
            "max_power_record": max_power_record,
            "datetime_record": datetime_record,
            "jour_actuel": jour_actuel,
            "conso_hier": conso_hier,
            "total_kwh_debut_jour": total_kwh_debut_jour
        }
        with open(FICHIER_SVG, "w") as f:
            f.write(ujson.dumps(data))
        print("💾 Données sauvegardées.")
    except Exception as e:
        print("❌ Erreur lors de la sauvegarde :", e)

# --- Connexion WiFi ---
def connect_wifi():
    wlan = network.WLAN(network.STA_IF)
    wlan.active(True)
    if not wlan.isconnected():
        wlan.ifconfig((ESP32_IP, NETMASK, GATEWAY, DNS))
        print("Connexion WiFi avec IP fixe...")
        wlan.connect("NUMERICABLE-63","camille01")  # <-- ton SSID / mot de passe
        while not wlan.isconnected():
            time.sleep(1)
            print(".", end="")
    ip = wlan.ifconfig()[0]
    print("\n✅ ESP32 connectée à l'IP:", ip)
    oled.fill(0)
    oled.text('OpenBK', 0, 0)  # Afficher "Hello," à la position (0, 0)
    oled.text(ip, 0, 16)  # Afficher "Hello," à la position (0, 0)
    oled.show()
    # Synchronisation de l'heure via NTP
    print("Synchronisation locale de l'heure...")
    # On spécifie le serveur NTP explicitement (pool.ntp.org par défaut, parfois instable)
    ntptime.host = "fr.pool.ntp.org"
    
    succes_ntp = False
    for i in range(5): # On tente 5 fois max
        try:
            ntptime.settime()
            print("✅ Heure synchronisée (Sources NTP)")
            succes_ntp = True
            break # C'est bon, on sort de la boucle
        except Exception as e:
            print(f"⚠️ Tentative NTP {i+1}/5 échouée ({e}). Nouvelle tentative dans 2s...")
            time.sleep(2)
            
    if not succes_ntp:
        print("❌ Erreur critique : Impossible de récupérer l'heure NTP après 5 tentatives.")

    return ip

def get_heure_locale():
    # --- AJUSTEMENT DÉCALAGE HORAIRE ---
    # Si vous avez 1h en trop, c'est que l'ESP a déjà l'heure locale (NTP ou Thonny).
    # Dans ce cas, les valeurs doivent être à 0.
    # Si le décalage revient lorsque l'ESP32 tourne de façon autonome (branchée sur secteur), 
    # remettez 3600 (Hiver) et 7200 (Eté).
    OFFSET_HIVER = 0      # (Avant: 3600)
    OFFSET_ETE = 3600     # (Avant: 7200)
    
    t = time.localtime(time.time())
    mois = t[1]
    
    # Si on est entre Avril (4) et Octobre (10), c'est souvent l'heure d'été
    offset = OFFSET_ETE if 4 <= mois <= 10 else OFFSET_HIVER
    return time.localtime(time.time() + offset)

# --- Requête HTTP vers la prise ---
async def requete_simple(path):
    try:
        reader, writer = await asyncio.open_connection(PRISE_IP, 80)
        req = f"GET {path} HTTP/1.1\r\nHost: {PRISE_IP}\r\nConnection: close\r\n\r\n"
        writer.write(req.encode())
        await writer.drain()
        res = b""
        while True:
            chunk = await reader.read(1024)
            if not chunk: break
            res += chunk
        writer.close()
        await writer.wait_closed()
        parts = res.split(b"\r\n\r\n",1)
        if len(parts) > 1: return parts[1]
    except Exception as e:
        print("❌ Requête échouée:", path, e)
    return b""

# --- Lecture consommation ---
async def lire_consommation_tache():
    global derniere_conso, dernier_total_kwh_5min, temps_depart_5min
    global etat_prise, etat_led_bleue, historique_5min
    global max_power_today, heure_pic, max_power_record, datetime_record
    global jour_actuel, conso_hier, total_kwh_debut_jour
    
    oled.fill(0)
#    oled.text('ENERGIE', 0, 0)  # Afficher "Hello," à la position (0, 0)
    oled.text("{:.2f}".format(total_kwh_debut_jour), 0, 0, 1)
    oled.text("{:.2f}".format(dernier_total_kwh_5min), 0, 10, 1)
    oled.text("{:.2f}".format(max_power_today), 0, 20, 1)
    oled.text("{:.2f}".format(conso_hier), 0, 30, 1)
    oled.show()

#    big_text(oled, "{:.2f}".format(total_kwh_debut_jour), 0, 32, scale=2)
#oled.text(total_kwh_debut_jour, 0, 16)  # Afficher "Hello," à la position (0, 0)
    oled.show()

    while True:
        try:
            body = await requete_simple("/cm?cmnd=Status%208")
            if body:
                donnees = ujson.loads(body)
                if "StatusSNS" in donnees and "ENERGY" in donnees["StatusSNS"]:
                    energie = donnees["StatusSNS"]["ENERGY"]
                    total_kwh = float(energie.get("ConsumptionTotal",0))
                    puissance = float(energie.get("Power",0))
                    
                    tm_now = get_heure_locale()
                    
                    
                    if tm_now[2] != jour_actuel:
                        if jour_actuel == 0:
                            print(f"🔄 Premier démarrage ou mise à l'heure ({tm_now[2]}). On garde le pic actuel.")
                        else:
                            print(f"🔄 Nouveau jour détecté ({jour_actuel} -> {tm_now[2]}). Transfert Aujourd'hui -> Hier.")
                            # On prend la somme actuelle du graphique pour "Hier" avant qu'elle ne glisse
                            conso_hier = round(sum(historique_5min), 3)
                            total_kwh_debut_jour = total_kwh
                            max_power_today = 0.0
                            heure_pic = "--:--"
                        jour_actuel = tm_now[2]
                        
                    # --- INITIALISATION DE SÉCURITÉ ---
                    # Si total_kwh_debut_jour est à 0, on essaie de le synchroniser avec la prise
                    if total_kwh_debut_jour == 0:
                        plug_today = float(energie.get("Today", 0))
                        total_kwh_debut_jour = round(total_kwh - plug_today, 3)
                        print(f"📍 Initialisation conso début jour: {total_kwh_debut_jour} Wh (basée sur Today prise: {plug_today})")
                        sauvegarder_donnees()
                        
                    # --- DEBUG DIAGNOSTIC (ACTIVÉ) ---
                    print(f"🔍 DEBUG -> Puissance lue: {puissance}W | Max en mémoire: {max_power_today}W")
                    
                    try:
                        # Sécurité: s'assurer des types ET prendre la valeur absolue (sans le moins)
                        # Car -12.7W ne sera jamais plus grand que 0W mathématiquement !
                        puissance_absolue = abs(float(puissance))
                        max_power_today = float(max_power_today)
                        
                        if puissance_absolue > max_power_today:
                            print(f"📈 NOUVEAU PIC BATTU ! Ancienne max: {max_power_today}W -> Nouvelle max: {puissance_absolue}W")
                            max_power_today = puissance_absolue
                            heure_pic = f"{tm_now[3]:02d}:{tm_now[4]:02d}"
                        
                        if puissance_absolue > max_power_record:
                            print(f"🏆 NOUVEAU RECORD ABSOLU ! {max_power_record}W -> {puissance_absolue}W")
                            max_power_record = puissance_absolue
                            datetime_record = f"{tm_now[2]:02d}/{tm_now[1]:02d} {tm_now[3]:02d}:{tm_now[4]:02d}"
                            sauvegarder_donnees() # On sauvegarde immédiatement le record
                    except Exception as loop_e:
                        print(f"🚨 ERREUR CRITIQUE DANS LA BOUCLE PIC: {loop_e}")
                        
                    derniere_conso = {
                        "voltage": float(energie.get("Voltage",0)),
                        "current": float(energie.get("Current",0)),
                        "power": puissance,
                        "apparent_power": float(energie.get("ApparentPower",0)),
                        "reactive_power": float(energie.get("ReactivePower",0)),
                        "factor": float(energie.get("Factor",0)),
                        "energy_total": total_kwh,
                        "today": float(energie.get("Today",0)),
                        "yesterday": float(energie.get("Yesterday",0)),
                        "total": total_kwh
                    }
                    maintenant = time.time()
                    if dernier_total_kwh_5min is None:
                        dernier_total_kwh_5min = total_kwh
                        sauvegarder_donnees() # Sauvegarde initiale
                        
                    if maintenant - temps_depart_5min >= 300:# 300 pour 5 minutes
                        conso_5min = round(total_kwh - dernier_total_kwh_5min,3)
                        historique_5min.append(conso_5min)
                        if len(historique_5min) > 288: historique_5min.pop(0)
                        dernier_total_kwh_5min = total_kwh
                        temps_depart_5min = maintenant
                        
                    # Sauvegarde toutes les 30 minutes (1800 secondes) pour éviter d'user la mémoire flash
                    global derniere_sauvegarde
                    if maintenant - derniere_sauvegarde >= 1800:
                        sauvegarder_donnees()
                        derniere_sauvegarde = maintenant

            body_pwr = await requete_simple("/cm?cmnd=Power0")
            if body_pwr:
                donnees_pwr = ujson.loads(body_pwr)
                etat_prise = donnees_pwr.get("POWER1", donnees_pwr.get("POWER","OFF"))
                etat_led_bleue = donnees_pwr.get("POWER2","OFF")
        except Exception as e:
            print("❌ Erreur:", e)
        await asyncio.sleep(3)

# --- Commandes ---
async def set_prise(etat):
    global etat_prise
    val = "ON" if etat else "OFF"
    if await requete_simple(f"/cm?cmnd=Power1%20{val}"):
        etat_prise = val
        return True
    return False

async def set_led_bleue(etat):
    global etat_led_bleue
    val = "ON" if etat else "OFF"
    if await requete_simple(f"/cm?cmnd=Power2%20{val}"):
        etat_led_bleue = val
        return True
    return False

# --- Endpoint JSON ---
async def envoyer_status(writer):
    global dernier_total_kwh_5min, total_kwh_debut_jour
    
    current_total = derniere_conso.get("energy_total", 0)
    
    # Calcul Aujourd'hui (depuis minuit) en live
    today_live = round(current_total - total_kwh_debut_jour, 3) if total_kwh_debut_jour > 0 else 0.0
    
    # --- Calcul du pending pour le graphique ---
    base_total = dernier_total_kwh_5min if dernier_total_kwh_5min is not None else current_total
    pending = round(current_total - base_total, 5)
    
    historique_visuel = historique_5min[-288:] + [pending]
    if len(historique_visuel) > 288:
        historique_visuel.pop(0)
        
    data = {
        "power": derniere_conso.get("power",0),
        "voltage": derniere_conso.get("voltage",0),
        "current": derniere_conso.get("current",0),
        "apparent_power": derniere_conso.get("apparent_power",0),
        "reactive_power": derniere_conso.get("reactive_power",0),
        "factor": derniere_conso.get("factor",0),
        "energy_total": current_total,
        "today": today_live,
        "yesterday": conso_hier,
        "total": current_total,
        "historique": historique_visuel,
        "etat_prise": etat_prise,
        "etat_led": etat_led_bleue,
        "max_power_today": max_power_today,
        "heure_pic": heure_pic,
        "max_power_record": max_power_record,
        "datetime_record": datetime_record
    }
    resp = ujson.dumps(data)
    writer.write(b"HTTP/1.0 200 OK\r\nContent-type: application/json\r\n\r\n")
    writer.write(resp.encode("utf-8"))
    await writer.drain()
    await writer.wait_closed()

# --- Serveur web ---
async def traiter_requete(reader, writer):
    try:
        request_line = await reader.readline()
        while await reader.readline() != b"\r\n": pass
        req = request_line.decode()
        if "/on" in req: await set_prise(True)
        if "/off" in req: await set_prise(False)
        if "/ledon" in req: await set_led_bleue(True)
        if "/ledoff" in req: await set_led_bleue(False)
        
        if "/set_time?" in req:
            try:
                # Format: GET /set_time?y=...&mo=...&d=...&h=...&m=...&s=... HTTP/1.x
                qs = req.split(' ')[1].split('?')[1]
                params = {k: int(v) for k, v in [p.split('=') for p in qs.split('&')]}
                # Configuration RTC ESP32 (année, mois, jour, jour_semaine, heures, minutes, secondes, microsecondes)
                machine.RTC().datetime((params['y'], params['mo'], params['d'], 0, params['h'], params['m'], params['s'], 0))
                print(f"🕒 Heure synchronisée (Web) : {params['h']:02d}:{params['m']:02d}:{params['s']:02d}")
            except Exception as e:
                print("Erreur set_time:", e)
            writer.write(b"HTTP/1.0 200 OK\r\n\r\nOK")
            await writer.drain()
            await writer.wait_closed()
            return
            
        if "/status" in req:
            await envoyer_status(writer)
            return
            
        if "/debug" in req:
            try:
                with open(FICHIER_SVG, "r") as f:
                    contenu = f.read()
                writer.write(b"HTTP/1.0 200 OK\r\nContent-type: application/json\r\n\r\n")
                writer.write(contenu.encode("utf-8"))
            except:
                writer.write(b"HTTP/1.0 404 Not Found\r\n\r\nFichier non trouve")
            await writer.drain()
            await writer.wait_closed()
            return
            
        writer.write(b"HTTP/1.0 200 OK\r\nContent-type: text/html\r\n\r\n")
        writer.write(dashboard_html.encode("utf-8"))
        await writer.drain()
        await writer.wait_closed()
    except Exception as e:
        print("Erreur web:", e)

# --- Main ---
async def main():
    charger_donnees() # On charge les données au démarrage !
    ip = connect_wifi()
    print(f"🚀 Dashboard: http://{ip}")
    server = await asyncio.start_server(traiter_requete,"0.0.0.0",80)
    async with server:
        await asyncio.gather(server.wait_closed(), lire_consommation_tache())

try:
    asyncio.run(main())
except KeyboardInterrupt:
    print("\n👋 Arrêté.")
