

//
//  HTML PAGE
//
const char PAGE_WirelessConfiguration[] PROGMEM = R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<a href="admin.html"  class="btn btn--s"><</a>&nbsp;&nbsp;<strong>Wireless Configuration</strong>
<hr>
Connect to Router with these settings:<br>
<form action="" method="get">
<table border="0"  cellspacing="0" cellpadding="3" style="width:360px" >
<tr><td align="right">WiFi SSID:</td><td><input type="text" id="ssid" name="ssid" value=""></td></tr>
<tr><td align="right">WiFi Key:</td><td><input type="text" id="WiFiKey" name="WiFiKey" value=""></td></tr>
<tr><td align="right">STA Mode:</td><td><input type="checkbox" id="STAEnabled" name="STAEnabled"></td></tr>
<tr><td align="right">AP Mode:</td><td><input type="checkbox" id="APEnabled" name="APEnabled"></td></tr>
<tr><td align="right">DHCP:</td><td><input type="checkbox" id="dhcp" name="dhcp"></td></tr>
<tr><td align="right">IP:     </td><td><input type="text" id="ip_0" name="ip_0" size="2">.<input type="text" id="ip_1" name="ip_1" size="2">.<input type="text" id="ip_2" name="ip_2" size="2">.<input type="text" id="ip_3" name="ip_3" size="2"></td></tr>
<tr><td align="right">Netmask:</td><td><input type="text" id="nm_0" name="nm_0" size="2">.<input type="text" id="nm_1" name="nm_1" size="2">.<input type="text" id="nm_2" name="nm_2" size="2">.<input type="text" id="nm_3" name="nm_3" size="2"></td></tr>
<tr><td align="right">Gateway:</td><td><input type="text" id="gw_0" name="gw_0" size="2">.<input type="text" id="gw_1" name="gw_1" size="2">.<input type="text" id="gw_2" name="gw_2" size="2">.<input type="text" id="gw_3" name="gw_3" size="2"></td></tr>
<tr><td align="right">DNS IP: </td><td><input type="text" id="dn_0" name="dn_0" size="2">.<input type="text" id="dn_1" name="dn_1" size="2">.<input type="text" id="dn_2" name="dn_2" size="2">.<input type="text" id="dn_3" name="dn_3" size="2"></td></tr>
<tr><td colspan="2" align="center"><input type="submit" style="width:150px" class="btn btn--m btn--blue" value="Save"></td></tr>
</table>
</form>
<hr>
<strong>Connection State:</strong>
<table border="0" cellspacing="0" cellpadding="3" style="width:320px" >
<tr><td><div id="connectionstate">N/A</div></td></tr>
<tr><td align="left">WiFi Mode :</td><td><span id="x_wlanmode"></span></td></tr>
<tr><td align="left">RSSI :</td><td><span id="x_wlanRSSI"></span></td></tr></table>
<hr>
<strong>Networks:</strong><br>
<table border="0"  cellspacing="3" style="width:320px" >
<tr><td><div id="networks">Scanning...</div></td></tr>
<tr><td align="center"><a href="javascript:GetState()" style="width:150px" class="btn btn--m btn--blue">Refresh</a></td></tr>
</table>


<script>

function GetState()
{
    setValues("/admin/connectionstate");
}
function selssid(value)
{
    document.getElementById("ssid").value = value; 
}


window.onload = function ()
{
    load("style.css","css", function() 
    {
        load("microajax.js","js", function() 
        {
                    setValues("/admin/wirelessvalues");
                    setTimeout(GetState,3000);
        });
    });
}
function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}




</script>


)=====";


const char PAGE_WiiFiDHCPConfiguration[] PROGMEM = R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<a href="admin.html"  class="btn btn--s"><</a>&nbsp;&nbsp;<strong>Wireless Configuration</strong>
<hr>
Connect to Router with these settings:<br>
<form action="" method="get">
<table border="0"  cellspacing="0" cellpadding="3" style="width:360px" >
<tr><td align="right">WiFi SSID:</td><td><input type="text" id="ssid" name="ssid" value=""></td></tr>
<tr><td align="right">WiFi Key:</td><td><input type="text" id="WiFiKey" name="WiFiKey" value=""></td></tr>
<tr><td align="right">STA Mode:</td><td><input type="checkbox" id="STAEnabled" name="STAEnabled"></td></tr>
<tr><td align="right">AP Mode:</td><td><input type="checkbox" id="APEnabled" name="APEnabled"></td></tr>
<tr><td align="right">DHCP:</td><td><input type="checkbox" id="dhcp" name="dhcp"></td></tr>
<tr><td align="right">IP:</td><td><span id="x_ip"></span></td></tr>
<tr><td align="right">Netmask:</td><td><span id="x_netmask"></span></td></tr>
<tr><td align="right">Gateway:</td><td><span id="x_gateway"></span></td></tr>
<tr><td align="right">DNS IP:</td><td><span id="x_dns_ip"></span></td></tr>
<tr><td colspan="2" align="center"><input type="submit" style="width:150px" class="btn btn--m btn--blue" value="Save"></td></tr>
</table>
</form>
<hr>
<strong>Connection State:</strong>
<table border="0" cellspacing="0" cellpadding="0" style="width:320px" >
<tr><td><div id="connectionstate">N/A</div></td></tr>
<tr><td align="left">WiFi Mode :</td><td><span id="x_wlanmode"></span></td></tr>
<tr><td align="left">RSSI :</td><td><span id="x_wlanRSSI"></span></td></tr>
</table>
<hr>
<strong>Networks:</strong><br>
<table border="0"  cellspacing="3" style="width:320px" >
<tr><td><div id="networks">Scanning...</div></td></tr>
<tr><td align="center"><a href="javascript:GetState()" style="width:150px" class="btn btn--m btn--blue">Refresh</a></td></tr>
</table>


<script>

function GetState()
{
    setValues("/admin/connectionstate");
}
function selssid(value)
{
    document.getElementById("ssid").value = value; 
}


window.onload = function ()
{
    load("style.css","css", function() 
    {
        load("microajax.js","js", function() 
        {
                    setValues("/admin/wirelessvalues");
                    setTimeout(GetState,3000);
        });
    });
}
function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}




</script>


)=====";

const char PAGE_WaitAndReload[] PROGMEM = R"=====(
<meta http-equiv="refresh" content="5; URL=wireless.html">
Configuring. Please Wait.... You may need to refresh your browser.
)=====";


//
//  SEND HTML PAGE OR IF A FORM SUMBITTED VALUES, PROCESS THESE VALUES
// 

void send_wireless_configuration_html()
{
    
    if (MyWebServer.args() > 0 )  // Save Settings
    {
        String temp = "";
        config.STAMode = false;
        config.APMode = false;
        config.DHCP = false;
        for ( uint8_t i = 0; i < MyWebServer.args(); i++ ) { 
            if (MyWebServer.argName(i) == "ssid") strcpy(config.SSID,   urldecode(MyWebServer.arg(i)).c_str());
            if (MyWebServer.argName(i) == "WiFiKey" && urldecode(MyWebServer.arg(i)) != "") strcpy(config.WiFiKey, urldecode(MyWebServer.arg(i)).c_str());
            if (MyWebServer.argName(i) == "STAEnabled") config.STAMode = true;
            if (MyWebServer.argName(i) == "APEnabled") config.APMode = true;
            if (MyWebServer.argName(i) == "dhcp") config.DHCP = true;
        }
        if (!config.DHCP) {
            for ( uint8_t i = 0; i < MyWebServer.args(); i++ ) { 
                if (MyWebServer.argName(i) == "ip_0") if (checkRange(MyWebServer.arg(i)))     config.IP[0] =  MyWebServer.arg(i).toInt();
                if (MyWebServer.argName(i) == "ip_1") if (checkRange(MyWebServer.arg(i)))     config.IP[1] =  MyWebServer.arg(i).toInt();
                if (MyWebServer.argName(i) == "ip_2") if (checkRange(MyWebServer.arg(i)))     config.IP[2] =  MyWebServer.arg(i).toInt();
                if (MyWebServer.argName(i) == "ip_3") if (checkRange(MyWebServer.arg(i)))     config.IP[3] =  MyWebServer.arg(i).toInt();
                if (MyWebServer.argName(i) == "nm_0") if (checkRange(MyWebServer.arg(i)))     config.Netmask[0] =  MyWebServer.arg(i).toInt();
                if (MyWebServer.argName(i) == "nm_1") if (checkRange(MyWebServer.arg(i)))     config.Netmask[1] =  MyWebServer.arg(i).toInt();
                if (MyWebServer.argName(i) == "nm_2") if (checkRange(MyWebServer.arg(i)))     config.Netmask[2] =  MyWebServer.arg(i).toInt();
                if (MyWebServer.argName(i) == "nm_3") if (checkRange(MyWebServer.arg(i)))     config.Netmask[3] =  MyWebServer.arg(i).toInt();
                if (MyWebServer.argName(i) == "gw_0") if (checkRange(MyWebServer.arg(i)))     config.Gateway[0] =  MyWebServer.arg(i).toInt();
                if (MyWebServer.argName(i) == "gw_1") if (checkRange(MyWebServer.arg(i)))     config.Gateway[1] =  MyWebServer.arg(i).toInt();
                if (MyWebServer.argName(i) == "gw_2") if (checkRange(MyWebServer.arg(i)))     config.Gateway[2] =  MyWebServer.arg(i).toInt();
                if (MyWebServer.argName(i) == "gw_3") if (checkRange(MyWebServer.arg(i)))     config.Gateway[3] =  MyWebServer.arg(i).toInt();
                if (MyWebServer.argName(i) == "dn_0") if (checkRange(MyWebServer.arg(i)))     config.DNS_IP[0] =  MyWebServer.arg(i).toInt();
                if (MyWebServer.argName(i) == "dn_1") if (checkRange(MyWebServer.arg(i)))     config.DNS_IP[1] =  MyWebServer.arg(i).toInt();
                if (MyWebServer.argName(i) == "dn_2") if (checkRange(MyWebServer.arg(i)))     config.DNS_IP[2] =  MyWebServer.arg(i).toInt();
                if (MyWebServer.argName(i) == "dn_3") if (checkRange(MyWebServer.arg(i)))     config.DNS_IP[3] =  MyWebServer.arg(i).toInt();
            }

        }
        yield();
        MyWebServer.send ( 200, "text/html", PAGE_WaitAndReload );
        wifi_connect();
        if ( WIFI_state == WL_CONNECTED || config.STAMode == false ) storage_write();
        if (config.DEBUG) storage_print();
        AdminTimeOutCounter=0;
        
    }
    else
    {
        if (config.DHCP) MyWebServer.send ( 200, "text/html", PAGE_WiiFiDHCPConfiguration ); 
        else MyWebServer.send ( 200, "text/html", PAGE_WirelessConfiguration ); 
    }
    Serial.println(__FUNCTION__); 
}



//
//   FILL THE PAGE WITH VALUES
//

void send_wireless_configuration_values_html()
{

    String values ="";

    values += "ssid|" + (String) config.SSID + "|input\n";
    //values += "WiFiKey|" +  (String) config.WiFiKey  + "|input\n";   // KEEP IT COMMENTED TO NOT SHOW THE WiFi KEY!!!
    values += "STAEnabled|" + (String) (config.STAMode ? "checked" : "") + "|chk\n";
    values += "APEnabled|" + (String) (config.APMode ? "checked" : "") + "|chk\n";
    values += "dhcp|" +  (String) (config.DHCP ? "checked" : "") + "|chk\n";
    if (config.DHCP) {
          values += "x_ip|" +  (String) WiFi.localIP()[0] + "." +  (String) WiFi.localIP()[1] + "." +  (String) WiFi.localIP()[2] + "." + (String) WiFi.localIP()[3] +  "|div\n";
          values += "x_gateway|" +  (String) WiFi.gatewayIP()[0] + "." +  (String) WiFi.gatewayIP()[1] + "." +  (String) WiFi.gatewayIP()[2] + "." + (String) WiFi.gatewayIP()[3] +  "|div\n";
          values += "x_netmask|" +  (String) WiFi.subnetMask()[0] + "." +  (String) WiFi.subnetMask()[1] + "." +  (String) WiFi.subnetMask()[2] + "." + (String) WiFi.subnetMask()[3] +  "|div\n";
          values += "x_dns_ip|" +  (String) WiFi.dnsIP()[0] + "." +  (String) WiFi.dnsIP()[1] + "." +  (String) WiFi.dnsIP()[2] + "." + (String) WiFi.dnsIP()[3] +  "|div\n";
    }
    else {
        values += "ip_0|" +  (String) config.IP[0] + "|input\n";
        values += "ip_1|" +  (String) config.IP[1] + "|input\n";
        values += "ip_2|" +  (String) config.IP[2] + "|input\n";
        values += "ip_3|" +  (String) config.IP[3] + "|input\n";
        values += "nm_0|" +  (String) config.Netmask[0] + "|input\n";
        values += "nm_1|" +  (String) config.Netmask[1] + "|input\n";
        values += "nm_2|" +  (String) config.Netmask[2] + "|input\n";
        values += "nm_3|" +  (String) config.Netmask[3] + "|input\n";
        values += "gw_0|" +  (String) config.Gateway[0] + "|input\n";
        values += "gw_1|" +  (String) config.Gateway[1] + "|input\n";
        values += "gw_2|" +  (String) config.Gateway[2] + "|input\n";
        values += "gw_3|" +  (String) config.Gateway[3] + "|input\n";
        values += "dn_0|" +  (String) config.DNS_IP[0] + "|input\n";
        values += "dn_1|" +  (String) config.DNS_IP[1] + "|input\n";
        values += "dn_2|" +  (String) config.DNS_IP[2] + "|input\n";
        values += "dn_3|" +  (String) config.DNS_IP[3] + "|input\n";        
    }
    MyWebServer.send ( 200, "text/plain", values);
    Serial.println(__FUNCTION__); 
    
}


//
//   FILL THE PAGE WITH NETWORKSTATE & NETWORKS
//

void send_connection_state_values_html()
{

    String state = "N/A";
    String Networks = "";
    if (WiFi.status() >= 0 && WiFi.status() <=6) state = WIFI_state_Name[WiFi.status()];

     int n = WiFi.scanNetworks();

     if (n == 0)
     {
         Networks = "<font color='#FF0000'>No networks found!</font>";
     }
    else
    {
     
        
        Networks = "Found " + String(n) + " Networks<br>";
        Networks += "<table border='0' cellspacing='0' cellpadding='3'>";
        Networks += "<tr bgcolor='#DDDDDD' ><td><strong>Name</strong></td><td><strong>Quality</strong></td><td><strong>Enc</strong></td><tr>";
        for (int i = 0; i < n; ++i)
        {
            int quality=0;
            if(WiFi.RSSI(i) <= -100)
            {
                    quality = 0;
            }
            else if(WiFi.RSSI(i) >= -50)
            {
                    quality = 100;
            }
            else
            {
                quality = 2 * (WiFi.RSSI(i) + 100);
            }


            Networks += "<tr><td><a href='javascript:selssid(\""  +  String(WiFi.SSID(i))  + "\")'>"  +  String(WiFi.SSID(i))  + "</a></td><td>" +  String(quality) + "%</td><td>" +  String((WiFi.encryptionType(i) == 0)?" ":"*")  + "</td></tr>"; // 0=>authenticate mode OPEN
        }
        Networks += "</table>";
    }
   
    String values ="";
    values += "connectionstate|" + state + "|div\n";
    #ifdef ESP8266
        values += "x_wlanmode|" + WIFI_PHY_Mode_Name[WiFi.getPhyMode()]  + "|div\n";
    #else
        values += "x_wlanmode|" + WIFI_PHY_Mode_Name[4]  + "|div\n";
    #endif
    values += "x_wlanRSSI|" + String(WiFi.RSSI())  + "|div\n";
    values += "networks|" +  Networks + "|div\n";
    MyWebServer.send ( 200, "text/plain", values);
    Serial.println(__FUNCTION__); 

}
