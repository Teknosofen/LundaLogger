#include "DataLogger.hpp"

DataLogger::DataLogger(int csPin, FileServer &fileServer)
    : csPin(csPin), fileIndex(0), fileServer(fileServer) {}

void DataLogger::begin() {
    if (!SD.begin(csPin)) {
        Serial.println("SD initialization failed!");
        return;
    }
    Serial.println("SD initialization done.");
    updateFileNames();
}

void DataLogger::logWaveformData(const String &data) {
    logData(waveformFile, "waveform", data);
    fileServer.sendDataUpdate(data, "", "");
}

void DataLogger::logBreathData(const String &data) {
    logData(breathFile, "breath", data);
    fileServer.sendDataUpdate("", data, "");
}

void DataLogger::logSettingsData(const String &data) {
    logData(settingsFile, "settings", data);
    fileServer.sendDataUpdate("", "", data);
}

void DataLogger::updateFileNames() {
    time_t now = time(nullptr);
    if ((now - lastFileTime) > 12 * 3600) { // Update every 12 hours
        lastFileTime = now;
        fileIndex++;
    }
    char filename[32];
    snprintf(filename, sizeof(filename), "/waveform_%d.txt", fileIndex);
    waveformFile = SD.open(filename, FILE_WRITE);
    snprintf(filename, sizeof(filename), "/breath_%d.txt", fileIndex);
    breathFile = SD.open(filename, FILE_WRITE);
    snprintf(filename, sizeof(filename), "/settings_%d.txt", fileIndex);
    settingsFile = SD.open(filename, FILE_WRITE);
}

void DataLogger::logData(File &file, const String &dataType, const String &data) {
    if (!file) {
        Serial.println("Failed to open file for writing");
        return;
    }
    time_t now = time(nullptr);
    String timestamp = String(now);
    file.println(timestamp + " " + dataType + ": " + data);
    file.flush();
    updateFileNames();
}

/* 
#include "DataLogger.hpp"

DataLogger::DataLogger(int csPin) : csPin(csPin), fileIndex(0) {}

void DataLogger::begin() {
    if (!SD.begin(csPin)) {
        Serial.println("SD initialization failed!");
        return;
    }
    Serial.println("SD initialization done.");
    updateFileNames();
}

void DataLogger::logWaveformData(const String &data) {
    logData(waveformFile, "waveform", data);
}

void DataLogger::logBreathData(const String &data) {
    logData(breathFile, "breath", data);
}

void DataLogger::logSettingsData(const String &data) {
    logData(settingsFile, "settings", data);
}

void DataLogger::updateFileNames() {
    time_t now = time(nullptr);
    if ((now - lastFileTime) > 12 * 3600) { // Update every 12 hours
        lastFileTime = now;
        fileIndex++;
    }
    char filename[32];
    snprintf(filename, sizeof(filename), "/waveform_%d.txt", fileIndex);
    waveformFile = SD.open(filename, FILE_WRITE);
    snprintf(filename, sizeof(filename), "/breath_%d.txt", fileIndex);
    breathFile = SD.open(filename, FILE_WRITE);
    snprintf(filename, sizeof(filename), "/settings_%d.txt", fileIndex);
    settingsFile = SD.open(filename, FILE_WRITE);
}

void DataLogger::logData(File &file, const String &dataType, const String &data) {
    if (!file) {
        Serial.println("Failed to open file for writing");
        return;
    }
    time_t now = time(nullptr);
    String timestamp = String(now);
    file.println(timestamp + " " + dataType + ": " + data);
    file.flush();
    updateFileNames();
}
*/
