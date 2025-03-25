#ifndef FILESERVER_HPP
#define FILESERVER_HPP

#include <WebServer.h>
#include <WebSocketsServer.h>
#include <SD.h>
#include <SPI.h>

class FileServer {
public:
    FileServer(int csPin, MyWiFiHandler& wifiHandler);
    void begin();
    void handleClient();
    void sendDataUpdate(const String &waveformData, const String &breathData, const String &settingsData);

private:
    int csPin;
    WebServer server;
    WebSocketsServer webSocket;
    // MyWiFiManager &wifiHandler;
    
    String waveformData;
    String breathData;
    String settingsData;

    void handleRoot();
    void handleDownload();
    void handleData();
    void handleWebSocketMessage(uint8_t num, WStype_t type, uint8_t *payload, size_t length);
};

#endif // FILESERVER_HPP
