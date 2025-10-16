#include "ModbusMeter.h"
#include <AllCommonPart.h>


HardwareSerial ModbusSerial(1);  // Use UART1
float VN1=0.0,Cu1=0.0,TP1=0.0,VN2=0.0,Cu2=0.0,TP2=0.0,VN3=0.0,Cu3=0.0,TP3=0.0,RePow1=0.0,RePow2=0.0,RePow3=0.0,pf1=0.0,pf2=0.0,pf3=0.0,TE1=0.0,TE2=0.0,TE3=0.0,VN12=0.0,VN23=0.0,VN31=0.0,
TKWHImport=0.0,TKWHExport=0.0,TotalKW=0.0;
int m=0;
byte MeterRqstArray[]  = {1, 4, 0, 0, 0, 113, 48, 46}; // AddressCode,fnCOde,2_Byte Starting Resister,2_Byte DataLenght, 2_Byte Chuksum


unsigned long modbusRqstOld=0;


ModbusMeter ::ModbusMeter(void){}
ModbusMeter ::~ModbusMeter(){
    // end...............
}

void ModbusMeter::ModbusUartinitilize(){
   ModbusSerial.begin(9600, SERIAL_8N1, 16, 17);
   digitalWrite(RS485Enable, LOW);
}



bool ModbusMeter::ModbusLOOP() {
preTransmission();
      ModbusSerial.write(MeterRqstArray, 8);
postTransmission();


      unsigned long timeoutStart = millis();

      while (!checkRecivingdataOnModbus()) {
        if (millis() - timeoutStart >= 700) {
          m++;
          return false;
        }
        // delay(10);
         vTaskDelay(20 / portTICK_PERIOD_MS);
      }

      m = 0;
      return true;
    
  

}























bool ModbusMeter::checkRecivingdataOnModbus() {
  const int EXPECTED_BYTES = 231;
//  Serial.println("Sending Rqst: -> "+String(ModbusSerial.available()));

  if (ModbusSerial.available() != EXPECTED_BYTES) return false;

  byte receivedData[EXPECTED_BYTES] = {0};
  ModbusSerial.readBytes(receivedData, EXPECTED_BYTES);

// Serial.println("Received data: -> =================================================");
// for (size_t i = 0; i < EXPECTED_BYTES; i++)
// {
//   Serial.print(receivedData[i],HEX);
//   Serial.print(" ");
// }
// Serial.println(" ");
  // Check header
  if (receivedData[0] != 1 || receivedData[1] != 4 || receivedData[2] != MeterRqstArray[5] * 2)
    return false;

  auto extract = [&](int index) -> float {
    return dataformatter(receivedData[index], receivedData[index+1], receivedData[index+2], receivedData[index+3]);
  };

  VN1 = extract(3);
  VN2 = extract(7);
  VN3 = extract(11);

  VN12= extract(19);
  VN23= extract(23);
  VN31= extract(27);  

  Cu1 = extract(35);
  Cu2 = extract(39);
  Cu3 = extract(43);

  TP1 = extract(51);
  TP2 = extract(55);
  TP3 = extract(59);

  RePow1 = extract(63);
  RePow2 = extract(67);
  RePow3 = extract(71);
  
  TotalKW = extract(87);

  pf1 = extract(99);
  pf2 = extract(103);
  pf3 = extract(107);

  TE1 = extract(155);
  TE2 = extract(159);
  TE3 = extract(163);

  TKWHImport = extract(179);
  TKWHExport = extract(183); 



// Serial.println("VN1 : "+String(VN1)+" VN2 : "+String(VN2)+" VN3 : "+String(VN3)+" Cu1 : "+String(Cu1)+" Cu2 : "+String(Cu2)+" Cu3 : "+String(Cu3));
  return true;
}





  float ModbusMeter::GetVoltagePhase(int Connector) {
    if (m >= 200) return 0;
    switch (Connector) {
      case 1:
        return VN1;
      case 2:
        return VN2;
      case 3:
        return VN3;
      case 12:
        return VN12;
      case 23:
        return VN23;
      case 31:
        return VN31;
      default:
        return 0; 
    }
  }





float ModbusMeter::GetCurrentPhase(int Connector){


  if(m>=200) return 0;


 switch (Connector) {
      case 1:
        return Cu1;
      case 2:
        return Cu2;
      case 3:
        return Cu3;
      
      default:
        return 0;
    }



}

float ModbusMeter::GetPowerPhase(int Connector){
  if(m>=200) return 0;

   switch (Connector) {
      case 1:
        return TP1;
      case 2:
        return TP2;
      case 3:
        return TP3;
      
      default:
        return 0;
    }
  
}



float ModbusMeter::GetEnergyPhase(int Connector){
  
   switch (Connector) {
      case 1:
        return TE1;
      case 2:
        return TE2;
      case 3:
        return TE3;
      
      default:
        return 0;
    }
}

float ModbusMeter::GetPowerfectorPhase(int Connector){

  
  switch (Connector) {
      case 1:
        return pf1;
      case 2:
        return pf2;
      case 3:
        return pf3;
      
      default:
        return 0; 
    }

  }
float ModbusMeter::GetReactivePowerPhase(int Connector){

   switch (Connector) {
      case 1:
        return RePow1;
      case 2:
        return RePow2;
      case 3:
        return RePow3;
      
      default:
        return 0;
    }
  }

float ModbusMeter::TotalkwhImport(){
  return TKWHImport;
}

float ModbusMeter::TotalkwhExport(){
  return TKWHExport;
}

float ModbusMeter::TotalPower(){
  return TotalKW;
}
float ModbusMeter::dataformatter(byte data1, byte data2, byte data3, byte data4){
  byte binaryvalue[32];
  for(int i=0; i<8; i++)
    binaryvalue[7-i] = bitRead(data1, i);

  for(int i=0; i<8; i++)
    binaryvalue[15-i] = bitRead(data2, i);

  for(int i=0; i<8; i++)
    binaryvalue[23-i] = bitRead(data3, i);

  for(int i=0; i<8; i++)
    binaryvalue[31-i] = bitRead(data4, i);

  byte signedvalue = binaryvalue[0];
  byte expo[8];
  for(int i=1; i<9; i++)
    expo[i-1] = binaryvalue[i];

  int exponential = 0;
  for(int i=7; i>=0; i--)
    exponential = exponential + pow(2,7-i) * expo[i];

  exponential = exponential - 127;

  byte mantisa[24];
  mantisa[0] = 1;
  for(int i=9; i<32; i++)
    mantisa[i-8] = binaryvalue[i];
  float value = 0;
  for(int i=0; i<24; i++){
    value = value + pow(2,exponential) * mantisa[i];
    exponential = exponential - 1;
  }

  if(signedvalue == 0)
    value = value * 1;
  else if(signedvalue == 1)
    value = value * (-1);
  return value;
}

void ModbusMeter::preTransmission() {

  while (ModbusSerial.available()) ModbusSerial.read();  
  digitalWrite(RS485Enable, HIGH);
delay(2);
}

void ModbusMeter::postTransmission() {
   ModbusSerial.flush();         // Wait until TX is complete
  digitalWrite(RS485Enable, LOW); // Switch to receive mode
  delay(5);
  

}






String ModbusMeter::getdataSendToServer(){
  DynamicJsonDocument meterDataFrame(1024);
    meterDataFrame["Voltage V1N (V)"] =      String(GetVoltagePhase(1));
    meterDataFrame["Voltage V2N (V)"] =      String(GetVoltagePhase(2));
    meterDataFrame["Voltage V3N (V)"] =      String(GetVoltagePhase(3));
    meterDataFrame["Voltage V12 (V)"] =      String(GetVoltagePhase(12));
    meterDataFrame["Voltage V23 (V)"] =      String(GetVoltagePhase(23));
    meterDataFrame["Voltage V31 (V)"] =      String(GetVoltagePhase(31));
    meterDataFrame["Current I1 (A)"] =       String(GetCurrentPhase(1));
    meterDataFrame["Current I2 (A)"] =       String(GetCurrentPhase(2));
    meterDataFrame["Current I3 (A)"] =       String(GetCurrentPhase(3));
    meterDataFrame["kW1"] =                  String(GetPowerPhase(1));
    meterDataFrame["kW2"] =                  String(GetPowerPhase(2));
    meterDataFrame["kW3"] =                  String(GetPowerPhase(3));
    meterDataFrame["Total kWh -Import"] =    String(TotalkwhImport());
    meterDataFrame["Total kWh -Export"] =    String(TotalkwhExport());
    meterDataFrame["Total kW"] =             String(TotalPower());
    meterDataFrame["PF1"] =                  String(GetPowerfectorPhase(1));
    meterDataFrame["PF2"] =                  String(GetPowerfectorPhase(2));
    meterDataFrame["PF3"] =                  String(GetPowerfectorPhase(3));
    meterDataFrame["Meter Number"] =         getMacAddress();
    meterDataFrame["Frequency (Hz)"] =  "60";
    meterDataFrame["contactorState"] = (int)digitalRead(ContactorPIN);
    meterDataFrame["liveStatus"] = 1;
    meterDataFrame["faultStatus"] = getfaultSatatus();
    meterDataFrame["txnId"] = getTransectionId();

String meterDataString;
    serializeJson(meterDataFrame,meterDataString);
    


//  Serial.println("Meter Data : "+meterDataString);   
    return meterDataString;
}

String getfaultSatatus(){
  if (EmergencyPressed && EarthFault)
 return "Emergency Pressed & EarthFault Detected";
 else if (EmergencyPressed)
 return "Emergency Pressed";
 else if (EarthFault)
 return "EarthFault Detected";
 else return "None";
}