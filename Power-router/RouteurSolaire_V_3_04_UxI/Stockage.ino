// ***************************
// Stockage des données en ROM
// ***************************
//Plan stockage
#define EEPROM_SIZE 4090
#define NbJour 370               //Nb jour historique stocké
#define adr_HistoAn 0            //taille  370*4=1480
#define adr_DateCeJour 1480      // String 8+1
#define adr_lastStockConso 1489  // Short taille 2
#define adr_ParaActions 1491     //Clé + ensemble parametres peu souvent modifiés


String DateCeJour = "";
int idxPromDuJour;



void INIT_EEPROM(void) {
  if (!EEPROM.begin(EEPROM_SIZE)) {
    Serial.println("Failed to initialise EEPROM");
    Serial.println("Restarting...");
//    Debug.println("Failed to initialise EEPROM");
//    Debug.println("Restarting...");
    delay(10000);
    ESP.restart();
  }
}

void RAZ_Histo_Conso() {
  //Mise a zero Zone stockage
  int Adr_SoutInjec = adr_HistoAn;
  for (int i = 0; i < NbJour; i++) {
    EEPROM.writeLong(Adr_SoutInjec, 0);
    Adr_SoutInjec = Adr_SoutInjec + 4;
  }

  EEPROM.writeString(adr_DateCeJour, "");
  EEPROM.writeUShort(adr_lastStockConso, 0);
  EEPROM.commit();
}

void LectureConsoMatinJour(void) {
  DateCeJour = EEPROM.readString(adr_DateCeJour);
  idxPromDuJour = EEPROM.readUShort(adr_lastStockConso);
  EASJ = 0;
  EAIJ = 0;
}


void JourHeureChange() {
  if (DATEvalid) {
    //Time Update / de l'heure
    time_t timestamp = time(NULL);
    char buffer[MAX_SIZE_T];
    struct tm *pTime = localtime(&timestamp);
    strftime(buffer, MAX_SIZE_T, "%d/%m/%Y %H:%M:%S", pTime);
    DATE = String(buffer);
    strftime(buffer, MAX_SIZE_T, "%d%m%Y", pTime);
    String JourCourant = String(buffer);
    strftime(buffer, MAX_SIZE_T, "%H", pTime);
    int hour = String(buffer).toInt();
    strftime(buffer, MAX_SIZE_T, "%M", pTime);
    int minute = String(buffer).toInt();
    HeureCouranteDeci = hour * 100 + minute * 10 / 6;

    if (DateCeJour != JourCourant) {  //Changement de jour
      idxPromDuJour = (idxPromDuJour + 1 + NbJour) % NbJour;
      DateCeJour = JourCourant;
      //On enregistre les conso en début de journée pour l'historique de l'année
      EEPROM.writeLong(idxPromDuJour * 4, floor(EASJ - EAIJ));
      EEPROM.writeString(adr_DateCeJour, JourCourant);
      EEPROM.writeUShort(adr_lastStockConso, idxPromDuJour);
      EEPROM.commit();
      LectureConsoMatinJour();
    }
  }
}
String HistoriqueEnergie1An(void) {
  String S = "";
  int Adr_SoutInjec = 0;
  long EnergieJour = 0;
  int iS = 0;

  for (int i = 0; i < NbJour; i++) {
    iS = (idxPromDuJour + i + 1) % NbJour;
    Adr_SoutInjec = adr_HistoAn + iS * 4;
    EnergieJour = EEPROM.readLong(Adr_SoutInjec);
    S += String(EnergieJour) + ",";
  }
  return S;
}
unsigned long LectureCle() {
  return EEPROM.readULong(adr_ParaActions);
}
void LectureEnROM() {
  int Hdeb;
  int address = adr_ParaActions;
  Cle_ROM = EEPROM.readULong(address);
  address += sizeof(unsigned long);
  ssid = EEPROM.readString(address);
  address += ssid.length() + 1;
  password = EEPROM.readString(address);
  address += password.length() + 1;
  dhcpOn = EEPROM.readByte(address);
  address += sizeof(byte);
  IP_Fixe = EEPROM.readULong(address);
  address += sizeof(unsigned long);
  Gateway = EEPROM.readULong(address);
  address += sizeof(unsigned long);
  UxIExterne = EEPROM.readByte(address);
  address += sizeof(byte);
  UxIExtIP = EEPROM.readULong(address);
  address += sizeof(unsigned long);
  DomoRepet = EEPROM.readUShort(address);
  address += sizeof(unsigned short);
  DomoIP = EEPROM.readULong(address);
  address += sizeof(unsigned long);
  DomoPort = EEPROM.readUShort(address);
  address += sizeof(unsigned short);
  DomoIdx = EEPROM.readUShort(address);
  address += sizeof(unsigned short);
  MQTTRepet = EEPROM.readUShort(address);
  address += sizeof(unsigned short);
  MQTTIP = EEPROM.readULong(address);
  address += sizeof(unsigned long);
  MQTTPort = EEPROM.readUShort(address);
  address += sizeof(unsigned short);
  MQTTUser = EEPROM.readString(address);
  address += MQTTUser.length() + 1;
  MQTTPwd = EEPROM.readString(address);
  address += MQTTPwd.length() + 1;
  MQTTPrefix = EEPROM.readString(address);
  address += MQTTPrefix.length() + 1;
  CalibU = EEPROM.readUShort(address);
  address += sizeof(unsigned short);
  CalibI = EEPROM.readUShort(address);
  address += sizeof(unsigned short);
  //Zone des actions
  NbActions = EEPROM.readUShort(address);
  address += sizeof(unsigned short);
  for (int iAct = 0; iAct < NbActions; iAct++) {
    LesActions[iAct].Actif = EEPROM.readByte(address);
    address += sizeof(byte);
    LesActions[iAct].Titre = EEPROM.readString(address);
    address += LesActions[iAct].Titre.length() + 1;
    LesActions[iAct].Host = EEPROM.readString(address);
    address += LesActions[iAct].Host.length() + 1;
    LesActions[iAct].Port = EEPROM.readUShort(address);
    address += sizeof(unsigned short);
    LesActions[iAct].OrdreOn = EEPROM.readString(address);
    address += LesActions[iAct].OrdreOn.length() + 1;
    LesActions[iAct].OrdreOff = EEPROM.readString(address);
    address += LesActions[iAct].OrdreOff.length() + 1;
    LesActions[iAct].Repet = EEPROM.readUShort(address);
    address += sizeof(unsigned short);
    LesActions[iAct].NbPeriode = EEPROM.readByte(address);
    address += sizeof(byte);
    Hdeb = 0;
    for (byte i = 0; i < LesActions[iAct].NbPeriode; i++) {
      LesActions[iAct].Type[i] = EEPROM.readByte(address);
      address += sizeof(byte);
      LesActions[iAct].Hfin[i] = EEPROM.readUShort(address);
      LesActions[iAct].Hdeb[i] = Hdeb;
      Hdeb = LesActions[iAct].Hfin[i];
      address += sizeof(unsigned short);
      LesActions[iAct].Vmin[i] = EEPROM.readShort(address);
      address += sizeof(unsigned short);
      LesActions[iAct].Vmax[i] = EEPROM.readShort(address);
      address += sizeof(unsigned short);
    }
  }
  Serial.println("Fin adresse lecture;" + String(address));
  kV=KV*CalibU/1000; //Calibration coefficient to be applied
  kI=KI*CalibI/1000;
}
int EcritureEnROM() {
  int address = adr_ParaActions;
  EEPROM.writeULong(address, Cle_ROM);
  address += sizeof(unsigned long);
  EEPROM.writeString(address, ssid);
  address += ssid.length() + 1;
  EEPROM.writeString(address, password);
  address += password.length() + 1;
  EEPROM.writeByte(address, dhcpOn);
  address += sizeof(byte);
  EEPROM.writeULong(address, IP_Fixe);
  address += sizeof(unsigned long);
  EEPROM.writeULong(address, Gateway);
  address += sizeof(unsigned long);
  EEPROM.writeByte(address, UxIExterne);
  address += sizeof(byte);
  EEPROM.writeULong(address, UxIExtIP);
  address += sizeof(unsigned long);
  EEPROM.writeUShort(address, DomoRepet);
  address += sizeof(unsigned short);
  EEPROM.writeULong(address, DomoIP);
  address += sizeof(unsigned long);
  EEPROM.writeUShort(address, DomoPort);
  address += sizeof(unsigned short);
  EEPROM.writeUShort(address, DomoIdx);
  address += sizeof(unsigned short);
  EEPROM.writeUShort(address, MQTTRepet);
  address += sizeof(unsigned short);
  EEPROM.writeULong(address, MQTTIP);
  address += sizeof(unsigned long);
  EEPROM.writeUShort(address, MQTTPort);
  address += sizeof(unsigned short);
  EEPROM.writeString(address, MQTTUser);
  address += MQTTUser.length() + 1;
  EEPROM.writeString(address, MQTTPwd);
  address += MQTTPwd.length() + 1;
  EEPROM.writeString(address, MQTTPrefix);
  address += MQTTPrefix.length() + 1;
  EEPROM.writeUShort(address, CalibU);
  address += sizeof(unsigned short);
  EEPROM.writeUShort(address, CalibI);
  address += sizeof(unsigned short);
  //Enregistrement des Actions
  EEPROM.writeUShort(address, NbActions);
  address += sizeof(unsigned short);
  for (int iAct = 0; iAct < NbActions; iAct++) {
    EEPROM.writeByte(address, LesActions[iAct].Actif);
    address += sizeof(byte);
    EEPROM.writeString(address, LesActions[iAct].Titre);
    address += LesActions[iAct].Titre.length() + 1;
    EEPROM.writeString(address, LesActions[iAct].Host);
    address += LesActions[iAct].Host.length() + 1;
    EEPROM.writeUShort(address, LesActions[iAct].Port);
    address += sizeof(unsigned short);
    EEPROM.writeString(address, LesActions[iAct].OrdreOn);
    address += LesActions[iAct].OrdreOn.length() + 1;
    EEPROM.writeString(address, LesActions[iAct].OrdreOff);
    address += LesActions[iAct].OrdreOff.length() + 1;
    EEPROM.writeUShort(address, LesActions[iAct].Repet);
    address += sizeof(unsigned short);
    EEPROM.writeByte(address, LesActions[iAct].NbPeriode);
    address += sizeof(byte);
    for (byte i = 0; i < LesActions[iAct].NbPeriode; i++) {
      EEPROM.writeByte(address, LesActions[iAct].Type[i]);
      address += sizeof(byte);
      EEPROM.writeUShort(address, LesActions[iAct].Hfin[i]);
      address += sizeof(unsigned short);
      EEPROM.writeShort(address, LesActions[iAct].Vmin[i]);
      address += sizeof(unsigned short);
      EEPROM.writeShort(address, LesActions[iAct].Vmax[i]);
      address += sizeof(unsigned short);
    }
  }

  Serial.println("Fin adresse stockage;" + String(address));
  EEPROM.commit();
  kV=KV*CalibU/1000; //Calibration coefficient to be applied
  kI=KI*CalibI/1000;
  return address;
}
