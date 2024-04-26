// Expose Espressif SDK functionality
/*
extern "C" {
#include "user_interface.h"
  typedef void (*freedom_outside_cb_t)(uint8 status);
  int  wifi_register_send_pkt_freedom_cb(freedom_outside_cb_t cb);
  void wifi_unregister_send_pkt_freedom_cb(void);
  int  wifi_send_pkt_freedom(uint8 *buf, int len, bool sys_seq);
}
*/

// WiFi VARIABLEs
#define My_WIFI_POWER WIFI_POWER_15dBm
#define WL_RADIO_OFF 8
static const String WIFI_state_Name[] = {
	"WL_IDLE_STATUS",			// 0
    "WL_NO_SSID_AVAIL",			// 1
    "WL_SCAN_COMPLETED",		// 2
    "WL_CONNECTED",				// 3
    "WL_CONNECT_FAILED",		// 4
    "WL_CONNECTION_LOST",		// 5
    "WL_WRONG_PASSWORD",        // 6
    "WL_DISCONNECTED",			// 7
    "WL_RADIO_OFF",             // 8
};

static const String WIFI_PHY_Mode_Name[] = {
	"RADIO_OFF",			    // 0
    "802.11b",			        // 1
    "802.11g",		            // 2
    "802.11n",				    // 3
    "802.11",				    // 4
};
int WIFI_state = WL_DISCONNECTED;
int Last_WIFI_state = WL_NO_SHIELD;
unsigned int WIFI_Retry = 35;                  // Timer to retry the WiFi connection
unsigned long WIFI_LastTime = 0;                // Last WiFi connection attempt time stamp
int WIFI_errors = 0;                            // WiFi errors Counter


// Wi-Fi functions

String WIFI_state_string(int wifistate = WIFI_state) {
    return WIFI_state_Name[wifistate];
}

void wifi_disconnect() {
    esp_wifi_disconnect();
    WIFI_state = WL_RADIO_OFF;
}

void wifi_connect() {
    // Radio OFF?
    if(WIFI_state != WL_RADIO_OFF) {
        //  Connect to Local wireless network or start as Access Point
        if ( WiFi.status() != WL_CONNECTED) {
            if (config.APMode) {
                if (config.STAMode) WiFi.mode(WIFI_AP_STA);     // Setup ESP in AP+Station  mode
                else WiFi.mode(WIFI_AP);                        // Setup ESP in AP only mode
            }
            else {
                #ifndef Modem_WEB_TELNET
                    config.STAMode = true;                      // To force having any conectivity to the world
                #endif
                if (config.STAMode) WiFi.mode(WIFI_STA);        // Setup ESP in Station only mode
            }

            if (config.STAMode) {
                // Handle DHCP, IP address and hostname for the shield
                if (  !config.DHCP || ( RTC_read() && (ESPWakeUpReason() == "Deep-Sleep Wake") )  ) {
                //if (!config.DHCP) {
                    WiFi.persistent(true);                   // required for fast WiFi registration
                    // Static IP (No DHCP) may be handy for fast WiFi registration
                    IPAddress StaticIP(config.IP[0], config.IP[1], config.IP[2], config.IP[3]);
                    IPAddress Gateway(config.Gateway[0], config.Gateway[1], config.Gateway[2], config.Gateway[3]);
                    IPAddress Subnet(config.Netmask[0], config.Netmask[1], config.Netmask[2], config.Netmask[3]);
                    IPAddress DNS(config.DNS_IP[0], config.DNS_IP[1], config.DNS_IP[2], config.DNS_IP[3]);
                    WiFi.config(StaticIP, Gateway, Subnet, DNS);
                };
                wifi_hostname();
                if( RTC_read() && (ESPWakeUpReason() == "Deep-Sleep Wake") ) {
                    // The RTC data was good, make a quick connection
                    if (config.DEBUG) Serial.print("Waking from DeepSleep and connecting to WiFi using RTD data and Static IP... ");
                    //#ifdef ESP32C3
                    //    WiFi.setTxPower(My_WIFI_POWER);
                    //#endif
                    WiFi.begin( config.SSID, config.WiFiKey, rtcData.LastWiFiChannel, rtcData.bssid, true );
                    WIFI_state = wifi_waitForConnectResult(2000);
                    if ( WIFI_state != WL_CONNECTED ) {
                        if (config.DEBUG) Serial.println(" ---ERROR!?!. Trying using config values. ");
                        if (config.DHCP) WiFi.config((uint32_t)0x0, (uint32_t)0x0, (uint32_t)0x0, (uint32_t)0x0);
                        WiFi.begin(config.SSID, config.WiFiKey);
                        WIFI_state = wifi_waitForConnectResult(10000);
                    };
                }
                else {
                    // The RTC data was not valid, so make a regular connection
                    if (config.DEBUG) Serial.print("NO RTD data or NOT waking from DeepSleep. Using configured WiFi values ... ");
                    //#ifdef ESP32C3
                    //    WiFi.setTxPower(My_WIFI_POWER);
                    //#endif
                    WiFi.begin(config.SSID, config.WiFiKey);
                    WIFI_state = wifi_waitForConnectResult(10000);
                }
                if ( WIFI_state == WL_CONNECTED ) {
                    if (config.DEBUG) { Serial.print("Connected to WiFi network! " + String(config.SSID) + " IP: "); Serial.println(WiFi.localIP());}
                    // state_update();      // relying on MQTT_connect() to execute this functions.

                    //if (!MDNS.begin(host_name)) {             // Start the mDNS responder for "host_name.local" domain
                    //    Serial.println("Error setting up MDNS responder!");
                    //}
                    //else Serial.println("mDNS responder started");
                }
                else if (config.DEBUG) Serial.println( "WiFI ERROR! ==> " + WIFI_state_string(WIFI_state) );
            }
            if (config.APMode) {
                //#ifdef ESP32C3
                //    WiFi.setTxPower(My_WIFI_POWER);
                //#endif
                WiFi.softAP(ESP_SSID.c_str());
                //WiFi.softAP(config.SSID);
                if (config.DEBUG) { Serial.print("WiFi in AP mode, with IP: "); Serial.println(WiFi.softAPIP());}
            }
        }
        else WIFI_state = WL_CONNECTED;
    }
}


void wifi_setup() {
    if (!config.APMode && !config.STAMode) {
        WIFI_state = WL_RADIO_OFF;
        wifi_disconnect();
    }
    else {
        WIFI_state = WL_DISCONNECTED;
        wifi_connect();
    }
}

void wifi_loop() {
    if(WIFI_state != WL_RADIO_OFF) {
        if ( WiFi.status() != WL_CONNECTED ) {
            if ( millis() - WIFI_LastTime > (WIFI_Retry * 1000)) {
                WIFI_errors ++;
                if (config.DEBUG) { Serial.println( "in loop function WiFI ERROR! #: " + String(WIFI_errors) + "  ==> " + WIFI_state_string(WiFi.status())); }
                WIFI_LastTime = millis();
                wifi_connect();
            }
        }
        else {
            if (WIFI_state != WL_CONNECTED) WIFI_state = WL_CONNECTED;
        }
    }
    yield();
}
