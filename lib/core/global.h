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

void deepsleep_loop() {
    if (config.DEEPSLEEP && millis() > ONTime_Offset + (ulong(config.ONTime) + Extend_time)*1000) {
        mqtt_publish(mqtt_pathtele, "Status", "DeepSleep");
        mqtt_disconnect();
        telnet_println("Going to sleep for " + String(SLEEPTime) + " min, or until next event ... zzZz :) ");
        //delay(100);
        telnet_println("Total time ON: " + String(millis()) + " msec");
        GoingToSleep(SLEEPTime, curUTCTime());
    }
}

float Batt_OK_check() {                     // If LOW Batt, it will DeepSleep forever!
    float Batt_Level = getBattLevel();      // Check Battery Level
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
        GoingToSleep(0, curUnixTime());   // Sleep forever
        return Batt_Level;                // Actually, it will never return !!
    }
    return Batt_Level;
}

void status_report() {
    if (BattPowered) {
        float Battery = Batt_OK_check();
        if (Battery >100) mqtt_publish(mqtt_pathtele, "Status", "Charging");
        else {
            mqtt_publish(mqtt_pathtele, "Status", "Battery");
            if (Battery >=0) mqtt_publish(mqtt_pathtele, "Battery", String(Battery,0));
        }
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

    SLEEPTime = config.SLEEPTime;          // Variable to allow temporary change the sleeptime (ex.: = 0)
}

void global_loop() {
    global_reset_button();
}