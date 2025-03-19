#ifndef servoCIE_FILE_h
#define servoCIE_FILE_h

#include <Arduino.h>
#include <main.hpp>


// Function definitions
void CIE_setup(void);       // Setup and start CIE communication
void Parse_CIE_data(char);  // parse data receive from the ventilator
char CRC_calc(String localstring); // CRC calculations
void ScaleMetrics(void);    // calculates the scaled metrics
void Send_SERVO_CMD(String InStr);
// unsigned char SendEmepStyle(unsigned char mID, unsigned char mLength, unsigned char *mData);

// int16_t i = 0; // general loop variable

char RunMode = 0, ByteCount, phase, Error_info;
char chk;  // checksum value

int cieFlow = 0; // stores the flow signal received from CIE
int cieFCO2 = 0;
int ciePaw = 0;

char inByte; // collects the latest char from the ventilator
int CurveCounter = 0; // Count which curve is presently being managed
#define NumberOfCurves 3 // Totalnumber of curves to receive from SCI/CIE
// debug
// #define NumMetrics 6 // number of metrics collected from SCI
#define NumMetrics 15 // number of metrics collected from SCI
int16_t MetricUnscaled[NumMetrics]; // raw data from CIE, CIE obviously sends negative values where they should be posistiie
float MetricScaled[NumMetrics]; // scaled metrics
float MetricOffset[NumMetrics+1] =       {0.0,     0.0,      0.0,      0.0,     0.0,      0.0,     0.0,      0.0,     0.0,     200.0,  200.0,   0.0,  200.0,   200.0,   0.0};
float MetricScaleFactors[NumMetrics+1] = {0.1,     0.1,      1.0,      0.1,     0.2,      0.2,     0.01,     0.01,    0.1,     0.1,    0.1,     0.01, 0.1,     0.1,     0.0001};
//             channel no:                #113      #114     #117      #100     #101      #102     #103      #104     #109    #108     #107     #122  #105     ¤106     #128
String MetricLabels[NumMetrics+1] =      {"VtCO2", "FetCO2", "MVCO2",  "RR",    "VtInsp", "VtExp", "MVinsp", "MVExp", "FiO2", "PEEP",  "EIP",   "IE", "PPeak", "Pmean", "Ti/Ttot"};
String MetricUnits[NumMetrics+1] =       {"ml",     "%",     "ml/min", "1/min", "mL",     "mL",    "L/min",  "L/min", "%",    "cmH2O", "cmH2O", "-",  "cmH2O", "chH2O", "-"};
int MetricsHaveArrived = false; // Used to signal that all metrics have arrived safely and should be output to host

unsigned int MetricNo; // count which metric is currently being received


#define numSettings 10 // number of settings collected from SCI
uint16_t settingsUnscaled[numSettings]; // raw data from CIE
float settingsScaled[numSettings]; // scaled settings
float settingsOffset[numSettings] =       {0.0,     0.0,     0.0,     0.0,    0.0,     0.0,  0.0,    0.0,      0.0,        0.0};
float settingsScaleFactors[numSettings] = {0.1,     0.01,    0.1,     0.1,    0.1,     0.2,  1,      1,        1,          0.01};
String settingsLabels[numSettings] =      {"RR",    "MV",    "PEEP",  "FiO2", "Pinsp", "Vt", "mode", "PatCat", "ComplComp", "IE"};
String settingsUnits[numSettings] =       {"1/min", "L/min", "cmH2O", "%",    "cmH2O", "mL", "N/A",  "N/A",    "N/A",       "N/A"};
int settingsHaveArrived = false; // Used to signal that all metrics have arrived safely and should be output to host

unsigned int settingsNo; // count which setting is currently being received

float ventFlowFiltConst = 0.3; // used for the recursive filtering of the breath by breath calculated insp and exp flows

// CIE constants
// -------------
#define EOT 0x04  // end of text
#define ESC 0x1B  // escape char
#define CR 0x0D   // CR char
#define LF 0x0A   // LF char
#define ValueFlag 0x80  // 
#define PhaseFlag 0x81  //
#define ErrorFlag 0xE0  //
#define EndFlag   0x7F  //

#define cieDataInvalid 0x7EFF  // 0x7EFF,= 32511 flags that cie data is not available or invalid
#define cieEIPInvalid 3000.0 // limit just below a scaled version of the 0x7EFF

// CIE states
// ----------
enum
{
  Awaiting_Info,
  End_Flag_Found,
  Breath_Data,
  Value_Data,
  Phase_Data,
  Trend_Data,
  Alarm_Data,
  Error_Data,
  Settings_Data,
  Run_Mode,
  StandBy
};

#include "servoCIE.c"
#endif