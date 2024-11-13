//JSON Variables
char telemetry_jsonString[256];
DynamicJsonDocument telemetry_doc(256);


void send_Telemetry() {
    char fbat[3];        // long enough to hold complete floating string
    custom_ambient_get_data();
    //ambient_send_data();
    // Purge old JSON data and Load new values
    telemetry_doc.clear();
    //telemetry_doc["Interval"] = config.SLEEPTime;
    telemetry_doc["Timestamp"] = curUnixTime();
    telemetry_doc["BatLevel"] = String(dtostrf(getBattLevel(),3,0,fbat)).toFloat();
    telemetry_doc["RSSI"] = getRSSI();
    telemetry_doc["Temperature"] = String(Temperature, 2);
    telemetry_doc["Humidity"] = String(Humidity, 2);
    telemetry_doc["Illuminance"] = String(Lux, 2);    
    telemetry_doc["HumVelocity"] = String(HumVelocity, 2);     // Delta Humdity per minute
    telemetry_doc["Interval"] = String(rtcData.ByteValue*config.DEEPSLEEP);
    if (config.HW_Module){
        gps_update();
        if (GPS_Valid){
            telemetry_doc["Sat"] = String(GPS_Sat).toInt();
            telemetry_doc["Lat"] = String(GPS_Lat, 6).toFloat();
            telemetry_doc["Lng"] = String(GPS_Lng, 6).toFloat();
            telemetry_doc["Speed"] = String(GPS_Speed, 1).toFloat();
        }
    }

    serializeJson(telemetry_doc, telemetry_jsonString);             //Serialize JSON data to string
    //telnet_println("Telemetry: " + String(telemetry_jsonString));
    mqtt_publish(mqtt_pathtele, "Telemetry", String(telemetry_jsonString));
}
