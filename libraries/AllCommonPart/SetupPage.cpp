#include "AllCommonPart.h"
#include <Arduino.h>
// #include <esp_task_wdt.h>



//===============================================================================
void setupAllConfig(u_int boudrate){
  Serial.begin(boudrate);
  Serial.println(F("=Start Setup="));
  Serial.setDebugOutput(true);
  setupForMemory();
  // esp_task_wdt_init(300, false); 

  // esp_task_wdt_add(NULL); 
  pinMode(GSMIndiPIN, OUTPUT);
  pinMode(RS485Enable, OUTPUT);
  pinMode(WIFIndiPIN, OUTPUT);
  pinMode(ContactorPIN, OUTPUT);
  pinMode(EmergencyButtonPin, INPUT);
  pinMode(EarthfAULTpIN, INPUT);
  pinMode(WifiConfigure, INPUT);
  delay(100);
setupGSMUart();
digitalWrite(GSMIndiPIN,LOW);

digitalWrite(WIFIndiPIN,LOW);
}


