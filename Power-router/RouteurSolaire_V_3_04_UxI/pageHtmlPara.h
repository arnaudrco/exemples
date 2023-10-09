//***************************************************
// Page HTML et Javascript de gestion des Paramètres
//***************************************************
const char *ParaHtml = R"====(
  <!doctype html>
  <html><head><style>
    * {box-sizing: border-box;}
    body {font-size:150%;text-align:center;width:100%;max-width:1000px;margin:auto;background: linear-gradient(#003,#77b5fe,#003);background-attachment:fixed;color:white;}
    h2{text-align:center;color:white;}
    a:link {color:#aaf;text-decoration: none;}
    a:visited {color:#ccf;text-decoration: none;}
    .form {margin:auto;padding:10px;display: table;text-align:left;}
    .ligne {display: table-row;padding:10px;}
    label,input {display: table-cell;margin: 5px;text-align:left;font-size:20px;height:25px;}
    .boldT{text-align:left;font-weight:bold;display: table-row;}
    .onglets{margin-top:4px;left:0px;font-size:130%;}
    .Baccueil,.Bbrut,.Bparametres,.Bactions{margin-left:20px;border:outset 4px grey;background-color:#333;border-radius:6px;padding-left:20px;padding-right:20px;display:inline-block;}
    .Bparametres{border:inset 10px azure;}
    #BoutonsBas{display:flex;justify-content:space-between;margin-top:20px;}
  </style>
  <script src="/ParaJS"></script>
  </head>
  <body onLoad="Init();">
    <div class='onglets'><div class='Baccueil'><a href='/'>Accueil</a></div><div class='Bbrut'><a href='/Brute'>Donn&eacute;es brutes</a></div><div class='Bparametres'><a href='/Para'>Param&egrave;tres</a></div><div class='Bactions'><a href='/Actions'>Actions</a></div></div>
    <h2>Routeur Solaire - UxI</h2><h4>Param&egrave;tres</h4>
    <div class="form"  >
      <div class="boldT">Adresse IP de l'ESP</div>
      <div class='ligne'>
        <label for='dhcp'>Adresse IP auto (DHCP) : </label>
        <input type='checkbox' name='dhcp' id='dhcp' style='width:25px;' onclick="checkDisabled();">
      </div>
      <div class='ligne'>
        <label for='adrIP'>Adresse IP si fixe (ex : 192.168.1.245) : <br><span style='font-size:10px;'>N&eacute;cessite un Reset de l'ESP32</span></label>
        <input type='text' name='adrIP' id='adrIP' >
      </div>
      <div class='ligne'>
        <label for='gateway'>Gateway (ex : 192.168.1.254) :  <br><span style='font-size:10px;'>En g&eacute;n&eacute;ral l'adresse de votre box internet</span></label>
        <input type='text' name='gateway' id='gateway' >
      </div>
      <div class="boldT"><br>Lecture puissance d'un ESP Externe</div>
      <div class='ligne'>
        <label for='UxIExterne'>Connexion &agrave; un ESP-UxI externe : </label>
        <input type='checkbox' name='UxIExterne' id='UxIExterne' style='width:25px;' onclick="checkDisabled();">
      </div>
      <div class='ligne'>
        <label for='UxIExtIP'>Adresse IP ESP-UxI externe (ex : 192.168.1.248) : </label>
        <input type='text' name='UxIExtIP' id='UxIExtIP' >
      </div>
      <div class="boldT"><br>Envoi Puissance &agrave; Domoticz</div>
      <div class='ligne'>
        <label for='DomoRepete'>P&eacute;riode (s) rafra&icirc;chissement  (0= pas d'envoi) : </label>
        <input type='number' name='DomoRepete' id='DomoRepete'  onclick="checkDisabled();" >
      </div>
      <div class='ligne'>
        <label for='DomoIP'>Adresse IP host Domoticz (ex : 192.168.1.201) : </label>
        <input type='text' name='DomoIP' id='DomoIP' >
      </div>
      <div class='ligne'>
        <label for='DomoPort'>http port (ex : 8080) : </label>
        <input type='number' name='DomoPort' id='DomoPort' >
      </div>
      <div class='ligne'>
        <label for='DomoIdx'>Index dispositif (ex : 148) : </label>
        <input type='number' name='DomoIdx' id='DomoIdx' >
      </div>
      <div class="boldT"><br>Envoi Puissance &agrave; MQTT (Home Assistant)</div>
      <div class='ligne'>
        <label for='MQTTRepete'>P&eacute;riode (s) r&eacute;petition  (0= pas d'envoi) : </label>
        <input type='number' name='MQTTRepete' id='MQTTRepete'  onclick="checkDisabled();" >
      </div>
      <div class='ligne'>
        <label for='MQTTIP'>Adresse IP host MQTT (ex : 192.168.1.18) : </label>
        <input type='text' name='MQTTIP' id='MQTTIP' >
      </div>
      <div class='ligne'>
        <label for='MQTTPort'> port (ex : 1883) : </label>
        <input type='number' name='MQTTPort' id='MQTTPort' >
      </div>
      <div class='ligne'>
        <label for='MQTTUser'>MQTT User nom : </label>
        <input type='text' name='MQTTUser' id='MQTTUser' >
      </div>
      <div class='ligne'>
        <label for='MQTTpwd'>MQTT mot de passe : </label>
        <input type='password' name='MQTTpwd' id='MQTTpwd' >
      </div>
      <div class='ligne'>
        <label for='MQTTprefix'>MQTT Topics Prefix (ex : UxI/ ) : </label>
        <input type='text' name='MQTTprefix' id='MQTTprefix' >
      </div>
      <div class="boldT"><br>Calibration Mesures Ueff et Ieff</div>
      <div class='ligne'>
        <label for='CalibU'>Coefficient multiplicateur Ueff (typique : 1000) : </label>
        <input type='number' name='CalibU' id='CalibU'   >
      </div>
      <div class='ligne'>
        <label for='CalibI'>Coefficient multiplicateur Ieff (typique : 1000) : </label>
        <input type='number' name='CalibI' id='CalibI'   >
      </div>
    </div>
    <div  id='BoutonsBas'>
        <input type='button' onclick='Reset();' value='ESP32 Reset' >
        <input type='button' onclick="SendValues();" value='Sauvegarder' >
    </div>
  </body></html>
)====";
const char *ParaJS = R"====(
  function Init(){
    LoadParametres();
  }
  function LoadParametres() {
    var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() { 
          if (this.readyState == 4 && this.status == 200) {
             var LesParas=this.responseText;
             var Para=LesParas.split(',');  
             GID("dhcp").checked = Para[0]==1  ? true:false;
             GID("adrIP").value=int2ip(Para[1]);
             GID("gateway").value=int2ip(Para[2]);
             GID("UxIExterne").checked = Para[3]==1  ? true:false;
             GID("UxIExtIP").value=int2ip(Para[4]);
             GID("DomoRepete").value = Para[5];
             GID("DomoIP").value=int2ip(Para[6]);
             GID("DomoPort").value=Para[7];
             GID("DomoIdx").value=Para[8];
             GID("MQTTRepete").value = Para[9];
             GID("MQTTIP").value=int2ip(Para[10]);
             GID("MQTTPort").value=Para[11];
             GID("MQTTUser").value=decodeString(Para[12]);
             GID("MQTTpwd").value=decodeString(Para[13]);
             GID("MQTTprefix").value=decodeString(Para[14]);
             GID("CalibU").value=Para[15];
             GID("CalibI").value=Para[16];
             checkDisabled();
          }         
        };
        xhttp.open('GET', 'ParaAjax', true);
        xhttp.send();
      }
  function GID(id){return document.getElementById(id)};
  function SendValues(){
    var dhcp=GID("dhcp").checked ? 1:0;
    var UxIExterne=GID("UxIExterne").checked ? 1:0;
    var S="?dhcp="+ dhcp+"&adrIP="+ ip2int(GID("adrIP").value)+"&gateway="+ ip2int(GID("gateway").value);
    S +="&UxIExterne="+UxIExterne+"&UxIExtIP="+ ip2int(GID("UxIExtIP").value);
    S +="&DomoRepete="+GID("DomoRepete").value +"&DomoIP="+ip2int(GID("DomoIP").value)+"&DomoPort="+GID("DomoPort").value+"&DomoIdx="+GID("DomoIdx").value;
    S +="&MQTTRepete="+GID("MQTTRepete").value +"&MQTTIP="+ip2int(GID("MQTTIP").value) +"&MQTTPort="+GID("MQTTPort").value +"&MQTTUser="+encodeString(GID("MQTTUser").value)+"&MQTTpwd="+encodeString(GID("MQTTpwd").value);
    S +="&MQTTprefix="+encodeString(GID("MQTTprefix").value)+ "&CalibU="+Math.floor(GID("CalibU").value) + "&CalibI="+Math.floor(GID("CalibI").value);
    if ((GID("dhcp").checked ||  checkIP("adrIP")&&checkIP("gateway"))  && (!GID("DomoRepete").checked ||  checkIP("DomoIP") )  && (!GID("MQTTRepete").checked ||  checkIP("MQTTIP"))){
      var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() { 
          if (this.readyState == 4 && this.status == 200) {
            var retour=this.responseText;
            location.reload();
          }         
        };
        xhttp.open('GET', 'ParaUpdate'+S, true);
        xhttp.send();
    }
  }
  function checkDisabled(){ 
    GID("adrIP").disabled=GID("dhcp").checked;
    GID("gateway").disabled=GID("dhcp").checked;
    GID("UxIExtIP").disabled=!GID("UxIExterne").checked;
    GID("CalibU").disabled=GID("UxIExterne").checked;
    GID("CalibI").disabled=GID("UxIExterne").checked;
    GID("DomoIP").disabled=GID("DomoRepete").value ==0?true:false;
    GID("DomoPort").disabled=GID("DomoRepete").value ==0?true:false;
    GID("DomoIdx").disabled=GID("DomoRepete").value ==0?true:false;
    GID("MQTTIP").disabled=GID("MQTTRepete").value ==0?true:false;
    GID("MQTTPort").disabled=GID("MQTTRepete").value ==0?true:false;
    GID("MQTTUser").disabled=GID("MQTTRepete").value ==0?true:false;
    GID("MQTTpwd").disabled=GID("MQTTRepete").value ==0?true:false; 
    GID("MQTTprefix").disabled=GID("MQTTRepete").value ==0?true:false; 
  }
  function checkIP(id){
    var S=GID(id).value;
    var Table=S.split(".");
    var valide=true;
    if (Table.length!=4) {
      valide=false;
    }else{
      for (var i=0;i<Table.length;i++){
        if (Table[i]>255 || Table[i]<0) valide=false;
      }
    }
    if (valide){
      GID(id).style.color="black";
    } else {
      GID(id).style.color="red";
    }
    return valide;
  }
  function encodeString(s){
    var S="";
    for (var i=0;i<s.length;i++){
        S +=s.charCodeAt(i)+"-";
    }
    return S;
  }
  function decodeString(s){
    s=s.split("-");
    var S="";
    for (var i=0;i<s.length-1;i++){
      S +=String.fromCharCode(s[i]);
    }
    return S;
  }
  function int2ip (V) {
    var ipInt=parseInt(V);
    return ( (ipInt>>>24) +'.' + (ipInt>>16 & 255) +'.' + (ipInt>>8 & 255) +'.' + (ipInt & 255) );
  }
  function ip2int(ip) {
    return ip.split('.').reduce(function(ipInt, octet) { return (ipInt<<8) + parseInt(octet, 10)}, 0) >>> 0;
  }
  function Reset(){
      var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() { 
          if (this.readyState == 4 && this.status == 200) {
            GID('BoutonsBas').innerHTML=this.responseText;
            setTimeout(location.reload(),3000);
          }         
        };
        xhttp.open('GET', 'restart', true);
        xhttp.send();
  }
)====";
