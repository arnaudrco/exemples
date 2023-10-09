// ********************
// Gestion des Actions
// ********************
class Action {
private:
  bool valide;
  int Idx;  //Index
  String DecodeString(String s);
  String EncodeString(String s);
  void CallExterne(String host,String url, int port);
  int GpioOn;
  int GpioOff;
  int OutOn;
  int OutOff;
  int actionTimer;
  

public:
  Action();  //Constructeur par defaut
  Action(int aIdx);
  
  void Definir(String ligne);
  String Lire();  
  void Activer(float Pw, int Heure);
 
  byte TypeEnCours(int Heure);
  int Valmin(int Heure);
  int Valmax(int Heure);
  void InitGpio();
  byte Actif;
  int Port;
  int Repet;
  String Titre;
  String Host;
  String OrdreOn;
  String OrdreOff;
  byte NbPeriode;
  bool On;
  byte Type[8];
  int Hdeb[8];
  int Hfin[8];
  int Vmin[8];
  int Vmax[8];
};
