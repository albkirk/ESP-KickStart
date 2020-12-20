//
//  HTML PAGE
//

const char PAGE_AdminMainPage[] PROGMEM = R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />

<strong>Administration Page</strong>
<hr>
<a href="general.html"  style="width:250px" class="btn btn--m btn--blue" >General Configuration</a><br>
<a href="wireless.html" style="width:250px" class="btn btn--m btn--blue" >Wireless Configuration</a><br>
<a href="mqtt.html"     style="width:250px" class="btn btn--m btn--blue" >MQTT Configuration</a><br>
<a href="ntp.html"      style="width:250px" class="btn btn--m btn--blue" >NTP Configuration</a><br>
<a href="modem.html"    style="width:250px" class="btn btn--m btn--blue" >Modem Configuration</a><br>
<a href="gps.html"      style="width:250px" class="btn btn--m btn--blue" >GPS Configuration</a><br>
<a href="reset.html"    style="width:250px" class="btn btn--m btn--blue" >Factory Reset</a><br>
<a href="savequit.html" style="width:250px" class="btn btn--m btn--blue" >Save & Quit</a><br>

<script>
window.onload = function ()
{
    load("style.css","css", function() 
    {
        load("microajax.js","js", function() 
        {
            // Do something after load...
        });
    });
}
function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}

</script>

)=====";


