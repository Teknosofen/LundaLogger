#ifndef DATALOGGER_HPP
#define DATALOGGER_HPP

#include <SD.h>
#include <SPI.h>
#include <time.h>

class DataLogger {
public:
    DataLogger(int csPin);
    void begin();
    void logWaveformData(const String &data);
    void logBreathData(const String &data);
    void logSettingsData(const String &data);

private:
    int csPin;
    int fileIndex;
    unsigned long lastFileTime;
    File waveformFile;
    File breathFile;
    File settingsFile;

    void updateFileNames();
    void logData(File &file, const String &dataType, const String &data);
};

#endif // DATALOGGER_HPP
