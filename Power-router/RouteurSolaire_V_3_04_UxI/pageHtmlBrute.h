//************************************************
// Page données UxI Brutes HTML et Javascript 
//************************************************
const char *PageBrute = R"====(
 <!doctype html>
   <html><head><style>
    * {box-sizing: border-box;}
    body {background: linear-gradient(#003,#77b5fe,#003);background-attachment:fixed;font-size:150%;text-align:center;width:100%;max-width:1000px;margin:auto;}
    h2{text-align:center;color:white;}
    a:link {color:#ccf;text-decoration: none;}
    a:visited {color:#ccf;text-decoration: none;}
    .ri { text-align: right;}
    .ce { text-align: center;position:relative;}
    .Wh { background-color:#fdd;}
    .A { background-color:#ddf;}
    .VA { background-color:#ffd;}
    .V { background-color:#dfd;}
    .foot { color:white;position:relative;}
    #date { color:white;}
    #DataUxI { background-color:white;display:inline-block;margin:auto;padding:4px;text-align:left;width:100%;overflow:hidden;font-size:12px;}
    #tableau,#DataESP32 { background-color:white;display:inline-block;margin:auto;padding:4px;}
    table {border:10px inset azure;}
    td { text-align: left;padding:4px;}
    #LED{position:absolute;top:4px;left:4px;width:0px;height:0px;border:5px solid red;border-radius:5px;}
    .onglets{margin-top:4px;left:0px;font-size:130%;}
    .Baccueil,.Bbrut,.Bparametres,.Bactions{margin-left:20px;border:outset 4px grey;background-color:#333;border-radius:6px;padding-left:20px;padding-right:20px;display:inline-block;}
    .Bbrut{border:inset 8px azure;}
    svg { border:10px inset azure;background: linear-gradient(#333,#666,#333);}
  </style></head>
  <body  onload='LoadData();LoadDataESP32();' >
    <div id='LED'></div>
    <div class='onglets'><div class='Baccueil'><a href='/'>Accueil</a></div><div class='Bbrut'><a href='/Brute'>Donn&eacute;es brutes</a></div><div class='Bparametres'><a href='/Para'>Param&egrave;tres</a></div><div class='Bactions'><a href='/Actions'>Actions</a></div></div>
    <h2>Routeur Solaire - UxI</h2>
    <div id='date'>Date</div><br>
    <div class='foot' >Tension et Courant sur 20ms</div>
    <div  class='ce'><h3 style='position:absolute;top:20px;right:40px;'><span style='color:red;'>U_</span><span style='color:lightgreen;'> I_</span></h3><p id='SVG'></p></div>
    <br><br><div class='foot' >Donn&eacute;es ESP32</div>
    <div><div id='DataESP32' ></div></div>
    <div class='ri'><small><a href='https:F1ATB.fr' >F1ATB.fr</a></small></div>
    <script src='BruteJS'></script>
    <br></body></html>
)====";

const char *PageBruteJS = R"====(
       function LoadData() {
        GID('LED').style='display:block;';
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() { 
          if (this.readyState == 4 && this.status == 200) {
             var DuUxI=this.responseText;
             var message=DuUxI.split('|');
             var volt=message[0].split(',');
             var amp=message[1].split(','); 
            var S= "<svg height='400' width='1000' >";
            S += "<line x1='0' y1='400' x2='0' y2='0' style='stroke:rgb(0,0,0);stroke-width:2' />";
            S += "<line x1='0' y1='197' x2='1000' y2='197' style='stroke:rgb(0,0,0);stroke-width:2' />";
            var  Vmax = 500;
            var Imax = 500;
            for (var i = 0; i < 100; i++) {
              Vmax = Math.max(Math.abs(volt[i]), Vmax);
              Imax = Math.max(Math.abs(amp[i]), Imax);
            }

            S += "<polyline points='";
            for (var i = 0; i < 100; i++) {
              var Y = 197 - 185 * volt[i] / Vmax;
              var X = 10 * i;
              S += X + ',' + Y + ' ';
            }
            S += "' style='fill:none;stroke:red;stroke-width:6' />";
            S += "<polyline points='";
            for (var i = 0; i < 100; i++) {
              var Y = 197 - 185 * amp[i] / Imax;
              var X = 10 * i;
              S += X + ',' + Y + ' ';
            }
            S += "' style='fill:none;stroke:lightgreen;stroke-width:6' />";
            S += "</svg>";
            console.log(message[0]);
            document.getElementById('SVG').innerHTML = S;
            GID('LED').style='display:none;';
            setTimeout('LoadData();',2000);
          }  
        };
        xhttp.open('GET', 'ajax_dataUxI', true);
        xhttp.send();
      }
      function LoadDataESP32() {
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() { 
          if (this.readyState == 4 && this.status == 200) {
              var dataESP=this.responseText;
              var message=dataESP.split('|');
              var S='<table>';
              var H=parseInt(message[0]);
              H=H + (message[0]-H)*0.6;
              H=H.toFixed(2);
              H=H.replace(".", "h ")+"mn";
              S+='<tr><td>ESP On depuis :<td><td>'+H+'</td></tr>';
              S+='<tr><td>Niveau WiFi :<td><td>'+message[1]+' dBm</td></tr>';
              S+="<tr><td>Point d'acc&egrave;s WiFi :<td><td>"+message[2]+'</td></tr>';
              S+='<tr><td>R&eacute;seau WiFi :<td><td>'+message[3]+'</td></tr>';
              S+='<tr><td>Adresse IP ESP32 :<td><td>'+message[4]+'</td></tr>';
              S+='<tr><td>Adresse passerelle :<td><td>'+message[5]+'</td></tr>';
              S+='<tr><td>Masque du r&eacute;seau :<td><td>'+message[6]+'</td></tr>';
              S+='<tr><td>Charge coeur 0 (Lecture UxI) Min, Moy, Max :<td><td>'+message[7]+' ms</td></tr>';
              S+='<tr><td>Charge coeur 1 (Calcul + Wifi) Min, Moy, Max :<td><td>'+message[8]+' ms</td></tr>';
              S+='</table>';
              GH('DataESP32', S);
              GH('date', message[9]);             
             setTimeout('LoadDataESP32();',5000);
          }
          
        };
        xhttp.open('GET', 'ajax_dataESP32', true);
        xhttp.send();
      }
      function LaDate(d){
          return d.substr(0,1)+' '+d.substr(5,2)+'/'+d.substr(3,2)+'/'+d.substr(1,2)+' '+d.substr(7,2)+'h '+d.substr(9,2)+'mn '+d.substr(11,2)+'s';
      }
      function LaVal(d){
          d=parseInt(d);
          d='           '+d.toString();
          return d.substr(-9,3)+' '+d.substr(-6,3)+' '+d.substr(-3,3);
      }
      function GID(id) {
          return document.getElementById(id);
      };
      function GH(id, T) {
          GID(id).innerHTML = T;
      }
)====";
