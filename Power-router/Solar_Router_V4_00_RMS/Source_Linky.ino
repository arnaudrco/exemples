// ****************************
// * Source de Mesures LINKY  *
// ****************************

float deltaWS=0;
float deltaWI=0;
void Setup_Linky(){
  //  Serial2.begin(9600, SERIAL_7E1, RXD2, TXD2);  //  7-bit Even parity 1 stop bit pour le Linky
  Serial2.begin(1200, SERIAL_7E1, RXD2, TXD2);  //  7-bit Even parity 1 stop bit pour le Linky

//   PLUS
}

void LectureLinky() {  //Lecture port série du LINKY .
  int V = 0;
  long OldWh = 0;
  float deltaWh = 0;
  float Pmax = 0;
  unsigned long Tm = 0;
  float deltaT = 0;
  while (Serial2.available() > 0) {
    V = Serial2.read();
    DataRawLinky[IdxDataRawLinky] = char(V);
    IdxDataRawLinky = (IdxDataRawLinky + 1) % 10000;
    switch (V) {
      case 2:  //STX (Start Text)
        break;
      case 3:  //ETX (End Text)
        previousETX = millis();
        cptLEDyellow = 4;
        LFon = false;
        break;
      case 10:  // Line Feed. Debut Groupe
        LFon = true;
        IdxBufDecodLinky = IdxDataRawLinky;
        break;
      case 13:       // Line Feed. Debut Groupe
        if (LFon) {  //Debut groupe OK
          LFon = false;
          int nb_tab = 0;
          String code = "";
          String val = "";
          int checksum = 0;
          int checkLinky = -1;
          
          while (IdxBufDecodLinky != IdxDataRawLinky) {
            if (DataRawLinky[IdxBufDecodLinky] == char(9)) {  //Tabulation
              nb_tab++;
            } else {
              if (nb_tab == 0) {
                code += DataRawLinky[IdxBufDecodLinky];
              }
              if (nb_tab == 1) {
                val += DataRawLinky[IdxBufDecodLinky];
              }
              if (nb_tab <= 1) {
                checksum += (int)DataRawLinky[IdxBufDecodLinky];
              }
            }
            IdxBufDecodLinky = (IdxBufDecodLinky + 1) % 10000;
            if (checkLinky == -1 && nb_tab == 2) {
              checkLinky = (int)DataRawLinky[IdxBufDecodLinky];
              checksum += 18;            //2 tabulations
              checksum = checksum & 63;  //0x3F
              checksum = checksum + 32;  //0x20
            }
          }
          if (code.indexOf("EAST") == 0 || code.indexOf("EAIT") == 0 || code=="SINSTS" || code.indexOf("SINSTI") == 0) {           
            if (checksum != checkLinky) {
              //DDD.println("Erreur checksum code : " + code + " " + String(checksum) + "," + String(checkLinky));
              Serial.println("Erreur checksum code : " + code + " " + String(checksum) + "," + String(checkLinky));
            }
          }
          if (code.indexOf("EAST") == 0) {
           
            OldWh = Energie_M_Soutiree;
            if (OldWh == 0) { OldWh = val.toInt(); }
            Energie_M_Soutiree = val.toInt();
            Tm = millis();
            deltaT = float(Tm - TlastEASTvalide);
            deltaT = deltaT / float(3600000);
            if (Energie_M_Soutiree == OldWh) {  //Pas de resultat en Wh
              Pmax = 1.3 / deltaT;
              moyPWS = min(moyPWS, Pmax);
            } else {
              TlastEASTvalide = Tm;
              deltaWh = float(Energie_M_Soutiree - OldWh);
              deltaWS=deltaWh / deltaT;        
            }
            moyPWS = 0.05 * deltaWS + 0.95 * moyPWS;
            EnerQuotidienM_Soutiree();
            EASTvalid = true;
            if (!EAITvalid && Tm>8000) { //Cas des CACSI ou EAIT n'est jamais positionné
              EAITvalid=true;
            }
          }
          if (code.indexOf("EAIT") == 0) {
            OldWh = Energie_M_Injectee;
            if (OldWh == 0) { OldWh = val.toInt(); }
            Energie_M_Injectee = val.toInt();
            Tm = millis();
            deltaT = float(Tm - TlastEAITvalide);
            deltaT = deltaT / float(3600000);
            if (Energie_M_Injectee == OldWh) {  //Pas de resultat en Wh
              Pmax = 1.3 / deltaT;
              moyPWI = min(moyPWI, Pmax);
            } else {
              TlastEAITvalide = Tm;
              deltaWh = float(Energie_M_Injectee - OldWh);
              deltaWI=deltaWh / deltaT;
            }
            moyPWI = 0.05 * deltaWI + 0.95 * moyPWI;
            EnerQuotidienM_Injectee();
            EAITvalid = true;
          }
          if ( EASTvalid && EAITvalid) {
            EnergieActiveValide=true;
          }
          if (code=="SINSTS") {  //Puissance apparente soutirée. Egalité pour ne pas confondre avec SINSTS1 (triphasé)
            PVAS_M = val.toInt();
            moyPVAS = 0.05 * float(PVAS_M) + 0.95 * moyPVAS;
            moyPWS=min(moyPWS,moyPVAS);
            if (moyPVAS > 0) {
              COSphiS = moyPWS / moyPVAS;
              COSphiS = min(float(1.0), COSphiS);
              PowerFactor_M=COSphiS;
            }
            PuissanceS_M = int(COSphiS * float(PVAS_M));
          }
          if (code.indexOf("SINSTI") == 0) {  //Puissance apparente injectée
            PVAI_M = val.toInt();
            moyPVAI = 0.05 * float(PVAI_M) + 0.95 * moyPVAI;
            moyPWI=min(moyPWI,moyPVAI);
            if (moyPVAI > 0) {
              COSphiI = moyPWI / moyPVAI;
              COSphiI = min(float(1.0), COSphiI);
              PowerFactor_M=COSphiI;
            }
            PuissanceI_M = int(COSphiI * float(PVAI_M));
          }
          if (code.indexOf("DATE") == 0) {
            esp_task_wdt_reset();  //Reset du Watchdog à chaque trame du Linky reçue
          }
          if (code.indexOf("URMS1") == 0) {
            Tension_M=val.toFloat();  //phase 1 uniquement
          }
          if (code.indexOf("IRMS1") == 0) {
            Intensite_M=val.toFloat();  //Phase 1 uniquement
          }
          if (code.indexOf("LTARF") == 0) {
            LTARF=val;  //Option Tarifaire
          }
        }
        break;
      default:
        break;
    }
  }
}
