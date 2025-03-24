#ifndef FILESERVER_HPP
#define FILESERVER_HPP

#include <WebServer.h>
#include <WebSocketsServer.h>
#include <SD.h>
#include <SPI.h>
#include "MyWiFiHandler.hpp"

class FileServer {
public:
    FileServer(int csPin, MyWiFiHandler &wifiHandler);
    void begin();
    void handleClient();
    void sendDataUpdate(const String &waveformData, const String &breathData, const String &settingsData);

private:
    int csPin;
    WebServer server;
    WebSocketsServer webSocket;
    MyWiFiHandler &wifiHandler;
    
    String waveformData;
    String breathData;
    String settingsData;

    void handleRoot();
    void handleDownload();
    void handleData();
    void handleWebSocketMessage(uint8_t num, WStype_t type, uint8_t *payload, size_t length);
};

#endif // FILESERVER_HPP

/* 

#ifndef FILESERVER_HPP
#define FILESERVER_HPP

#include <WebServer.h>
#include <SD.h>
#include <SPI.h>
#include "WiFiHandler.hpp"

class FileServer {
public:
    FileServer(int csPin, WiFiHandler &wifiHandler);
    void begin();
    void handleClient();
    void sendDataEvent(const String &waveformData, const String &breathData, const String &settingsData);

private:
    int csPin;
    WebServer server;
    WiFiHandler &wifiHandler;

    void handleRoot();
    void handleDownload();
    void handleData();
    void handleSSE();
};

#endif // FILESERVER_HPP
*/

/* OLD STUFF 
#ifndef FILESERVER_HPP
#define FILESERVER_HPP

#include <WiFi.h>
#include <WebServer.h>
#include <SD.h>
#include <SPI.h>

class FileServer {
public:
    FileServer(int csPin, const char *ssid, const char *password);
    void begin();
    void handleClient();

private:
    int csPin;
    WebServer server;
    const char *ssid;
    const char *password;

    void handleRoot();
    void handleDownload();
};

#endif // FILESERVER_HPP
*/