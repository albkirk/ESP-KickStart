/*
## Store any struct in flash ##
 Kudos for this author: Robert Duality4Y [GitHub]
 https://github.com/esp8266/Arduino/issues/1539
*/
#include <EEPROM.h>
#define EEPROMZize 2048                     // Lowered 4K to 2KB as it was failing on FireBeetle 
#define Mem_Start_Pos 512                   // Memory starting point Position for the space to write/read data

#include <ArduinoJson.h>

bool Load_Config = false;                   // Will it Load Default values? It means it's running for first time.  

#ifndef custo_strDateTime
struct strDateTime                          // Date & Time Struture for usage on NTP, and Alarm Clock
{
  byte hour;
  byte minute;
  byte second;
  int year;
  byte month;
  byte day;
  byte wday;
  bool alarm;
  byte sound;
};
#endif



//
//  AUXILIAR functions to handle EEPROM
//
void EEPROMWritelong(int address, long value) {
  byte four = (value & 0xFF);
  byte three = ((value >> 8) & 0xFF);
  byte two = ((value >> 16) & 0xFF);
  byte one = ((value >> 24) & 0xFF);

  //Write the 4 bytes into the eeprom memory.
  EEPROM.write(address, four);
  EEPROM.write(address + 1, three);
  EEPROM.write(address + 2, two);
  EEPROM.write(address + 3, one);
}

long EEPROMReadlong(long address) {
  //Read the 4 bytes from the eeprom memory.
  long four = EEPROM.read(address);
  long three = EEPROM.read(address + 1);
  long two = EEPROM.read(address + 2);
  long one = EEPROM.read(address + 3);

  //Return the recomposed long by using bitshift.
  return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
}

void loadStruct(void *data_dest, size_t size, int start_address) {
    for(size_t i = 0; i < size; i++) {
        char data = EEPROM.read(i + start_address);
        ((char *)data_dest)[i] = data;
    }
}

void storeStruct(void *data_source, size_t size, int start_address) {
    for(size_t i = 0; i < size; i++) {
        char data = ((char *)data_source)[i];
        EEPROM.write(i + start_address, data);
    }
    EEPROM.commit();
}


//
//  CONFIG Struct
//
struct __attribute__((__packed__)) strConfig {
  char DeviceName[16];            // Device Name
  char Location[16];              // Device Location
  char ClientID[8];               // Client ID (used on MQTT)
  byte ONTime;                    // 0-255 seconds (Byte range)
  byte SLEEPTime;                 // 0-255 minutes (Byte range)
  bool DEEPSLEEP;                 // 0 - Disabled, 1 - Enabled
  bool LED;                       // 0 - OFF, 1 - ON
  bool TELNET;                    // 0 - Disabled, 1 - Enabled
  bool OTA;                       // 0 - Disabled, 1 - Enabled
  bool WEB;                       // 0 - Disabled, 1 - Enabled
  bool Remote_Allow;              // 0 - Not Allow, 1 - Allow remote operation  
  bool STAMode;                   // 0 - AP or AP+STA Mode, 1 - Station only Mode
  bool APMode;                    // 0 - AP Mode Disabled, 1 - AP Mode Enabled
  char SSID[32];                  // Wireless LAN SSID (STA mode)
  char WiFiKey[32];               // Wireless LAN Key (STA mode)
  bool DHCP;                      // 0 - Static IP, 1 - DHCP
  byte IP[4];
  byte Netmask[4];
  byte Gateway[4];
  byte DNS_IP[4];
  char NTPServerName[128];
  long TimeZone;
  unsigned long Update_Time_Via_NTP_Every;
  bool isDayLightSaving;
  char MQTT_Server[128];
  long MQTT_Port;
  bool MQTT_Secure;
  char MQTT_User[32];
  char MQTT_Password[32];
  char UPDATE_Server[128];
  long UPDATE_Port;
  char UPDATE_User[16];
  char UPDATE_Password[32];
  char SIMCardPIN[5];
  char APN[128];
  char MODEM_User[32];
  char MODEM_Password[32];
  char WEB_User[16];
  char WEB_Password[32];
  float Temp_Corr;
  float LDO_Corr;
  bool HW_Module;
  bool HASSIO_CFG;
  bool DEBUG;
  bool SW_Upgraded;
  bool SWITCH_Default;
  unsigned int UPPER_LEVEL;
  unsigned int LOWER_LEVEL;
  long MIN_TRAVEL;
  long MAX_TRAVEL;
  double Voltage_Multiplier;
  double Current_Multiplier;
  double Power_Multiplier;
  char InitColor[8];
  byte Volume;
  bool Alarm_State;
  strDateTime AlarmDateTime;  
} config;


#include <def_conf.h>


//
//  STORAGE functions
//

boolean storage_read() {
    //if (config.DEBUG) Serial.println("Reading Configuration");
    if (EEPROM.read(0) == 'C' && EEPROM.read(1) == 'F'  && EEPROM.read(2) == 'G' && EEPROMReadlong(3) > 2 && EEPROMReadlong(3) == sizeof(config)) {
        //if (config.DEBUG) Serial.println("Configurarion Found!");
        loadStruct(&config, EEPROMReadlong(3), 15);     // I just decided that it will read/write after address 15
        return true;   // "false" means no error
    }
    else {
        if (config.DEBUG) Serial.println("Configurarion NOT FOUND!!!!");
        //Serial.println("Value of 0,1,2: " + String(EEPROM.read(0)) + String(EEPROM.read(1)) + String(EEPROM.read(2)));
        //Serial.println("Value of 3: " + String(EEPROMReadlong(3)));
        return false;      // failed to read configuration
    }
}

void storage_write() {
  if (config.DEBUG) Serial.println("Writing Config");
  EEPROM.write(0, 'C');
  EEPROM.write(1, 'F');
  EEPROM.write(2, 'G');
  EEPROMWritelong(3, sizeof(config));
  //Serial.println("Value of 3 WRITE: " + String(sizeof(config)));

  storeStruct(&config, sizeof(config), 15);             // I just decided that it will read/write after address 15
  //Serial.println("Value of 0,1,2 READ: " + String(EEPROM.read(0)) + String(EEPROM.read(1)) + String(EEPROM.read(2)));
  //Serial.println("Value of 3 READ: " + String(EEPROMReadlong(3)));
}

void storage_reset() {
  if (config.DEBUG) Serial.println("Reseting Config");
  EEPROM.write(0, 'R');
  EEPROM.write(1, 'S');
  EEPROM.write(2, 'T');
  for (size_t i = 3; i < (EEPROMZize-1); i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
}

void storage_setup() {
    EEPROM.begin(EEPROMZize);         // define an EEPROM space of 2048 Bytes to store data
    //storage_reset();                // Hack to reset storage during boot
    config_defaults();                // This initializes the config structure
    Load_Config = storage_read();     // When a configuration exists, it uses stored values
    if (!Load_Config) {               // If NOT, it load DEFAULT VALUES to "config" struct
        config_defaults();
        storage_write();
    }
}
