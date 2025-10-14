#include "AllCommonPart.h"
bool atMode = true;
String APN = "Jionet", buffer = "";
String IMSI ="";
String ICCID="";
char *data = (char *)malloc(1024);
HardwareSerial GSMuart(2);
//=================================================================================================
void setupGSMUart(){GSMuart.begin(115200, SERIAL_8N1, 14, 12);}
  
//=================================================================================================
bool startCommunicationOnGSM(String apn){

   
  // Ethernet.deinit();
  
  APN=apn;
   InititateGSMModem();
   return dataModeActivateonModule();

}

bool dataModeActivateonModule(){
   

unsigned long gsmATrespncTime = millis();
    while (gsmATrespncTime + 35000 > millis() && !sendCommandWithAnswer("AT\r\n", "OK"))
    {
        // delay(100); //delay for some time in AT command
    //     digitalWrite(GSMReset, LOW);
    // delay(150);
    // digitalWrite(GSMReset, HIGH);
    delay(1000);
    }
      
 if (!sendCommandWithAnswer("AT\r\n", "OK"))
    {
      Serial.println(F("GSM Module Not Initiated, AT Not Responding, Please Check Wiring !"));
      return false;
    }

    
    if(!sendCommandWithAnswer("AT+IPR?\r\n", "+IPR: 115200")){
     if(DEBUG_OUT) Serial.println(F("Boudrate not Matched!, Try to Update Boudrate"));
      sendCommandWithAnswer("AT+IPR=115200\r\n", "OK");
      }

  
    if(!sendCommandWithAnswer("AT+CPIN?\r\n", "+CPIN: READY")){

      
  
      Serial.println(F("SIM Not inserted properly"));
      if(sendCommandWithAnswer("AT+CPIN?\r\n", "+CPIN: NO SIM")){
        Serial.println(F("There is no SIM in Module"));
        return false;}
        
    return false;
    }
  

    else  

    
    
   
    Serial.println(F("SIM OK"));
    gsmATrespncTime = millis();
    while (sendCommandWithAnswer("AT+CSQ\r\n","+CSQ: 99,99")  )
    {
      if (gsmATrespncTime + 4000 < millis())
      {
        return false;
      }
      
      delay(150);
    }


    sendCommandWithAnswer("AT+CREG=1\r\n", "OK");

    IMSI =  sendCommandWithAnswer("AT+CIMI\r\n");
    ICCID = sendCommandWithAnswer("AT+CCID\r\n");

    if (startPPPOS(sendCommandWithAnswer("ATI\r\n"),sendCommandWithAnswer("AT+COPS?\r\n"),sendCommandWithAnswer("AT+CREG?\r\n"))) {
     
    
     
     return true;}


     else { Serial.println("Starting ... Failed");return false; }
    
    }

//===========================================================================================
void InititateGSMModem()
{
    
   
    Serial.println(F("GSM Trying To Turn ON..."));
  


    // delay(100);
    // digitalWrite(GSMReset, LOW);
    // delay(200);
    // digitalWrite(GSMReset, HIGH);
    // delay(1000);

}

//===========================================================================================
String sendCommandWithAnswer(String cmd)
{ String buffer;
  GSMuart.write((char *)cmd.c_str());
  unsigned long _tg = millis();
  while (true)
  {
    // LOCAL_loop();
    // data = (char)Serial.read();

int index = 0;

while (GSMuart.available()) {
  char c = GSMuart.read();
  data[index++] = c;
}
data[index] = '\0';



    if (data != NULL)
    {
      char *command = strtok(data, "\n");
      while (command != 0)
      {
        buffer = String(command);
        buffer.replace("\r", "");
        command = strtok(0, "\n");
        if (buffer != "")
        {
         if(DEBUG_OUT)  Serial.println(buffer);
        }
        cmd.replace("\r\n","");
        if (buffer != "" && buffer != "OK" &&  buffer != cmd)
        {
          return buffer;
        }
        
        
        
        buffer = "";
        // LOCAL_loop();
      }
    }
    if (millis() > (_tg + 5000))
    {
      buffer = "";
      // Serial.println("buffer123======");
      return buffer;
    }
  }
  buffer = "";
  return buffer;
}
//===========================================================================================
bool sendCommandWithAnswer(String cmd, String ans)
{
  GSMuart.write((char *)cmd.c_str());
  unsigned long _tg = millis();
  while (true)
  {
    
 




int index = 0;

while (GSMuart.available()) {
  char c = GSMuart.read();
  data[index++] = c;
}
data[index] = '\0';



    if (data != NULL)
    {
      char *command = strtok(data, "\n");
      while (command != 0)
      {
        buffer = String(command);
        buffer.replace("\r", "");
        command = strtok(0, "\n");
        if (buffer != "")
        {
         if(DEBUG_OUT)  Serial.println(buffer);
          // Serial.println("try to match    :"+ans+":");
        }
        buffer.trim();
        if (buffer == ans)
        {
          buffer = "";
          return true;
        }
        buffer = "";
      }
    }
    if (millis() > (_tg + 5000))
    {
      buffer = "";
      return false;
    }
  }
  buffer = "";
  return false;
}
//===========================================================================================

//===========================================================================================
bool startPPPOS(String module,String SimComp , String Regestat){
  String apnSet;











    apnSet = "AT+CGDCONT=1,\"IP\",\"" + APN + "\"\r\n";

    if (!sendCommandWithAnswer(apnSet, "OK"))
      {
        return false;
        }


  if (!sendCommandWithAnswer("AT+CGATT=1\r\n", "OK"))
  {
    return false;
  }
  if (!sendCommandWithAnswer("AT+CGACT=1,1\r\n", "OK"))
  {
    return false;
  }

//---------------------------------------------------------------------------------------------------
  Serial.println("GSM started, Ready to Communicate to Server");
  return true;
}
//===========================================================================================





bool dataSendToServerOverGSM(String urlPost,String meterDataString){
  
//   Serial.println("");
// Serial.println(meterDataString);

if(DEBUG_OUT) Serial.println("");


String Data;



Data  = "AT+HTTPPARA=\"URL\",\"" + urlPost + "\"\r\n";
if (!sendCommandWithAnswer(Data,"OK"))return false;
if(DEBUG_OUT) Serial.println("");

if (!sendCommandWithAnswer("AT+HTTPSETUP\r\n","OK"))return false;
if(DEBUG_OUT) Serial.println("");



Data = "AT+HTTPACTION=2," + String(meterDataString.length()) + ",2\r\n";
  if (!sendCommandWithAnswer(Data, ">")) {
    Serial.println("Failed to get DOWNLOAD prompt");
    return false;
  }
if(DEBUG_OUT) Serial.println("");
delay(100);



while (GSMuart.available()) GSMuart.read();


// meterDataString = meterDataString+"\r\n";
GSMuart.print(meterDataString);  // NO \r\n needed in JSON
GSMuart.flush();
  delay(50);                    // Give time to send

  // 5. Wait and read full response
  unsigned long startTime = millis();
  String response = "";
  while ((millis() - startTime < 5000)  ) { 

    while (GSMuart.available()) {
      char c = GSMuart.read();
      response += c;
      // vTaskDelay(2 / portTICK_PERIOD_MS); 
    }
    vTaskDelay(5 / portTICK_PERIOD_MS); 
  }

//   Serial.println("Complete Response:");
//   Serial.println(response);
// Serial.println("Actual Response : "+extractJSONBody(response));

 responseFromServer(extractJSONBody(response));


sendCommandWithAnswer("AT+HTTPCLOSE\r\n", "OK");
return true;

}


String extractJSONBody(String response) {
  int jsonStart = response.indexOf('{'); // Find first {
  int jsonEnd = response.lastIndexOf('}'); // Find last }

  if (jsonStart != -1 && jsonEnd != -1 && jsonEnd > jsonStart) {
    return response.substring(jsonStart, jsonEnd + 1);
  } else {
    return "ERROR: JSON not found";
  }
}
