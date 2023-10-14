// **********************************************************
//  Page de connexion 'Acces Point' pour définir réseau WIFI
// **********************************************************
const char *ConnectAP_Html = R"====(
<!doctype html>
<html><head><meta charset="UTF-8"><style>
body {font-size:150%;text-align:center;width:1000px;margin:auto;background: background: linear-gradient(#eef,#77b5fe,#0f056b);background-attachment:fixed;}
form {margin:auto;padding:10px;}
form.form-example {display: table;text-align:left;}
div.form-example {display: table-row;padding:10px;}
label,input {display: table-cell;margin: 10px;text-align:left;font-size:120%;}
</style></head>
<body>
<h1>Routeur Solaire - RMS</h1><h4>Connexion au r&eacute;seau WIFI local</h4>
<form action='' method='get' >
<div class='form-example'>
<label for='ssid'>Entrez le nom du r&eacute;seau WIFI ou SSID : </label>
<input type='text' name='ssid' id='ssid' required>
</div>
<div class='form-example'>
<label for='passe'>Mot de passe du r&eacute;seau : </label>
<input type='password' name='passe' id='passe' required>
</div>
<div  style='text-align:right;padding-top:20px;'>
<input type='submit' value='Envoyer' >
</div>
</form>
</body></html>
)====";
//Switch Wifi Web Page 
const char *SwitchWifi_Html = R"====(
<!doctype html>
  <head>
    <title>F1ATB - ESP32 RMS</title>
  </head>
  <body>
    <h1>Basculez vers le r&eacute;seau WiFi : SSID</h1>    
  </body>
</html>
)====";