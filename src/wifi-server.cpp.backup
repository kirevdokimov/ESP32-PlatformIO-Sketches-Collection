#include <WiFi.h>
#include <ESPAsyncWebServer.h>

// Add
// lib_deps = ESP Async WebServer
// to platformio.ini

const char *ssid = "Rostelecom160386";
const char *password = "93708547";

AsyncWebServer server(80);

void setup()
{
  Serial.begin(9600);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Print the ESP32's IP address
  Serial.print("ESP32 Web Server's IP address: ");
  Serial.println(WiFi.localIP());

  // Define a route to serve the HTML page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    Serial.println("ESP32 Web Server: New request received:");  // for debugging
    Serial.println("GET /");        // for debugging
    request->send(200, "text/html", "<html><body><h1>Hello, ESP32!</h1></body></html>"); });

  // Start the server
  server.begin();
}

void loop() {}