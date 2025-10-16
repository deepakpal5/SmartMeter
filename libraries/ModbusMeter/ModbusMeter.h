#ifndef MODBUSMETER_H
#define MODBUSMETER_H
#include <Arduino.h>

class ModbusMeter
{
private:
    bool checkRecivingdataOnModbus();
    float dataformatter(byte data1, byte data2, byte data3, byte data4);
    void preTransmission() ;
void postTransmission();
public:
    
    ModbusMeter(void);
    ~ModbusMeter(void);
    void ModbusUartinitilize();
    bool ModbusLOOP();
    String getdataSendToServer();
    float GetVoltagePhase(int Connector);
    float GetCurrentPhase(int Connector);
    float GetPowerPhase(int Connector);
    float GetEnergyPhase(int Connector);
    float TotalkwhImport();
    float TotalkwhExport();
    float TotalPower();
    float GetPowerfectorPhase(int Connector);
    float GetReactivePowerPhase(int Connector);
  
    
};

// dataReceiveModbus::dataReceiveModbus(/* args */)
// {
// }

// dataReceiveModbus::~dataReceiveModbus()
// {
// }
#endif