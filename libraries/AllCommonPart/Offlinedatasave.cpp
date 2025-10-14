#include "AllCommonPart.h"
#include <Arduino.h>
#include <SPIFFS.h>
// #include <ArduinoJson.h>
#include <vector>
#define MAX_PACKETS 10
#define INTERVAL 2000
#define MAX_FILE_SIZE 120351506
#define MAX_CONFIGURATIONS 200



int OfflineCount = 0;
const char* filename = "/packets_data.txt";
std::vector<String> entriesOffline;

void setupForMemory(){
      if (!SPIFFS.begin(true)) {
    if(DEBUG_OUT)Serial.println("SPIFFS Mount Failed! Formatting...");
    if (!SPIFFS.format()) {
      if(DEBUG_OUT)Serial.println("SPIFFS format failed!");
      return;
    }
    if(DEBUG_OUT)Serial.println("SPIFFS formatted successfully.");
  } else {
    if(DEBUG_OUT)Serial.println("SPIFFS Mounted Successfully!");
    OfflineLoad();
   
}





}
void SaveDataInMemory(String data){
    entriesOffline.push_back(data);
      OfflineSave();
}

bool loadSendFirstData(bool Wifimode, String Url) {
  auto it = entriesOffline.begin();
  if (it != entriesOffline.end()) {
    // Serial.println(*it);

    

DynamicJsonDocument jsonResponse(1024);
        DeserializationError error = deserializeJson(jsonResponse, *it);
        if(error){
            Serial.print("deserializeJson() failed: ");
            Serial.println(error.c_str());
        }
        else  jsonResponse["liveStatus"] = 0;

String DataString;
    serializeJson(jsonResponse,DataString);
// Serial.println(DataString);

        










    bool sent = false;
    if (Wifimode) {
      sent = dataSendToServerOverWiFi(Url, DataString);
    } else {
      sent = dataSendToServerOverGSM(Url,DataString);
    }

    if (sent) {
      it=entriesOffline.erase(it); // Remove the sent packet
      OfflineSave();
      return true;              // Indicate that we sent and removed it
    } else {
      return false;             // Packet not sent, do not remove
    }
  }

  return true; // Nothing to send — empty vector
}







bool OfflineLoad(){




 if(DEBUG_OUT)Serial.print(F("[OfflineContainerFlash] Trying to Load Offline Data\n"));
        File file = SPIFFS.open(filename, "r");

    if (!file) {
       if(DEBUG_OUT)
        Serial.print(F("[OfflineContainerFlash] Unable to initialize: could not open Offline Data\n"));
        return false;
    }

    if (!file.available()) {
      if(DEBUG_OUT)  
      Serial.print(F("[OfflineContainerFlash] Populate FS: create Offline Data\n"));
        file.close();
        return true;
    }

    int file_size = file.size();
    // Serial.println("file_size : " +String(file_size));
    if (file_size < 2) {
       if(DEBUG_OUT) 
       Serial.print(F("[OfflineContainerFlash] Unable to initialize: Offline Data too short for json\n"));
        file.close();
        return false;
    } 
    else if (file_size > MAX_FILE_SIZE) {
       
      if(DEBUG_OUT) 
       {Serial.println(file_size);
        Serial.print(F("[OfflineContainerFlash] Unable to initialize: Offline Data filesize is too long!\n"));}
        file.close();
        return false;
    }

    String token = file.readStringUntil('\n');
    if (!token.equals("content:Offline_Data")) {
       if(DEBUG_OUT) 
       Serial.print(F("[OfflineContainerFlash] Unable to initialize: unrecognized configuration file format\n"));
        file.close();
        return false;
    }


   








token = file.readStringUntil(':');
    if (!token.equals("packetcount")) {
       if(DEBUG_OUT)
       
        Serial.print(F("[OfflineContainerFlash] Unable to initialize: missing Stop Count statement\n"));
        file.close();
        return false;
    }

    token = file.readStringUntil('\n');

OfflineCount= token.toInt();
    if(DEBUG_OUT)Serial.println("Offline Count : " +String(OfflineCount));
    
    if (OfflineCount > MAX_CONFIGURATIONS) {
       if(DEBUG_OUT)
       
        {Serial.println(OfflineCount);
        Serial.print(F("[OfflineContainerFlash] Unable to initialize: Stop Count is too big!\n"));}
        file.close();
        return false;
    }








    size_t jsonCapacity = file_size + JSON_OBJECT_SIZE(1) + JSON_ARRAY_SIZE(OfflineCount) + OfflineCount * JSON_OBJECT_SIZE(5);

    if (DEBUG_OUT) 
    {Serial.print(F("[OfflineContainerFlash] Config capacity = "));
    Serial.print(jsonCapacity);
    Serial.print(F("\n"));}

    DynamicJsonDocument configDoc(jsonCapacity);




    DeserializationError error = deserializeJson(configDoc, file);
    if (error) {
        if(DEBUG_OUT)
        {Serial.println(F("[OfflineContainerFlash] Unable to initialize: config file deserialization failed: "));
        Serial.print(error.c_str());
        Serial.print(F("\n"));}
        file.close();
        return false;
    }




    JsonArray configurationsArray = configDoc["OfflineData"];



if (OfflineCount != configurationsArray.size() )
{
    SPIFFS.remove(filename);

   file.close();
   Serial.println("[OfflineContainerFlash] Unable to initialize: offline data length mismatch ");
   return false;
}

if(DEBUG_OUT)Serial.println("configurationsArray.size() : "+String(configurationsArray.size()));
entriesOffline.clear();
for (size_t i = 0; i < configurationsArray.size(); i++)
{
   entriesOffline.push_back(configurationsArray[i].as<String>());
   delay(5);
}

file.close();
delay(10);

Serial.println("Total Saved Offline Packetes : "+String(entriesOffline.size()));
// for (auto entry = entriesOffline.begin(); entry != entriesOffline.end(); entry++) 
// {
// Serial.println(*entry);

//      }
return true;
    }






bool OfflineSave() {
  SPIFFS.remove(filename);
  File file = SPIFFS.open(filename, "w");
  if (!file) {
    if (DEBUG_OUT)
      Serial.println(F("[OfflineContainerFlash] Unable to save: could not open file"));
    return false;
  }

  file.print("content:Offline_Data\n");
  file.print("packetcount:");
  file.print(entriesOffline.size(), DEC);
  file.print("\n");










  String pr = "{\"OfflineData\":[";
  for (size_t i = 0; i < entriesOffline.size(); ++i) {
    pr += entriesOffline[i];
    if (i < entriesOffline.size() - 1)
      pr += ",";
  }
  pr += "]}\n";

  file.print(pr);
  file.close();

  // if (DEBUG_OUT)
    Serial.println(F("[OfflineContainerFlash] Saved successfully"));
  return true;
}
