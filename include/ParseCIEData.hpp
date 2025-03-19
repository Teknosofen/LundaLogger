#ifndef PARSECIE_DATA_HPP
#define PARSECIE_DATA_HPP

#include <Arduino.h>

class ParseCIEData {
public:
    ParseCIEData();
    void parse(char NextSCI_chr);

private:
    enum RunModeType {
        Awaiting_Info,
        End_Flag_Found,
        Breath_Data,
        // Add other modes as needed
    };

    RunModeType RunMode;
    int ByteCount;
    int MetricNo;
    int settingsNo;
    float MetricUnscaled[15]; // Adjust size as needed
    float MetricScaled[15];   // Adjust size as needed
    float MetricScaleFactors[15]; // Define scale factors
    float MetricOffset[15];       // Define offsets

    // Add other private members as needed
};

#endif // PARSECIE_DATA_HPP
