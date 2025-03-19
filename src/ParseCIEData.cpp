#include "ParseCIEData.hpp"

ParseCIEData::ParseCIEData() : RunMode(Awaiting_Info), ByteCount(0), MetricNo(0), settingsNo(0) {
    // Initialize other members if needed
}

void ParseCIEData::parse(char NextSCI_chr) {
    switch (RunMode) {
        case Awaiting_Info:
            switch (NextSCI_chr) {
                case 'S':
                    RunMode = Settings_Data;
                    settingsNo = 0;
                    ByteCount = 2;
                    break;
                case 'B':
                    RunMode = Breath_Data;
                    MetricNo = 0;
                    ByteCount = 2;
                    break;
                case 'T':
                    RunMode = Trend_Data;
                    break;
                case 'A':
                    RunMode = Alarm_Data;
                    break;
                case 0xE0:
                    RunMode = Error_Data;
                    ByteCount = 1;
                    break;
                case 0x80:
                    RunMode = Value_Data;
                    ByteCount = 2;
                    break;
                case 0x81:
                    RunMode = Phase_Data;
                    ByteCount = 1;
                    break;
                case 0x7F:
                    RunMode = Awaiting_Info;
                    break;
                default:
                    break;
            }
            break;

        case End_Flag_Found:
            RunMode = Awaiting_Info;
            break;

        case Breath_Data:
            if (ByteCount == 2) {
                MetricUnscaled[MetricNo] = 256 * NextSCI_chr;
                --ByteCount;
            } else if (ByteCount == 1) {
                MetricUnscaled[MetricNo] += NextSCI_chr;
                --ByteCount;
                MetricScaled[MetricNo] = MetricUnscaled[MetricNo] * MetricScaleFactors[MetricNo] - MetricOffset[MetricNo];
            } else if (ByteCount == 0 && NextSCI_chr == EndFlag) {
                int k = 0;
                ventO2BreathData.cieVtCO2 = MetricScaled[k++];
                ventO2BreathData.cieFetCO2 = MetricScaled[k++];
                ventO2BreathData.cieMVCO2 = MetricScaled[k++];
                ventO2BreathData.cieRR = MetricScaled[k++];
                ventO2BreathData.cieVtInsp = MetricScaled[k++];
                ventO2BreathData.cieVtExp = MetricScaled[k++];
                ventO2BreathData.cieMVinsp = MetricScaled[k++];
                ventO2BreathData.cieMVExp = MetricScaled[k++];
                ventO2BreathData.cieFiO2 = MetricScaled[k++];
                ventO2BreathData.ciePEEP = MetricScaled[k++];
                ventO2BreathData.ciePplat = MetricScaled[k++];
                ventO2BreathData.cieIE = MetricScaled[k++];
                ventO2BreathData.ciePpeak = MetricScaled[k++];
                ventO2BreathData.Pmean = MetricScaled[k++];
                ventO2BreathData.cieTi2Ttot = MetricScaled[k++];
            }
            break;

        // Add other cases as needed
    }
}
