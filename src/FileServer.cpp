#include "FileServer.hpp"

// Constructor to initialize
FileServer::FileServer(int csPin /*, MyWiFiHandler *wifiHdl*/)
    : csPin(csPin)/*, wifiHdl(wifiHdl)*/, server(80), webSocket(81)  {}

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
    // Set up WebSocket event handler to process incoming messages
    // Expected messages: text messages containing data updates
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
    char message[256];
    snprintf(message, sizeof(message), "waveform:%s,breath:%s,settings:%s", waveformData.c_str(), breathData.c_str(), settingsData.c_str());
    webSocket.broadcastTXT(message);
}
void FileServer::handleRoot() {
    String html = "<html><body><h1>Select a File to Download</h1><ul>";
    File root = SD.open("/");
    if (!root) {
    if (root) {
        File entry;
        while (entry = root.openNextFile()) {
            if (!entry.isDirectory()) {
                html += "<li><a href=\"/download?file=" + String(entry.name()) + "\">" + String(entry.name()) + "</a></li>";
            }
            entry.close();
        }
        root.close();
    }
    html += "</ul></body></html>";
    server.send(200, "text/html", html);
}
}

void FileServer::handleDownload() {
    if (server.hasArg("file")) {
        String filename = server.arg("file");
        File file = SD.open(filename);
        if (file) {
        String filename = server.arg("file");
        if (filename.indexOf("..") != -1 || filename.indexOf("/") == 0 || filename.indexOf("\\") == 0) {
            server.send(400, "text/plain", "Invalid filename");
            return;
        }
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
    // Expected messages: text messages containing data updates
    // Example: "waveform:123,breath:456,settings:789"
    // You can parse the payload and update the corresponding data fields
}
