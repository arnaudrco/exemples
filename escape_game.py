from datetime import datetime, timedelta
from flask import Flask, request, redirect, url_for, session, render_template_string

app = Flask(__name__)
app.secret_key = '6f5c8f9ad84b40b8af4f2f4a9c99d72781a2f965e244ee8f8b1035e2f0c9aefb'
ADMIN_PASSWORD = 'ToBeChanged'  # Change ce mot de passe pour sécuriser la page admin
TIMER_DURATION = timedelta(hours=1)

VILLES = [
    {"nom": "Viroflay", "mdp": "eiffel", "chiffre": "I"},
    {"nom": "Agon-Coutainville", "mdp": "fourviere", "chiffre": "L"},
    {"nom": "Aix-en-Provence", "mdp": "vieuxport", "chiffre": "9"},
    {"nom": "Gradignan", "mdp": "vin", "chiffre": "7"},
    {"nom": "Gardanne", "mdp": "violette", "chiffre": "O"},
    {"nom": "Stuttgart", "mdp": "promenade", "chiffre": "3"},
    {"nom": "Wageningen", "mdp": "elephant", "chiffre": "V"},
    {"nom": "Hossegor", "mdp": "friterie", "chiffre": "1"},
    {"nom": "Kourou", "mdp": "cathedrale", "chiffre": "E"},
    {"nom": "Montpellier", "mdp": "moutarde", "chiffre": "0"},
    {"nom": "Pessac", "mdp": "alpes", "chiffre": "U"},
]

etat_jeu = {
    "villes_validées": [],
    "classement": [],
    "start_time": None
}

@app.route("/", methods=["GET", "POST"])
def index():
    message = ""
    jeu_lance = etat_jeu["start_time"] is not None
    end_timestamp = ""
    start_time = etat_jeu["start_time"]

    if request.method == "POST" and jeu_lance:
        ville = request.form["ville"]
        motdepasse = request.form["motdepasse"]
        for v in VILLES:
            if v["nom"] == ville and v["mdp"] == motdepasse:
                if ville not in etat_jeu["villes_validées"]:
                    etat_jeu["villes_validées"].append(ville)
                    etat_jeu["classement"].append((ville, datetime.now()))
                break
        else:
            message = "Mot de passe incorrect."

    if jeu_lance:
        end_time = etat_jeu["start_time"] + TIMER_DURATION
        end_timestamp = int(end_time.timestamp() * 1000)

    toutes_validées = len(etat_jeu["villes_validées"]) == len(VILLES)
    return render_template_string(PAGE_HTML,
        villes=VILLES,
        valides=etat_jeu["villes_validées"],
        classement=etat_jeu["classement"],
        message=message,
        toutes_validées=toutes_validées,
        jeu_lance=jeu_lance,
        end_timestamp=end_timestamp,
        start_time=start_time
    )

@app.route("/admin", methods=["GET", "POST"])
def admin():
    if "admin" not in session:
        if request.method == "POST":
            if request.form.get("password") == ADMIN_PASSWORD:
                session["admin"] = True
                return redirect(url_for("admin"))
            else:
                return render_template_string(PAGE_ADMIN_LOGIN, error=True)
        return render_template_string(PAGE_ADMIN_LOGIN, error=False)
    return render_template_string(PAGE_ADMIN)

@app.route("/start")
def start():
    if "admin" not in session:
        return redirect(url_for("admin"))
    etat_jeu["start_time"] = datetime.now()
    return redirect(url_for("admin"))

@app.route("/reset")
def reset():
    if "admin" not in session:
        return redirect(url_for("admin"))
    etat_jeu["villes_validées"].clear()
    etat_jeu["classement"].clear()
    etat_jeu["start_time"] = None
    return redirect(url_for("admin"))

@app.route("/logout")
def logout():
    session.pop("admin", None)
    return redirect(url_for("index"))

PAGE_HTML = """<!doctype html>
<html>
<head><title>Escape Game</title>
<style>
body { font-family: sans-serif; background: #f0f0f0; padding: 20px; }
.ville { background: white; margin: 10px; padding: 10px; border-radius: 8px; }
.valide { background: #d4ffd4; }
</style>
<script>
function startTimer(endTime) {
    function updateTimer() {
        const now = new Date().getTime();
        const distance = endTime - now;
        if (distance < 0) {
            document.getElementById("timer").innerHTML = "00:00:00";
            clearInterval(x);
            return;
        }
        const h = Math.floor(distance / (1000 * 60 * 60));
        const m = Math.floor((distance % (1000 * 60 * 60)) / (1000 * 60));
        const s = Math.floor((distance % (1000 * 60)) / 1000);
        document.getElementById("timer").innerHTML =
            String(h).padStart(2, '0') + ":" +
            String(m).padStart(2, '0') + ":" +
            String(s).padStart(2, '0');
    }
    updateTimer();
    const x = setInterval(updateTimer, 1000);
}
{% if end_timestamp %}
window.onload = function() { startTimer({{ end_timestamp }}); };
{% endif %}
</script>
</head>
<body>
{% if not jeu_lance %}
    <h1>Tu es un peu impatient jeune homme !</h1>
    <p><a href="/admin">Administration</a></p>
{% else %}
    <h1>Escape Game - Tables</h1>
    <p><strong>Temps restant :</strong> <span id="timer"></span></p>
    {% if message %}<p style="color:red">{{ message }}</p>{% endif %}
    {% for v in villes %}
        <div class="ville {% if v.nom in valides %}valide{% endif %}">
            <strong>{{ v.nom }}</strong><br>
            {% if v.nom in valides %}
                Code : {{ v.chiffre }}
            {% else %}
                <form method="post">
                    <input type="hidden" name="ville" value="{{ v.nom }}">
                    <input type="text" name="motdepasse" placeholder="Mot de passe">
                    <button>Valider</button>
                </form>
            {% endif %}
        </div>
    {% endfor %}
    <h2>Classement</h2>
    <ol>
    {% for c in classement %}
        {% set delta = (c[1] - start_time).total_seconds() | int %}
        {% set h = delta // 3600 %}
        {% set m = (delta % 3600) // 60 %}
        {% set s = delta % 60 %}
        <li>{{ c[0] }} - {{ "%02d:%02d:%02d" % (h, m, s) }}</li>
    {% endfor %}
    </ol>
    {% if toutes_validées %}
        <h2 style="color:green">🎉 Félicitations à toutes les équipes ! 🎉</h2>
    {% endif %}
    <p><a href="/admin">Administration</a></p>
{% endif %}
</body>
</html>"""

PAGE_ADMIN_LOGIN = """<!doctype html>
<html><head><title>Admin</title></head><body>
<h1>Connexion admin</h1>
{% if error %}<p style="color:red;">Mot de passe incorrect</p>{% endif %}
<form method="post">
    <input type="password" name="password" placeholder="Mot de passe admin" required>
    <button>Se connecter</button>
</form>
</body></html>"""

PAGE_ADMIN = """<!doctype html>
<html><head><title>Admin</title></head><body>
<h1>Page d'administration</h1>
<p><a href="/start">▶️ Lancer le jeu (1h)</a></p>
<p><a href="/reset">🔄 Réinitialiser le jeu</a></p>
<p><a href="/logout">🚪 Se déconnecter</a></p>
<p><a href="/">Retour au jeu</a></p>
</body></html>"""
