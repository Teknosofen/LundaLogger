#include "FileServer.hpp"

// Constructor to initialize
FileServer::FileServer(int csPin, MyWiFiHandler *wifiHdl)
    : csPin(csPin), server(80), webSocket(81), wifiHdl(wifiHdl)  {}

void FileServer::begin() {
   
    if (!SD.begin(csPin)) {
        Serial.println("SD initialization failed!");
        return;
    }
    Serial.println("SD initialization done.");

    server.on("/", HTTP_GET, [this]() { handleRoot(); });
    server.on("/download", HTTP_GET, [this]() { handleDownload(); });
    server.on("/data", HTTP_GET, [this]() { handleData(); });

    server.begin();
    webSocket.begin();
    webSocket.onEvent([this](uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
        handleWebSocketMessage(num, type, payload, length);
    });
}

void FileServer::handleClient() {
    server.handleClient();
    webSocket.loop();
}

void FileServer::sendDataUpdate(const String &waveformData, const String &breathData, const String &settingsData) {
    this->waveformData = waveformData;
    this->breathData = breathData;
    this->settingsData = settingsData;
    String message = "waveform:" + waveformData + ",breath:" + breathData + ",settings:" + settingsData;
    webSocket.broadcastTXT(message);
}

void FileServer::handleRoot() {
    String html = "<html><body><h1>Select a File to Download</h1><ul>";
    File root = SD.open("/");
    while (true) {
        File entry = root.openNextFile();
        if (!entry) {
            break;
        }
        if (!entry.isDirectory()) {
            html += "<li><a href=\"/download?file=" + String(entry.name()) + "\">" + String(entry.name()) + "</a></li>";
        }
        entry.close();
    }
    html += "</ul></body></html>";
    server.send(200, "text/html", html);
}

void FileServer::handleDownload() {
    if (server.hasArg("file")) {
        String filename = server.arg("file");
        File file = SD.open(filename);
        if (file) {
            server.streamFile(file, "application/octet-stream");
            file.close();
        } else {
            server.send(404, "text/plain", "File not found");
        }
    } else {
        server.send(400, "text/plain", "Bad request");
    }
}

void FileServer::handleData() {
    String html = R"rawliteral(
        <html>
            <body>
                <h1>Live Data</h1>
                <div id="waveform"></div>
                <div id="breath"></div>
                <div id="settings"></div>
                <script>
                    var connection = new WebSocket('ws://' + window.location.hostname + ':81/');
                    connection.onmessage = function(event) {
                        var data = event.data.split(',');
                        document.getElementById('waveform').innerText = data[0];
                        document.getElementById('breath').innerText = data[1];
                        document.getElementById('settings').innerText = data[2];
                    };
                </script>
            </body>
        </html>
    )rawliteral";
    server.send(200, "text/html", html);
}

void FileServer::handleWebSocketMessage(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
    // Handle WebSocket messages if needed
}
