//***************************************************
// Page HTML et Javascript de gestion des Paramètres
//***************************************************
const char *ParaHtml = R"====(
  <!doctype html>
  <html><head><meta charset="UTF-8"><style>
    * {box-sizing: border-box;}
    body {font-size:150%;text-align:center;width:100%;max-width:1000px;margin:auto;background: linear-gradient(#003,#77b5fe,#003);background-attachment:fixed;color:white;}
    h2{text-align:center;color:white;}
    a:link {color:#aaf;text-decoration: none;}
    a:visited {color:#ccf;text-decoration: none;}
    .form {margin:auto;padding:10px;display: table;text-align:left;width:100%;}
    .ligne {display: table-row;padding:10px;}
    label,input{display: table-cell;margin: 5px;text-align:left;font-size:20px;height:25px;}
    .boldT{text-align:left;font-weight:bold;display: table-row;}
    .onglets{margin-top:4px;left:0px;font-size:130%;}
    .Baccueil,.Bbrut,.Bparametres,.Bactions{margin-left:20px;border:outset 4px grey;background-color:#333;border-radius:6px;padding-left:20px;padding-right:20px;display:inline-block;}
    .Bparametres{border:inset 10px azure;}
    #BoutonsBas{display:flex;justify-content:space-between;margin-top:20px;}
    .pied{display:flex;justify-content:space-between;font-size:14px;color:white;}
    #ligneFixe,#sondeMobile,#Tui,#CUi,#CuI,#ligneExt{display:none;}
  </style>
  <script src="/ParaJS"></script>
  <script src="/ParaRouteurJS"></script>
  </head>
  <body onLoad="Init();">
    <div class='onglets'><div class='Baccueil'><a href='/'>Accueil</a></div><div class='Bbrut'><a href='/Brute'>Donn&eacute;es brutes</a></div><div class='Bparametres'><a href='/Para'>Param&egrave;tres</a></div><div class='Bactions'><a href='/Actions'>Actions</a></div></div>
    <h2 id='nom_R'>Routeur Solaire - RMS</h2><h4>Param&egrave;tres</h4>
    <div class="boldT"><br>Source des mesures</div>
    <div class="form"  >
      <div class="ligne">
        <label for='UxI' style='text-align:right;'>UxI :</label>
        <input type='radio' name='sources' id='UxI' value="UxI"  onclick="checkDisabled();">
        <label for='UxIx2' style='text-align:right;'>UxIx2 :</label>
        <input type='radio' name='sources' id='UxIx2' value="UxIx2"  onclick="checkDisabled();">       
        <label for='Linky' style='text-align:right;'>Linky :</label>
        <input type='radio' name='sources' id='Linky' value="Linky"  onclick="checkDisabled();">
        <label for='Ext' style='text-align:right;'>ESP Externe :</label>
        <input type='radio' name='sources' id='Ext' value="Ext"  onclick="checkDisabled();">
      </div>
      <div><span style='font-size:10px;'>N&eacute;cessite un Reset de l'ESP32</span></div>
    </div>
    <div class="form"  >
     <div class='ligne' id="ligneExt">
        <label for='RMSextIP'>Adresse IP ESP-RMS externe (ex : 192.168.1.248) : </label>
        <input type='text' name='RMSextIP' id='RMSextIP' >
      </div>
      <br>
      <div class='ligne boldT'>
        <label for='nomRouteur' >Nom du routeur : </label>
        <input type='text' name='nomRouteur' id='nomRouteur' >
      </div>
      <div class='ligne boldT' id='ligneFixe'>
        <label for='nomSondeFixe' >Nom Données Sonde de Courant Fixe : </label>
        <input type='text' name='nomSondeFixe' id='nomSondeFixe' >
      </div>
      <div class='ligne boldT' id='ligneMobile'>
        <label for='nomSondeMobile' >Nom Données <span id='sondeMobile'>Sonde de Courant Mobile</span> : </label>
        <input type='text' name='nomSondeMobile' id='nomSondeMobile' >
      </div>
      <br>
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
        <label for='MQTTdeviceName'>MQTT Device Name (1 seul mot ex : routeur_rms ) : </label>
        <input type='text' name='MQTTdeviceName' id='MQTTdeviceName' >
      </div>
      
      <div class="boldT" id='Tui'><br>Calibration Mesures Ueff et Ieff</div>
      <div class='ligne' id='CUi'>
        <label for='CalibU'>Coefficient multiplicateur Ueff (typique : 1000) : </label>
        <input type='number' name='CalibU' id='CalibU'   >
      </div>
      <div class='ligne' id='CuI'>
        <label for='CalibI'>Coefficient multiplicateur Ieff (typique : 1000) : </label>
        <input type='number' name='CalibI' id='CalibI'   >
      </div>
    </div>
    <div  id='BoutonsBas'>
        <input type='button' onclick='Reset();' value='ESP32 Reset' >
        <input type='button' onclick="SendValues();" value='Sauvegarder' >
    </div>
    <br>
    <div class='pied'><div>Routeur Version : <span id='version'></span></div><div><a href='https:F1ATB.fr' >F1ATB.fr</a></div></div>
    <br>
  </body></html>
)====";
const char *ParaJS = R"====(
  function Init(){
    LoadParametres();
    LoadParaRouteur();
  }
  function LoadParametres() {
    var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() { 
          if (this.readyState == 4 && this.status == 200) {
             var LesParas=this.responseText;
             var Para=LesParas.split(RS);  
             GID("dhcp").checked = Para[0]==1  ? true:false;
             GID("adrIP").value=int2ip(Para[1]);
             GID("gateway").value=int2ip(Para[2]);
             GID(Para[3]).checked = true;
             GID("RMSextIP").value=int2ip(Para[4]);
             GID("DomoRepete").value = Para[5];
             GID("DomoIP").value=int2ip(Para[6]);
             GID("DomoPort").value=Para[7];
             GID("DomoIdx").value=Para[8];
             GID("MQTTRepete").value = Para[9];
             GID("MQTTIP").value=int2ip(Para[10]);
             GID("MQTTPort").value=Para[11];
             GID("MQTTUser").value=Para[12];
             GID("MQTTpwd").value=Para[13];
             GID("MQTTdeviceName").value=Para[14];
             GID("nomRouteur").value=Para[15];
             GID("nomSondeFixe").value=Para[16];
             GID("nomSondeMobile").value=Para[17];
             GID("CalibU").value=Para[18];
             GID("CalibI").value=Para[19];
             checkDisabled();
          }         
        };
        xhttp.open('GET', 'ParaAjax', true);
        xhttp.send();
      }
  function SendValues(){
    var dhcp=GID("dhcp").checked ? 1:0;
    var Source_new=document.querySelector('input[name="sources"]:checked').value;
    var S=dhcp+RS+ ip2int(GID("adrIP").value)+RS+ ip2int(GID("gateway").value);
    S +=RS+Source_new+RS+ ip2int(GID("RMSextIP").value);
    S +=RS+GID("DomoRepete").value +RS+ip2int(GID("DomoIP").value)+RS+GID("DomoPort").value+RS+GID("DomoIdx").value;
    S +=RS+GID("MQTTRepete").value +RS+ip2int(GID("MQTTIP").value) +RS+GID("MQTTPort").value +RS+GID("MQTTUser").value+RS+GID("MQTTpwd").value;
    S +=RS+GID("MQTTdeviceName").value+RS+GID("nomRouteur").value+RS+GID("nomSondeFixe").value+RS+GID("nomSondeMobile").value;
    S +=RS+GID("CalibU").value+RS+GID("CalibI").value
    S="?lesparas="+clean(S);
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
    GID("RMSextIP").disabled=!GID("Ext").checked;
    GID("DomoIP").disabled=GID("DomoRepete").value ==0?true:false;
    GID("DomoPort").disabled=GID("DomoRepete").value ==0?true:false;
    GID("DomoIdx").disabled=GID("DomoRepete").value ==0?true:false;
    GID("MQTTIP").disabled=GID("MQTTRepete").value ==0?true:false;
    GID("MQTTPort").disabled=GID("MQTTRepete").value ==0?true:false;
    GID("MQTTUser").disabled=GID("MQTTRepete").value ==0?true:false;
    GID("MQTTpwd").disabled=GID("MQTTRepete").value ==0?true:false; 
    GID("MQTTdeviceName").disabled=GID("MQTTRepete").value ==0?true:false; 
    Source = document.querySelector('input[name="sources"]:checked').value;
    if (Source !='Ext') Source_data=Source;
    AdaptationSource();
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
  function AdaptationSource(){
      GID('ligneFixe').style.display = (Source_data=='UxIx2') ? "table-row" : "none";
      GID('sondeMobile').style.display = (Source_data=='UxI' || Source_data=='UxIx2') ? "inline-block" : "none";
      if (Source_data=='UxI' ) {
        GID('Tui').style.display="table-row";
        GID('CUi').style.display="table-row";
        GID('CuI').style.display="table-row";
      } else {
        GID('Tui').style.display="none";
        GID('CUi').style.display="none";
        GID('CuI').style.display="none";
      }
      GID('ligneExt').style.display = (Source=='Ext') ? "table-row" : "none";
  }
)====";

//Paramètres du routeur et fonctions générales pour toutes les pages.
const char *ParaRouteurJS = R"====(
  var Source="";
  var Source_data="";
  var RMSextIP="";
  var GS=String.fromCharCode(29); //Group Separator
  var RS=String.fromCharCode(30); //Record Separator
  var nomSondeFixe="Sonde Fixe";
  var nomSondeMobile="Sonde Mobile"; 
  function LoadParaRouteur() {
    var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() { 
          if (this.readyState == 4 && this.status == 200) {
             var LesParas=this.responseText;
             var Para=LesParas.split(RS);
             Source=Para[0];
             Source_data=Para[1];
             RMSextIP= Para[6]; 
             AdaptationSource();  
             GH("nom_R",Para[2]);
             GH("version",Para[3]);
             GH("nomSondeFixe",Para[4]);
             GH("nomSondeMobile",Para[5]); 
             nomSondeFixe=Para[4];
             nomSondeMobile=Para[5]; 
               
          }         
        };
        xhttp.open('GET', 'ParaRouteurAjax', true);
        xhttp.send();
  }
  function GID(id) { return document.getElementById(id); };
  function GH(id, T) {
    if ( GID(id)){
     GID(id).innerHTML = T; }
    }
  function GV(id, T) { GID(id).value = T; }
  function clean(S){ //Remplace & et ? pour les envois au serveur
    let res = S.replace(/\&/g, "%26");
    res=res.replace(/amp;/g,"");
    return res.replace(/\?/g,"%3F");
  }
  function int2ip (V) {
    var ipInt=parseInt(V);
    return ( (ipInt>>>24) +'.' + (ipInt>>16 & 255) +'.' + (ipInt>>8 & 255) +'.' + (ipInt & 255) );
  }
  function ip2int(ip) {
    return ip.split('.').reduce(function(ipInt, octet) { return (ipInt<<8) + parseInt(octet, 10)}, 0) >>> 0;
  }
  
)====";