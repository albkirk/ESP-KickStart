// Declare and define here all sensors and actuators used in this project
// **** Project code definition here ...
//#include <buttons.h>
//#include <ambient.h>
//#include <mygps.h>


void peripherals_setup() {
 // Output GPIOs
 


 // Input GPIOs

   //buttons_setup();
/*
   // Avoid full setup cycle if Humidity didn't changed since last read.
   if (ESPWakeUpReason() == "Deep-Sleep Wake" && RTC_read() )) {  // If true, I can also assume rtc.ByteValue >= 0
      if ((rtcData.FloatValue - Humidity) <=3 && (rtcData.FloatValue - Humidity) >=-3 && rtcData.ByteValue <4) {              // No change on humidity, so back to deep-sleep
        rtcData.ByteValue ++;
        telnet_println("No big change on Humidity, back to deep-sleep for " + String(config.SLEEPTime) + " min, or until next event ... zzZz :) ");
        telnet_println("Total time ON: " + String(millis()) + " msec");
        GoingToSleep(config.SLEEPTime, rtcData.lastUTCTime);
      };
   };
*/
}

