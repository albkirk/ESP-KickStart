#include <ESP8266WiFi.h>
#ifndef ESP8285
    #include <ESP8266WebServer.h>
    #include <WiFiSec.h>
    #include <secure_credentials.h>
    #include <Wire.h>
#endif



//System Parameters
#define ChipID HEXtoUpperString(ESP.getChipId(), 6)
#define ESP_SSID String("ESP-" + ChipID)               // SSID used as Acces Point
#define Number_of_measures 5                           // Number of value samples (measurements) to calculate average
byte SLEEPTime = config.SLEEPTime;          // Variable to allow temporary change the sleeptime (ex.: = 0)
bool Celular_Connected = false;                          // Modem Connection state


// The ESP8266 RTC memory is arranged into blocks of 4 bytes. The access methods read and write 4 bytes at a time,
// so the RTC data structure should be padded to a 4-byte multiple.
struct __attribute__((__packed__, aligned(4))) struct_RTC {
  uint32_t crc32 = 0U;                      // 4 bytes   
  unsigned long lastUTCTime = 0UL;          // 4 bytes
  uint8_t bssid[6];                        // 32 bytes
  uint8_t LastWiFiChannel = 0;              // 1 byte,   1 in total
  //uint8_t padding[3];                       // 2 bytes,  4 in total
  uint8_t ByteValue = 0;                    // 1 byte,   2 in total
  //uint8_t padding1[3];                      // 2 bytes,  4 in total
  float FloatValue = 0.0f;                  // 4 bytes
} rtcData;

static const String flash_size_map_Name[] = {
    "512KB (MAP: 256/256)",                    /**<  Flash size : 4Mbits. Map : 256KBytes + 256KBytes */
    "256KB (MAP: 256/ - )",                    /**<  Flash size : 2Mbits. Map : 256KBytes */
    "1MB (MAP: 512/512)",                    /**<  Flash size : 8Mbits. Map : 512KBytes + 512KBytes */
    "2MB (MAP: 512/512)",                   /**<  Flash size : 16Mbits. Map : 512KBytes + 512KBytes */
    "4MB (MAP: 512/512)",                   /**<  Flash size : 32Mbits. Map : 512KBytes + 512KBytes */
    "2MB (MAP: 1024/1024)",                 /**<  Flash size : 16Mbits. Map : 1024KBytes + 1024KBytes */
    "4MB (MAP: 1024/1024)",                 /**<  Flash size : 32Mbits. Map : 1024KBytes + 1024KBytes */
    "4MB (MAP: 2048/2048)",                 /**<  attention: don't support now ,just compatible for nodemcu;
                                                  Flash size : 32Mbits. Map : 2048KBytes + 2048KBytes */
    "8MB (MAP: 1024/1024)",                 /**<  Flash size : 64Mbits. Map : 1024KBytes + 1024KBytes */
    "16MB (MAP: 1024/1024)"                 /**<  Flash size : 128Mbits. Map : 1024KBytes + 1024KBytes */
};


// ADC to internal voltage
#if Using_ADC == false
    ADC_MODE(ADC_VCC)                       // Get voltage from Internal ADC
#endif


#ifndef ESP8285
// Initialize the Webserver
ESP8266WebServer MyWebServer(80);  
#endif

#ifndef ESP8285
// initialize WiFi Client
    // initialize WiFi Security
    WiFiSec wifisec(CA_CERT_PROG, CLIENT_CERT_PROG, CLIENT_KEY_PROG);
    WiFiClient secureclient = wifisec.getWiFiClient();    // Use this for secure connection
#endif
WiFiClient unsecuclient;                    // Use this for unsecure connection

// Battery & ESP Voltage
#define Batt_Max float(4.1)                 // Battery Highest voltage.  [v]
#define Batt_Min float(2.8)                 // Battery lowest voltage.   [v]
#define Vcc float(3.3)                      // Theoretical/Typical ESP voltage. [v]
#define VADC_MAX float(1.0)                 // Maximum ADC Voltage input

// Timers for millis used on Sleeping and LED flash
unsigned long ONTime_Offset=0;              // [msec]
unsigned long Extend_time=0;                // [sec]
unsigned long now_millis=0;
unsigned long Pace_millis=3000;
unsigned long LED_millis=300;               // 10 slots available (3000 / 300)
unsigned long BUZZER_millis=100;            // Buzz time (120ms Sound + 120ms  Silent)


// Standard Actuators STATUS
float CALIBRATE = 0;                        // float
float CALIBRATE_Last = 0;                   // float
unsigned int LEVEL = 0;                     // [0-100]
unsigned int LEVEL_Last = 0;                // [0-100]
int POSITION = 0;                           // [-100,+100]
int POSITION_Last = 0;                      // [-100,+100]
bool SWITCH = false;                        // [OFF / ON]
bool SWITCH_Last = false;                   // [OFF / ON]
unsigned long TIMER = 0;                    // [0-7200]  Minutes                 
unsigned long TIMER_Last = 0;               // [0-7200]  Minutes                 
static long TIMER_Current = 0;
unsigned long COUNTER = 0;


// Functions //
String HEXtoUpperString(uint32_t hexval, uint hexlen) {
    String strgval = String(hexval, HEX);
    String PADZero;
    for (uint i = 0; i < (hexlen - strgval.length()) ; i++) PADZero +="0";
    strgval = PADZero + strgval;
    char buffer[hexlen+1];
    strcpy(buffer, strgval.c_str());
    for (uint i = 0; i < strgval.length() ; i++) {
        if (char(buffer[i]) >= 97 ) buffer[i] = (char)(char(buffer[i] - 32));
    }
    return String(buffer);
}

String CharArray_to_StringHEX(const char *CharArray_value, uint CharArray_length) {
    String strgHEX = "";
    for (size_t i = 0; i < CharArray_length; i++)
    {
        strgHEX +=  String(CharArray_value[i], HEX);
    }
    return strgHEX;
}
    
uint32_t calculateCRC32( const uint8_t *data, size_t length ) {
  uint32_t crc = 0xffffffff;
  while( length-- ) {
    uint8_t c = *data++;
    for( uint32_t i = 0x80; i > 0; i >>= 1 ) {
      bool bit = crc & 0x80000000;
      if( c & i ) {
        bit = !bit;
      }

      crc <<= 1;
      if( bit ) {
        crc ^= 0x04c11db7;
      }
    }
  }

  return crc;
}

void keep_IP_address() {
if (config.DHCP) {
  if (  WiFi.status() == WL_CONNECTED && IPAddress(config.IP[0], config.IP[1], config.IP[2], config.IP[3]) != WiFi.localIP() ) {
    if (config.DEBUG) {Serial.print("OLD: "); Serial.print(IPAddress(config.IP[0], config.IP[1], config.IP[2], config.IP[3])); Serial.print("\t NEW: "); Serial.println(WiFi.localIP());}
    config.IP[0] = WiFi.localIP()[0]; config.IP[1] = WiFi.localIP()[1]; config.IP[2] = WiFi.localIP()[2]; config.IP[3] = WiFi.localIP()[3];
    config.Netmask[0] = WiFi.subnetMask()[0]; config.Netmask[1] = WiFi.subnetMask()[1]; config.Netmask[2] = WiFi.subnetMask()[2]; config.Netmask[3] = WiFi.subnetMask()[3];
    config.Gateway[0] = WiFi.gatewayIP()[0]; config.Gateway[1] = WiFi.gatewayIP()[1]; config.Gateway[2] = WiFi.gatewayIP()[2]; config.Gateway[3] = WiFi.gatewayIP()[3];
    config.DNS_IP[0] = WiFi.dnsIP()[0]; config.DNS_IP[1] = WiFi.dnsIP()[1]; config.DNS_IP[2] = WiFi.dnsIP()[2]; config.DNS_IP[3] = WiFi.dnsIP()[3];
    if (config.DEBUG) Serial.println("Storing new Static IP address for quick boot");
    storage_write();
    }
}
}

void myconfigTime(const char* tz, const char* server1, const char* server2, const char* server3) {
    configTime(tz, server1, server2, server3);
}

void esp_wifi_disconnect() {
    WiFi.mode(WIFI_OFF);
}

void wifi_hostname() {
    String host_name = String(config.DeviceName + String("-") + config.Location);
    WiFi.hostname(host_name.c_str());
}

uint8_t wifi_waitForConnectResult(unsigned long timeout) {
    return WiFi.waitForConnectResult(timeout);
}

// Read RTC memory (where the Wifi data is stored)
bool RTC_read() {
    if (ESP.rtcUserMemoryRead(0, (uint32_t*) &rtcData, sizeof(rtcData))) {
        // Calculate the CRC of what we just read from RTC memory, but skip the first 4 bytes as that's the checksum itself.
        uint32_t crc = calculateCRC32( ((uint8_t*)&rtcData) + 4, sizeof( rtcData ) - 4 );
        if( crc == rtcData.crc32 ) {
            if (config.DEBUG) {
                Serial.print("Read  crc: " + String(rtcData.crc32) + "  -  ");
                Serial.print("Read  Last Date: " + String(rtcData.lastUTCTime) + "  -  ");
                Serial.print("Read  BSSID: " + CharArray_to_StringHEX((const char*)rtcData.bssid, sizeof(rtcData.bssid)) + "  -  ");
                Serial.print("Read  LastWiFiChannel: " + String(rtcData.LastWiFiChannel) + "  -  ");
                Serial.print("Read  Byte Value: " + String(rtcData.ByteValue) + "  -  ");
                Serial.println("Read  Float Value: " + String(rtcData.FloatValue));
            }
            return true;
        }
    }
    return false;
}

bool RTC_write() {
// Update rtcData structure
    if ( WiFi.status() == WL_CONNECTED ) rtcData.LastWiFiChannel = WiFi.channel();
    memcpy( rtcData.bssid, WiFi.BSSID(), 6 ); // Copy 6 bytes of BSSID (AP's MAC address)
    rtcData.crc32 = calculateCRC32( ((uint8_t*)&rtcData) + 4, sizeof( rtcData ) - 4 );
    //rtcData.lastUTCTime = curUnixTime();
    if (config.DEBUG) {
        Serial.print("Write crc: " + String(rtcData.crc32) + "  -  ");
        Serial.print("Write Last Date: " + String(rtcData.lastUTCTime) + "  -  ");
        Serial.print("Write BSSID: " + CharArray_to_StringHEX((const char*)rtcData.bssid, sizeof(rtcData.bssid)) + "  -  ");
        Serial.print("Write LastWiFiChannel: " + String(rtcData.LastWiFiChannel) + "  -  ");
        Serial.print("Write Byte value: " + String(rtcData.ByteValue) + "  -  ");
        Serial.println("Write Float value: " + String(rtcData.FloatValue));
    }

// Write rtcData back to RTC memory
    if (ESP.rtcUserMemoryWrite(0, (uint32_t*) &rtcData, sizeof(rtcData))) return true;
    else return false;
}


bool RTC_reset() {
// Update rtcData structure
    rtcData.LastWiFiChannel = 0;
    memcpy( rtcData.bssid, "000000", 6 ); // Copy 6 bytes of BSSID (AP's MAC address)
    rtcData.crc32 = 0;
    rtcData.lastUTCTime = 0;

// Write rtcData back to RTC memory
    if (ESP.rtcUserMemoryWrite(0, (uint32_t*) &rtcData, sizeof(rtcData))) return true;
    else return false;
}

//  ESP8266
void GoingToSleep(byte Time_minutes = 0, unsigned long currUTime = 0 ) {
    rtcData.lastUTCTime = currUTime;
    keep_IP_address();
    RTC_write();
    ESP.deepSleep( Time_minutes * 60 * 1000000);            // time in minutes converted to microseconds
}


float getBattLevel() {                                      // return Battery level in Percentage [0 - 100%]
    float voltage = 0.0;                                    // Input Voltage [v]
    for(int i = 0; i < Number_of_measures; i++) {
        if (Using_ADC) {voltage += analogRead(A0) * Vcc;}
        else {voltage += ESP.getVcc();}         // only later, the (final) measurement will be divided by 1000
        delay(1);
    };
    voltage = voltage / Number_of_measures;
    voltage = voltage / 1000.0 + config.LDO_Corr;
    if (config.DEBUG) Serial.println(" Averaged and Corrected Voltage: " + String(voltage));
    if (voltage > Batt_Max ) {
        if (config.DEBUG) Serial.println("Voltage will be truncated to Batt_Max: " + String(Batt_Max));
        voltage = Batt_Max;
    }
    return ((voltage - Batt_Min) / (Batt_Max - Batt_Min)) * 100.0;
}

long getRSSI() {
    // return WiFi RSSI Strength signal [dBm]
    long r = 0;

    for(int i = 0; i < Number_of_measures; i++) {
        r += WiFi.RSSI();
    }
    r = r /Number_of_measures;
    return r;
}


void ESPRestart() {
    if (config.DEBUG) Serial.println("Restarting ESP...");
    delay(100);
    ESP.restart();
}

String ESPWakeUpReason() {    // WAKEUP_REASON
  return ESP.getResetReason();
}

String Flash_Size() {
    return flash_size_map_Name[system_get_flash_size_map()];
}

uint32_t CPU_Clock() {
    uint32_t cpu_clock = uint32_t(system_get_cpu_freq());
    // Serial.println("CPU Clock: " + String(cpu_clock) + " MHz");
    return cpu_clock;
}

void CPU_Boost(bool boost = true) {
    if(boost) system_update_cpu_freq(160);
    else system_update_cpu_freq(80);
}

void FormatConfig() {                                   // WARNING!! To be used only as last resource!!!
    Serial.println(ESP.eraseConfig());
    RTC_reset();
    delay(100);
    ESP.reset();
}

void blink_LED(unsigned int slot, int bl_LED = LED_ESP, bool LED_OFF = !config.LED) { // slot range 1 to 10 =>> 3000/300
    if (bl_LED>=0) {
        now_millis = millis() % Pace_millis;
        if (now_millis > LED_millis*(slot-1) && now_millis < LED_millis*slot-LED_millis/2) digitalWrite(bl_LED, !LED_OFF); // Turn LED on
        now_millis = (millis()-LED_millis/3) % Pace_millis;
        if (now_millis > LED_millis*(slot-1) && now_millis < LED_millis*slot-LED_millis/2) digitalWrite(bl_LED, LED_OFF); // Turn LED on
    }
}

void flash_LED(unsigned int n_flash = 1, int fl_LED = LED_ESP, bool LED_OFF = !config.LED) {
    if (fl_LED>=0) {
        for (size_t i = 0; i < n_flash; i++) {
            digitalWrite(fl_LED, !LED_OFF);             // Turn LED on
            delay(LED_millis/3);
            digitalWrite(fl_LED, LED_OFF);              // Turn LED off
            yield();
            delay(LED_millis);
        }
    }
}

void Buzz(unsigned int n_beeps = 1, unsigned long buzz_time = BUZZER_millis ) {
    if (BUZZER>=0) {
        for (size_t i = 0; i < n_beeps; i++) {
            digitalWrite(BUZZER, HIGH);                 // Turn Buzzer on
            delay(BUZZER_millis);
            digitalWrite(BUZZER, LOW);                  // Turn Buzzer off
            yield();
            delay(BUZZER_millis);
        }
    }
}


void hw_setup() {
  // Output GPIOs
      if (LED_ESP>=0) {
          pinMode(LED_ESP, OUTPUT);
          digitalWrite(LED_ESP, HIGH);                  // initialize LED off
      }
      if (BUZZER>=0) {
          pinMode(BUZZER, OUTPUT);
          digitalWrite(BUZZER, LOW);                    // initialize BUZZER off
      }

  // Input GPIOs
    if (Def_Config>=0) {
        pinMode(Def_Config, INPUT_PULLUP);
        if (!digitalRead(Def_Config)) {
            delay(5000);
            if (!digitalRead(Def_Config)) {
                flash_LED(5);
                storage_reset();
                RTC_reset();
                ESPRestart();
            }
        }
    }


      //RTC_read();                                      // Read the RTC memmory
}

void hw_loop() {
  // LED handling usefull if you need to identify the unit from many
      //if (LED_ESP>=0) digitalWrite(LED_ESP, boolean(!config.LED));  // Values is reversed due to Pull-UP configuration
      yield();
}
