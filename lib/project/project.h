// WRITE HERE all the Project's logic / Flow.
// **** Libraries to include here ...

// **** Project code definition here ...

// **** Project code functions here ...
void project_setup() {
  Extend_time = 0;
}

void project_loop() {
  // Ambient handing
      //if (TIMER >0) if ((millis() - 3500) % (TIMER * 60000) < 5) ambient_data();      // TIMER bigger than zero on div or dog bites!!

        if (A_COUNT >= 1 && A_STATUS && (millis() - Last_A > 5000)) {
            mqtt_publish(mqtt_pathtele, "Button", "Holded");
            telnet_println("Button A HOLDED for 5 seconds!!!");
            flash_LED(20);
            global_reset();
            A_COUNT = 0;
        }


        if (A_COUNT >= 1 && !A_STATUS && (millis() - Last_A > Butt_Interval)) {
            mqtt_publish(mqtt_pathtele, "Button", String(A_COUNT));
            flash_LED(A_COUNT);
            if (A_COUNT == 2) global_restart();

            if (A_COUNT == 5) {
                config.TELNET = false;
                config.OTA = false;
                config.WEB = false;
                config.APMode = false;
                config.LED = false;
                config.DEEPSLEEP = true;
                telnet_println("COUNT 5!!!");
                storage_write();
                web_setup();                // needed to clean the ON extender time.
                telnet_setup();
            }
            A_COUNT = 0;
        }

}
