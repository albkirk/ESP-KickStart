#define TOUCH_H true

#ifdef ESP32
#endif


void ICACHE_RAM_ATTR T_Left_pressed ();
void ICACHE_RAM_ATTR T_Right_pressed ();

// **** Normal code definition here ...
//#define T_Left T7                                 // Defined in project/def_conf.h
//#define T_Right T9                                // Defined in project/def_conf.h
#define Touch_Bounce_Time 10                        // Defined in features/buttons.h
#define Touch_Interval 500                          // Defined in features/buttons.h
static volatile bool TL_STATUS = false;             // status of Touch Left  (true = pressed, false = released)
static volatile bool TR_STATUS = false;             // status of Touch Right (true = pressed, false = released)
static volatile unsigned long Previous_TL;          // Touch Left Interrupt running (true = yes, false = no)
static volatile bool TR_Interrupt = false;          // Touch Right Interrupt running (true = yes, false = no)
static volatile int TL_COUNT = 0;                   // to count number of times Touch Left is pressed within interval
static volatile int TR_COUNT = 0;                   // to count number of times Touch Right is pressed within interval
static volatile bool Last_TL_STATUS = false;        // Last status of Touch Left
static volatile bool Last_TR_STATUS = false;        // Last status of Touch Right
static volatile unsigned long Last_TL = 0;          // timer var to avoid function call trigger due contact bounce
static volatile unsigned long Last_TR = 0;          // timer var to avoid function call trigger due contact bounce

//int TouchPins[] = { 04, 00, 02, 15, 13, 12, 14, 27, 33, 32 };  // ALL TOUCH PIN available!
//int TouchPins[] = { T0, T1, T2, T3, T4, T5, T6, T7, T8, T9 };  // ALL TOUCH values available!
int TouchPins[] = { T_Left, T_Right };              // Used on this project
#define NumTouchPins sizeof(TouchPins) / sizeof(*TouchPins)
uint16_t TouchThreshold[NumTouchPins] = {};
uint16_t TouchValMin = 10;                          // Minimum Touch Analog value measured
byte CalibrateTouch = 10;                           // Number of measurements it will do to calibrate the Touch threshold
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;    // "critical section" Flag inside the callback


// TOUCH related functions
void T_Left_pressed () {
    //portENTER_CRITICAL(&mux);
    unsigned long NOW = millis();
    if (TL_STATUS == false) {
        while (millis() - NOW < Touch_Bounce_Time) {};      // loop to allow button status be stable before reading it
            if ( touchRead(TouchPins[0]) <= TouchThreshold[0] ) {
                TL_STATUS = true;
                if (NOW - Last_TL < Touch_Interval) {
                    TL_COUNT += 1;
                    Serial.println("Touch Left pressed " + String(TL_COUNT) + " times!");
                }
                else {
                    TL_COUNT = 1;
                    Serial.println("Touch Left pressed");
                }
            };
    }
    Last_TL = NOW;
    //portEXIT_CRITICAL(&mux);
}

void T_Right_pressed () {   // IRAM_ATTR
    //portENTER_CRITICAL(&mux);
    unsigned long NOW = millis();
    if (TR_STATUS == false) {
        while (millis() - NOW < Touch_Bounce_Time) {};      // loop to allow button status be stable before reading it
            if ( touchRead(TouchPins[1]) <= TouchThreshold[1] ) {
                TR_STATUS = true;
                if (NOW - Last_TR < Touch_Interval * 2) {
                    TR_COUNT += 1;
                    Serial.println("Touch Right pressed " + String(TR_COUNT) + " times!");
                }
                else {
                    TR_COUNT = 1;
                    Serial.println("Touch Right pressed");
                }
            };
    }
    Last_TR = NOW;
    //portEXIT_CRITICAL(&mux);
}

void touch_calibrate() {
    Serial.println("Calibrating Touch....");
    for( byte n = 1; n < CalibrateTouch; ++n ) {
        for(byte idx = 0; idx < NumTouchPins; ++ idx ) TouchThreshold[idx] = TouchThreshold[idx] + touchRead(TouchPins[idx]);
    };
    for(byte idx = 0; idx < NumTouchPins; ++ idx ) {
        TouchThreshold[idx] = (TouchThreshold[idx] / CalibrateTouch - TouchValMin )* 1/4 + TouchValMin ;
        Serial.println("Touch: " + String(TouchPins[idx]) + " Threshold value: " + String(TouchThreshold[idx] ));
    };
}

void touch_setup() {
  touch_calibrate();
  if(TouchPins[0] >=0) touchAttachInterrupt(TouchPins[0], &T_Left_pressed, TouchThreshold[0]);
  if(TouchPins[1] >=0) touchAttachInterrupt(TouchPins[1], &T_Right_pressed, TouchThreshold[1]);
}


void touch_loop() {
    if (TL_STATUS == true && (millis() - Last_TL > Touch_Bounce_Time)) {
        if ( touchRead(TouchPins[0]) > TouchThreshold[0]) {
            TL_STATUS = false;
            Serial.println("Touch Left released");
        }
    }
}
