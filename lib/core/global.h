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
float Batt_Level = 0;
float Batt_Last  = 0;
bool CPU_High = false;                      // System flag indicating being busy executing any CPU intensive task, such capture VIDEO


String trimword( String str) {
    str.toLowerCase();
    str.replace(" ", "_");
    str.replace("-", "_");
    return str;
}

void storage_print() {
    telnet_println("Config Size: [" + String(sizeof(config)) + " bytes]");
    if (sizeof(config) + 16 > (EEPROMZize - Mem_Start_Pos)) Serial.println ("-- WARNING: Memory zones overlapinng!! --");
    telnet_println("Device Name: " + String(config.DeviceName) + "  -  Location: " + String(config.Location));
    telnet_println("ON time[sec]: " + String(config.ONTime) + "  -  SLEEP Time[min]: " + String(config.SLEEPTime) + " -  DEEPSLEEP enabled: " + String(config.DEEPSLEEP));
    telnet_println("LED enabled: " + String(config.LED) + "   -  TELNET enabled: " + String(config.TELNET) + "  -  OTA enabled: " + String(config.OTA) + "  -  WEB enabled: " + String(config.WEB));
    telnet_println("WiFi AP Mode: " + String(config.APMode) + "  -  WiFi STA Mode: " + String(config.STAMode) + "   -  WiFi SSID: " + String(config.SSID) + "  -  WiFi Key: " + String(config.WiFiKey));
  
    telnet_println("DHCP enabled: " + String(config.DHCP));
    if(!config.DHCP) {
      telnet_print("IP: " + String(config.IP[0]) + "." + String(config.IP[1]) + "." + String(config.IP[2]) + "." + String(config.IP[3]) + "  -  ");
      telnet_print("Mask: "  + String(config.Netmask[0]) + "." + String(config.Netmask[1]) + "." + String(config.Netmask[2]) + "." + String(config.Netmask[3]) + "  -  ");
      telnet_print("Gateway: " + String(config.Gateway[0]) + "." + String(config.Gateway[1]) + "." + String(config.Gateway[2]) + "." + String(config.Gateway[3]) + "  -  ");
      telnet_println("DNS IP: " + String(config.DNS_IP[0]) + "." + String(config.DNS_IP[1]) + "." + String(config.DNS_IP[2]) + "." + String(config.DNS_IP[3]));
    }
    telnet_println("MODEM APN: " + String(config.APN) + "  -  User: " + String(config.MODEM_User) + "  -  Pass: " + String(config.MODEM_Password) + "  -  PIN: " + String(config.SIMCardPIN));
    telnet_print("MQTT Server: " + String(config.MQTT_Server) + "  -  Port: " + String(config.MQTT_Port) + "  -  Secure: " + String(config.MQTT_Secure) + "  -  ");
    telnet_println("MQTT User: " + String(config.MQTT_User) + "  -  MQTT Pass: " + String(config.MQTT_Password));
    telnet_print("NTP Server Name: " + String(config.NTPServerName) + "  -  ");
    telnet_print("NTP update every " + String(config.Update_Time_Via_NTP_Every) + " minutes  -  ");
    telnet_println("Timezone: " + String(config.TimeZone) + "  -  DayLight: " + String(config.isDayLightSaving));

    telnet_println("Debug: " + String(config.DEBUG) + "  -  HW Module: " + String(config.HW_Module) + "  -  Remote Allowed: " + String(config.Remote_Allow) + "  -  WEB User: " + String(config.WEB_User) + "  -  WEB Pass: " + String(config.WEB_Password));
    telnet_println("SWITCH default: " + String(config.SWITCH_Default) + "  -  Temperature Correction: " + String(config.Temp_Corr) + "  -  Voltage Correction: " + String(config.LDO_Corr));
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

void deepsleep_procedure(unsigned long deepsleeptime = SLEEPTime) {
    mqtt_publish(mqtt_pathtele, "Status", "DeepSleep");
    mqtt_disconnect();
    telnet_println("Going to sleep for " + String(deepsleeptime) + " seconds, or until next event ... zzZz :) ");
    //delay(100);
    telnet_println("Total time ON: " + String(millis()) + " msec");
    GoingToSleep(deepsleeptime, curUTCTime());
}

void deepsleep_loop() {
    if (config.DEEPSLEEP && millis() > ONTime_Offset + (ulong(config.ONTime) + Extend_time)*1000) {
        deepsleep_procedure();
    }
}

float getBattLevel() {                                      // return Battery level in Percentage [0 - 100%]
#ifdef IP5306
    float tempval;
    float value = -2;
    for(int i = 0; i < Number_of_measures; i++) {
        tempval = float(getBatteryLevel());
        if (tempval > value) value = tempval;
        delay(10);
    }
    return value;
#else
    if (Batt_ADC_PIN<0) return -1;
    else {
        float voltage = 0.0;                                    // Input Voltage [v]
    
        for(int i = 0; i < Number_of_measures; i++) {
            if(Res_Div) voltage += ReadVoltage() * 2;
            else voltage += ReadVoltage();
            delay(1);
        }
        voltage = voltage / Number_of_measures + config.LDO_Corr; 
        if (config.DEBUG) telnet_println("Averaged and Corrected Voltage: " + String(voltage));
        /*
        if (voltage > Batt_Max ) {
            if (config.DEBUG) Serial.println("Voltage will be truncated to Batt_Max: " + String(Batt_Max));
            voltage = Batt_Max;
        }
        */
        return ((voltage - Batt_Min) / (Batt_Max - Batt_Min)) * 100.0;
    }
#endif
}

void Batt_OK_check() {                      // If LOW Batt, it will DeepSleep forever!
    Batt_Level = getBattLevel();            // Check Battery Level
    if (Batt_Level < Batt_L_Thrs && Batt_Level >= 0) {
        mqtt_publish(mqtt_pathtele, "Status", "LOW Battery");
        mqtt_publish(mqtt_pathtele, "Battery", String(Batt_Level,0));
        mqtt_disconnect();
        telnet_println("Going to sleep forever. Please, recharge the battery ! ! ! ");
        //#ifdef IP5306
        //    setPowerBoostKeepOn(false);
        //#endif
        flash_LED(10);
        delay(100);
        GoingToSleep(0, curUTCTime());     // Sleep forever
        return;                             // Actually, it will never return !!
    }
    else return;                            // Batt OK, returning null
}

void status_report() {
    if (BattPowered) {
        Batt_OK_check();                    // Update Battery Level and sleeps If LOW Batt.
        if (Batt_Level >100) {
            mqtt_publish(mqtt_pathtele, "Status", "Charging");
            mqtt_publish(mqtt_pathtele, "Battery", "100");
        }
        else {
            mqtt_publish(mqtt_pathtele, "Status", "Battery");
            if (Batt_Level >=0) mqtt_publish(mqtt_pathtele, "Battery", String(Batt_Level,0));
        };
    }
    else mqtt_publish(mqtt_pathtele, "Status", "Mains");
}

void global_restart(String message = "Restarting") {
    mqtt_publish(mqtt_pathtele, "Status", message);
    mqtt_disconnect();
    if (config.TELNET) telnet_stop();
    ESPRestart();
}

void global_reset() {
    mqtt_publish(mqtt_pathtele, "Status", "Reseting");
    mqtt_disconnect();
    if (config.TELNET) telnet_stop();
    storage_reset();
    RTC_reset();
    ESPRestart();    
}

void global_reset_button(){
    int Counter = 0;
    if (Reset_Btn>=0) {
        if (!digitalRead(Reset_Btn)) {
            for (size_t i = 0; i < 10; i++) {        
                delay(1000);
                if (!digitalRead(Reset_Btn)) Counter ++; // Button must be pressed during the 10 sec!
            }
            if (Counter ==10) global_reset();
        }
    }
}

void global_setup() {
    hw_setup();
    if (config.DEBUG) storage_print();
    
  // Output GPIOs
    if (LED_ESP>=0) {
        pinMode(LED_ESP, OUTPUT);
        digitalWrite(LED_ESP, LOW);                     // initialize LED off
    }
    if (BUZZER>=0) {
        pinMode(BUZZER, OUTPUT);
        digitalWrite(BUZZER, LOW);                      // initialize BUZZER off
    }

  // Input GPIOs
    if (Reset_Btn>=0) {
        pinMode(Reset_Btn, INPUT_PULLUP);
    }

    SLEEPTime = config.SLEEPTime * 60UL;          // Variable [in seconds] to allow temporary change the sleeptime (ex.: = 0)
}

void global_loop() {
    global_reset_button();
}
