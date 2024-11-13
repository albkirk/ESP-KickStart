/*
 * * Main File to trigger all C code using "arduino" style.
 * * It contains all necessary function calls on setup and loop functions
 * * HOW TO USE THIS TEMPLATE:
 * * -- Keep this file as is. and use the lib/project/project.h 
 * *    Use the "// **** Normal code ..." zones to add you own definition, functions, setup and loop code
 * * -- Suggest to use "lib/project/" to add your own h files
 * * -- Adjust the PIN numbering  in lib/project/def_conf.h
 * * -- Adjust the "default.config" in lib/project/def_conf.h
 * * -- Add you own HASSIO discovery declaration on "lib/project/custohassio.h"
 * * -- Add you own MQTT actions on "lib/project/customactions.h"
 */

// Libraries to INCLUDE
#include <storage.h>

#ifdef ESP32
    #include <esp32hw.h>
#else
    #include <hw8266.h>
#endif

#include <mywifi.h>

//#ifdef ESP8266
//    #include <httpupd.h>
//#endif

#include <console.h>
#include <ntp.h>
#include <mqtt.h>
#include <global.h>
#include <ota.h>
#include <peripherals.h>
#include <hassio.h>
#ifndef ESP8285
    #include <web.h>
#endif
#include <project.h>
#include <actions.h>                        // Added later because functions from project are called here.


void setup() {
  // Starting with WiFi interface shutdown in order to save energy
    wifi_disconnect();

  // Start SERIAL console
      //Serial.begin(74880);                  // This odd baud speed will shows ESP8266 boot diagnostics too.
      Serial.begin(115200);                 // For faster communication use 115200
      //Serial.setTimeout(1000);
      //Serial.setDebugOutput(false);

      Serial.println("");
      Serial.println(" -- Hello World! -- ");
      Serial.println("My ID is " + ChipID + " and I'm running version " + SWVer);
      Serial.println("Reset reason: " + ESPResetReason());

  // Start Storage service and read stored configuration
      storage_setup();

  // Start Global features, such as: ESP_LED, Buzzer, internal ADC, ...
      global_setup();

  //  Project peripherals initialization, such as: GPIO config, Sensors, Actuators, ...  
      peripherals_setup();

  // Start WiFi service (Station or/and as Access Point)
      wifi_setup();

  // Check for HTTP Upgrade
//#ifdef ESP8266
//      http_upg();               // Note: this service kills all running UDP and TCP services
//#endif
  // Start TELNET console service
      telnet_setup();

 // Start MQTT service
      mqtt_setup();

  // Start OTA service
      ota_setup();

  // Start NTP service
      ntp_setup();

  // **** Project SETUP Sketch code here...
      project_setup();

#ifndef ESP8285
  // Start ESP Web Service
      if (config.WEB) web_setup();
#endif

  // with all setup tasks done it's time to prompt!
      console_prompt();

  // Last bit of code before leave setup
      ONTime_Offset = millis() + 200UL;     //  200ms after finishing the SETUP function it starts the "ONTime" countdown.
                                            //  it should be good enough to receive the MQTT "ExtendONTime" msg
} // end of setup()


void loop() {
  // allow background process to run.
      yield();

  // Global handling, namely the button to return to default configuration
      //global_loop();

  // WiFi handling
      wifi_loop();

  // Serial handling
      serial_loop();

  // TELNET handling
      if (config.TELNET) telnet_loop();

  // MQTT handling
      mqtt_loop();

  // OTA request handling
      if (config.OTA) ota_loop();

  // NTP handling
      ntp_loop();

#ifndef ESP8285
  // ESP Web Server requests handling
      if (config.WEB) web_loop();
#endif

  // **** Project LOOP Sketch code here ...
      project_loop();

  // Global loops handling
    global_loop();
    
}  // end of loop()
