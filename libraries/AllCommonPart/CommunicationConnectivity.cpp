#include "AllCommonPart.h"
#include <Arduino.h>

int txnID=0;

int InternetConnectivity(){
   txnID=getSavedTxnId();
  preferences.begin("credentials", false);
  
String ssid = preferences.getString("ssid", "evev");
String apn = preferences.getString("APNSIM", "JioNet");
String psk= preferences.getString("password", "test1");
int Mode=preferences.getInt("Mode", 1);
preferences.end();

Serial.printf("----------------------------------------------------------------------------------------------------\n");
Serial.println("[CONFIGURATION PARAMETERS]");
Serial.printf("Received SSID             : %s\n", ssid.c_str());
Serial.printf("Received PSK              : %s\n", psk.c_str());
Serial.printf("Received APN              : %s\n", apn.c_str());
Serial.printf("Network Mode              : %s\n",Mode == 1 ? "WiFi" :Mode == 2 ? "GSM" : "Auto_Selection");
Serial.printf("----------------------------------------------------------------------------------------------------\n");




  
 if (Mode == 1 ){
  if (connectToWifi(ssid,psk))
  return 1;
    return 0;
     
}  
else if(Mode == 2 ){
     

      if (startCommunicationOnGSM(apn))
  return 2;
    return 0;
}



else if(Mode==3 ){
for (int i = 1; i <= 4; i++)
{
 
   if (connectToWifi(ssid,psk))
  {
   return 1;
  }
  else if (startCommunicationOnGSM(apn))
  {
    return 2;
  }

  
}
  return 0;
}

return 0;
}

int getSavedTxnId(){
 preferences.begin("credentials", false);

txnID=preferences.getInt("TxnID", 0);
preferences.end();
return txnID;
}

int getTransectionId(){
txnID++;
preferences.begin("credentials", false);
preferences.putInt("TxnID", txnID);
preferences.end();
return txnID;
}




//===========================================================================================
