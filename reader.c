#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "Your_SSID";
const char* password = "Your_Password";
const char* server_url = "http://192.168.1.10/data"; // Linux server IP

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("WiFi connected!");
}

void loop() {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin(server_url);

        int httpResponseCode = http.GET();
        if (httpResponseCode > 0) {
            String response = http.getString();
            Serial.print("Sensor Data: ");
            Serial.println(response);
        } else {
            Serial.print("Error: ");
            Serial.println(httpResponseCode);
        }
        http.end();
    }
    delay(10000); // Read data every 10 seconds
}

