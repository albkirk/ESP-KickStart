#include <LittleFS.h>                       // Include the SPIFFS library
#define MAX_File_Zize 131072                // 128KB
#define Telemetry_FILE "/telemetry.txt"
File Flash_file;


//
//  FLASH functions
//

bool flash_save_data(String *data_source, const char *path_name = Telemetry_FILE) {
    Flash_file = LittleFS.open(path_name, "a");
    if (!Flash_file) {
        Serial.println("Unable To Open '" + String(path_name) + "' for Appending");
        return false;
    }
    else if (Flash_file.size() >= MAX_File_Zize) {
        Serial.println("File is already BIG!");
        return false;
    }
    else {
        Serial.println("Appending data to file '" + String(path_name) + "'");
        Flash_file.println(*data_source);
        Flash_file.close();
        return true;
    }
}

bool flash_get_data(String *data_dest, bool from_begin = true, const char *path_name = Telemetry_FILE) {
    if (from_begin) {
        Flash_file = LittleFS.open(path_name, "r");
    }
    if (!Flash_file) {
        Serial.println("File '" + String(path_name) + "' not found");
        return false;
    }
    else {
        Serial.print("Getting data from file '" + String(path_name) + "'");
        *data_dest = Flash_file.readStringUntil('\n');
        //Serial.println("Data:  '" + *data_dest);
        if(Flash_file.position() == Flash_file.size() ) {
            Flash_file.close();
            LittleFS.remove(path_name);
        }
        return true;
    }

}    

void flash_setup() {
    LittleFS.begin();                 // Start the SPI Flash Files System
}


void mqtt_dump_data(String subpath, String subtopic) {
    static String dumpvalue;                                       // mem space to handle msg payload
    bool first_time = true;
    while (flash_get_data(&dumpvalue, first_time)) {
        mqtt_publish(subpath, subtopic, dumpvalue);
        first_time = false;
    } 
}