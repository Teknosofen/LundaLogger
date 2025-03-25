#include <Arduino.h>
#include "main.hpp"

// Graphix CFG
// #define WIDTH  536
// #define HEIGHT 240

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprite = TFT_eSprite(&tft);

// Analog Clock
AnalogClock myClock(&sprite, CLOCK_XPOS, CLOCK_YPOS, CLOCK_SIZE);

// Display management
DisplayManager displayManager(&tft, &sprite, &myClock);

MyWiFiHandler wifiHandler("your_ssid", "your_password");
FileServer fileServer(SDCARD_CS_PIN, &wifiHandler);                      // SD card CS pin
DataLogger dataLogger(SDCARD_CS_PIN, &fileServer);                       // SD card CS pin
ServoCIEData servoCIEData;                                              // handler for data from SERVO ventilator

void setup() {
    hostCom.begin(115200);
    servoCom.begin(38400, SERIAL_8E1, 36, 4); 
    servoCIEData.begin();                       // initiates the CIE config
    wifiHandler.begin();
    dataLogger.begin();
    fileServer.begin();

    // Display update
    rm67162_init();
    lcd_setRotation(1);
    sprite.setTextColor(TFT_WHITE, TFT_BLACK);
    sprite.createSprite(LCD_WIDTH, LCD_HEIGHT);
    sprite.drawString(lundaLoggerVerLbl, 5, 100, 4);
    lcd_PushColors(0, 0, LCD_WIDTH, LCD_HEIGHT, (uint16_t *)sprite.getPointer());
    delay(3000);

    displayManager.begin();
    displayManager.renderHeader();

    // Initialize the clock
    myClock.begin();
    // myClock.setRTCTime(); // Set the RTC time with the compile time clock.setRTCTime
    lcd_PushColors(0, 0, LCD_WIDTH, LCD_HEIGHT, (uint16_t *)sprite.getPointer());

}

void loop() {
    // Simulate data logging
    String waveformData = "waveform data";
    String breathData = "breath data";
    String settingsData = "settings data";

    dataLogger.logWaveformData(waveformData);
    dataLogger.logBreathData(breathData);
    dataLogger.logSettingsData(settingsData);

    fileServer.sendDataUpdate(waveformData, breathData, settingsData);

    fileServer.handleClient();

    // Handle SCI traffic
    // handle when B2B data or settings hve asrrived
    if (servoCom.available()) {
        char NextSCI_chr = servoCom.read();
        servoCIEData.parseCIEData(NextSCI_chr);
    }

    static int lastLoopTime = 0;

    if ((millis() - lastLoopTime) > SET_LOOP_TIME) {
      lastLoopTime = millis();

        // update the display here so the servoCIE does not have to use the display


      
    }


}
