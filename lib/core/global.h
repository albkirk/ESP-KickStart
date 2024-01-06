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

void global_setup() {
    SLEEPTime = config.SLEEPTime;          // Variable to allow temporary change the sleeptime (ex.: = 0)
}
