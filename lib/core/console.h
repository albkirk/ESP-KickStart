//Variables used by telnet
#define BUFFER_PRINT 150							// length of buffer
#define MAX_TIME_INACTIVE 300000					// Maximun time for inactivity (miliseconds)

unsigned long TELNET_Timer = millis();				// To measure Telnet idle time
String bufferPrint = "";                            // buffer for printable text

// declare telnet server and client (do NOT put in setup())
WiFiServer telnetServer(23);
WiFiClient telnetClient;
WiFiClient newClient;

void telnet_print(String msg, bool Forced = false) {
		if (config.DEBUG || Forced) {
		    Serial.print(msg);
			if (config.TELNET && telnetClient && telnetClient.connected()) {  // send data to Client
				if (bufferPrint == "") {
						bufferPrint=msg;
						telnetClient.print(bufferPrint);
						telnetClient.flush();  // clear input buffer, else you get strange characters
						bufferPrint="";
						delay(10);  // to avoid strange characters left in buffer
				}
				else if (config.DEBUG) Serial.println("Buffer not empty");
			}
		}
}


void telnet_println(String msg, bool Forced = false) {
		if (config.DEBUG || Forced) {
		    Serial.println(msg);
			if (config.TELNET && telnetClient && telnetClient.connected()) {  // send data to Client
				if (bufferPrint == "") {
                        bufferPrint=msg;
                        telnetClient.println(bufferPrint);
                        telnetClient.flush();  // clear input buffer, else you get strange characters
                        bufferPrint="";
                        delay(10);  // to avoid strange characters left in buffer
				}
				else if (config.DEBUG) Serial.println("Buffer not empty");
			}
		}
}


void telnet_stop() {
    // Stop Client
    if (telnetClient && telnetClient.connected()) {
		telnet_println("Telnet Service STOP.");
        telnetClient.stop();
    }
    // Stop server
    telnetServer.stop();
}


void telnet_setup() {
    if (WIFI_state != WL_CONNECTED) telnet_println( "Telnet ERROR! ==> NO WiFi connection!" );
    else {
		if (config.TELNET){
	   		//start Telnet service
	   		telnetServer.begin();
	   		telnetServer.setNoDelay(true);
	   		telnet_println("Telnet Service READY.", true);
	   		bufferPrint.reserve(BUFFER_PRINT);
    	}
    	else telnet_stop();
	}
}

void console_prompt() {
	String prmptln;
	telnet_print( String(config.DeviceName) + String("> "), true);
}