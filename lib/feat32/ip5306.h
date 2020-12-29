#include <Wire.h>

// ================ PMU IP5306 ===================
#define IP5306_ADDR (0X75)
#define IP5306_REG_SYS_CTL0 (0x00)
#define IP5306_REG_SYS_CTL1 (0x01)
#define IP5306_REG_SYS_CTL2 (0x02)
#define IP5306_REG_READ0 (0x70)
#define IP5306_REG_READ1 (0x71)
#define IP5306_REG_READ3 (0x78)

//- REG_CTL0
#define BOOST_ENABLE_BIT (0x20)
#define CHARGE_OUT_BIT (0x10)
#define BOOT_ON_LOAD_BIT (0x04)
#define BOOST_OUT_BIT (0x02)
#define BOOST_BUTTON_EN_BIT (0x01)

//- REG_CTL1
#define BOOST_SET_BIT (0x80)
#define WLED_SET_BIT (0x40)
#define SHORT_BOOST_BIT (0x20)
#define VIN_ENABLE_BIT (0x04)

//- REG_CTL2
#define SHUTDOWNTIME_MASK (0x0c)
#define SHUTDOWNTIME_64S (0x0c)
#define SHUTDOWNTIME_32S (0x04)
#define SHUTDOWNTIME_16S (0x08)
#define SHUTDOWNTIME_8S  (0x00)

//- REG_READ0
#define CHARGE_ENABLE_BIT (0x08)

//- REG_READ1
#define CHARGE_FULL_BIT (0x08)

//- REG_READ2
#define LIGHT_LOAD_BIT (0x20)
#define LOWPOWER_SHUTDOWN_BIT (0x01)

// -- ShutdownTimeParam
enum ShutdownTime {
    SHUTDOWN_8S = 0,
    SHUTDOWN_16S,
    SHUTDOWN_32S,
    SHUTDOWN_64S
};

//- Communication Utility 
// Wire.h read and write protocols
bool readByte(uint8_t address, uint8_t subAddress,uint8_t *result) {
  #ifdef I2C_DEBUG_TO_SERIAL
    Serial.printf("readByte :read from 0x%02x [0x%02x] requestByte=1 receive=", address, subAddress);
  #endif

  Wire.beginTransmission(address);         // Initialize the Tx buffer
  Wire.write(subAddress);                  // Put slave register address in Tx buffer
  if (Wire.endTransmission(false) == 0 && Wire.requestFrom(address, (uint8_t)1)) {
    *result = Wire.read();                 // Fill Rx buffer with result
    #ifdef I2C_DEBUG_TO_SERIAL
      Serial.printf("%02x\n",*result);
    #endif
    return true;
  }
  #ifdef I2C_DEBUG_TO_SERIAL
    Serial.printf("none\n");
  #endif
  return false;
}


bool writeByte(uint8_t address, uint8_t subAddress, uint8_t data) {
  bool function_result = false;
  Wire.beginTransmission(address);  // Initialize the Tx buffer
  Wire.write(subAddress);           // Put slave register address in Tx buffer
  Wire.write(data);                 // Put data in Tx buffer
  function_result = (Wire.endTransmission() == 0); // Send the Tx buffer

  #ifdef I2C_DEBUG_TO_SERIAL
    Serial.printf("writeByte:send to 0x%02x [0x%2x] data=0x%02x result:%s\n", address, subAddress, data, function_result ? "OK" : "NG");
  #endif

  return (function_result);
}

bool writeCommand(uint8_t address, uint8_t subAddress) {
  bool function_result = false;
  Wire.beginTransmission(address);               // Initialize the Tx buffer
  Wire.write(subAddress);                        // Put slave register address in Tx buffer
  function_result = (Wire.endTransmission() == 0); // Send the Tx buffer

  #ifdef I2C_DEBUG_TO_SERIAL
    Serial.printf("writeCommand:send to 0x%02x [0x%02x] result:%s\n", address, subAddress, function_result ? "OK" : "NG");
  #endif

  return (function_result);
}



///////  IP5306 Functions

bool setPowerBoostOnOff(bool en) {
  uint8_t data;
  if (readByte(IP5306_ADDR, IP5306_REG_SYS_CTL1, &data) == true) {
    return writeByte(IP5306_ADDR, IP5306_REG_SYS_CTL1, en ? (data | BOOST_SET_BIT) : (data & (~BOOST_SET_BIT)));
  }
  return false;
}

bool setPowerBoostSet(bool en) {
  uint8_t data;
  if (readByte(IP5306_ADDR, IP5306_REG_SYS_CTL1, &data) == true) {
    return writeByte(IP5306_ADDR, IP5306_REG_SYS_CTL1, en ? (data | SHORT_BOOST_BIT) : (data & (~SHORT_BOOST_BIT)));
  }
  return false;
}

bool setPowerVin(bool en) {
  uint8_t data;
  if (readByte(IP5306_ADDR, IP5306_REG_SYS_CTL1, &data) == true) {
    return writeByte(IP5306_ADDR, IP5306_REG_SYS_CTL1, en ? (data | VIN_ENABLE_BIT) : (data & (~VIN_ENABLE_BIT)));
  }
  return false;
}

bool setPowerWLEDSet(bool en) {
  uint8_t data;
  if (readByte(IP5306_ADDR, IP5306_REG_SYS_CTL1, &data) == true) {
      return writeByte(IP5306_ADDR, IP5306_REG_SYS_CTL1, en ? (data | WLED_SET_BIT) : (data & (~WLED_SET_BIT)));
  }
  return false;
}

bool setPowerBtnEn(bool en) {
  uint8_t data;
  if (readByte(IP5306_ADDR, IP5306_REG_SYS_CTL0, &data) == true) {
    return writeByte(IP5306_ADDR, IP5306_REG_SYS_CTL0, en ? (data | BOOST_BUTTON_EN_BIT) : (data & (~BOOST_BUTTON_EN_BIT)));
  }
  return false;
}

bool setLowPowerShutdownTime(ShutdownTime time)
{
  uint8_t data;
  bool ret;
  if (readByte(IP5306_ADDR, IP5306_REG_SYS_CTL2, &data) == true){
    switch (time){
      case ShutdownTime::SHUTDOWN_8S:
        ret = writeByte(IP5306_ADDR, IP5306_REG_SYS_CTL2, ((data & (~SHUTDOWNTIME_MASK)) | SHUTDOWNTIME_8S));
        break;
      case ShutdownTime::SHUTDOWN_16S:
        ret = writeByte(IP5306_ADDR, IP5306_REG_SYS_CTL2, ((data & (~SHUTDOWNTIME_MASK)) | SHUTDOWNTIME_16S));
        break;
      case ShutdownTime::SHUTDOWN_32S:
        ret = writeByte(IP5306_ADDR, IP5306_REG_SYS_CTL2, ((data & (~SHUTDOWNTIME_MASK)) | SHUTDOWNTIME_32S));
        break;
      case ShutdownTime::SHUTDOWN_64S:
        ret = writeByte(IP5306_ADDR, IP5306_REG_SYS_CTL2, ((data & (~SHUTDOWNTIME_MASK)) | SHUTDOWNTIME_64S));
        break;
      default:
        ret = false;
        break;
    }
    return ret;
  }
  return false;
}

/*
  default: false
  false: when the current is too small, ip5306 will automatically shut down
  note: it seem not work and has problems
        Function has disabled.(Stab for compatibility)
        This function will be removed in a future release.
*/
bool setKeepLightLoad(bool en)
{
  uint8_t data;
  if (readByte(IP5306_ADDR, IP5306_REG_SYS_CTL0, &data) == true) {
      return writeByte(IP5306_ADDR, IP5306_REG_SYS_CTL0, !en ? (data | LIGHT_LOAD_BIT) : (data & (~LIGHT_LOAD_BIT)));
  }
  return false;
}

// true: Output normally open
bool setPowerBoostKeepOn(bool en = true) {
  uint8_t data;
  if (readByte(IP5306_ADDR, IP5306_REG_SYS_CTL0, &data) == true) {
    return writeByte(IP5306_ADDR, IP5306_REG_SYS_CTL0, en ? (data | BOOST_OUT_BIT)  : (data & (~BOOST_OUT_BIT)));
  }
  return false;
}

/**
* Function has disabled.(Stab for compatibility)
* This function will be removed in a future release.
*/
bool setLowPowerShutdown(bool en)
{
  //uint8_t data;
  //if (readByte(IP5306_ADDR, IP5306_REG_SYS_CTL1, &data) == true) {
  //  return writeByte(IP5306_ADDR, IP5306_REG_SYS_CTL1, en ? (data | LOWPOWER_SHUTDOWN_BIT) : (data & (~LOWPOWER_SHUTDOWN_BIT)));
  //}
  return setPowerBoostKeepOn(!en);
}
/*
  default: true
  true: If enough load is connected, the power will turn on.
*/
bool setAutoBootOnLoad(bool en) {
  uint8_t data;
  if (readByte(IP5306_ADDR, IP5306_REG_SYS_CTL0, &data) == true) {
    return writeByte(IP5306_ADDR, IP5306_REG_SYS_CTL0, en ? (data | BOOT_ON_LOAD_BIT) : (data & (~BOOT_ON_LOAD_BIT)));
  }
  return false;
}

// if charge full,try set charge enable->disable->enable,can be recharged
bool setCharge(bool en) {
  uint8_t data;
  if (readByte(IP5306_ADDR, IP5306_REG_SYS_CTL0, &data) == true) {
      return writeByte(IP5306_ADDR, IP5306_REG_SYS_CTL0, en ? (data | CHARGE_OUT_BIT) : (data & (~CHARGE_OUT_BIT)));
  }
  return false;
}

// full return true, else return false
bool isChargeFull() {
  uint8_t data;
  return (readByte(IP5306_ADDR, IP5306_REG_READ1, &data))
         ? (data & CHARGE_FULL_BIT)
         : false;
}

// test if ip5306 is an i2c version
bool canControl() {
  return writeCommand(IP5306_ADDR, IP5306_REG_READ0);
}

//true:charge, false:discharge
bool isCharging() {
  uint8_t data;
  return (readByte(IP5306_ADDR, IP5306_REG_READ0, &data))
         ? (data & CHARGE_ENABLE_BIT)
         : false;
}

// Return percentage * 100
int8_t getBatteryLevel() {
  uint8_t data;
  if (readByte(IP5306_ADDR, IP5306_REG_READ3, &data) == true) {
    switch (data & 0xF0) {
      case 0x00:
        return 100;
      case 0x80:
        return 75;
      case 0xC0:
        return 50;
      case 0xE0:
        return 25;
      default:
        return -2;
    }
    Serial.printf("Batt_data: %d", (data & 0xF0));
  }
  return -1;
}

bool ip5306_setup() {
    Wire.begin(SDAPIN, SCKPIN);
    return setPowerBoostKeepOn();
}
