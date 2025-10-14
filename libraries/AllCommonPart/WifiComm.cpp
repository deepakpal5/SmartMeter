#include "AllCommonPart.h"
#include <WiFi.h>
#include "esp_system.h"
#include <HTTPClient.h>
#include <WebServer.h>
#include <DNSServer.h>


HTTPClient http;
DNSServer dnsServer;
WebServer server(80);
const byte DNS_PORT = 53;

String savedSSID = "",savedAPN = "";
String  Mode="GSM";



char ssidRouter[64];
char passRouter[64];



void handleRoot() {
  String html = R"rawliteral(
    




  






<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1.0" />
  <title>Servotech SmartMeter Config</title>
  <!-- <link href="https://fonts.googleapis.com/css2?family=Roboto:wght@400;700&display=swap" rel="stylesheet"> -->
  <style>
    body {
      margin: 0;
      padding: 0;
      font-family: 'Roboto', sans-serif;
      background: linear-gradient(to right, #004e92, #000428);
      display: flex;
      justify-content: center;
      align-items: center;
      height: 100vh;
      color: #333;
    }
  .radio-group {
    display: flex;
    justify-content: center;
    gap: 20px; /* space between radios */
    margin-bottom: 20px;
  }
  .radio-group input[type="radio"] {
    display: none; /* hide the default radio */
  }
  .radio-group label {
    padding: 10px 15px;
    border: 2px solid #004e92;
    border-radius: 8px;
    cursor: pointer;
    color: #004e92;
    font-weight: 600;
    user-select: none;
    transition: background-color 0.3s, color 0.3s;
  }
  .radio-group input[type="radio"]:checked + label {
    background-color: #004e92;
    color: white;
  }
    .container {
      background: #fff;
      padding: 30px 40px;
      border-radius: 15px;
      box-shadow: 0 8px 20px rgba(0, 0, 0, 0.3);
      max-width: 400px;
      width: 100%;
      text-align: center;
    }

    h1 {
      color: #004e92;
      margin-bottom: 25px;
      font-size: 26px;
    }

    .form-group {
      margin-bottom: 20px;
    }

    input[type="text"],
    input[type="password"] {
      width: 100%;
      padding: 12px;
      border: 1px solid #ccc;
      border-radius: 8px;
      font-size: 16px;
      box-sizing: border-box;
      transition: 0.3s;
    }

    input:focus {
      border-color: #004e92;
      outline: none;
      box-shadow: 0 0 5px rgba(0, 78, 146, 0.5);
    }

    button {
      width: 100%;
      padding: 12px;
      margin-top: 10px;
      font-size: 16px;
      border: none;
      border-radius: 8px;
      cursor: pointer;
      transition: background 0.3s;
      font-weight: bold;
    }

    .submit-btn {
      background-color: #004e92;
      color: white;
    }

    .submit-btn:hover {
      background-color: #003366;
    }

    .cancel-btn {
      background-color: #ddd;
      color: #333;
    }

    .cancel-btn:hover {
      background-color: #bbb;
    }

    .reboot-btn {
      background-color: #e63946;
      color: white;
    }

    .reboot-btn:hover {
      background-color: #b71c1c;
    }

    .info {
      margin-top: 20px;
      font-size: 14px;
      color: #444;
    }

    .info b {
      color: #000;
    }

    @media (max-width: 480px) {
      .container {
        padding: 20px;
      }
    }
  </style>

  <script>
    function sendData() {
      const ssid = document.getElementById('ssid').value;
      const psk = document.getElementById('password').value;
      const apn = document.getElementById('apn').value;
      const radios = document.getElementsByName('Carrier');
    let selectedId = null;
    for (const radio of radios) {
      if (radio.checked) {
        selectedId = radio.id;  // send the numeric id here
        break;
      }
    }




if ((ssid && !psk) || (!ssid && psk)) {
   alert("Please fill ssid or Password  as well.");
        return;
}

   

const bodywr= `ssid=${encodeURIComponent(ssid)}&password=${encodeURIComponent(psk)}&apn=${encodeURIComponent(apn)}&Carrier=${encodeURIComponent(selectedId)}`;

console.log(bodywr);

      fetch('/submit', {
        method: 'POST',
        headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
        body: bodywr,
      }).then(response => {
        alert('Configuration submitted!');
        location.reload();
      });
    }

    function Reboot() {
      fetch('/reboot', {
        method: 'POST',
        headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
        body: `Reboot=true`
      }).then(response => {
        alert('Device is rebooting...');
      });
    }

    function cancelForm() {
      document.getElementById('ssid').value = '';
      document.getElementById('password').value = '';
      document.getElementById('apn').value = '';
    }
  </script>
</head>

<body>
  <div class="container">
    <h1>SmartMeter Configuration</h1>

    <div class="form-group">
      <input type="text" id="ssid" placeholder="WiFi SSID">
    </div>

    <div class="form-group">
      <input type="password" id="password" placeholder="WiFi Password">
    </div>

    <div class="form-group">
      <input type="text" id="apn" placeholder="APN">
    </div>






<div class="radio-group">
  <input type="radio" id="1" name="Carrier" value="WiFi">
  <label for="1">WiFi</label>

  <input type="radio" id="2" name="Carrier" value="GSM">
  <label for="2">GSM</label>

  <input type="radio" id="3" name="Carrier" value="Auto">
  <label for="3">Auto</label>
</div>




    <button class="submit-btn" onclick="sendData()">Submit</button>
    <button class="cancel-btn" onclick="cancelForm()">Cancel</button>
    <button class="reboot-btn" onclick="Reboot()">Reboot</button>

    <div class="info">
     <p>Last SSID: <b>)rawliteral" + savedSSID +R"rawliteral(</b></p>
      <p>Last APN: <b>)rawliteral" + savedAPN + R"rawliteral(</b></p>
      <p>Last SSID: <b>)rawliteral" + Mode +R"rawliteral(</b></p>
    </div>
  </div>
</body>
</html>












  )rawliteral";

  server.send(200, "text/html", html);
}

void handleSubmit() {

  preferences.begin("credentials", false);
  if (server.hasArg("ssid")) preferences.putString("ssid", server.arg("ssid"));
  if (server.hasArg("password")) preferences.putString("password", server.arg("password"));
  if (server.hasArg("apn")) preferences.putString("APNSIM", server.arg("apn"));
  if (server.hasArg("Carrier")) {
    int mode = server.arg("Carrier").toInt();  // Use toInt() here
    preferences.putInt("Mode", mode);
  }


  Serial.println(preferences.getString("ssid", "test0"));
  Serial.println(preferences.getString("password", "test1"));
  Serial.println(preferences.getString("APNSIM", "test2"));
  Serial.println(preferences.getInt("Mode", 1));
preferences.end();





  


  server.send(200, "text/plain", "OK");
}

void handleReboot() {
  server.send(200, "text/html", "<h2>Rebooting...</h2>");
  delay(1000);
  ESP.restart();
}











bool connectToWifi(String ssid, String psk)
{
  if (!WiFi.isConnected())
  {
strncpy(ssidRouter, ssid.c_str(), sizeof(ssidRouter));
ssidRouter[sizeof(ssidRouter) - 1] = '\0';  // Ensure null termination

strncpy(passRouter, psk.c_str(), sizeof(passRouter));
passRouter[sizeof(passRouter) - 1] = '\0';
    if (DEBUG_OUT)
      Serial.print(F("Connecting to WiFi... "));

    WiFi.setHostname("Wifi-Servotech");
    WiFi.begin(ssidRouter, passRouter);

    unsigned long wifiNetworkCounter = millis();
    while (!WiFi.isConnected() && millis() - wifiNetworkCounter < 15000)
    {
      delay(500); // Give WiFi time to connect
    }

    if (WiFi.isConnected())
    {
      if (true)
      {
        Serial.print(F("Connected to "));
        Serial.print(ssidRouter);
        Serial.print(F(" | IP: "));
        Serial.println(WiFi.localIP());
      }
      return true;
    }
    else
    {
      if (DEBUG_OUT)
        Serial.println("WiFi connection failed!");
      return false;
    }
  }
  else
  {
    if (DEBUG_OUT)
    {
      Serial.print(F("Already connected | IP: "));
      Serial.println(WiFi.localIP());
    }
    return true;
  }

  // Just in case something falls through
  return false;
}








bool connectToWifidummy()
{
  WiFi.disconnect(true);
  WiFi.begin(ssidRouter, passRouter);

  if (WiFi.isConnected())
  {
    // LOCAL_loop();
    if (DEBUG_OUT)
    {
      Serial.print(F("Connected to "));
      Serial.print(ssidRouter);
      Serial.print(F(" And IP address: "));
      Serial.println(WiFi.localIP());
    }

    return true;
  }
}
bool checkSTAConnected()
{
  return WiFi.isConnected();
}


IPAddress getIPFromStr(String IPSTRece)
{
  IPAddress apip;
  apip.fromString(IPSTRece);

  return apip;
}
String getIPAddressForConfigWifi(int m)
{
  if (m == 1)
    return IpconntoString(WiFi.localIP());

  else if (m == 2)
    return IpconntoString(WiFi.gatewayIP());
  else if (m == 3)
    return IpconntoString(WiFi.subnetMask());
  else if (m == 4)
    return IpconntoString(WiFi.dnsIP());
}
String IpconntoString(IPAddress ipAd)
{
  return String(ipAd[0]) + "." + String(ipAd[1]) + "." + String(ipAd[2]) + "." + String(ipAd[3]);
}







void disableAppConfig()
{
  unsigned long timeout = millis() + 3000;  // 3-second timeout

  // Disable AP mode
  while (!WiFi.enableAP(false))
  {
    if (millis() > timeout)
    {
      Serial.println("Timeout disabling AP mode!");
      break;
    }
    delay(10);
  }

  // Disconnect SoftAP
  timeout = millis() + 3000;
  while (WiFi.softAPdisconnect(true))
  {
    if (millis() > timeout)
    {
      Serial.println("Timeout disconnecting SoftAP!");
      break;
    }
    delay(10);
  }


  // Serial.println("WiFi completely disabled.");
}













void websocketSetupApp()
{
  

  unsigned long timeout;


  timeout = millis() + 3000;
  while (!WiFi.softAP("ServoTech_Power", "0123456789"))
  {
    if (millis() > timeout) {
      Serial.println("Failed to start softAP.");
      return;
    }
    delay(10);
  }

  IPAddress apIP = WiFi.softAPIP();
  Serial.println("AP started. IP: " + apIP.toString());


preferences.begin("credentials", false);
savedSSID = preferences.getString("ssid", "evev");
savedAPN = preferences.getString("APNSIM", "JioNet");
int mode =preferences.getInt("Mode", 1);

Mode=mode==1?"WiFi":mode==2?"GSM":"Auto";

preferences.end();
 

  dnsServer.start(DNS_PORT, "*", apIP);

  server.on("/", handleRoot);
  server.on("/submit", HTTP_POST, handleSubmit);
  server.on("/reboot", handleReboot);
  server.onNotFound(handleRoot);

  server.begin();
  Serial.println("Web server started.");


}

//=============================================EEPROM==================================================


void writeEnergyInMem(int connector, uint64_t value)
{
  // String s = "ConEne" + String(connector);
  // preferences.begin("credentials", false);
  // preferences.putULong(s.c_str(), value);
  // preferences.end();
}
uint64_t readEnergyInMem(int connector)
{
  uint64_t data = 0;
  // String s = "ConEne" + String(connector);
  // preferences.begin("credentials", false);
  // data = preferences.getULong(s.c_str(), 0);
  // preferences.end();
  return data;
}



//=======================================================================================================================
bool dataSendToServerOverWiFi(String urlPost,String data) 
{
    
        
              http.begin(urlPost);
              http.addHeader("Content-Type", "application/json");
                int httpResponseCode = http.POST(data);
                 
                  if(httpResponseCode > 0){
                 
                     responseFromServer(http.getString());
                     http.end();
                     return true;


                      // Serial.println("HTTP POST Response code: " + String(httpResponseCode));
                        //  Serial.println("HTTP POST Response: " + payload);
                   }else {
                        Serial.println("HTTP POST request failed");
                        http.end();
                        return false;
                  }
        
        return false;
}


void dataGetFromServer(String UrlGet) {

http.begin(UrlGet);
      int ResponseCode = http.GET();
      if(ResponseCode > 0)
      {
        // String payload = http.getString();
        // Serial.println("HTTP GET Response code: " + String(ResponseCode));
        // Serial.println("HTTP GET Response: " + payload);
        // DynamicJsonDocument jsonResponse(512);
        // DeserializationError error = deserializeJson(jsonResponse, payload);
        // if(error){
        //     Serial.print("deserializeJson() failed: ");
        //     Serial.println(error.c_str());
        // }
        // else {
        //   bool uNewContacterStatus = jsonResponse["contactorState"] ;                         //Contactor Value
        //   Serial.println("Contactor State: " + String(uNewContacterStatus));
        //   if (uNewContacterStatus && !digitalRead(ContactorPIN))
        //   digitalWrite(ContactorPIN,HIGH);

        //   else if (!uNewContacterStatus && digitalRead(ContactorPIN))
        //   digitalWrite(ContactorPIN,LOW);
          
          
        // }
      }else{
           Serial.println("HTTP GET request failed");
      }
    http.end();
}


void loopConfig(){
  dnsServer.processNextRequest();
  server.handleClient();
}

String getMacAddress(){
   uint64_t chipid = ESP.getEfuseMac();  // Get the MAC from eFuse (always available)
  char id[13];  // 12 chars + null terminator
  sprintf(id, "%012llX", chipid);  // Format as uppercase hex without colons
  String deviceID = String(id);
  // Serial.println("Device ID: " + deviceID);
  return deviceID;
}