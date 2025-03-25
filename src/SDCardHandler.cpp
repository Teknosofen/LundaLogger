#include "SDCardHandler.hpp"

SDCardHandler::SDCardHandler(int csPin) : _csPin(csPin) {}

bool SDCardHandler::begin() {
    if (!SD.begin(_csPin)) {
        Serial.println("SD Card initialization failed!");
        return false;
    }
    Serial.println("SD Card initialized.");
    return true;
}

bool SDCardHandler::writeFile(const char* path, const char* message) {
    File file = SD.open(path, FILE_WRITE);
    if (!file) {
        Serial.println("Failed to open file for writing");
        return false;
    }
    if (file.print(message)) {
        Serial.println("File written successfully");
    } else {
        Serial.println("Write failed");
    }
    file.close();
    return true;
}

String SDCardHandler::readFile(const char* path) {
    File file = SD.open(path);
    if (!file) {
        Serial.println("Failed to open file for reading");
        return "";
    }
    String content;
    while (file.available()) {
        content += (char)file.read();
    }
    file.close();
    return content;
}

bool SDCardHandler::appendFile(const char* path, const char* message) {
    File file = SD.open(path, FILE_APPEND);
    if (!file) {
        Serial.println("Failed to open file for appending");
        return false;
    }
    if (file.print(message)) {
        Serial.println("File appended successfully");
    } else {
        Serial.println("Append failed");
    }
    file.close();
    return true;
}

bool SDCardHandler::writeSettings(const char* message) {
    return writeFile(settingsFile, message);
}

bool SDCardHandler::writeMetrics(const char* message) {
    return writeFile(metricsFile, message);
}

bool SDCardHandler::writeCurves(const char* message) {
    return writeFile(curvesFile, message);
}



// ******************* USAGE

/*
#include <Arduino.h>
#include "SDCardHandler.hpp"

SDCardHandler sdCardHandler(5); // Assuming CS pin is 5

void setup() {
    Serial.begin(115200);
    if (sdCardHandler.begin()) {
        sdCardHandler.writeSettings("Settings data");
        sdCardHandler.writeMetrics("Metrics data");
        sdCardHandler.writeCurves("Curves data");

        String settingsContent = sdCardHandler.readFile("/settings.txt");
        Serial.println("Settings file content: " + settingsContent);

        String metricsContent = sdCardHandler.readFile("/metrics.txt");
        Serial.println("Metrics file content: " + metricsContent);

        String curvesContent = sdCardHandler.readFile("/curves.txt");
        Serial.println("Curves file content: " + curvesContent);
    }
}

void loop() {
    // Your main loop code
} */