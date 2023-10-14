// *******************
// * DOMOTICZ client * - Option pour transferer vers systeme Domoticz
// *******************
void EnvoiaDomoticz(int PW) {
  unsigned long tps = millis();
  if (int((tps - previousDomoMillis) / 1000) > DomoRepet && DomoRepet != 0) {
    previousDomoMillis = tps;
    byte arr[4];
    arr[0] = DomoIP & 0xFF;          // 0x78
    arr[1] = (DomoIP >> 8) & 0xFF;   // 0x56
    arr[2] = (DomoIP >> 16) & 0xFF;  // 0x34
    arr[3] = (DomoIP >> 24) & 0xFF;  // 0x12

    String host = String(arr[3]) + "." + String(arr[2]) + "." + String(arr[1]) + "." + String(arr[0]);
    WiFiClient DomoClient;
    if (!DomoClient.connect(host.c_str(), DomoPort)) {
      Serial.println("connection vers Domoticz plante");
      return;
    }
    String url;
    url = "/json.htm?type=command&param=udevice&idx=";
    url += String(DomoIdx);
    url += "&nvalue=0&svalue=";
    url += String(PW);
    // This will send the request to the Domoticz server
    DomoClient.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");
    unsigned long timeout = millis();
    while (DomoClient.available() == 0) {
      if (millis() - timeout > 5000) {
        Serial.println(">>> Client Domoticz Timeout !");
        DomoClient.stop();
        return;
      }
    }
    // Read all the lines of the reply from server and print them to Serial
    while (DomoClient.available()) {
      String line = DomoClient.readStringUntil('\r');
      //No interest of the response
    }
  }
}