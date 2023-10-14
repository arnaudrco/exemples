// *******************************
// * Source de Mesures UI Double *
// *      Capteur JSY-MK-194     *
// *******************************

void Setup_UxIx2() {
  Serial2.begin(4800, SERIAL_8N1, RXD2, TXD2);  //PORT DE CONNEXION AVEC LE CAPTEUR JSY-MK-194
}
void LectureUxIx2() {  //Ecriture et Lecture port série du JSY-MK-194  .

  int i, j;
  byte msg_send[] = { 0x01, 0x03, 0x00, 0x48, 0x00, 0x0E, 0x44, 0x18 };
  // Demande Info sur le Serial port 2 (Modbus RTU)
  for (i = 0; i < 8; i++) {
    Serial2.write(msg_send[i]);
  }

  //Réponse en général à l'appel précédent (seulement 4800bauds)
  int a = 0;
  while (Serial2.available()) {
    ByteArray[a] = Serial2.read();
    a++;
  }


  if (a == 61) {  //Message complet reçu
    j = 3;
    for (i = 0; i < 14; i++) {  // conversion séries de 4 octets en long
      LesDatas[i] = 0;
      LesDatas[i] += ByteArray[j] << 24;
      j += 1;
      LesDatas[i] += ByteArray[j] << 16;
      j += 1;
      LesDatas[i] += ByteArray[j] << 8;
      j += 1;
      LesDatas[i] += ByteArray[j];
      j += 1;
    }
    Sens_1 = ByteArray[27];  // Sens 1
    Sens_2 = ByteArray[28];

    //Données du Triac
    Tension_T = LesDatas[0] * .0001;
    Intensite_T = LesDatas[1] * .0001;
    int Puiss_1 = LesDatas[2] * .0001;
    Energie_T_Soutiree = int(LesDatas[3] * .1);
    EnerQuotidienT_Soutiree();
    PowerFactor_T = LesDatas[4] * .001;
    Energie_T_Injectee = int(LesDatas[5] * .1);
    EnerQuotidienT_Injectee();
    Frequence = LesDatas[7] * .01;
    int PVA1 = 0;
    if (PowerFactor_T > 0) {
      PVA1 = int(Puiss_1 / PowerFactor_T);
    }
    if (Sens_1 > 0) {  //Injection sur TRiac. Ne devrait pas arriver
      PuissanceI_T = Puiss_1;
      PuissanceS_T = 0;
      PVAI_T = PVA1;
      PVAS_T = 0;
    } else {
      PuissanceS_T = Puiss_1;
      PuissanceI_T = 0;
      PVAI_T = 0;
      PVAS_T = PVA1;
    }
    // Données générale de la Maison
    Tension_M = LesDatas[8] * .0001;
    Intensite_M = LesDatas[9] * .0001;
    int Puiss_2 = LesDatas[10] * .0001;
    Energie_M_Soutiree = int(LesDatas[11] * .1);
    EnerQuotidienM_Soutiree();
    PowerFactor_M = LesDatas[12] * .001;
    Energie_M_Injectee = int(LesDatas[13] * .1);
    EnerQuotidienM_Injectee();
    int PVA2 = 0;
    if (PowerFactor_M > 0) {
      PVA2 = int(Puiss_2 / PowerFactor_M);
    }
    if (Sens_2 > 0) {  //Injection en entrée de Maison
      PuissanceI_M = Puiss_2;
      PuissanceS_M = 0;
      PVAI_M = PVA2;
      PVAS_M = 0;
    } else {
      PuissanceS_M = Puiss_2;
      PuissanceI_M = 0;
      PVAI_M = 0;
      PVAS_M = PVA2;
    }

    EnergieActiveValide = true;
    esp_task_wdt_reset();  //Reset du Watchdog à chaque trame du module JSY-MK-194 reçue
    if (cptLEDyellow > 30) {
      cptLEDyellow = 4;
    }
  }
}