// Adding function with project's customized MQTT actions
#include <customactions.h>

void state_update() {
            //mqtt_publish(mqtt_pathtele, "Boot", ESPWakeUpReason());
            //mqtt_publish(mqtt_pathtele, "Brand", BRANDName);
            //mqtt_publish(mqtt_pathtele, "Model", MODELName);
            //mqtt_publish(mqtt_pathtele, "ChipID", ChipID);
            //mqtt_publish(mqtt_pathtele, "SWVer", SWVer);
            //mqtt_publish(mqtt_pathtele, "IP", WiFi.localIP().toString());
            status_report();                            // should send Status MAIN or Battery
            if (LED_ESP>=0) mqtt_publish(mqtt_pathtele, "LED", String(config.LED));
            mqtt_publish(mqtt_pathtele, "RSSI", String(getRSSI()));
            custom_update();
            hassio_attributes();
}


// Handling of received message
void on_message(const char* topic, byte* payload, unsigned int msg_length) {

    telnet_println("New message received from Broker");

    char msg[msg_length + 1];
    strncpy (msg, (char*)payload, msg_length);
    msg[msg_length] = '\0';

    //telnet_println("Topic: " + String(topic));
    //telnet_println("Payload: " + String((char*)msg));

    // Check requested command
    String command = String(topic);
    command.replace(mqtt_pathsubs, "");
    String cmd_value = String((char*)msg);
    telnet_print("Requested Command: " + command, true);
    telnet_println(" Command Value: " + cmd_value, true);

    // System Configuration 
    if ( command == "DeviceName") {hassio_delete(); strcpy(config.DeviceName, cmd_value.c_str()); hassio_discovery(); state_update(); config_backup(); }
    if ( command == "Location" && cmd_value !="") {hassio_delete(); strcpy(config.Location, cmd_value.c_str()); hassio_discovery(); state_update(); config_backup(); mqtt_publish(mqtt_pathcomd, "Location", "", true); }
    if ( command == "ClientID") {hassio_delete(); strcpy(config.ClientID, cmd_value.c_str()); config_backup(); global_restart(); }
    if ( command == "DEEPSLEEP" && cmd_value !="") { config.DEEPSLEEP = bool(cmd_value.toInt()); storage_write(); mqtt_publish(mqtt_pathcomd, "DEEPSLEEP", "", true);}
    if ( command == "SLEEPTime" && cmd_value !="" && cmd_value.toInt() >= 0) { config.SLEEPTime = byte(cmd_value.toInt()); SLEEPTime = config.SLEEPTime; storage_write(); mqtt_publish(mqtt_pathcomd, "SLEEPTime", "", true); }
    if ( command == "ONTime") { config.ONTime = byte(cmd_value.toInt());storage_write(); }
    if ( command == "ExtendONTime") if (bool(cmd_value.toInt()) == true) Extend_time = 60;
    if ( command == "LED") {config.LED = bool(cmd_value.toInt()); mqtt_publish(mqtt_pathtele, "LED", String(config.LED));}
    if ( command == "TELNET" && cmd_value !="") { config.TELNET = bool(cmd_value.toInt()); storage_write(); mqtt_publish(mqtt_pathcomd, "TELNET", "", true); telnet_setup(); }
    if ( command == "OTA" && cmd_value !="") { config.OTA = bool(cmd_value.toInt()); storage_write(); mqtt_publish(mqtt_pathcomd, "OTA", "", true); global_restart(); }
    if ( command == "NTP") if (bool(cmd_value.toInt()) == true) { getNTPtime(); mqtt_publish(mqtt_pathtele, "DateTime", String(curDateTime()));}
#ifndef ESP8285
    if ( command == "WEB") { config.WEB = bool(cmd_value.toInt()); storage_write(); web_setup(); }
#endif
    if ( command == "DHCP") { config.DHCP = bool(cmd_value.toInt()); storage_write(); wifi_connect(); }
    if ( command == "STAMode") { config.STAMode = bool(cmd_value.toInt()); storage_write(); wifi_setup(); }
    if ( command == "APMode") { config.APMode = bool(cmd_value.toInt()); storage_write(); wifi_setup(); }
    if ( command == "SSID") strcpy(config.SSID, cmd_value.c_str());
    if ( command == "WiFiKey") strcpy(config.WiFiKey, cmd_value.c_str());
    if ( command == "NTPServerName") strcpy(config.NTPServerName, cmd_value.c_str());
    if ( command == "MQTT_Server") { strcpy(config.MQTT_Server, cmd_value.c_str()); storage_write(); }
    if ( command == "MQTT_Port") { config.MQTT_Port = (long)cmd_value.toInt();storage_write(); }
    if ( command == "MQTT_Secure") { config.MQTT_Secure = bool(cmd_value.toInt()); storage_write(); }
    if ( command == "MQTT_User") { strcpy(config.MQTT_User, cmd_value.c_str()); storage_write(); }
    if ( command == "MQTT_Password") { strcpy(config.MQTT_Password, cmd_value.c_str()); storage_write(); }   
    if ( command == "SIMCardPIN") { strcpy(config.SIMCardPIN, cmd_value.c_str()); storage_write(); }
    if ( command == "APN") { strcpy(config.APN, cmd_value.c_str()); storage_write(); }
    if ( command == "MODEM_User") { strcpy(config.MODEM_User, cmd_value.c_str()); storage_write(); }
    if ( command == "MODEM_Password") { strcpy(config.MODEM_Password, cmd_value.c_str()); storage_write(); }   
    if ( command == "Update_Time_Via_NTP_Every") config.Update_Time_Via_NTP_Every = (ulong)abs(atol(cmd_value.c_str()));
    if ( command == "TimeZone") config.TimeZone = (long)cmd_value.toInt();
    if ( command == "isDayLightSaving") config.isDayLightSaving = bool(cmd_value.toInt());
    if ( command == "DEBUG") { config.DEBUG = bool(cmd_value.toInt()); telnet_println("DEBUG = " + String(config.DEBUG)); storage_write(); }
    if ( command == "Store") if (bool(cmd_value.toInt()) == true) storage_write();
    if ( command == "Restart")  if (bool(cmd_value.toInt()) == true) {mqtt_publish(mqtt_pathcomd, "Restart", "", true); global_restart();}
    if ( command == "Reset") if (bool(cmd_value.toInt()) == true) {mqtt_publish(mqtt_pathcomd, "Reset", "", true); hassio_delete(); global_reset();}
    if ( command == "Format") if (bool(cmd_value.toInt()) == true) FormatConfig();
    if ( command == "Version") {mqtt_publish(mqtt_pathtele, "Version", String(SWVer)); telnet_println("Version: " + String(SWVer));}
    if ( command == "HASSIO") if (bool(cmd_value.toInt()) == true) {
            mqtt_publish(mqtt_pathcomd, "HASSIO", "", true);
            hassio_delete();
            hassio_discovery();
            delay(10);
            state_update();
        }
    if ( command == "HASSIODEL") if (bool(cmd_value.toInt()) == true) {
            mqtt_publish(mqtt_pathcomd, "HASSIODEL", "", true);
            hassio_delete();
        }
    if ( command == "Switch_Def") { 
            config.SWITCH_Default = bool(cmd_value.toInt());
            storage_write();
            mqtt_publish(mqtt_pathtele, "Switch", String(SWITCH));
       }
    if ( command == "Temp_Corr") { 
            config.Temp_Corr = cmd_value.toFloat();
            storage_write();
            mqtt_publish(mqtt_pathtele, "Temp_Corr", String(config.Temp_Corr));
       }
    if ( command == "LDO_Corr") { 
            config.LDO_Corr = cmd_value.toFloat();
            storage_write();
            mqtt_publish(mqtt_pathtele, "Battery", String(getBattLevel(),0));
       }
    if ( command == "HW_Module") {
            config.HW_Module = bool(cmd_value.toInt());
            storage_write();
        }

    // Standard Actuators/Actions 
    if ( command == "Level") LEVEL = (uint)abs(cmd_value.toInt());
    if ( command == "Position") POSITION = cmd_value.toInt();
    if ( command == "Switch") {
        if ( SWITCH_Last == bool(cmd_value.toInt()) ) mqtt_publish(mqtt_pathtele, "Switch", String(SWITCH));
        else SWITCH = bool(cmd_value.toInt());
    }
    if ( command == "Timer") TIMER = (ulong)abs(atol(cmd_value.c_str()));
    if ( command == "Counter") COUNTER = (ulong)abs(atol(cmd_value.c_str()));
    if ( command == "Calibrate") { CALIBRATE = cmd_value.toFloat(); }

    // System Information and configuration
    if ( command == "Info" && bool(cmd_value.toInt()) ) {
        //mqtt_publish(mqtt_pathtele, "DateTime", String(curDateTime()));
        //mqtt_publish(mqtt_pathtele, "NTP_Sync", String(NTP_Sync));
        if (BattPowered) { telnet_print("Power: BATT", true); telnet_print("  -  Level: " + String(getBattLevel(),0), true); }
        else { telnet_print("Power: MAINS", true); }
        //telnet_print("  -  SW Ver: " + String(SWVer), true);
        //if (WIFI_state == WL_CONNECTED) telnet_print("  -  IP: " + WiFi.localIP().toString(), true);
        telnet_print("  -  Uptime: " + String(millis()/1000), true);
        telnet_print("  -  Current Date/Time: " + curDateTime(), true);
        telnet_println("  -  NTP Sync: " + String(NTP_Sync), true);
        #ifdef ESP8266
            telnet_println("Flash: " + Flash_Size() + "  -  CPU Clock: " + String(CPU_Clock()) + " MHz  -  WiFi State: " + WIFI_state_Name[WIFI_state] + " - Phy Mode: " + WIFI_PHY_Mode_Name[WiFi.getPhyMode()] + "  -  MQTT State: " + MQTT_state_string(), true);
        #else
            telnet_println("Flash: " + Flash_Size() + "  -  CPU Clock: " + String(CPU_Clock()) + " MHz  -  WiFi State: " + WIFI_state_Name[WIFI_state] + "  -  MQTT State: " + MQTT_state_string(), true);
        #endif
        #ifdef Modem_WEB_TELNET
            telnet_println("Modem State: " + Modem_state_Name[Modem_state] + "\t REG State: " + RegStatus_string(modem.getRegistrationStatus()));
        #endif
        state_update();
    }
    if ( command == "Config" && bool(cmd_value.toInt()) ) {
        mqtt_publish(mqtt_pathtele, "OTA", String(config.OTA));
        mqtt_publish(mqtt_pathtele, "TELNET", String(config.TELNET));
        mqtt_publish(mqtt_pathtele, "WEB", String(config.WEB));
        mqtt_publish(mqtt_pathtele, "Interval", String(config.SLEEPTime));   
        mqtt_publish(mqtt_pathtele, "Temp_Corr", String(config.Temp_Corr));
        mqtt_publish(mqtt_pathtele, "LDO_Corr", String(config.LDO_Corr));
        config_backup();
        storage_print();
    }  
    if ( command == "CPU_Boost" ) { CPU_Boost(bool(cmd_value.toInt())); delay(10); telnet_println("CPU Clock: " + String(CPU_Clock()) + " MHz"); }
    if ( command == "CPU_Clock" && bool(cmd_value.toInt()) ) telnet_println("CPU Clock: " + String(CPU_Clock()) + " MHz");
    #ifdef ESP8266
        if ( command == "PHY_Mode" ) {WiFi.setPhyMode((WiFiPhyMode_t)cmd_value.toInt()); wifi_connect(); }
            // WIFI_PHY_MODE_11B = 1, WIFI_PHY_MODE_11G = 2, WIFI_PHY_MODE_11N = 3
    #endif
    if ( command == "BattPowered" ) BattPowered = bool(cmd_value.toInt());

    custom_mqtt(command, cmd_value);

//    if (config.DEBUG) {
//        storage_print();
//        if (BattPowered) { Serial.printf("Power: BATT  -  Level: %.0f\t", getBattLevel()); }
//        else { Serial.printf("Power: MAINS\t"); }
//        Serial.print("Current Date/Time: " + curDateTime());
//        Serial.printf("\t NTP Sync: %d\n", NTP_Sync);
//    }
}


// The callback to handle the MQTT PUBLISH messages received from Broker.
void mqtt_setcallback() {
    MQTTclient.setCallback(on_message);
}

void mqtt_init_path() {
    mqtt_pathbase = String(config.ClientID) + "/" + String(ChipID) + "/" + String(MODELName);
    mqtt_pathtele = mqtt_pathbase + "/inform/";
    mqtt_pathcomd = mqtt_pathbase + "/command/";
    mqtt_pathconf = mqtt_pathbase + "/config/";
    mqtt_pathsubs = mqtt_pathcomd;
}

// MQTT commands to run on setup function.
void mqtt_setup() {
    float Batt_Level; 

    mqtt_init_path();
    mqtt_set_client();
    mqtt_connect();
    mqtt_setcallback();
    if (MQTT_state == MQTT_CONNECTED) {
        if (ESPWakeUpReason() == "Deep-Sleep Wake") {
            mqtt_publish(mqtt_pathtele, "Status", "WakeUp");
            Batt_Level = getBattLevel();
            if (Batt_Level >= 0) mqtt_publish(mqtt_pathtele, "Battery", String(Batt_Level,0));
        }
        else {
            // 1st RUN ?
            //if(!Load_Config) yield();                 // NOTE! this var drops after boot/sleep, regardless of Wifi/MQTT connection success.
            /* 
            if(config.SW_Upgraded) {
                config.HASSIO_CFG = false;              // this forces HASSIO Discovery device info to be updated
                config.SW_Upgraded = false;             // The house is cleaned... clearing the flag.
                storage_write();
            }
            */
            // HASSIO Discovery configured?
            if(!config.HASSIO_CFG) {
                mqtt_restore();                         // restore data from BckpRstr topic before announcing to HA
                hassio_discovery();
                if (!bckp_rstr_flag) config_backup();   // If true means we need to generate and upload the config backup
            }
            else trigger_syncme();
            state_update();
        }
    }
}

// MQTT commands to run on loop function.
void mqtt_loop() {
    if (!MQTTclient.loop()) {
        if ( millis() - MQTT_LastTime > (MQTT_Retry * 1000)) {
            MQTT_errors ++;
            telnet_println( "in loop function MQTT ERROR! #: " + String(MQTT_errors) + "  ==> " + MQTT_state_string(MQTTclient.state()) );
            MQTT_LastTime = millis();
            mqtt_connect();
            if( MQTT_state == MQTT_CONNECTED) state_update();
        }
    }
    yield();
}

// SERIAL and TELNET Parsing functions
void parse_at_command(String msg) {
    #ifdef Modem_WEB_TELNET
        msg.remove(0, 2);       // to remove "AT"
        modem.sendAT(msg);
        String res = "";
        unsigned long timeout = millis();
        while ( (millis() - timeout < 1000) && modem.stream.available() < 0 ) yield();
        timeout = millis();
        while ((millis() - timeout < 1000)) {
            if (modem.stream.available() > 0) {
                res = modem.stream.readStringUntil('\n');
                telnet_println(res);
                timeout = millis();
            }
        }
    #endif
    yield();
}

void parse_command_msg(String bufferRead) {
        char msg_array[bufferRead.length()+1];
        strcpy(msg_array, bufferRead.c_str());
        msg_array[bufferRead.length()] = 0;

        if((msg_array[0]=='A' || msg_array[0]=='a') && (msg_array[1] == 'T' || msg_array[1] == 't')) {
            if (config.DEBUG) Serial.println(bufferRead);
            parse_at_command(bufferRead);
        }
        else {
            char * equalPosition = strchr(msg_array,'=');
            //Serial.println(equalPosition);
            if(equalPosition > 0) {
                char * pch;
                pch = strtok (msg_array," ,-=\"\t\r\n");
                String command = String((char *)pch);
                pch = strtok (NULL, " ,=\"\t\r\n");
                String value = String((char *)pch);
                //Serial.print(command); Serial.print("\t<-->\t"); Serial.println(value);

                byte B_value[value.length()+1];
                value.getBytes(B_value, value.length()+1);
                //B_value[value.length()+1] = 0;

                on_message(command.c_str(), B_value, value.length()+1);
            }
            else telnet_println("",true);
        }
        TELNET_Timer = millis();                // Update timer to extend telnet inactivity timeout
}


// TELNET commands to run on loop function.
void telnet_loop() {
	blink_LED(3);
    if (WIFI_state != WL_CONNECTED) yield();
    else {
	    //check if there are any new clients
        if (telnetClient.connected()) {
            if ((millis() - TELNET_Timer) > MAX_TIME_INACTIVE) {
                telnet_println("Closing Telnet session by inactivity", true);
                telnetClient.stop();
            }
        }

        if (telnetServer.hasClient()) {
            if (telnetClient && telnetClient.connected()) {
                // Verify if the IP is same than actual conection
                newClient = telnetServer.accept();
                if (newClient.remoteIP() == telnetClient.remoteIP() ) {
                    // Reconnect
                    telnetClient.stop();
                    telnetClient = newClient;
                }
                else {
                    // Disconnect (not allow more than one connection)
                    newClient.stop();
                    return;
                }
            }
            else {
                // New TCP client
                telnetClient = telnetServer.accept();
                telnetClient.setNoDelay(true);      // Faster... ?
                telnetClient.flush();               // clear input buffer, else you get strange characters
                TELNET_Timer = millis();            // initiate timer for inactivity
                if (config.DEBUG) Serial.println("New telnet client!");
            }
        }

        if (telnetClient.available()) {
            parse_command_msg(telnetClient.readStringUntil(char(10)));
            console_prompt();
        }
        yield();
    }
}


// Serial loop function to parse anny command written on the serial interface
void serial_loop() {
        if (Serial.available()) {
            parse_command_msg(Serial.readStringUntil(char(10)));
            console_prompt();
        }
        yield();
}
