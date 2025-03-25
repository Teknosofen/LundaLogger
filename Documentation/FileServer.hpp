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
