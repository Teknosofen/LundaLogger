#ifndef WIFIMANAGER_HPP
#define WIFIMANAGER_HPP

// #include <WiFi.h>
#include "main.hpp"

class MyWiFiHandler {
public:
    MyWiFiHandler(const char *ssid, const char *password);
    void begin();

private:
    const char *ssid;
    const char *password;
};

#endif // WIFIMANAGER_HPP
