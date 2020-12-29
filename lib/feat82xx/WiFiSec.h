#ifdef ESP8266

#ifndef WIFISEC_H
#define WIFISEC_H

#include <ESP8266WiFi.h>

class WiFiSec
{
public:
    WiFiSec(
        const char *ca_cert_PROG_,
        const char *client_cert_PROG_,
        const char *client_key_PROG_
    )
     : ca_cert_PROG(ca_cert_PROG_)
     , client_cert_PROG(client_cert_PROG_)
     , client_key_PROG(client_key_PROG_)

     , x509CaCert(ca_cert_PROG)
     , x509ClientCert(client_cert_PROG)
     , PrivateClientKey(client_key_PROG)
    {
        randomSeed(micros());
        wifiClient.setTrustAnchors(&x509CaCert);
        wifiClient.setClientRSACert(&x509ClientCert, &PrivateClientKey);
    }

    virtual ~WiFiSec() { }

    WiFiClient & getWiFiClient() {
        return wifiClient;
    }

    void SecDummy();

private:

    //TODO: ?? the following three may not need to be persisted ??
    const char *ca_cert_PROG;
    const char *client_cert_PROG;
    const char *client_key_PROG; // KEEP THIS VALUE PRIVATE AND SECURE!!!

    BearSSL::WiFiClientSecure wifiClient;
    BearSSL::X509List x509CaCert;
    BearSSL::X509List x509ClientCert;
    BearSSL::PrivateKey PrivateClientKey;
};

#endif      /* WIFISEC_H  */

#endif      /*  ESP8266   */