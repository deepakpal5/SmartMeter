#ifndef ALLCOMMONPART_H_
#define ALLCOMMONPART_H_
#include <Arduino.h>
#include <string.h>
#include <HardwareSerial.h>
#include <Preferences.h>
#include <ArduinoJson.h>
#define EmergencyButtonPin 39
#define EarthfAULTpIN 36
#define GSMIndiPIN 5 
#define WIFIndiPIN 27
#define ContactorPIN 32
#define WifiConfigure 4
#define RS485Enable 26
#define GSMReset 27

extern Preferences preferences;
#define EmergencyPressed !digitalRead(EmergencyButtonPin)
#define EarthFault !digitalRead(EarthfAULTpIN)
#define DEBUG_OUT true
  
//================================_CommunicationConnectivity_=========================================
int InternetConnectivity();



//================================_GSMComm_=========================================
void setupGSMUart();
bool startCommunicationOnGSM(String apn);
bool dataSendToServerOverGSM(String urlPost,String data);
bool dataSendToServerOverWiFi(String UrlPost,String data);
String extractJSONBody(String response);



bool startPPPOS(String s1,String s2,String s3);
bool sendCommandWithAnswer(String cmd, String ans);
String sendCommandWithAnswer(String cmd);
void InititateGSMModem();
bool dataModeActivateonModule();

//=================================_WifiComm_========================================
bool connectToWifi(String ssid, String psk);
bool connectToWifidummy();
bool checkSTAConnected();
String getIPAddressForConfigWifi(int m);
IPAddress getIPFromStr(String IPSTRece);

void dataGetFromServer(String UrlGet);
String getdataSendToServer();
void websocketSetupApp();
void disableAppConfig();
void loopConfig();
//==================================_EthernetComm_=======================================

String IpconntoString(IPAddress ipAd);



//================================_SetupPage_=========================================
void setupAllConfig(u_int Br);
//===============================_EEPROMeadWriteOcpp_==========================================

void writeEnergyInMem(int conn, uint64_t value);
uint64_t readEnergyInMem(int connector);
void responseFromServer(String Response);
void setupForMemory();
void SaveDataInMemory(String data);
bool loadSendFirstData(bool Wifimode,String Url);
bool OfflineLoad();
bool OfflineSave();
String getfaultSatatus();
int getTransectionId();
int getSavedTxnId();
String getMacAddress();
#endif