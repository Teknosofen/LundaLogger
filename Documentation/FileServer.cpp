#include "FileServer.hpp"

FileServer::FileServer(int csPin, const char *ssid, const char *password)
    : csPin(csPin), server(80), ssid(ssid), password(password) {}

void FileServer::begin() {
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi.");

    if (!SD.begin(csPin)) {
        Serial.println("SD initialization failed!");
        return;
    }
    Serial.println("SD initialization done.");

    server.on("/", HTTP_GET, [this]() { handleRoot(); });
    server.on("/download", HTTP_GET, [this]() { handleDownload(); });
    server.begin();
}

void FileServer::handleClient() {
    server.handleClient();
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
