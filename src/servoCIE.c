#include <Arduino.h>
#include <main.hpp>
#include <servoCIE.h> 

void Parse_CIE_data(char NextSCI_chr)
{
  // CIE_statemachine
  switch (RunMode)
  { 
    case Awaiting_Info: // Assuming RADC command issued and thus expecting cont. data
    {
      // hostCom.println("DEBUG - in Await Info");      
      switch (NextSCI_chr)   // check whether the ByteCount is to be used at all...
      {
        case 'S':   // Settings data
          RunMode = Settings_Data;
          settingsNo = 0;
          ByteCount = 2;
          // hostCom.println("DEBUG - S rec start");
          break;  // Settings data
        case 'B':   // Breath data

          RunMode = Breath_Data;
          MetricNo = 0;
          ByteCount = 2;
          // hostCom.println("DEBUG - B rec start");          
          break; // Breath data

        case 'T':   // Trend Data
          RunMode = Trend_Data;
          break; // Trend Data

        case 'A':   // Alarm data
          RunMode = Alarm_Data;
          break; // Alarm data

        case 0xE0:  // Error flag
          // hostCom.println("DEBUG - err data start");           
          RunMode = Error_Data;
          ByteCount = 1;
          break;  // Error flag

        case 0x80:  // Value data
          // hostCom.println("DEBUG - Value_data start");         
          RunMode = Value_Data;
          ByteCount = 2;
          break; // Value data

        case 0x81:  // Phase flag
          // hostCom.println("DEBUG - Phase data start");         
          RunMode = Phase_Data;
          ByteCount = 1;
          break; // Phase flag

        case 0x7F:  // End Flag
          // hostCom.print("DEBUG -Standby Start");        
          RunMode = Awaiting_Info;
          break; // Endflag

        default:
          // hostCom.print("DEBUG - in default case "); 
          // hostCom.println(NextSCI_chr, HEX); 
          break; // default:
      }  // switch NextSCI_chr

    } // case awaiting info
      break; // Awaiting_info

    case End_Flag_Found:    // Read the <chk> and start looking for the next clue
    {
      // hostCom.println("DEBUG - in End Flag found"); 
      RunMode = Awaiting_Info;
    }
      break; // End_flag_found

    case Breath_Data: // Collect B2B data, 
    {
      // hostCom.println("DEBUG - in Breath ata");       
      if (ByteCount == 2) 
      {
        MetricUnscaled[MetricNo] = 256 * NextSCI_chr;
        --ByteCount;
      }
      else if (ByteCount == 1) 
      {
        MetricUnscaled[MetricNo] += NextSCI_chr;
        --ByteCount;
        MetricScaled[MetricNo] = MetricUnscaled[MetricNo] * MetricScaleFactors[MetricNo] - MetricOffset[MetricNo];
      // hostCom.print("DEBUG - rec B# "); 
      // hostCom.print(MetricNo); 
      // hostCom.print(" : "); 
      // hostCom.println(MetricScaled[MetricNo]); 

      }
      else if (ByteCount == 0 && NextSCI_chr == EndFlag) 
      {                               // ScaleMetrics(); // The whole B2B data set has arrived, print it to USB hostCom
        int k = 0;
        // hostCom.println("DEBUG - B complete"); 
        ventO2BreathData.cieVtCO2   = MetricScaled[k++];
        ventO2BreathData.cieFetCO2  = MetricScaled[k++];
        ventO2BreathData.cieMVCO2   = MetricScaled[k++];
        ventO2BreathData.cieRR      = MetricScaled[k++];
        ventO2BreathData.cieVtInsp  = MetricScaled[k++];
        ventO2BreathData.cieVtExp   = MetricScaled[k++];
        ventO2BreathData.cieMVinsp  = MetricScaled[k++];
        ventO2BreathData.cieMVExp   = MetricScaled[k++];
        ventO2BreathData.cieFiO2    = MetricScaled[k++];
        ventO2BreathData.ciePEEP    = MetricScaled[k++];
        ventO2BreathData.ciePplat   = MetricScaled[k++]; // ciePplat =  scaled plateau press, #107
        ventO2BreathData.cieIE      = MetricScaled[k++];
        ventO2BreathData.ciePpeak   = MetricScaled[k++];
        ventO2BreathData.Pmean      = MetricScaled[k++];
        ventO2BreathData.cieTi2Ttot = MetricScaled[k];

        if (ventO2BreathData.cieIE < (cieDataInvalid - 1) * 0.01) { // check if the IE value from SCI is invalid or not
                                                                    // the cieIE is scaled with 0.01 so compare to the scaled data invalid flag
          ventO2BreathData.calcExpTime = 60.0 /(ventO2BreathData.cieRR * (1.0 + ventO2BreathData.cieIE));
          ventO2BreathData.calcInspTime = ventO2BreathData.cieIE * 60.0 /(ventO2BreathData.cieRR * (1.0 + ventO2BreathData.cieIE));
        }
        else {                                              // sciIE invalid, use Ti/Ttot instead
          ventO2BreathData.calcExpTime = (1 - ventO2BreathData.cieTi2Ttot) * 60 / ventO2BreathData.cieRR;
          ventO2BreathData.calcInspTime = ventO2BreathData.cieTi2Ttot * 60 / ventO2BreathData.cieRR;
        }
                
        // send B2B metric data as part of the standard package:
        // calculate deltaP depending on mode and in there is a pause in VCV or not
        if (ventO2BreathData.ciePplat < cieEIPInvalid) {  // an invalid EIP is scaled as 3051.1, a scaled 0x7EFF,= 32511 flags that cie data is not available or invalid, scaled as 3051.1
          // vent in VCV mode, use plateau pressure to calculate VBS pressure swing
          ventO2BreathData.deltaP = (ventO2BreathData.ciePplat - ventO2BreathData.ciePEEP); 
        }  // If in VCV
        else { // vent inPCV mdoe, no pause pressure avauilable, use set insp instead
          if (servoSettings.setInspPress < 3000.0) { // in PCV
            ventO2BreathData.deltaP = (servoSettings.setInspPress); //  - ventO2BreathData.ciePEEP); 
          }
          else { // plateau set to zero in VCV mode
            ventO2BreathData.deltaP = (ventO2BreathData.ciePpeak - ventO2BreathData.ciePEEP) / 2.0;
          }
        }
      
        RunMode = End_Flag_Found;
      }
      else 
      { // it was not endflag, thus the first byte of a new data word
        MetricNo ++;
        MetricUnscaled[MetricNo] = 256 * NextSCI_chr;
        ByteCount = 1; // the char we just got was the first so lets look for the second
      }
    }
      break;

    case Settings_Data: //
      {
        if (ByteCount == 2) 
        {
          settingsUnscaled[settingsNo] = 256 * NextSCI_chr;
          --ByteCount;
        }
        else if (ByteCount == 1) 
        {
          settingsUnscaled[settingsNo] += NextSCI_chr;
          --ByteCount;
          settingsScaled[settingsNo] = settingsUnscaled[settingsNo] * settingsScaleFactors[settingsNo] - settingsOffset[settingsNo]; 
        //  hostCom.print(settingsNo);
        //  hostCom.print(" --- ");
        //  hostCom.println(settingsUnscaled[settingsNo]);
        // hostCom.print("DEBUG - rec S# "); 
        // hostCom.print(settingsNo); 
        // hostCom.print(" : "); 
        // hostCom.println(settingsScaled[settingsNo]); 
        
        }
        else if (ByteCount == 0 && NextSCI_chr == EndFlag) 
        {
          // ScaleMetrics(); // The whole d B2B data set has arrived, print it to USB hostCom
          int k = 0;
          servoSettings.setRespRate = settingsScaled[k++];
          servoSettings.setMinuteVol = settingsScaled[k++];
          servoSettings.setPeep = settingsScaled[k++];
          servoSettings.setFiO2 = settingsScaled[k++];
          servoSettings.setInspPress = settingsScaled[k++];
          servoSettings.setVt = settingsScaled[k++];
          servoSettings.setVentMode = settingsScaled[k++];
          servoSettings.setPatRange = settingsScaled[k++];
          servoSettings.setComplianceCompensationOn = settingsScaled[k++];
          servoSettings.setIERatio = settingsScaled[k];

          // if ASCII not selected, send using EMEP
          RunMode = End_Flag_Found;
        }
        else 
        { // it was not endflag, thus the first byte of a new data word
          settingsNo ++;
          settingsUnscaled[settingsNo] = 256 * NextSCI_chr;
          ByteCount = 1; // the char we just got was the first so lets look for the second
        }
      }
      
      break; // Settings_Data:

    case Value_Data: // When valuedata comes alone, it is absolute in two or more bytes preceeded by a <value-flag>
    {
      //  hostCom.println("DEBUG - in Value Data");       
      switch (CurveCounter)
      {
        case 0: // Data for the first curve
          {
            if (ByteCount == 2)
            {
              cieFlow = 256 * NextSCI_chr;
              --ByteCount;
              break;
            }
            if (ByteCount == 1)
            {
              cieFlow += NextSCI_chr;
              ventO2CurveData.cieFlow = cieFlow * 0.25 - 4000.0;   // save scaled data
              RunMode = Run_Mode; // expecting more curve data so go for it again
              --ByteCount;
            //  NextSCI_chr = ValueFlag; // Just try something and see what happens, try to signal that weshall still parse valuedata
            if (NumberOfCurves > 1) CurveCounter = 1;
                else
                  CurveCounter = 0;
                break;
            break;
            }
          }
        break; // case 0: 

     case 1: // Data for the second curve
      {
        if (ByteCount == 2)
        {
          cieFCO2 = 256 * NextSCI_chr;
          --ByteCount;
          break;
        }
        if (ByteCount == 1)
        {
          cieFCO2 += NextSCI_chr; // Possibly generalize this and call it the seond curve using an array
          ventO2CurveData.cieCO2 = (cieFCO2 * 0.1);  // save scaled data  
          RunMode = Run_Mode;
          --ByteCount;

        // The following will not happen but is kept to remember how to extend the function / switch for more curves
          if (NumberOfCurves > 2) CurveCounter = 2;
            else
              CurveCounter = 0;
          break; // Is this one really needed
        }
      }
      break;  // case 1:

      // 
      case 2: // data for the 3rd curve 
      {
        if (ByteCount == 2) {
          ciePaw = 256 * NextSCI_chr;
          --ByteCount;
          break;
        } // if first byte
        if (ByteCount == 1)  {
          ciePaw += NextSCI_chr; // Possibly generalize this and call it the seond curve using an array
          ventO2CurveData.ciePaw = (ciePaw * 0.1);  // save scaled data  
          RunMode = Run_Mode;
          --ByteCount;
        // All  curves have arrived with absolute values, print them to the hostCom port and also B2B data iw nre such is available.
     
        // The following will not happen but is kept to remember how to extend the function / switch for more curves
          if (NumberOfCurves > 3) CurveCounter = 3;
            else
              CurveCounter = 0;
          break; // Is this one really needed
        }
      }
      break;

      default:
        break; // default:
     } // Switch CurveCounter
    }
    break; // case Valuedata:
    
    case Phase_Data: // convert 0x10..0x30 to numbers and put on LCD
      {
      // hostCom.println("DEBUG - in Phase data"); 
        phase = NextSCI_chr;
        ventO2CurveData.ciePhase = phase;
        //      hostCom.print("p: ");
        //      hostCom.println(phase, HEX);
        --ByteCount;
        RunMode = Run_Mode;
      }
      break; // Phase_data:
      
    case Trend_Data: // should not arrive, read until endflag and scrap the data
      {
        while (NextSCI_chr != EndFlag)
        {
          // hostCom.print(NextSCI_chr);
        }
        RunMode = End_Flag_Found;
      }
      break; // Trend_Data:

    case Alarm_Data: // should not arrive, read until endflag and scrap the data
      {
        while (NextSCI_chr != EndFlag)
        {
          // hostCom.print(NextSCI_chr);
        }
        RunMode = End_Flag_Found;
      }
      break; // Alarm_Data:

    case Error_Data:
      {     
  // hostCom.println("DEBUG - in Error_Data Mode");   
        if (NextSCI_chr == EndFlag) {
          RunMode = End_Flag_Found;          
        }
        else 
          Error_info = NextSCI_chr;
      }
      break; // Error_Data:

    case Run_Mode:
      {
        // hostCom.println("DEBUG - in Run Mode");         
        switch (NextSCI_chr)          // Now diff data value, phase or end flag is expected!
        {
          case ValueFlag:             // the absolute value arrives again preceeded by a <value-flag>
            RunMode = Value_Data;
            ByteCount = 2; // 2 channels expected, thus bytecount = 4
            break;
          case PhaseFlag:
            RunMode = Phase_Data;
            ByteCount = 1;
            break;
          case EndFlag:
            RunMode = End_Flag_Found;
            break;
          default: // The differential signal from the curve data end up here...
            switch (CurveCounter)
            {
              case 0: // Data for the first curve
              {
                cieFlow += int8_t(NextSCI_chr); // type cast the unsigned char into int
                // store the data in the struct:
                ventO2CurveData.cieFlow = cieFlow * 0.25 - 4000.0;   // save scaled data
                // hostCom.print(cieFlow *0.25 - 4000.0, 1); // flow curve channel 001
                // hostCom.print('\t');
                if (NumberOfCurves > 1) CurveCounter = 1;
                else
                  CurveCounter = 0;
              }
              break; // case 0:

              case 1: // data for the second curve
              {
                cieFCO2 += int8_t(NextSCI_chr); // Possibly generalize this and call it the seond curve using an array
                // hostCom.print(100*(cieFCO2 * 0.1) , 1); // CO2 curve channel 0 
                ventO2CurveData.cieCO2 = (cieFCO2 * 0.1);  // save scaled data 

                if (NumberOfCurves > 2) CurveCounter = 2;
                else
                  CurveCounter = 0;
              }
              break; // case 1:
              
              case 2:
             {
                ciePaw += int8_t(NextSCI_chr); // Possibly generalize this and call it the third curve using an array
                // hostCom.print(100*(cieFCO2 * 0.1) , 1); // CO2 curve channel 0 
                ventO2CurveData.ciePaw = (ciePaw * 0.1);  // save scaled data 

                if (settings.Quiet & sciCurveDataBinaryMask) { 
                  // send data using EMEP, send both structs
                  unsigned char* byteDataPtr = (byte*)(ventO2CurveDataPtr); // typecast the struct pointer to a byte pointer
                  SendEmepStyle(ventO2CurveDataMsgID, sizeof(ventO2CurveData), byteDataPtr); // returns num of bytes sent
                }

                if (settings.Quiet & sciCurveDataAsciiMask) {
                // ASCII output
                  hostCom.print(ventO2CurveData.cieFlow, 2); // flow curve channel 001
                  hostCom.print('\t');
                  hostCom.print(ventO2CurveData.cieCO2, 2); // CO2 curve channel 0 
                  hostCom.print('\t');
                  hostCom.print(ventO2CurveData.ciePaw, 2); // CO2 curve channel 0 
                  hostCom.print('\t');
                  hostCom.println(ventO2CurveData.ciePhase);
                }

                if (NumberOfCurves > 3) CurveCounter = 3;
                else
                  CurveCounter = 0;
              } // case 2:
              break;
            } // Switch CurveCounter
          break; // the default for RunMode
        } // Switch NextSCI_chr
      }
      break; // case Runmode:

    default: // runmode?
// hostCom.println("DEBUG - Does runmode standby end up here?");    
      break; // default:
  } // switch (RunMode)
} // ParseData

// Functions

void ScaleMetrics(void) {
  for (int i = 0; i < NumMetrics; i++) {
    MetricScaled[i] = MetricUnscaled[i] * MetricScaleFactors[i] - MetricOffset[i];
    hostCom.print(MetricLabels[i]);
    hostCom.print(" = ");
    hostCom.print(MetricScaled[i]);
    hostCom.print(" ");
    hostCom.println(MetricUnits[i]);
  }
  hostCom.println();
} // ScaleMetrics


// Calculate CRC checksum according to the SERVO manual
char CRC_calc(String localstring) {
  char chk = 0;
  unsigned int len = localstring.length();
  for (int i = 0; i < len; i++) {
    chk = chk ^ localstring[i]; // bitwise XOR on each byte
  }
  return chk;
}

// Send a command to the SERVO with ASCII CRC and <EOT>
// Input String with command to send
// Input binaryOutputFlag flag to indicate whether to echo command string to the USB port or not
void Send_SERVO_CMD(String InStr) {
  char CRC;
  CRC = CRC_calc(InStr);
  servoCom.print(InStr);
  if (CRC < 0x10) {
    servoCom.print("0");
  }
  servoCom.print(CRC, HEX);
  servoCom.write(EOT);
  // Debug to the USB port:
  if (settings.Quiet & sciSetupInfoAsciiMask) {
    hostCom.print(InStr);
    if (CRC < 0x10) {
      hostCom.print("0");
    }
    hostCom.print(CRC, HEX);
  } // ASCII Output
}

void getCIEResponse() {
  delay(30);
  while (servoCom.available()) { // always read all input but send to host if binary output selected
    inByte = servoCom.read();
   //  if (settings.Quiet & sciSetupInfoAsciiMask) { hostCom.print(inByte); }
    hostCom.print(' ');
    hostCom.print(inByte);
    hostCom.print(' ');
    hostCom.print(inByte, HEX);
    hostCom.print(' ');
    if (inByte == EOT) break;
  }
}

// Start the CIE/SCI communication
void CIE_setup(void)           // initiate the CIE communication
{
  servoCom.write(EOT);   // Empty command, should get response "*<CHK>"
  if (settings.Quiet & sciSetupInfoAsciiMask) { hostCom.print("\nsending EOT "); }
  getCIEResponse(); // read and output response unless binary output selected 

  if (settings.Quiet & sciSetupInfoAsciiMask) { hostCom.print("\nsending ESC "); }

  servoCom.write(ESC);   // reset command, should get response ER20<CHK><EOT>"
    if (settings.Quiet & sciSetupInfoAsciiMask) { hostCom.println(""); }
  getCIEResponse(); // read and output response unless binary output selected

  if (settings.Quiet & sciSetupInfoAsciiMask) { hostCom.print("\nsending RTIM "); }
  Send_SERVO_CMD("RTIM");
  if (settings.Quiet & sciSetupInfoAsciiMask) { hostCom.println(""); }
  getCIEResponse(); // read and output response unless binary output selected


  if (settings.Quiet & sciSetupInfoAsciiMask) { hostCom.print("\nsending RCTY"); }
  Send_SERVO_CMD("RCTY");
  if (settings.Quiet & sciSetupInfoAsciiMask) { hostCom.println(""); }
  getCIEResponse();// read and output response unless binary output selected


// Setup B2B channels 
// B2B data:
// float cieVtCO2 = 0.0; // scaled tidal production CO2 [mL], 4 bytes, chan #113, [ml]
// float cieFetCO2 = 0.0; // scaled end-tidal CO2 [%], 4 bytes, chan #114, [%]
// float cieMVCO2 = 0.0; // scaled minute production CO2 [ml/min], 4 bytes, chan #117, [ml/min] 
// float cieRR = 0.0; // scaled resp rate [1/min], chan #100, +1000E-004, +0000E+000, 06, BR
// float cieVtInsp = 0.0; // scaled insp tidal volume [ml]], chan #101, +2000E-004, +0000E+000, 01, BR
// float cieVtExp = 0.0; // scaled exp tidal volume [ml]], chan #102 +2000E-004, +0000E+000, 01, BR
// float cieMVinsp = 0.0; // scaled insp minute volume rate [L/min], chan #103 +1000E-005, +0000E+000, 08, BR
// float cieMVExp = 0.0; // scaled exp minute volume rate [L/min], chan #104 +1000E-005, +0000E+000, 08, BR  
// float cieFiO2 = 0.0; // scaled FiO2 [vol%], chan #109, +1000E-004, +0000E+000, 07, BR
// float ciePEEP = 0.0; // scaled PEEP, chan # 108 +1000E-004, +2000E-001, 04, BR
// float ciePplat = 0.0; // scaled plateau press, #107
// float cieIE = 0.0 ; // IE ratio chan #122 +1000E-005, +0000E+000, 20, BR
// float cieTi2Ttot = 0.0 ; // Ti/Ttot ratio chan #128 +1000E-007, +0000E+000, 20, BR

// Update the #define NumMetrics in the servoCIE.h if you change this
  if (settings.Quiet & sciSetupInfoAsciiMask) { hostCom.print("\nsending SDADB "); }
// Debugtest      
    // *Send_SERVO_CMD("SDADB113114117100102101"); // debug med enbart 6 metrics
    Send_SERVO_CMD("SDADB113114117100102101103104109108107122105106128"); // all settings
// Send_SERVO_CMD("SDADB113114117100102101103104109108148122");
  if (settings.Quiet & sciSetupInfoAsciiMask) { hostCom.println(""); }

// setup setingschannels to be read
// uint32_t setRespRate = 0;       // chan 400,  +1000E-004, +0000E+000, 06, SD
// uint32_t setMinuteVol = 0.0;    // chan 405, +1000E-005, +0000E+000, 08, SD
// uint32_t setPeep = 0.0;         // chan 408, +1000E-004, +0000E+000, 04, SD
// uint32_t setFiO2 = 0.0;         // chan 414, mode in list
// uint32_t setInspPress = 0.0; // chan 406,  +1000E-004, +0000E+000, 04, SD
// uint32_t setVt = 0.0;       // chan 420 +2000E-004, +0000E+000, 01, SD
// uint32_t setVentMode = 0.0;     // chan 410, mode in list
// uint32_t setPatRange = 0.0;     // chan 409, setting in list
// uint32_t setComplianceCompensationOn = 0; // chan #437 gives info om the received airway flow signal
// uint32_t setIERatio = 0         // chan 419 I:E Ratio+1000E-005, +0000E+000, 20, SD

// Update the #define numSettings in the servoCIE.h if you change this
// Debugtest  
  if (settings.Quiet & sciSetupInfoAsciiMask) { hostCom.print("\nsending SDADS "); }    
  Send_SERVO_CMD("SDADS400405408414406420410409437419");
  if (settings.Quiet & sciSetupInfoAsciiMask) { hostCom.println(""); }
  getCIEResponse(); // read and output response unless binary output selected


// Setup curve channels:
// FLOW curve , #0, scaled 0.25*10-4, [ml/s]
// CO2 curve #3, scaled 0.1, [%]
// Airway flow is channel 0,  is channel +2500E-004, +4000E+000, 02, CU
// CO2 is channel 4, +1000E-004, +0000E+000, 07, CU
// Paw is channel 1, +1000E-004, +2000E-001, 04, CU
  
 // Debugtest  
  if (settings.Quiet & sciSetupInfoAsciiMask) { hostCom.println("\nsending SDADC "); } 
  Send_SERVO_CMD("SDADC000004001");
  if (settings.Quiet & sciSetupInfoAsciiMask) { hostCom.println(""); }
  getCIEResponse(); // read and output response unless binary output selected

 // Debugtest  
  if (settings.Quiet & sciSetupInfoAsciiMask) { hostCom.print("\nsending RCCO102 "); }
  Send_SERVO_CMD("RCCO102");
  if (settings.Quiet & sciSetupInfoAsciiMask) { hostCom.println(""); }
  getCIEResponse(); // read and output response unless binary output selected
 

  if (settings.Quiet & sciSetupInfoAsciiMask) { hostCom.print("\nsending RDAD "); }
  Send_SERVO_CMD("RDAD"); // check channel config
  if (settings.Quiet & sciSetupInfoAsciiMask) { hostCom.println(""); }
  getCIEResponse(); // read and output response unless binary output selected

  if (settings.Quiet & sciSetupInfoAsciiMask) { hostCom.print("\nsending RADAB "); }
  Send_SERVO_CMD("RADAB"); // Get started!
  if (settings.Quiet & sciSetupInfoAsciiMask) { hostCom.println(""); } 

  delay(10);
  while (servoCom.available()) {
    inByte = servoCom.read();
    if (inByte < 0x10 && (settings.Quiet & sciSetupInfoAsciiMask)) {
      hostCom.print("0");
    }
    if (settings.Quiet & sciSetupInfoAsciiMask) { hostCom.print(inByte, HEX); }    
    if (inByte == EOT) break;
  }

  if (settings.Quiet & sciSetupInfoAsciiMask) { hostCom.print("\nSending RADAS "); }
  Send_SERVO_CMD("RADAS"); // Get started!
  if (settings.Quiet & sciSetupInfoAsciiMask) { hostCom.println(""); } 
  // get the response
  delay(10);
  while (servoCom.available()) {
    inByte = servoCom.read();
    if (inByte < 0x10 && (settings.Quiet & sciSetupInfoAsciiMask)) {
      hostCom.print("0");
    }
    if (settings.Quiet & sciSetupInfoAsciiMask) { hostCom.print(inByte, HEX); }    
    if (inByte == EOT) break;
  }

  if (settings.Quiet & sciSetupInfoAsciiMask) { hostCom.print("\nSending RADC "); }
  Send_SERVO_CMD("RADC"); // Get started!
  if (settings.Quiet & sciSetupInfoAsciiMask) { hostCom.println(""); }
}
