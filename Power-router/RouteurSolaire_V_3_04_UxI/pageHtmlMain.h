//************************************************
// Page principale HTML et Javascript 
//************************************************
const char *MainHtml = R"====(
 <!doctype html>
  <html><head><style>
    * {box-sizing: border-box;}
    body {background: linear-gradient(#003,#77b5fe,#003);background-attachment:fixed;font-size:150%;text-align:center;width:100%;max-width:1000px;margin:auto;}
    a:link {color:#aaf;text-decoration: none;}
    a:visited {color:#ccf;text-decoration: none;}
    h2{text-align:center;color:white;}
    .ri { text-align: right;}
    .ce { text-align: center;}
    .blue { background-color:#ddf;font-weight: bold;}
    .Wh { background-color:#ff8;}
    .W { background-color:#f88;}
    .VA { background-color:aqua;}
    .UI { background-color:#bbb;}
    .foot { color:white;font-size:16px;}
    .pied{display:flex;justify-content:space-between;font-size:14px;color:white;}
    #date { color:white;}
    .tableau { background-color:white;display:inline-block;margin:auto;padding:4px;}
    table {border:10px inset azure;}
    td { text-align: left;padding:4px;}
    svg { border:10px inset azure;background: linear-gradient(#333,#666,#333);}
    #LED{position:absolute;top:4px;left:4px;width:0px;height:0px;border:5px solid red;border-radius:5px;}
    .onglets{margin-top:4px;font-size:130%;}
    .Baccueil,.Bbrut,.Bparametres,.Bactions{margin-left:20px;border:outset 4px grey;background-color:#333;border-radius:6px;padding-left:20px;padding-right:20px;display:inline-block;}
    .Baccueil{border:inset 8px azure;}
    .jauge{background-color:#ff8;height:28px;text-align:left;overflow: visible;position:absolute;top:4px;left:4px;}
    .jaugeBack{background-color:aqua;width:208px;height:36px;position:relative;padding:4px;}
    .w100{width:100%;position:absolute;top:4px;left:4px;}
    .centrer{text-align:center;}
  </style></head>
  <body onload='LoadHisto10mn();LoadData();EtatActions();' >
    <div id='LED'></div>
    <div class='onglets'><div class='Baccueil'><a href='/'>Accueil</a></div><div class='Bbrut'><a href='/Brute'>Donn&eacute;es brutes</a></div><div class='Bparametres'><a href='/Para'>Param&egrave;tres</a></div><div class='Bactions'><a href='/Actions'>Actions</a></div></div>
    <h2>Routeur Solaire - UxI</h2>
    <div id='date'>Date</div>
    <div><div class='tableau'><table >
      <tr class='UI'><td>Tension Efficace</td><td id='Uef' class='ri'></td><td>V</td></tr>
      <tr class='UI'><td>Courant Efficace</td><td id='Ief' class='ri'></td><td>A</td></tr>
      <tr class='VA'><td>Puissance Apparente</td><td id='PVA' class='ri'></td><td>VA</td></tr>
      <tr class='W'><td>Puissance Active (Pw)</td><td id='PW' class='ri'></td><td>W</td></tr>
      <tr class='UI'><td>Cosinus Phi / Power Factor</td><td id='PwFact' class='ri'></td><td></td></tr>
      <tr class='Wh'><td>Energie Active Soutir&eacute;e ce Jour</td><td id='EASJ' class='ri'></td><td>Wh</td></tr>
      <tr class='Wh'><td>Energie Active Inject&eacute;e ce Jour</td><td id='EAIJ' class='ri'></td><td>Wh</td></tr>
    </table></div></div>
    <div id='etatActions'></div>
    <p id='SVG_PW2s'></p>
    <p id='SVG_PW48h'></p>
    <p id='SVG_Wh1an'></p>
    <br><br><div class='foot' id='source'></div>
    <div class='pied'><div>Routeur Version : 3.04_UxI</div><div><a href='https:F1ATB.fr' >F1ATB.fr</a></div></div>
    <script src='MainJS'></script>
    <br></body></html>
)====";

//Les Wh cumulés données par le systeme toutes les 5mn sont convertis en W (*12)
const char *MainJS = R"====(
    var T0=Date.now();
    var tabPW2s=[];
  
    function LoadData() {
      document.getElementById('LED').style='display:block;';
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() { 
        if (this.readyState == 4 && this.status == 200) {
            var DuUxI=this.responseText;
            var message=DuUxI.split(',');
            document.getElementById('date').innerHTML = message[1];
            document.getElementById('Uef').innerHTML = LaVal(message[2]);
            document.getElementById('Ief').innerHTML = LaVal(message[3]);
            document.getElementById('PW').innerHTML = LaVal(message[4]);
            document.getElementById('PVA').innerHTML = LaVal(message[5]);
            document.getElementById('PwFact').innerHTML = message[6];     
            document.getElementById('EASJ').innerHTML = LaVal(message[7]);
            document.getElementById('EAIJ').innerHTML = LaVal(message[8]);           
            document.getElementById('LED').style='display:none;';
            tabPW2s.shift(); //Enleve Pw
            tabPW2s.shift(); //Enleve Pva
            tabPW2s.push(parseFloat(message[4]));
            tabPW2s.push(parseFloat(message[5]));
            Plot('SVG_PW2s',tabPW2s,'#f44','Moyenne Puissance Active W sur 10 mn','aqua','Puissance Apparente VA sur 10 mn');
            setTimeout('LoadData();',2000);
        }
        
      };
      xhttp.open('GET', 'ajax_data', true);
      xhttp.send();
    }
    
    function LoadHisto10mn() {
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() { 
        if (this.readyState == 4 && this.status == 200) {
          var retour=this.responseText;
          tabPW2s.splice(0,tabPW2s.length);
          tabPW2s=retour.split(',');
          tabPW2s.pop();
          Plot('SVG_PW2s',tabPW2s,'#f44','Moyenne Puissance Active W sur 10 mn','aqua','Puissance Apparente VA sur 10 mn');
          LoadHisto1an();
        }
        
      };
      xhttp.open('GET', 'ajax_data10mn', true);
      xhttp.send();
    }
    function LoadHisto48h() {
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() { 
        if (this.readyState == 4 && this.status == 200) {
          var retour=this.responseText;
          var tabPW=retour.split(',');
          tabPW.pop();
          Plot('SVG_PW48h',tabPW,'#f33','Puissance Active W sur 48h','','');
          setTimeout('LoadHisto48h();',300000);
        }
        
      };
      xhttp.open('GET', 'ajax_histo48h', true);
      xhttp.send();
    }
    function LoadHisto1an() {
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() { 
        if (this.readyState == 4 && this.status == 200) {
          var retour=this.responseText;
          var tabWh=retour.split(',');
          tabWh.pop();
          
          Plot('SVG_Wh1an',tabWh,'#ff4','Energie Active Wh / Jour sur 1an','','');
          LoadHisto48h();
        }
        
      };
      xhttp.open('GET', 'ajax_histo1an', true);
      xhttp.send();
    }
    function Plot(SVG,Tab,couleur1,titre1,couleur2,titre2){
        var Vmax=0;
        var Vmin=0;
        for (var i = 0; i < Tab.length; i++) {
              Vmax = Math.max(Math.abs(Tab[i]), Vmax);       
        }    
        var cadrageMax=1;
        var cadrage1=1000000;
        var cadrage2=[10,8,5,4,2,1];
        for (var m=0;m<7;m++){
          for (var i=0;i<cadrage2.length;i++){
              var X=cadrage1*cadrage2[i];
              if ((Vmax)<=X) cadrageMax=X;
          }
          cadrage1=cadrage1/10;
        }
        var S= "<svg viewbox='0 0 1030 500' height='500' width='100%' >";
        S += "<line x1='100' y1='20' x2='100' y2='480' style='stroke:white;stroke-width:2' />";
        S += "<line x1='100' y1='250' x2='1000' y2='250' style='stroke:white;stroke-width:2' />";
        var dX=900/Tab.length;
        const d = new Date();
        var dI=1;
        switch (SVG){
          case  'SVG_PW48h':
            var label='heure';
            var pixelTic=72;
            var dTextTic=4;
            var moduloText=24;
            var H0=d.getHours()+d.getMinutes()/60;
            var H00= 4*Math.floor(H0/4);
            var X0=18*(H00-H0);
          break;
          case  'SVG_PW2s':
            var label='mn';
            var pixelTic=90;
            var X0=0;
            var dTextTic=1;
            var moduloText=-100;
            var H00= 0;
            dI=2; //2 courbes PW et PVA
           
          break;
          case  'SVG_Wh1an':
            var label='Mois';
            var pixelTic=dX*30.4375;//Mois moyen
            var dTextTic=1;
            var moduloText=12;
            var H00= d.getMonth();
            var X0=dX*(1-d.getDate());
            var Mois=['Jan','Fev','Mars','Avril','Mai','Juin','Juil','Ao&ucirc;t','Sept','Oct','Nov','Dec'];
          break;
          
        }
        
        
        for (var x=1000+X0;x>100;x=x-pixelTic){
          var X=x;
          S +="<line x1='"+X+"' y1='250' x2='"+X+"' y2='256' style='stroke:white;stroke-width:2' />";
          X=X-8;
          if (SVG=='SVG_Wh1an') {
            X=X+8;
            S +="<text x='"+X+"' y='272' style='font-size:16px;fill:white;'>"+Mois[H00]+"</text>";
          }else{
            S +="<text x='"+X+"' y='272' style='font-size:16px;fill:white;'>"+H00+"</text>";
          }
          H00=(H00-dTextTic+moduloText)%moduloText;
        }
        S +="<text x='980' y='247' style='font-size:14px;fill:white;'>"+label+"</text>";
        for (var y=-10 ;y<=10;y=y+2){
          
          var Y=250-23*y;
          S +="<line x1='100' y1='"+Y+"' x2='1000' y2='"+Y+"' style='stroke:white;stroke-width:1;stroke-dasharray:2 10;' />";
          Y=Y+7;
          var T=cadrageMax*y/10;T=T.toString();
          var X=90-9*T.length;
          S +="<text x='"+X+"' y='"+Y+"' style='font-size:16px;fill:white;'>"+T+"</text>";
        }
        S +="<text x='450' y='18' style='font-size:18px;fill:"+couleur1+";'>"+titre1+"</text>";
        S += "<polyline points='";       
          for (var i = 0; i < Tab.length; i = i+dI) {
            var Y = 250 - 230 * Tab[i] / cadrageMax;
            var X = 100+dX * i;
            S += X + "," + Y + " ";
          }
        S += "' style='fill:none;stroke:"+couleur1+";stroke-width:4' />";
        if (dI==2){
          S +="<text x='450' y='40' style='font-size:18px;fill:"+couleur2+";'>"+titre2+"</text>";
          S += "<polyline points='";       
            for (var i = 1; i < Tab.length; i = i+dI) {
              var Y = 250 - 230 * Tab[i] / cadrageMax;
              var X = 100+dX * i;
              S += X + "," + Y + " ";
            }
          S += "' style='fill:none;stroke:"+couleur2+";stroke-width:4' />";
        }
        S += "</svg>";
        document.getElementById(SVG).innerHTML = S;
    }
    function EtatActions() {
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() { 
        if (this.readyState == 4 && this.status == 200) {
          var retour=this.responseText;
          var message=retour.split('|');
          var S='Source : ' 
          if(message[0]==1){
            S +='ESP distant '+int2ip(message[1]);
          }else {
            S +='ESP local';
          }
          GH('source',S);
          if (message[2]>0){ //Nb Actions
            S="<div><div class='tableau'><table >";
            for (var i=0;i<message[2];i++){ 
              var data=message[i+3].split(",");
              S+="<tr><td>"+data[1]+"</td>";
              if (data[0]==0){ //Action 0, Triac
                var W=1+1.99*data[2];
                S+="<td><div class='jaugeBack'><div class='jauge' style='width:"+W+"px'></div><div class='centrer w100'>"+data[2]+"%</div></div></td></tr>";
              }else{
                S+="<td class='centrer'>"+data[2]+"</td></tr>";
              }
            }
            S +="</table>";
            GH("etatActions",S);
          }
          console.log(retour);
          setTimeout('EtatActions();',3500);
        }
        
      };
      xhttp.open('GET', 'ajax_etatActions', true);
      xhttp.send();
    }
    
    function LaVal(d){
        d=parseInt(d);
        d='           '+d.toString();
        return d.substr(-9,3)+' '+d.substr(-6,3)+' '+d.substr(-3,3);
    }
    function int2ip (V) {
      var ipInt=parseInt(V);
      return ( (ipInt>>>24) +'.' + (ipInt>>16 & 255) +'.' + (ipInt>>8 & 255) +'.' + (ipInt & 255) );
    }
    function GID(id) {
        return document.getElementById(id);
    };
    function GH(id, T) {
        GID(id).innerHTML = T;
    }
)====";
