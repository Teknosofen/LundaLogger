#include "main.hpp"
#include <SPI.h>
#include <SD.h>
#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include "DataLogger.hpp"
#include "MyWiFiHandler.hpp"
#include "FileServer.hpp"

MyWiFiHandler wifiHandler("your_ssid", "your_password");
FileServer fileServer(5, wifiHandler);  // SD card CS pin
DataLogger dataLogger(5, fileServer);  // SD card CS pin

void setup() {
    Serial.begin(115200);
    wifiHandler.begin();
    dataLogger.begin();
    fileServer.begin();
}

void loop() {
    // Simulate data logging
    String waveformData = "waveform data";
    String breathData = "breath data";
    String settingsData = "settings data";

    dataLogger.logWaveformData(waveformData);
    dataLogger.logBreathData(breathData);
    dataLogger.logSettingsData(settingsData);

    fileServer.sendDataUpdate(waveformData, breathData, settingsData);

    fileServer.handleClient();
    delay(1000);
}

/*






#include <SPI.h>
#include <SD.h>
#include <WiFi.h>
#include <WebServer.h>

DataLogger dataLogger(5);  // SD card CS pin
FileServer fileServer(5, "your_ssid", "your_password");

void setup() {
    Serial.begin(115200);
    dataLogger.begin();
    fileServer.begin();
}

void loop() {
    // Simulate data logging
    dataLogger.logWaveformData("waveform data");
    dataLogger.logBreathData("breath data");
    dataLogger.logSettingsData("settings data");

    fileServer.handleClient();
    delay(1000);
}

*/

/* can you please recapitulate the lunlogger project and also separate the wifi related methods into to a separate class for me and allow the other classes to use that class? */
/* excellent, can you also add a new /data html page that presents data from the three data sets that are being logged. Note that I want the HTML page to be updated as new data arrives. */

/* the usage example seems to be missing some details, can you please update it for me? */