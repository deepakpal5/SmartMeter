#include "ModbusMeter.h"
#include <AllCommonPart.h>


HardwareSerial ModbusSerial(1);  // Use UART1
float VN1=0.0,Cu1=0.0,TP1=0.0,VN2=0.0,Cu2=0.0,TP2=0.0,VN3=0.0,Cu3=0.0,TP3=0.0,RePow1=0.0,RePow2=0.0,RePow3=0.0,pf1=0.0,pf2=0.0,pf3=0.0,TE1=0.0,TE2=0.0,TE3=0.0;
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
        delay(10);
      }

      m = 0;
      return true;
    
  

}























bool ModbusMeter::checkRecivingdataOnModbus() {
  const int EXPECTED_BYTES = 231;
 

  if (ModbusSerial.available() != EXPECTED_BYTES) return false;

  byte receivedData[EXPECTED_BYTES] = {0};
  ModbusSerial.readBytes(receivedData, EXPECTED_BYTES);

// Serial.println("Received data: -> ");
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
  Cu1 = extract(35);
  Cu2 = extract(39);
  Cu3 = extract(43);
  TP1 = extract(51) * 1000;
  TP2 = extract(55) * 1000;
  TP3 = extract(59) * 1000;
  RePow1 = extract(63);
  RePow2 = extract(67);
  RePow3 = extract(71);
  pf1 = extract(99);
  pf2 = extract(103);
  pf3 = extract(107);
  TE1 = extract(155) * 1000;
  TE2 = extract(159) * 1000;
  TE3 = extract(163) * 1000;

  static float OldTE1 = TE1, OldTE2 = TE2, OldTE3 = TE3;

  if (m >= 40) {
    OldTE1 = TE1;
    OldTE2 = TE2;
    OldTE3 = TE3;
  }

  // Ensure energy values only increase realistically
  auto validateEnergy = [](float &TE, float &oldTE) {
    if (TE >= oldTE && TE < oldTE + 90 && TE >= 0) {
      oldTE = TE;
    } else {
      TE = oldTE;
    }
  };

  validateEnergy(TE1, OldTE1);
  validateEnergy(TE2, OldTE2);
  validateEnergy(TE3, OldTE3);
// Serial.println("VN1 : "+String(VN1)+" VN2 : "+String(VN2)+" VN3 : "+String(VN3)+" Cu1 : "+String(Cu1)+" Cu2 : "+String(Cu2)+" Cu3 : "+String(Cu3));
  return true;
}




float ModbusMeter ::GetVoltagePhase(int Connector){
  if(m>=80) return 0;
  // if (TestingHMIE)return 220;
  if (Connector==1)
  {
  return VN1;
  }
  else if (Connector==2)
  {
    return VN2;
  }
  else if (Connector==3)
  {
   return VN3;
  }  
}




float ModbusMeter::GetCurrentPhase(int Connector){
// return 5;

  if(m>=40) return 0;
   if (Connector==1)
  {
    
  return Cu1;  
  }
  else if (Connector==2)
  {
    return Cu2;
  }
  else if (Connector==3)
  {
   return Cu3; 
  } 
}

float ModbusMeter::GetPowerPhase(int Connector){
  if(m>=40) return 0;
   if (Connector==1)
  {
  return TP1;  
  }
  else if (Connector==2)
  {
    return TP2;
  }
  else if (Connector==3)
  {
   return TP3; 
  } 
  
}



float ModbusMeter::GetEnergyPhase(int Connector){
  
   if (Connector==1)
  {
   
  return TE1;  
  }
  else if (Connector==2)
  {
    return TE2;
  }
  else if (Connector==3)
  {
   return TE3; 
  } 
}

float ModbusMeter::GetPowerfectorPhase(int Connector){

  
   if (Connector==1)
  {
  return pf1;  
  }
  else if (Connector==2)
  {
    return pf2;
  }
  else if (Connector==3)
  {
   return pf3; 
  } 

  }
float ModbusMeter::GetReactivePowerPhase(int Connector){
   if (Connector==1)
  {
  return RePow1;  
  }
  else if (Connector==2)
  {
    return RePow2;
  }
  else if (Connector==3)
  {
   return RePow3; 
  } 
  
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
  digitalWrite(RS485Enable, HIGH);
delay(2);
}

void ModbusMeter::postTransmission() {
   ModbusSerial.flush();         // Wait until TX is complete
  digitalWrite(RS485Enable, LOW); // Switch to receive mode
  

}
String ModbusMeter::getdataSendToServer(){
  DynamicJsonDocument meterDataFrame(1024);
    meterDataFrame["Voltage V1N (V)"] =      String(GetVoltagePhase(1));
    meterDataFrame["Voltage V2N (V)"] =      String(GetVoltagePhase(2));
    meterDataFrame["Voltage V3N (V)"] =      String(GetVoltagePhase(3));
    meterDataFrame["Voltage V12 (V)"] =      String(GetVoltagePhase(1));
    meterDataFrame["Voltage V23 (V)"] =      String(GetVoltagePhase(2));
    meterDataFrame["Voltage V31 (V)"] =      String(GetVoltagePhase(3));
    meterDataFrame["Current I1 (A)"] =       String(GetCurrentPhase(1));
    meterDataFrame["Current I2 (A)"] =       String(GetCurrentPhase(2));
    meterDataFrame["Current I3 (A)"] =       String(GetCurrentPhase(3));
    meterDataFrame["kW1"] =                  String(GetPowerPhase(1));
    meterDataFrame["kW2"] =                  String(GetPowerPhase(2));
    meterDataFrame["kW3"] =                  String(GetPowerPhase(3));
    meterDataFrame["Total kWh -Import"] =    String(GetEnergyPhase(1));
    meterDataFrame["Total kWh -Export"] =    String(GetEnergyPhase(2));
    meterDataFrame["Total kW"] =             String(GetEnergyPhase(3));
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
    


 Serial.println("Meter Data : "+meterDataString);   
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