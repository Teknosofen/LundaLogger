// ServoCIE.hpp
#ifndef SERVO_CIE_HPP
#define SERVO_CIE_HPP

#include <Arduino.h>

#define NumberOfCurves 3
#define NumMetrics 15
#define numSettings 10

#define EOT 0x04
#define ESC 0x1B
#define CR 0x0D
#define LF 0x0A
#define ValueFlag 0x80
#define PhaseFlag 0x81
#define ErrorFlag 0xE0
#define EndFlag 0x7F

#define cieDataInvalid 0x7EFF
#define cieEIPInvalid 3000.0

class ServoCIE {
public:
    ServoCIE();
    void parseCIEData(char nextChar);
    void scaleMetrics();
    char calculateCRC(const String& data);
    void sendServoCommand(const String& command);
    void setupCIE();

private:
    void getCIEResponse();

    enum RunModeType {
        Awaiting_Info,
        End_Flag_Found,
        Breath_Data,
        Settings_Data,
        Value_Data,
        Phase_Data,
        Trend_Data,
        Alarm_Data,
        Error_Data,
        Run_Mode,
        StandBy
    };

    RunModeType runMode;
    int byteCount;
    int metricNo;
    int settingsNo;
    char errorInfo;
    char phase;
    char chk;
    int cieFlow;
    int cieFCO2;
    int ciePaw;
    char inByte;
    int CurveCounter;
    bool MetricsHaveArrived;
    bool settingsHaveArrived;
    float ventFlowFiltConst = 0.3;

    int16_t metricUnscaled[NumMetrics];
    float metricScaled[NumMetrics];
    float metricOffsets[NumMetrics+1] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 200.0, 200.0, 0.0, 200.0, 200.0, 0.0};
    float metricScaleFactors[NumMetrics+1] = {0.1, 0.1, 1.0, 0.1, 0.2, 0.2, 0.01, 0.01, 0.1, 0.1, 0.1, 0.01, 0.1, 0.1, 0.0001};
    String metricLabels[NumMetrics+1] = {"VtCO2", "FetCO2", "MVCO2", "RR", "VtInsp", "VtExp", "MVinsp", "MVExp", "FiO2", "PEEP", "EIP", "IE", "PPeak", "Pmean", "Ti/Ttot"};
    String metricUnits[NumMetrics+1] = {"ml", "%", "ml/min", "1/min", "mL", "mL", "L/min", "L/min", "%", "cmH2O", "cmH2O", "-", "cmH2O", "cmH2O", "-"};

    uint16_t settingsUnscaled[numSettings];
    float settingsScaled[numSettings];
    float settingsOffsets[numSettings] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    float settingsScaleFactors[numSettings] = {0.1, 0.01, 0.1, 0.1, 0.1, 0.2, 1, 1, 1, 0.01};
    String settingsLabels[numSettings] = {"RR", "MV", "PEEP", "FiO2", "Pinsp", "Vt", "mode", "PatCat", "ComplComp", "IE"};
    String settingsUnits[numSettings] = {"1/min", "L/min", "cmH2O", "%", "cmH2O", "mL", "N/A", "N/A", "N/A", "N/A"};
};

#endif // SERVO_CIE_HPP

// main.cpp Example
#include <Arduino.h>
#include "ServoCIE.hpp"

ServoCIE servoCIE;

void setup() {
    Serial.begin(115200);
    servoCIE.setupCIE();
}

void loop() {
    if (Serial.available()) {
        char receivedChar = Serial.read();
        servoCIE.parseCIEData(receivedChar);
    }
}
