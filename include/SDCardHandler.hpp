#ifndef SDCARD_HANDLER_HPP
#define SDCARD_HANDLER_HPP

#include <Arduino.h>
#include <SD.h>
#include <SPI.h>

class SDCardHandler {
public:
    SDCardHandler(int csPin);
    bool begin();
    bool writeFile(const char* path, const char* message);
    String readFile(const char* path);
    bool appendFile(const char* path, const char* message);

    bool writeSettings(const char* message);
    bool writeMetrics(const char* message);
    bool writeCurves(const char* message);

private:
    int _csPin;
    const char* settingsFile = "/settings.txt";
    const char* metricsFile = "/metrics.txt";
    const char* curvesFile = "/curves.txt";
};

#endif // SDCARD_HANDLER_HPP