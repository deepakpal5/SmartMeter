#include <Arduino.h>
#include <ModbusMeter.h>
#include <AllCommonPart.h>


ModbusMeter energy_meter{};
Preferences preferences;
String urlGet="http://3.108.193.28:5000/meter/userVariable?meterNumber=101";
String urlPost="http://3.108.193.28:5000/meter/reading";
bool relayStatus=false;

void setup() {
  int Reason =esp_reset_reason();
  setupAllConfig(115200);


Serial.println("Controller Reboot Reason : "+String(Reason==0?"ESP_RST_UNKNOWN":Reason==1?"ESP_RST_POWERON":Reason==2?"ESP_RST_EXT":Reason==3?"ESP_RST_SW":Reason==4?"ESP_RST_PANIC":Reason==5?"ESP_RST_INT_WDT":Reason==6?"ESP_RST_TASK_WDT":Reason==7?"ESP_RST_WDT":Reason==8?"ESP_RST_DEEPSLEEP":Reason==9?"ESP_RST_BROWNOUT":"ESP_RST_SDIO"));
xTaskCreatePinnedToCore(RS485Metertask,"RS485Metertask",2048,&energy_meter,3,NULL,1);
delay(1000);
xTaskCreatePinnedToCore(AppConfigTask,"AppConfigTask",7168,NULL,1,NULL,1);
if (InternetConnectivity()==1)xTaskCreatePinnedToCore(ServerComOverWifi,"ServerComOverWifi",6144,NULL,2,NULL,1);
else if (InternetConnectivity()==2)xTaskCreatePinnedToCore(ServerComOverGSM,"ServerComOverGSM",6144,NULL,2,NULL,1);
else
  {
    Serial.println("_________No Network Found________");
  }
  

xTaskCreatePinnedToCore(otherTask,"otherTask",1024,NULL,1,NULL,1);

}

void loop() {



       

}



void RS485Metertask(void *pvParameters){
    ModbusMeter* modbusInstance = (ModbusMeter*) pvParameters;
    modbusInstance->ModbusUartinitilize();
  while (true) {
      modbusInstance->ModbusLOOP();
      vTaskDelay(1100 / portTICK_PERIOD_MS); 

  }
}



void ServerComOverWifi(void *pvParameters){
  digitalWrite(WIFIndiPIN,HIGH);
  Serial.println("Started Communication with SERVER over WiFi");
  while (true) {
   String data = energy_meter.getdataSendToServer();
    if (!dataSendToServerOverWiFi(urlPost,data))
    {
     if(DEBUG_OUT)  Serial.println("Trying to Save Data Offline");
      SaveDataInMemory(data);
    }
    
     else{
          if(DEBUG_OUT) Serial.println("Data Update SucessFully");
         loadSendFirstData(true,urlPost);
    }



      vTaskDelay(7000 / portTICK_PERIOD_MS); 
  }
}



void ServerComOverGSM(void *pvParameters){
digitalWrite(GSMIndiPIN,HIGH);
  Serial.println("Started Communication with SERVER over GSM");
while (true) {
String data = energy_meter.getdataSendToServer();
if (!dataSendToServerOverGSM(urlPost,data))
    {
      if(DEBUG_OUT) Serial.println("Trying to Save Data Offline");
     SaveDataInMemory(data);
    }
    else{
         if(DEBUG_OUT)  Serial.println("Data Update SucessFully");
         loadSendFirstData(false,urlPost);
    }
  vTaskDelay(2000 / portTICK_PERIOD_MS);  
    }
}




void otherTask(void* Parameter){
   while (true)
 {
  if (relayStatus && digitalRead(ContactorPIN) && (EmergencyPressed || EarthFault)) digitalWrite(ContactorPIN,LOW);
else if (relayStatus && !digitalRead(ContactorPIN) && !EmergencyPressed && !EarthFault) digitalWrite(ContactorPIN,HIGH);

   vTaskDelay(10 / portTICK_PERIOD_MS);
 }
  
}

void AppConfigTask(void* Parameter) {


   websocketSetupApp(); //Hotspot genrated 
 while (true)
 {
   loopConfig();
   vTaskDelay(10 / portTICK_PERIOD_MS);
 }
 
  // disableAppConfig();//Hotspot Closed

//  esp_task_wdt_delete(NULL); 
//  vTaskDelete(NULL);


}

void responseFromServer(String Response){





if(DEBUG_OUT)Serial.println("HTTP POST Response: " + Response);
Serial.println("EmergencyPressed : "+String(EmergencyPressed?"YES":"NO")+" EarthFault : "+String(EarthFault?"YES":"NO"));

        DynamicJsonDocument jsonResponse(512);
        DeserializationError error = deserializeJson(jsonResponse, Response);
        if(error){
            Serial.print("deserializeJson() failed: ");
            Serial.println(error.c_str());
        }
        else {
          bool NewrelayStatus = jsonResponse["desiredContactorState"] ;  
          if (NewrelayStatus != relayStatus)
          {
            relayStatus=NewrelayStatus;
            Serial.print("Contactor ");
            Serial.println(relayStatus?"ON":"OFF");
          if (relayStatus && !digitalRead(ContactorPIN))
          digitalWrite(ContactorPIN,HIGH);
          else if (!relayStatus && digitalRead(ContactorPIN))
          digitalWrite(ContactorPIN,LOW);}
        }

}
