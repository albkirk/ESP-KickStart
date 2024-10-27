/*      H O W  T O   U S E   t H I S   L I B R A R Y ?
Add these 3 lines to your main.cpp file:
#define LED_RGB                                         // Uncoment to be used by color.h library
#define LED_NEO                                         // Uncoment to be used by color.h library
#include <color.h>

On SETUP function add this line:
color_setup();

On LOOP function add this line:
color_loop();

change color by changing the value of config.color
Change Light brightness by changing the value of: GAIN


*/

#ifdef LED_NEO
    #include <Adafruit_NeoPixel.h>
    //#include <FastLED.h>
    //#include <NeoPixelBus.h>
#endif

// CONSTANTs
#ifndef LED_NEO
    #ifdef ESP32
        #define RESOLUTION           8          // Duty cycle bits (8 -> 255) equalto FF
        #define PWM_FREQ          1000          // PWM Frequency
        #define CHANNELR             0
        #define CHANNELG             1
        #define CHANNELB             2
    #else   // is ESP8266
        #define PWM_RANGE 255                   // Duty cycle value 255 equalto FF
        #define PWM_FREQ 250                    // PWM Frequency
    #endif
#endif

    char BLACK[8] =        "#000000";       // RGB code for BLACK or LED Strip OFF
    char WHITE[8] =        "#FFFFFF";       // RGB code for WHITE
    char RED[8] =          "#FF0000";       // RGB code for RED
    char GREEN[8] =        "#00FF00";       // RGB code for GREEN
    char BLUE[8] =         "#0000FF";       // RGB code for BLUE
    char ORANGE[8] =       "#FFA500";       // RGB code for ORANGE
    char LIGHT_GREEN[8] =  "#90EE90";       // RGB code for LIGHT GREEN
    char DEEP_BLUE[8] =    "#000080";       // RGB code for NAVY
    char WARM_WHITE[8] =   "#FF930F";       // RGB code for ----------
    char MILK_WHITE[8] =   "#FFF0F0";       // RGB code for SNOW
    char DEEP_YELLOW[8] =  "#FFD700";       // RGB code for GOLD
    char CYAN[8] =         "#00FFFF";       // RGB code for CYAN
    char BLUE_PURPLE[8] =  "#9370DB";       // RGB code for MEDIUM PURPLE
    char PINK_WHITE[8] =   "#FFC0CB";       // RGB code for PINK
    char YELLOW[8] =       "#FFFF00";       // RGB code for YELLOW
    char LIGHT_BLUE[8] =   "#ADD8E6";       // RGB code for LIGHT BLUE
    char PURPLE[8] =       "#800080";       // RGB code for PURPLE
    char GREEN_WHITE[8] =  "#E0FFE0";       // RGB code for HONEYDEW
    char LIGHT_YELLOW[8] = "#FFFFD0";       // RGB code for LIGHT YELLOW
    char SKY_BLUE[8] =     "#87CEEB";       // RGB code for SKY BLUE
    char BROWN[8] =        "#A52A2A";       // RGB code for BROWN
    char BLUE_WHITE[8] =   "#E0E0FF";       // RGB code for ALICE BLUE

    // Rainbow colors: red, orange, yellow, green, blue, indigo, and violet
    char *rainbow[] = {RED, ORANGE, YELLOW, GREEN, BLUE, CYAN, PURPLE};

//
// VARIABLES
//
    char Color[8]     = "#000000";  // RGB Color code. syntax: '#' + RED + GREEN + BLUE
    char LastColor[8] = "#000000";  // each param use 2 CHARs and range from 0 to FF (HEX format of 0-255).
    char LastNBlack[8]= "#000000";  // To track the Last NON Black color used
    byte RGB[3];                    // RRB trio byte Values to control the OUTPut PWM PINs
    byte neoID = 0;                 // NEO Pixel ID [0 - n] to change color
    byte zeros[3] = {0, 0, 0};      // trio of byte Value 0 to represnt Black color
    char Xval[2];                   // Buffer memory to convert byte into double HEX char
    char *Xarr;
    long RANGE[] = {15, 31, 47, 63, 79, 95, 111, 127, 143, 159, 175, 191, 207, 223, 239, 255};
    byte GAIN = 255;                // Color gain 0 to 255
    bool Light = false;             // [OFF / ON] Light switch
    bool Light_Last = true;         // [OFF / ON] Light switch  (Last state)
// EFX
    const String EFXName[] = {"NoEFX", "Auto", "Flash", "Fade3", "Fade7", "Scan", "Rainbow"};
    byte sizeof_EFXName = 7;        // The number of EFX in the list (must be manually counted)
    byte EFX = 0;                   // The EFX to be played
    byte EFX_Last = 255;            // The Last EFX to be played
    byte EFX_RGB[3] = {0, 0, 0};    // Effects RGB trio byte values used during Effects
    int  RGB_idx=0;                 // RGB array index
    long EFX_Delta=-2;              // RGB delta value change (-2 or +2)
    byte EFX_GAIN = 255;            // Effect Color gain 0 to 255
    uint EFX_Interval=1500;         // Effect interval timer in milliseconds
    byte GAIN_Last = 0;             // Last GAIN value
    uint Last_T_EFX=0;              // last time Effect status was changed
    uint Last_T_Flash=0;            // last time Flash status was changed
    bool Bool_flash=false;          // Boolean to toggle flash status
    byte RainBow_idx = 0;           // Raibow color index

 #ifdef LED_NEO
    // When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
    // Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
    // example for more information on possible values.
    Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NEOPixelsNUM, NEOPixelsPIN, NEO_GRB + NEO_KHZ800);

    //NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> pixels(NEOPixelsNUM, NEOPixelsPIN);
#endif

//
//  AUXILIAR functions
//
byte CharHEX_to_byte(char X) {
  byte num;
  if (byte(X) >= 48 && byte(X) <= 57) num = byte(X)- 48;   // 48 ='0' and 57 = '9'
  else if (byte(X) >= 65 && byte(X) <= 70) num = byte(X) - 65 + 10;   // 65 = 'A'  and 70 ='F'
  else if (byte(X) >= 97 && byte(X) <= 102) num = byte(X) - 97 + 10;   // 97 = 'a'  and 102 ='f'
  else num = 0;
  //Serial.print(" ->" + String(num));
  return num;
}


char *byte_to_CharHEX(byte Bval, char arr[2]) {
  if (Bval/16 >= 0 && Bval/16 <= 9) arr[0] =char(48 + Bval/16);   // 48 ='0' and 57 = '9'
  else if (Bval/16 >= 10 && Bval/16 <= 15) arr[0] = char(65 + Bval/16 - 10);   // 65 = 'A'  and 70 ='F'
  if (Bval%16 >= 0 && Bval%16 <= 9) arr[1] = char(48 + Bval%16);   // 48 ='0' and 57 = '9'
  else if (Bval%16 >= 10 && Bval%16 <= 15) arr[1] = char(65 + Bval%16 - 10);   // 65 = 'A'  and 70 ='F'
  return arr;

  // then you want to use it.
//Xarr = byte_to_CharHEX(255, Xval);
}

bool barraycpy (byte *arrout, byte *arrin) {
  if (sizeof(arrin) > sizeof(arrout)) return false;
  for (size_t i = 0; i < 3; i++) {
    arrout[i] = arrin[i];
  }
  return true;
}

byte EFX_index(String efxname) {
    //this functions return the index position which the input string matches to the EFXName string array
    for (byte i = 0; i < sizeof_EFXName; i++) {
        if (efxname == EFXName[i]) {
            return i;
        }
    }
    return 0; // Return 0 if no match found
}

byte color_gain (byte Cval, byte Gval) {
   return (byte) (Cval * Gval/255.0);
}

void gain_change(byte cgain, char color_code[8] = Color){       // change R, G or B to Color
    char arr[2];
    RGB[0] = CharHEX_to_byte(color_code[1]) * 16 + CharHEX_to_byte(color_code[2]);
    RGB[1] = CharHEX_to_byte(color_code[3]) * 16 + CharHEX_to_byte(color_code[4]);
    RGB[2] = CharHEX_to_byte(color_code[5]) * 16 + CharHEX_to_byte(color_code[6]);
    RGB[0] = color_gain(RGB[0], cgain);
    RGB[1] = color_gain(RGB[1], cgain);
    RGB[2] = color_gain(RGB[2], cgain);
    byte_to_CharHEX(RGB[0], arr); Color[1] = arr[0]; Color[2] = arr[1];
    byte_to_CharHEX(RGB[1], arr); Color[3] = arr[0]; Color[4] = arr[1];
    byte_to_CharHEX(RGB[2], arr); Color[5] = arr[0]; Color[6] = arr[1];
    telnet_println("Color: " + String(Color) );
}

void color_change(int C_RED = 0, int C_GREEN = 0, int C_BLUE = 0, char color_code[8] = Color){       // change R, G or B to Color
    char arr[2];
    RGB[0] = CharHEX_to_byte(color_code[1]) * 16 + CharHEX_to_byte(color_code[2]);
    //telnet_println(String(byte_to_CharHEX(RGB[0], Xval)));
    RGB[1] = CharHEX_to_byte(color_code[3]) * 16 + CharHEX_to_byte(color_code[4]);
    RGB[2] = CharHEX_to_byte(color_code[5]) * 16 + CharHEX_to_byte(color_code[6]);
    if (int(RGB[0])+ C_RED >= 0   && int(RGB[0])+ C_RED   <= 255 ) RGB[0] = byte(int(RGB[0]) + C_RED);
    if (int(RGB[1])+ C_GREEN >= 0 && int(RGB[1])+ C_GREEN <= 255 ) RGB[1] = byte(int(RGB[1]) + C_GREEN);
    if (int(RGB[2])+ C_BLUE >= 0  && int(RGB[2])+ C_BLUE  <= 255 ) RGB[2] = byte(int(RGB[2]) + C_BLUE);
    byte_to_CharHEX(RGB[0], arr); Color[1] = arr[0]; Color[2] = arr[1];
    byte_to_CharHEX(RGB[1], arr); Color[3] = arr[0]; Color[4] = arr[1];
    byte_to_CharHEX(RGB[2], arr); Color[5] = arr[0]; Color[6] = arr[1];
    telnet_println("Color: " + String(Color) );
}

void HARGB_to_color(String RGBstring, char color_code[8] = Color){       // converts RGB string 100,120,23 to color array
    char arr[2];
    int r, g, b;
    // Use sscanf to extract RGB values from the input string
    sscanf(RGBstring.c_str(), "%d,%d,%d", &r, &g, &b);
    // Ensure that the RGB values are within the valid range (0-255)
    r = constrain(r, 0, 255);
    g = constrain(g, 0, 255);
    b = constrain(b, 0, 255);
    RGB[0] = byte(r);
    RGB[1] = byte(g);
    RGB[2] = byte(b);
    byte_to_CharHEX(RGB[0], arr); Color[1] = arr[0]; Color[2] = arr[1];
    byte_to_CharHEX(RGB[1], arr); Color[3] = arr[0]; Color[4] = arr[1];
    byte_to_CharHEX(RGB[2], arr); Color[5] = arr[0]; Color[6] = arr[1];
    telnet_println("Color: " + String(Color) );
}


void color_paint (byte PAINT[3], byte PGain = GAIN, byte nID = 0) {  // byte PGain = GAIN || 255
#ifndef LED_NEO
    #ifdef ESP32
        ledcWrite(CHANNELR, color_gain(PAINT[0], PGain));                     // OUTPUT RED level
        ledcWrite(CHANNELG, color_gain(PAINT[1], (byte)(PGain*CALIBRATE)));   // OUTPUT GREEN level
        ledcWrite(CHANNELB, color_gain(PAINT[2], (byte)(PGain*CALIBRATE)));   // OUTPUT BLUE level
    #else   //is ESP8266
        analogWrite(PIN_RED,   color_gain(PAINT[0], PGain));                  // OUTPUT RED level
        analogWrite(PIN_GREEN, color_gain(PAINT[1], (byte)(PGain*CALIBRATE)));// OUTPUT GREEN level
        analogWrite(PIN_BLUE,  color_gain(PAINT[2], (byte)(PGain*CALIBRATE)));// OUTPUT BLUE level
    #endif
#else
  pixels.setPixelColor(nID, pixels.Color(color_gain(PAINT[0], PGain), color_gain(PAINT[1], PGain), color_gain(PAINT[2], PGain))); // Moderately bright green color.
  //pixels.SetPixelColor(nID, RgbColor(color_gain(PAINT[0], PGain), color_gain(PAINT[1], PGain), color_gain(PAINT[2], PGain))); // Moderately bright green color.
  pixels.show(); // This sends the updated pixel color to the hardware.
#endif
}

bool color_set (char color_code[8], byte nID = 0) {
    if (color_code[0] == '#') {
        RGB[0] = CharHEX_to_byte(color_code[1]) * 16 + CharHEX_to_byte(color_code[2]);
        //telnet_println(String(byte_to_CharHEX(RGB[0], Xval)));
        RGB[1] = CharHEX_to_byte(color_code[3]) * 16 + CharHEX_to_byte(color_code[4]);
        RGB[2] = CharHEX_to_byte(color_code[5]) * 16 + CharHEX_to_byte(color_code[6]);
        //telnet_println(" R=" + String(RGB[0]) + " G=" + String(RGB[1]) + " B=" + String(RGB[2]));
        //Serial.println(String(Color) + " : " + String(color_code) + " : " + String(LastColor));
        color_paint (RGB, GAIN, nID);
#ifndef LED_NEO
 //       strcpy(LastColor,color_code);
        barraycpy(EFX_RGB, RGB);
        EFX_GAIN = GAIN;
#endif
        return true;
    }
    else return false;
}

// Color Effect functions
void color_Auto() {
    if ( millis() - Last_T_EFX > EFX_Interval/10) {
      int idx = random(0,3);
      int deviate = random(-25,25);
      if (EFX_RGB[idx] + deviate >=0 && EFX_RGB[idx] + deviate <=255) {
          EFX_RGB[idx] += deviate ;
          color_paint(EFX_RGB, EFX_GAIN);
      }
      Last_T_EFX = millis();
    }
}

void color_Flash() {
    if ( millis() - Last_T_Flash > EFX_Interval/4) {
        if (Bool_flash) {
            EFX_GAIN = 0;
            color_paint(EFX_RGB, EFX_GAIN);
        }
        else {
            EFX_GAIN = GAIN;
            color_paint(EFX_RGB, EFX_GAIN);
        }
        Bool_flash = !Bool_flash;
        Last_T_Flash = millis();
    }
}

void color_Fade3() {
    if ( millis() - Last_T_EFX > EFX_Interval/20) {
      if (EFX_RGB[RGB_idx] + EFX_Delta >=0 && EFX_RGB[RGB_idx] + EFX_Delta <=RGB[RGB_idx]) {
          EFX_RGB[RGB_idx] += EFX_Delta;
          color_paint(EFX_RGB, EFX_GAIN);
      }
      else {
        if (EFX_Delta <0) RGB_idx = (RGB_idx +1) % 3;
        EFX_Delta = - EFX_Delta;
      }
      Last_T_EFX = millis();
    }
}

void color_Fade7() {
    if ( millis() - Last_T_EFX > EFX_Interval/20) {
      if (EFX_RGB[RGB_idx] + EFX_Delta >=0 && EFX_RGB[RGB_idx] + EFX_Delta <=RGB[RGB_idx]) {
          EFX_RGB[RGB_idx] += EFX_Delta;
          color_paint(EFX_RGB, EFX_GAIN);
      }
      else {
        RGB_idx = (RGB_idx +1) % 3;
        EFX_Delta = - EFX_Delta;
      }
      Last_T_EFX = millis();
    }
}

void color_rainbow() {
    if ( millis() - Last_T_EFX > EFX_Interval/20) {
      color_set(rainbow[RainBow_idx], neoID);
      color_set(rainbow[RainBow_idx], neoID+1);
      //Serial.println("Color" + String(rainbow[RainBow_idx]) + "  NEO: " + neoID);
      RainBow_idx = (RainBow_idx + 1) % (sizeof(rainbow)/sizeof(*rainbow));
      neoID  = (neoID + 2)% NEOPixelsNUM;
      Last_T_EFX = millis();
    }
}

void color_scan() {
    if ( millis() - Last_T_EFX > EFX_Interval/20) {
      color_set(BLACK, neoID);
      RainBow_idx = (RainBow_idx + 1) % (sizeof(rainbow)/sizeof(*rainbow));
      neoID  = (neoID + 1) % NEOPixelsNUM;
      color_set( rainbow[RainBow_idx], neoID);
      Last_T_EFX = millis();
    }
}

//
//  MAIN functions
//
void color_setup() {
    // ***********  Color SETUP
#ifndef LED_NEO
    #ifdef ESP32
        ledcSetup(CHANNELR, PWM_FREQ, RESOLUTION);  // Duty cycle Range of values [o-PWMRANGE] equal to FF
        ledcSetup(CHANNELG, PWM_FREQ, RESOLUTION);  // PWM Frequency 1000Hz
        ledcSetup(CHANNELB, PWM_FREQ, RESOLUTION);
        // attach the channel to the GPIO to be controlled
        ledcAttachPin(PIN_RED, CHANNELR);
        ledcAttachPin(PIN_GREEN, CHANNELG);
        ledcAttachPin(PIN_BLUE, CHANNELB);
    #else  // is 8266
        pinMode(PIN_RED,   OUTPUT);
        pinMode(PIN_GREEN, OUTPUT);
        pinMode(PIN_BLUE,  OUTPUT);
        analogWriteRange(PWM_RANGE);        // Duty cycle Range of values [o-PWMRANGE] equal to FF
        analogWriteFreq(PWM_FREQ);          // PWM Frequency 250Hz
    #endif
    color_set(BLACK);                       // Initiate LED Strip turned off
    strcpy(LastNBlack,config.InitColor);    // Setup the initial color for the Light/Switch command
    delay(200);
    if (config.SWITCH_Default) {
        strcpy(Color,config.InitColor);     // for faster control of RGB, independent of WiFI/MQTT/system status
        Light = true;                       // Turning LED Strip to ON
        Light_Last = false;                 // and force publishing initial Light State
        color_set(Color);                   // Turning LED Strip to initial color (typicaly white...)
        strcpy(LastColor,BLACK);            // and force publishing initial color after Wifi and MQQT initialization
    }
#else
    // Set NeoPixel configuration
    //pixels.SetBrightness(BRIGHTNESS);
    // Start NeoPixel library with all LEDs off
    pixels.begin();                     // This initializes the NeoPixel library.
    for (size_t i = 0; i < NEOPixelsNUM; i++) NEOcolor_set (BLACK, i);  // Initiate NEO Pixel Strip turned off
#endif
}

  // Color commands to run on loop function.
void color_loop() {
    if (String(Color) != String(LastColor)) {
        if (String(Color) != String(BLACK)) {
            Light = true;
            strcpy(LastNBlack,Color);
        }
        else Light = false;
        if (color_set(Color)) mqtt_publish(mqtt_pathtele, "Color", String(Color));
        strcpy(LastColor,Color);
    }
    if ( GAIN!= GAIN_Last) {
        if (color_set(Color)) mqtt_publish(mqtt_pathtele, "Gain", String(GAIN));
        GAIN_Last = GAIN;
    }
    if (Light != Light_Last) {
        if (Light) { EFX = 0; strcpy(Color,LastNBlack); }
        else { EFX = 0; strcpy(Color,BLACK); }
        mqtt_publish(mqtt_pathtele, "Light", String(Light));
        //mqtt_publish(mqtt_pathtele, "Gain", String(GAIN));
        Light_Last = Light;
    }
    if (EFX != EFX_Last) {
         mqtt_publish(mqtt_pathtele, "EFX", EFXName[EFX]);
         EFX_Last = EFX;
    }
    switch (EFX) {
        case 0:
            #ifndef LED_NEO
                color_paint (RGB, GAIN);
            #else
                for (size_t i = 0; i < NEOPixelsNUM; i++) {
                    NEOcolor_set(BLACK, i);
                }
            #endif
            break;
        case 1:
            color_Auto();
            break;
        case 2:
            color_Flash();
            break;
        case 3:
            color_Fade3();
            break;
        case 4:
            color_Fade7();
            break;
        case 5:
            color_rainbow();
            break;
        case 6:
            color_scan();
            break;
        case 7:
            color_Fade3();
            color_Flash();
            break;
        case 8:
            color_Fade7();
            color_Flash();
            break;
        default:
            EFX = 0;
    }
    yield();
}
