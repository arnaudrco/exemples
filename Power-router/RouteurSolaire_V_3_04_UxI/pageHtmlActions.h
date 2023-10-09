//************************************************
// Page HTML et Javascript de gestion des Actions
//************************************************

const char *ActionsHtml = R"====(
   <!doctype html>
  <html><head><style>
    * {box-sizing: border-box;}
    body {font-size:150%;text-align:center;background: linear-gradient(#003,#77b5fe,#003);background-attachment:fixed;color:white;}
    h2{text-align:center;color:white;}
    a:link {color:#aaf;text-decoration: none;}
    a:visited {color:#ccf;text-decoration: none;}
    .onglets{margin-top:4px;left:0px;font-size:130%;}
    .Baccueil,.Bbrut,.Bparametres,.Bactions{margin-left:20px;border:outset 4px grey;background-color:#333;border-radius:6px;padding-left:20px;padding-right:20px;display:inline-block;}
    .Bactions{border:inset 8px azure;}
    .cadre {width:100%;max-width:1200px;margin:auto;}
    .form {width:100%;text-align:left;}
    .titre{display:flex;justify-content:center;cursor:pointer;}
    .slideTriac{width:100%;position:relative;margin:4px;padding:4px;border:2px inset grey;background-color:#fff8f8;color:black;font-size:14px;}
    .slideTriacIn{display:flex;justify-content:center;width:100%;}
    .planning{width:100%;position:relative;margin:4px;padding:4px;border:2px inset grey;background-color:#fff8f8;color:black;}
    .periode{position:absolute;height:100%;border:outset 4px;border-radius:10px;display:flex;justify-content:space-between;align-items: center;color:white;cursor:ew-resize;}
    .curseur{position:relative;width:100%;height:20px;}
    .infoAction{position:relative;width:100%;height:40px;font-size:20px;}
    .infoZone{position:absolute;height:100%;display:flex;justify-content: space-around;border:2px inset;align-items: center;text-align:center;background-color:#ddd;cursor:pointer;}
    .infoZ{position:absolute;bottom:2px;left:2px;display:none;border:2px inset grey;background-color:#bbb;}
    .infoZ input{width:100px;}
    .Hfin{position:absolute;bottom:2px;right:2px;}
    .zNo{background-color:#666;}
    .zOff{background-color:#66f;}
    .zOn{background-color:#f66;}
    .zPw{background-color:#6f6;}
    .bInset{border:4px inset grey;}
    .bOutset{border:2px outset grey;cursor:pointer;}
    .zTriac{background-color:#cc4;}
    .selectZ{background-color:#aaa;height:20px;font-size:16px;}
    .closeZ{background-color:black;border:outset 2px grey;color:white;position:absolute;top:0px;height:20px;width:20px;right:0px;cursor:pointer;}
    .tm{width:100px;text-align:center;padding-left:10px;padding-right:10px;}
    .tbut{width:40px;padding-left:10px;padding-right:10px;text-align:center;font-weight:bold;font-size:24px;cursor:pointer;}
    .ligne {display: table-row;padding:10px;}
    tr {margin: 2px;text-align:left;font-size:20px;}
    h4{padding:2px;margin:0px;}
    h5{text-align:left;padding:2px;margin:0px;}
    .px70{width:70px;}
    .px100{width:70px;}
    label{text-align:right;}
    #message{position:fixed;border:inset 4px grey;top:2px;right:2px;background-color:#333;color:white;font-size:16px;display:none;text-align:left;padding:5px;}
    #F1ATB{position:fixed;bottom:2px;left:2px;font-size:14px;}
  </style>
  <script src="ActionsJS"></script>
  </head>
  <body onLoad="Init();" onmouseup='mouseClick=false;' >
    <div class="cadre">
      <div class='onglets'><div class='Baccueil'><a href='/'>Accueil</a></div><div class='Bbrut'><a href='/Brute'>Donn&eacute;es brutes</a></div><div class='Bparametres'><a href='/Para'>Param&egrave;tres</a></div><div class='Bactions'><a href='/Actions'>Actions</a></div></div>
      <h2>Routeur Solaire - UxI</h2><h4>Planning des Routages</h4>
      <h5>Routage du Triac</h5>
      <div class="form"   >
        <div id="planning0" class="planning" ></div>
        <div class="slideTriac">
          <div class="slideTriacIn">
            <div>R&eacute;activit&eacute; lente ou charge importante</div>
            <input type="range" min="1" max="100" value="50" id="sliderTriac" style="width:30%;" oninput="GV('port0',Math.floor(this.value));GH('sensiTriac',Math.floor(this.value));" onmousemove='Disp(this)' >
            <div>R&eacute;activit&eacute; rapide ou charge faible</div><br>
          </div>
          <div class="slideTriacIn"><strong><div id="sensiTriac">0</div></strong></div>
        </div>
        
      </div>
		  <h5>Actions de Routage</h5>
      <div class="form"   >
        <div id="plannings"></div>
      </div> 
      <div  style='text-align:right;padding-top:20px;'>
        <input type='button' value='Sauvegarder' onclick="SendValues();">
      </div>
    </div>
    <div id="message"></div>
    <div id="F1ATB"><a href="http://f1atb.fr" target="_blank">F1ATB.fr</a></div> 
  </body></html>
)====";
const char *ActionsJS = R"====(
  var LesActions = [];
  var mouseClick = false;
  var blockEvent = false;
  function Init() {
      LoadActions();
      DispTimer();
  }
  function creerAction(aActif, aTitre, aHost, aPort, aOrdreOn, aOrdreOff, aRepet, aPeriodes) {
      var S = {
          Actif: aActif,
          Titre: aTitre,
          Host: aHost,
          Port: aPort,
          OrdreOn: aOrdreOn,
          OrdreOff: aOrdreOff,
          Repet: aRepet,
          Periodes: aPeriodes
      }
      return S;
  }
  function TracePlanning(iAct) {
      var S = "<div class='titre'><input type='checkbox'  id='actif" + iAct + "' onmousemove='Disp(this)' ><h4 id ='titre" + iAct + "' onmousemove='Disp(this)' onclick='editTitre(" + iAct + ")'>Titre</h4></div>";
      if (iAct > 0) {
          S += "<table><tr>";       
          S += "<td>Host</td><td><input type='text' id='host" + iAct + "' onmousemove='Disp(this)' ></td>";
          S += "<td>Ordre On</td><td><input type='text' id='ordreOn" + iAct + "' onmousemove='Disp(this)'></td>";
          S += "<td class='tm'>R&eacute;p&eacute;t.(s)</td><td ><input id='adds' type='button' value='+' class='tbut' onclick='AddSub(1," + iAct + ")' onmousemove='Disp(this)'></td>";
          S += "</tr><tr>";
          S += "<td>Port</td><td><input type='number' id='port" + iAct + "' onmousemove='Disp(this)'></td>";
          S += "<td>Ordre Off</td><td><input type='text' id='ordreOff" + iAct + "' onmousemove='Disp(this)'></td>";
          S += "<td class='tm'><input type='number' id='repet" + iAct + "' class='tm' onmousemove='Disp(this)'></td><td >";
          S += "<input id='adds' type='button' value='-' class='tbut'  onclick='AddSub(-1," + iAct + ")' onmousemove='Disp(this)'></td>";
          S += "</tr></table>";
      } else {
          S += "<div style='display:none'><input type='text' id='host" + iAct + "'><input type='text' id='ordreOn" + iAct + "'>";
          S += "<input type='text' id='ordreOff" + iAct + "'><input type='number' id='port" + iAct + "'><input type='number' id='repet" + iAct + "' ></div>";
          S += "<table><tr><td ><input id='adds' type='button' value='-' class='tbut'  onclick='AddSub(-1," + iAct + ")' onmousemove='Disp(this)'></td>"
          S += "<td ><input id='adds' type='button' value='+' class='tbut' onclick='AddSub(1," + iAct + ")' onmousemove='Disp(this)'></td></tr></table>";
      }
      S += "<div id='infoAction" + iAct + "' class='infoAction'></div>";
      S += "<div id='curseurs" + iAct + "' class='curseur'  onmousedown='mouseClick=true;'  onmousemove='mouseMove(this,event," + iAct + ");'  ontouchstart='touchMove(this,event," + iAct + ");'  ontouchmove='touchMove(this,event," + iAct + ");' ></div>";

      GH("planning" + iAct, S);
      GID("actif" + iAct).checked = LesActions[iAct].Actif;
      GH("titre" + iAct, LesActions[iAct].Titre);
      if (iAct > 0) {
          GV("host" + iAct, LesActions[iAct].Host);
          GV("port" + iAct, LesActions[iAct].Port);
          GV("ordreOn" + iAct, LesActions[iAct].OrdreOn);
          GV("ordreOff" + iAct, LesActions[iAct].OrdreOff);
          GV("repet" + iAct, LesActions[iAct].Repet);
      }
      TracePeriodes(iAct);
  }
  function TracePeriodes(iAct) {
      var S = "";
      var Sinfo = "";
      var left = 0;
      var H0 = 0;
      var colors = ["#666", "#66f", "#f66", "#6f6", "#cc4"]; //NO,OFF,ON,PW,Triac
      blockEvent = false;
      for (var i = 0; i < LesActions[iAct].Periodes.length; i++) {
        var w = (LesActions[iAct].Periodes[i].Hfin - H0) / 24;
        left = H0 / 24;
        H0 = LesActions[iAct].Periodes[i].Hfin;
        var Type = LesActions[iAct].Periodes[i].Type;
        var color = colors[Type];
		    var TexteMinMax="<div>On si Pw&lt;"+LesActions[iAct].Periodes[i].Vmin+"W</div><div>Off si Pw&gt;"+LesActions[iAct].Periodes[i].Vmax+"W</div>";
		    var TexteTriac="<div>Seuil Pw : "+LesActions[iAct].Periodes[i].Vmin+"W</div><div>Ouvre Max : "+LesActions[iAct].Periodes[i].Vmax+"%</div>";
        var paras = ["Pas de contr&ocirc;le", "OFF", "ON", TexteMinMax, TexteTriac];
        var para = paras[Type];
        S += "<div class='periode' style='width:" + w + "%;left:" + left + "%;background-color:" + color + ";'   ><div>&lArr;</div><div>&rArr;</div></div>";
        Hmn = Math.floor(H0 / 100) + ":" + ("0" + Math.floor(0.6 * (H0 - 100 * Math.floor(H0 / 100)))).substr(-2, 2);
        fs = Math.max(10, Math.min(20, w)) + "px";
        Sinfo += "<div class='infoZone' style='width:" + w + "%;left:" + left + "%;border-color:" + color + ";font-size:" + fs + "'  onclick='infoZclicK(" + i + "," + iAct + ")'  >"
        Sinfo += "<div class='Hfin'>" + Hmn + "</div><div id='info" + iAct + "Z" + i + "' class='infoZ' ></div>" + para + "</div>";
      }
      GH("curseurs" + iAct, S);
      GH("infoAction" + iAct, Sinfo);
  }
  function touchMove(t, ev, iAct) {
      var leftPos = ev.touches[0].clientX - GID(t.id).getBoundingClientRect().left;
      NewPosition(t, leftPos, iAct);
  }
  function mouseMove(t, ev, iAct) {
      if (mouseClick) {
          var leftPos = ev.clientX - GID(t.id).getBoundingClientRect().left;
          NewPosition(t, leftPos, iAct);
      }
  }
  function NewPosition(t, leftPos, iAct) {
      var G = GID(t.id).style.left;
      //+ window.scrollX;
      var width = GID(t.id).getBoundingClientRect().width;
      var HeureMouse = leftPos * 2420 / width;
      var idxClick = 0;
      var deltaX = 999999;
      for (var i = 0; i < LesActions[iAct].Periodes.length - 1; i++) {
          var dist = Math.abs(HeureMouse - LesActions[iAct].Periodes[i].Hfin)
              if (dist < deltaX) {
                  idxClick = i;
                  deltaX = dist;
              }
      }
      var NewHfin = Math.max(0, Math.min(HeureMouse, 2400));
      if (idxClick == LesActions[iAct].Periodes.length - 1) NewHfin=2400;
      if (idxClick < LesActions[iAct].Periodes.length - 1)
          NewHfin = Math.min(NewHfin, LesActions[iAct].Periodes[idxClick + 1].Hfin);
      if (idxClick > 0)
          NewHfin = Math.max(NewHfin, LesActions[iAct].Periodes[idxClick - 1].Hfin);
      LesActions[iAct].Periodes[idxClick].Hfin = Math.floor(NewHfin);
      TracePeriodes(iAct);

  }
  function AddSub(v, iAct) {
      if (v == 1) {
          LesActions[iAct].Periodes.push({
              Type: 1,
              Hfin: 2400
          });
          var Hbas = 0;
          if (LesActions[iAct].Periodes.length > 2)
              Hbas = LesActions[iAct].Periodes[LesActions[iAct].Periodes.length - 3].Hfin;
          if (LesActions[iAct].Periodes.length > 1)
              Math.floor(LesActions[iAct].Periodes[LesActions[iAct].Periodes.length - 2].Hfin = (Hbas + 2400) / 2);
      } else {
          LesActions[iAct].Periodes.pop();
          if (LesActions[iAct].Periodes.length > 0)
              LesActions[iAct].Periodes[LesActions[iAct].Periodes.length - 1].Hfin = 2400;
      }
      TracePeriodes(iAct);
  }
  function infoZclicK(i, iAct) {
      if (!blockEvent) {
          blockEvent = true;
          var Type = LesActions[iAct].Periodes[i].Type;
          var idZ = "info" + iAct + "Z" + i;
          var S = "<div class='selectZ'>S&eacute;lection Action<div class='closeZ' onclick='infoZclose(\"" + idZ + "\")'>X</div></div>";
          var c = (Type == 0) ? "bInset" : "bOutset";
          if (iAct > 0) { //Pas un Triac
              S += "<div class='zNo " + c + "' onclick='selectZ(0," + i + "," + iAct + ");' onmousemove='Disp(\"zNo\")' >Pas de contr&ocirc;le</div>";
          }
          c = (Type == 1) ? "bInset" : "bOutset";
          S += "<div class='zOff " + c + "' onclick='selectZ(1," + i + "," + iAct + ");' onmousemove='Disp(\"zOff\")'>OFF</div>";
          c = (Type == 2) ? "bInset" : "bOutset";
          S += "<div  class='zOn " + c + "' onclick='selectZ(2," + i + "," + iAct + ");' onmousemove='Disp(\"zOn\")' >ON</div>";
          c = (Type > 2) ? "bInset" : "bOutset";
          var Vmin=LesActions[iAct].Periodes[i].Vmin;
		      var Vmax=LesActions[iAct].Periodes[i].Vmax;
          if (iAct > 0) {
              S += "<div class='zPw " + c + "' onclick='selectZ(3," + i + "," + iAct + ");'><div>Pw &lt;<input id='pw_min_"+idZ+"' onmousemove='Disp(this)' type='number' value='"+Vmin+"' onchange='NewVal(this)' ></div>";
              S += "<div>Pw &gt;<input id='pw_max_"+idZ+"' onmousemove='Disp(this)' type='number' value='"+Vmax+"' onchange='NewVal(this)'></div></div>";
          } else {
              S += "<div  class='zTriac " + c + "' onclick='selectZ(4," + i + "," + iAct + ");'><div>Seuil Pw <input id='pw_min_"+idZ+"' onmousemove='Disp(\"pwTr\")' type='number' value='"+Vmin+"' onchange='NewVal(this)'></div>";
              S +="<div>Ouvre Max <input id='pw_max_"+idZ+"' onmousemove='Disp(\"mxTr\")' type='number' value='"+Vmax+"' onchange='NewVal(this)'></div></div>";
          }
          GH(idZ, S);
          GID(idZ).style.display = "block";
      }
  }
  function infoZclose(idx) {
      var champs=idx.split("info");
	    var idx=champs[1].split("Z");
      S="TracePeriodes("+idx[0]+");"
      setTimeout(S, 100);
  }
  function selectZ(T, i, iAct) {
      if (LesActions[iAct].Periodes[i].Type != T) {
          LesActions[iAct].Periodes[i].Type = T;
          var idZ = "info" + iAct + "Z" + i;
          if (T <= 2)
              infoZclose(idZ);
          TracePeriodes(iAct);
      }
  }
  function NewVal(t){
      var champs=t.id.split("info");
      var idx=champs[1].split("Z");   
      if (champs[0].indexOf("min")>0){
        LesActions[idx[0]].Periodes[idx[1]].Vmin=Math.floor(GID(t.id).value);
      }else{
        LesActions[idx[0]].Periodes[idx[1]].Vmax=Math.floor(GID(t.id).value);
        if (idx[0]==0){
          LesActions[idx[0]].Periodes[idx[1]].Vmax=Math.max(LesActions[idx[0]].Periodes[idx[1]].Vmax,5);
          LesActions[idx[0]].Periodes[idx[1]].Vmax=Math.min(LesActions[idx[0]].Periodes[idx[1]].Vmax,100);
        }
      }	
  }
  function editTitre(iAct) {
      if (GID("titre" + iAct).innerHTML.indexOf("<input") == -1) {
          GH("titre" + iAct, "<input type='text' value='" + GID("titre" + iAct).innerHTML + "' id='Etitre" + iAct + "'  onblur='TitreValid(" + iAct + ")' >");
      }
  }
  function TitreValid(iAct) {
      LesActions[iAct].Titre = GID("Etitre" + iAct).value.trim();
      GH("titre" + iAct, LesActions[iAct].Titre);
  }
  function LoadActions() {
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function () {
          if (this.readyState == 4 && this.status == 200) {
              var LeRetour = this.responseText;
              var Les_ACTIONS = LeRetour.split('|');
              LesActions.splice(0,LesActions.length);
              for (var iAct=0;iAct<Les_ACTIONS.length-1;iAct++){
                var champs=Les_ACTIONS[iAct].split(",");
                var NbPeriodes=champs[7];
                var Periodes=[];
                var j=8;
                for (var i=0;i<NbPeriodes;i++){
                  Periodes[i]={Type:champs[j],Hfin:champs[j+1],Vmin:champs[j+2],Vmax:champs[j+3]};
                  j=j+4;
                }
                var act=(champs[0]==1)?true:false;
                LesActions[iAct]=creerAction(act, decodeString(champs[1]),decodeString( champs[2]), champs[3], decodeString(champs[4]), decodeString(champs[5]), champs[6], Periodes);
              }    
              if (LesActions.length==0){  //Action Triac
                  LesActions.push( creerAction(true, "Titre", "", 50, "", "", 0, [{
                          Hfin: 2400,
                          Type: 4,
                          Vmin:0,
                          Vmax:100
                      }
                  ]));
              }
              LesActions.push( creerAction(true, "Titre", "localhost", 80, "", "", 240, [{
                      Hfin: 2400,
                      Type: 3,
                      Vmin:0,
                      Vmax:0
                  }
              ]));
              var S = "";
              for (var i = 1; i < LesActions.length; i++) {
                  S += "<div id='planning" + i + "' class='planning' ></div>";
              }
              GH("plannings", S);
              for (var iAct = 0; iAct < LesActions.length; iAct++) {
                  TracePlanning(iAct);
                  GID("actif" + iAct).checked= LesActions[iAct].Actif;
                  GH("titre" + iAct,LesActions[iAct].Titre);
                  GV("host" + iAct,LesActions[iAct].Host);
                  GV("port" + iAct,LesActions[iAct].Port);
                  GV("ordreOn" + iAct,LesActions[iAct].OrdreOn);
                  GV("ordreOff" + iAct,LesActions[iAct].OrdreOff);
                  GV("repet" + iAct,LesActions[iAct].Repet);
              }
              if (LesActions[0].Port<1) LesActions[0].Port=50;
              GV("sliderTriac",LesActions[0].Port); //Port cache slider triac
              GH('sensiTriac',LesActions[0].Port)
          }
      };
      xhttp.open('GET', 'ActionsAjax', true);
      xhttp.send();
  }
  function GID(id) {
      return document.getElementById(id);
  };
  function GH(id, T) {
      GID(id).innerHTML = T;
  }
  function GV(id, T) {
      GID(id).value = T;
  }
  function SendValues() {
      for (var iAct = 0; iAct < LesActions.length; iAct++) {
        LesActions[iAct].Actif = GID("actif" + iAct).checked
        LesActions[iAct].Titre = GID("titre" + iAct).innerHTML.trim();
        LesActions[iAct].Host = GID("host" + iAct).value.trim();
        LesActions[iAct].Port = GID("port" + iAct).value;
        LesActions[iAct].OrdreOn = GID("ordreOn" + iAct).value.trim();
        LesActions[iAct].OrdreOff = GID("ordreOff" + iAct).value.trim();
        LesActions[iAct].Repet = GID("repet" + iAct).value;
      }
      var S="";
      for (var iAct = 0; iAct < LesActions.length; iAct++) {
        if ((iAct==0)||(LesActions[iAct].Host.length>2 && (LesActions[iAct].OrdreOn.length>2 || LesActions[iAct].OrdreOff.length>2))){
            var act= LesActions[iAct].Actif ? 1:0;
            S +=act+","+encodeString(LesActions[iAct].Titre)+",";
            S +=encodeString(LesActions[iAct].Host)+","+LesActions[iAct].Port+",";
            S +=encodeString(LesActions[iAct].OrdreOn)+","+encodeString(LesActions[iAct].OrdreOff)+","+LesActions[iAct].Repet+",";
            S +=LesActions[iAct].Periodes.length+",";
            for (var i=0;i<LesActions[iAct].Periodes.length;i++){
              S +=LesActions[iAct].Periodes[i].Type+","+Math.floor(LesActions[iAct].Periodes[i].Hfin)+",";
              S +=Math.floor(LesActions[iAct].Periodes[i].Vmin)+","+Math.floor(LesActions[iAct].Periodes[i].Vmax)+",";  
            }
            S +="|";
        }
      }
      S = "?actions="+S;
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function () {
          if (this.readyState == 4 && this.status == 200) {
              var retour = this.responseText;
              location.reload();
          }
      };
      xhttp.open('GET', 'ActionsUpdate' + S, true);
      xhttp.send();
      
  }
  function encodeString(s) {
      var S = "";
      for (var i = 0; i < s.length; i++) {
          S += s.charCodeAt(i) + "-";
      }
      return S;
  }
  function decodeString(s) {
      s = s.split("-");
      var S = "";
      for (var i = 0; i < s.length - 1; i++) {
          S += String.fromCharCode(s[i]);
      }
      return S;
  }
  function Disp(t) {
    if (t!="zNo" && t!="zOn" && t!="zOff" && t!="pwTr" && t!="mxTr" ) t=t.id.substr(0, 4);
      switch (t) { 
      case "acti":
          var m = "Activation / D&eacute;sactivation du routage."
              break;
      case "titr":
          var m = "Nom ou Titre";
          break;
      case "slid":
          var m = "Gain de la boucle d'asservissement. Faible, la r&eacute;gulation est lente mais stable. Elev&eacute;, la r&eacute;gulation est rapide mais risque d'oscillations. A ajuster suivant la charge branch&eacute;e au triac.";
          break;
      case "host":
          var m = "Adresse IP locale, nom de domaine ou localhost pour l'ESP32.<br>Ex : <b>192.168.1.25</b> ou <b>machine.local</b> ou <b>localhost</b>.";
          break;
      case "port":
          var m = "Port d'acc&egrave;s via le protocole http , uniquement pour machine distante. En g&eacute;n&eacute;ral <b>80</b>.";
          break;
      case "ordr":
          var m = "Page appel&eacute;e avec les param&egrave;tres. <br>";
          m += "Ex. pour une machine sur le r&eacute;seau :<br><b>/commande?idx=23&position=on</b>. Se r&eacute;f&eacute;rer &agrave; la documentation constructeur.<br>"
          m += "Ex. pour un GPIO de l'ESP32 en localhost :<br><b>gpio=5&out=1&init=0</b> mettre le GPIO 5 &agrave; 1. &Agrave; la mise enroute (optionel), mettre &agrave; 0.<br>"
          break;
      case "repe":
          var m = "P&eacute;riode en s de r&eacute;p&eacute;tition/rafra&icirc;chissement de la commande. Uniquement pour les commandes vers l'extP&eacute;rieur.<br>";
          m += "0= pas de r&eacute;p&eacute;tition.";
          break;
      case "adds":
          var m = "Ajout ou retrait d'une p&eacute;riode horaire."
              break;
      case "pw_m":
          var m = "Seuil inf&eacute;rieur  de puissance mesur&eacute;e Pw &lt; pour d&eacute;marrer le routage  et seuil sup&eacute;rieur de puissance  &gt; pour l'arr&ecirc;ter.<br> ";
      m +="Attention, la diff&eacute;rence, seuil sup&eacute;rieur moins  seuil inf&eacute;rieur doit &ecirc;tre sup&eacute;rieure &agrave; la consommation du dipositif pour &eacute;viter l'oscillation du relais de commande."
          break;
      case "pwTr":
          var m = "Seuil en W de r&eacute;gulation par le Triac de la puissance mesur&eacute;e Pw. Valeur typique : 0.";
          break;
      case "mxTr":
          var m = "Ouverture maximum du triac entre 5 et 100%. Valeur typique : 100%";
          break;
      case "zNo":
            var m = "Pas d'action On ou Off de routage";
            break;
      case "zOff":
            var m = "Ordre Arr&ecirc;t Routage";
            break;
      case "zOn":
            var m = "Ordre Activation Routage";
            break;
      }
      GH("message", m);
      GID("message").style = "display:inline-block;";
      Timer = 10;
  }
  var Timer = 0;
  function DispTimer() {
      Timer = Timer - 1;
      if (Timer < 0) {
          GID('message').style = 'display:none;';
      }
      setTimeout("DispTimer();", 1000);
  }
)====";
