#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "Your_SSID";
const char* password = "Your_Password";
const char* server_url = "http://192.168.1.10/upload"; // Linux server IP

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

        String payload = "Temperature=26.5,Humidity=45%"; // Data to send
        http.addHeader("Content-Type", "text/plain");

        int httpResponseCode = http.POST(payload);
        if (httpResponseCode > 0) {
            Serial.println("Data uploaded successfully");
        } else {
            Serial.print("Error: ");
            Serial.println(httpResponseCode);
        }
        http.end();
    }
    delay(10000); // Upload data every 10 seconds
}

