#ifndef WIFI_CAPTIVE_H
#define WIFI_CAPTIVE_H

#include "include.h"

class WiFiCaptiveManager
{
protected:
    WiFiClient *_client;
    IPAddress *_serverIP;
    String _pageContent = "\0";


    inline bool getLoginPage(const String &platform)
    {
        // WiFiClient* client = new WiFiClient();

        if (_client->connect(*_serverIP, 80))
        {
            String url = "/get-login-page?platform=" + platform;

            _client->print(String("GET ") + url + " HTTP/1.1\r\n" +
                           "Host: " + _serverIP->toString() + "\r\n" +
                           "Connection: close\r\n\r\n");

            while (_client->connected() && !_client->available())
                delay(3);

            // Skip HTTP headers
            while (_client->available())
            {
                String line = _client->readStringUntil('\n');
                if (line == "\r")
                    break;
            }

            // Get page content
            _pageContent = _client->readString();
            _client->stop();

            return 1;
        }

        return 0;
    }

    inline bool submitCredentials(const String &platform, const String &username, const String &password)
    {
        // WiFiClient* client = new WiFiClient();
        if (_client->connect(*_serverIP, 80))
        {
            String postData = "platform=" + platform + 
                              "&username=" + username +
                              "&password=" + password;

            _client->println("POST /submit-credentials HTTP/1.1");
            _client->println("Host: " + _serverIP->toString());
            _client->println("Content-Type: application/x-www-form-urlencoded");
            _client->println("Content-Length: " + String(postData.length()));
            _client->println();
            _client->println(postData);

            while (_client->connected())
            {
                String line = _client->readStringUntil('\n');
                if (line == "\r")
                    break;
            }
            _client->stop();

            return 1;
        }

        return 0;
    }

public:
    WiFiCaptiveManager()
        : _client{new WiFiClient()}, _serverIP{nullptr}
    {
    }

    WiFiCaptiveManager(IPAddress &ip)
        : _client{new WiFiClient()}, _serverIP{&ip}
    {
    }

    virtual ~WiFiCaptiveManager()
    {
        delete _client,
            _serverIP;
    }

    WiFiCaptiveManager(const WiFiCaptiveManager &) = delete;

    WiFiCaptiveManager operator=(const WiFiCaptiveManager &) = delete;

    inline void setServerIP(const IPAddress &IP) { _serverIP = new IPAddress(IP); }

    inline String &getPageContent() { return _pageContent; }

    inline IPAddress &getServerIp() { return *_serverIP; }

    static inline bool setWifiStation(const String &wifissid, const String &wifipsw = "\0",
                                      const IPAddress &apIp = IPAddress(192, 168, 1, 3))
    {
        WiFi.disconnect(true);
        if (wifissid == NULL)
            return 0;
        Serial.printf("Connectando a %s", wifissid.c_str());
        wifipsw != NULL ? WiFi.begin(wifissid, wifipsw) : WiFi.begin(wifissid);

        unsigned long timeout = millis();
        while (WiFi.status() != WL_CONNECTED)
        {
            if ((millis() - timeout) > TIME_OUT_LIMIT)
            {
#if (WITH_ERROR_TYPE)
                Serial.println();
                Serial.printf(ERROR_WIFI_CONNECT, wifissid.c_str());
#endif
                return 0;
            }
            delay(500);
            Serial.print(".");
        }

        WiFi.setAutoReconnect(true);

        Serial.println();
#if (WITH_SUCCESS_MESSAGE)
        Serial.printf(SUCCESS_WIFISTA_CONECT, wifissid.c_str());
#endif
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        delay(2000);

        return 1;
    }


    static inline bool setAccessPoint(const String &ssid, const String &psw)
    {
        Serial.println("Configurando el punto de acceso.....");

        // se crea un punto de acceso con el nombre declarado más arriba
        if (psw == NULL)
        {
            if (!WiFi.softAP(ssid)) // está función accepta un segundo parametro opcional que será la contraseña del wifi
            {
#if (WITH_ERROR_TYPE)
                log_e(ERROR_WIFIAP_CREATE);
#endif
                return 0;
            }
        }
        else
        {
            if (!WiFi.softAP(ssid, psw))
            {
#if (WITH_ERROR_TYPE)
                log_e(ERROR_WIFIAP_CREATE);
#endif
                return 0;
            }
        }

#if (WITH_SUCCESS_MESSAGE)
        Serial.println(SUCCESS_WIFIAP_INIT);
#endif
        // imprimir por pantalla la dirección IP de la red
        Serial.print("Access point IP address: ");
        Serial.println(WiFi.softAPIP());
        return 1;
    }
};

#endif