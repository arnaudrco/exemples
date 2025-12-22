#define ARDUINOJSON_DECODE_UNICODE 1
#include <ArduinoJson.h>

struct SpiRamAllocator {
        void* allocate(size_t size) {
                return ps_malloc(size);

        }
        void deallocate(void* pointer) {
                free(pointer);
        }
};



void setup() {
// put your setup code here, to run once:
Serial.begin(115200);
//Initialisation
if(psramInit()){
        Serial.println("\nLa PSRAM est correctement initialisée");
}else{
        Serial.println("\nLa PSRAM ne fonctionne pas");
}

int n_elements = 20;
Serial.println((String)"Mémoire disponible PSRAM (octets): " +ESP.getFreePsram());
Serial.println("Tableau d'entiers initialisés à 0");
int *tableau = (int *) ps_calloc(n_elements, sizeof(int));
Serial.print("[tableau] : ");
for(int i=0; i!= n_elements;i++){
        Serial.print((String)tableau[0] + " ");
}
Serial.println((String)"\nMémoire disponible PSRAM (octets): " +ESP.getFreePsram());
}

void loop() {
}