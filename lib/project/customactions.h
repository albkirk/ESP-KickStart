// Function to insert customized MQTT Configuration actions
void custom_mqtt(String command, String cmd_value) {
    if ( command == "BckpRstr" ) {
        if (cmd_value == "") telnet_println("Restore data is empty");
        else {
            DeserializationError error = deserializeJson(config_doc, cmd_value); //Deserialize string to JSON data
            if (error) {telnet_print("JSON DeSerialization failure: "); telnet_println(error.f_str());}
            else {
                strcpy(config.DeviceName, config_doc["DeviceName"]);
                strcpy(config.Location, config_doc["Location"]);
/*
                config.LOWER_LEVEL =    config_doc["LOWER_Pos"];
                config.UPPER_LEVEL =    config_doc["UPPER_Pos"];
*/
                storage_write();
                bckp_rstr_flag = true;
                telnet_println("BckpRstr with success");
            }
        }
    }

//  if ( command == "send_Telemetry" && bool(cmd_value.toInt())) { gps_update(); print_gps_data(); send_Telemetry(); }

}

void custom_update(){
    yield();
    //mqtt_publish(mqtt_pathtele, "DEEPSLEEP", String(config.DEEPSLEEP));
    //mqtt_publish(mqtt_pathtele, "Switch_Def", String(config.SWITCH_Default));
    //ambient_data();
    //mqtt_dump_data(mqtt_pathtele, "Telemetry");
}
