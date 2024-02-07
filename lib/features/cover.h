// Internal Variables


Ticker travel_ticker;

// MOTOR related functions
void move_stop() {
    //hcp2731_stop();
    digitalWrite(MOTOR_UP, LOW);
    digitalWrite(MOTOR_DN, LOW);
    DIRECTION = 0;

    //hcp2731_start();
    delay(100);
    mqtt_publish(mqtt_pathtele, "Motor", String(DIRECTION));
    telnet_println("motor stop");
    telnet_println("Axel moved to position: " + String(TRAVEL));
    mqtt_publish(mqtt_pathtele, "Cover", String(100 * TRAVEL / config.MAX_TRAVEL));
}

void move_up() {
    if (DIRECTION != 1) {
        //hcp2731_stop();
        digitalWrite(MOTOR_DN, LOW);
        delay(50);
        digitalWrite(MOTOR_UP, HIGH);
        DIRECTION = 1;
        //hcp2731_start();
        delay(100);
        mqtt_publish(mqtt_pathtele, "Motor", String(DIRECTION));
        telnet_println("motor moving UP");
    }
  //else delay(10);
}

void move_dn() {
    if (DIRECTION != -1) {
        //hcp2731_stop();
        digitalWrite(MOTOR_UP, LOW);
        delay(50);
        digitalWrite(MOTOR_DN, HIGH);
        DIRECTION = -1;
        //hcp2731_start();
        delay(100);
        mqtt_publish(mqtt_pathtele, "Motor", String(DIRECTION));
        telnet_println("motor moving DOWN");
    }
  //else delay(10);
}

void IRAM_ATTR update_travel() {
    
    if (LAST_TRAVEL_Update < millis()) TRAVEL = (millis() - LAST_TRAVEL_Update) * DIRECTION + LAST_TRAVEL;
    
    LAST_TRAVEL = TRAVEL;
    LAST_TRAVEL_Update = millis();
}

void motor_hw() {
    pinMode(MOTOR_UP, OUTPUT);
    pinMode(MOTOR_DN, OUTPUT);
    digitalWrite(MOTOR_UP, LOW);                      // initialize MOTOR UP off
    digitalWrite(MOTOR_DN, LOW);                      // initialize MOTOR DN off
}


void motor_setup() {
    travel_ticker.attach_ms(checker_interval, update_travel);
}

void motor_loop() {
    // Motor and axel movement handling
    if (DIRECTION !=0 && (millis() % 1000 <= 1)) {
        //mqtt_publish(mqtt_pathtele, "Level", String(to_TRAVEL));
        mqtt_publish(mqtt_pathtele, "Cover", String(100 * TRAVEL / config.MAX_TRAVEL));
    }

    if ((TRAVEL >= to_TRAVEL && DIRECTION == 1) || (TRAVEL <= to_TRAVEL && DIRECTION == -1)) {
        move_stop();
        //mqtt_publish(mqtt_pathtele, "Level", String(to_TRAVEL));
        to_TRAVEL = TRAVEL;
    }

    if (TRAVEL != to_TRAVEL) {
        if (TRAVEL > to_TRAVEL) move_dn();
        if (TRAVEL < to_TRAVEL) move_up();
    }

    if (DIRECTION ==0 && TRAVEL < 0) {                  // After stopping the motor, check is position is outside boundaries and re-align it
        LAST_TRAVEL = 0;
        TRAVEL = 0;
        to_TRAVEL = 0;
        mqtt_publish(mqtt_pathtele, "Cover", String(100 * TRAVEL / config.MAX_TRAVEL));
    }

    if (DIRECTION ==0 && TRAVEL > config.MAX_TRAVEL && !Learning) {  // After stopping the motor, check is position is outside boundaries and re-align it
        LAST_TRAVEL = config.MAX_TRAVEL;
        TRAVEL = config.MAX_TRAVEL;
        to_TRAVEL = config.MAX_TRAVEL;
        mqtt_publish(mqtt_pathtele, "Cover", String(100 * TRAVEL / config.MAX_TRAVEL));
    }

}