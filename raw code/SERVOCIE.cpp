// ServoCIE.cpp
#include "ServoCIE.hpp"

ServoCIE::ServoCIE() : runMode(Awaiting_Info), byteCount(0), metricNo(0), settingsNo(0), errorInfo(0), phase(0) {
    memset(metricUnscaled, 0, sizeof(metricUnscaled));
    memset(metricScaled, 0, sizeof(metricScaled));
    memset(settingsUnscaled, 0, sizeof(settingsUnscaled));
    memset(settingsScaled, 0, sizeof(settingsScaled));
}

void ServoCIE::parseCIEData(char nextChar) {
    switch (runMode) {
        case Awaiting_Info:
            switch (nextChar) {
                case 'S':
                    runMode = Settings_Data;
                    settingsNo = 0;
                    byteCount = 2;
                    break;
                case 'B':
                    runMode = Breath_Data;
                    metricNo = 0;
                    byteCount = 2;
                    break;
                case 'T':
                    runMode = Trend_Data;
                    break;
                case 'A':
                    runMode = Alarm_Data;
                    break;
                case 0xE0:
                    runMode = Error_Data;
                    byteCount = 1;
                    break;
                case valueFlag:
                    runMode = Value_Data;
                    byteCount = 2;
                    break;
                case phaseFlag:
                    runMode = Phase_Data;
                    byteCount = 1;
                    break;
                case endFlag:
                    runMode = Awaiting_Info;
                    break;
                default:
                    break;
            }
            break;

        case End_Flag_Found:
            runMode = Awaiting_Info;
            break;

        case Breath_Data:
            if (byteCount == 2) {
                metricUnscaled[metricNo] = 256 * nextChar;
                --byteCount;
            } else if (byteCount == 1) {
                metricUnscaled[metricNo] += nextChar;
                metricScaled[metricNo] = metricUnscaled[metricNo] * metricScaleFactors[metricNo] - metricOffsets[metricNo];
                --byteCount;
            } else if (byteCount == 0 && nextChar == endFlag) {
                runMode = End_Flag_Found;
            } else {
                ++metricNo;
                metricUnscaled[metricNo] = 256 * nextChar;
                byteCount = 1;
            }
            break;

        case Settings_Data:
            if (byteCount == 2) {
                settingsUnscaled[settingsNo] = 256 * nextChar;
                --byteCount;
            } else if (byteCount == 1) {
                settingsUnscaled[settingsNo] += nextChar;
                settingsScaled[settingsNo] = settingsUnscaled[settingsNo] * settingsScaleFactors[settingsNo] - settingsOffsets[settingsNo];
                --byteCount;
            } else if (byteCount == 0 && nextChar == endFlag) {
                runMode = End_Flag_Found;
            } else {
                ++settingsNo;
                settingsUnscaled[settingsNo] = 256 * nextChar;
                byteCount = 1;
            }
            break;

        default:
            break;
    }
}

void ServoCIE::scaleMetrics() {
    for (int i = 0; i < 20; ++i) {
        metricScaled[i] = metricUnscaled[i] * metricScaleFactors[i] - metricOffsets[i];
    }
}

char ServoCIE::calculateCRC(const String& data) {
    char crc = 0;
    for (char c : data) {
        crc ^= c;
    }
    return crc;
}

void ServoCIE::sendServoCommand(const String& command) {
    char crc = calculateCRC(command);
    Serial.print(command);
    Serial.print(crc, HEX);
    Serial.write(eot);
}

void ServoCIE::setupCIE() {
    Serial.write(eot);
    getCIEResponse();

    Serial.write(esc);
    getCIEResponse();

    sendServoCommand("RTIM");
    getCIEResponse();

    sendServoCommand("RCTY");
    getCIEResponse();

    sendServoCommand("SDADB113114117100102101103104109108107122105106128");
    getCIEResponse();

    sendServoCommand("SDADS400405408414406420410409437419");
    getCIEResponse();

    sendServoCommand("SDADC000004001");
    getCIEResponse();

    sendServoCommand("RCCO102");
    getCIEResponse();

    sendServoCommand("RDAD");
    getCIEResponse();

    sendServoCommand("RADAB");
    getCIEResponse();

    sendServoCommand("RADAS");
    getCIEResponse();

    sendServoCommand("RADC");
    getCIEResponse();
}

void ServoCIE::getCIEResponse() {
    delay(30);
    while (Serial.available()) {
        char inByte = Serial.read();
        Serial.print(inByte);
    }
}
