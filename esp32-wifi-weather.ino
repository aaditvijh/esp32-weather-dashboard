#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WebServer.h>

#define SSID "your-wifi-name"         //wifi name
#define PASSWORD "your-wifi-password" //wifi password
#define API_KEY "your-api-key"        //api key
#define CITY "your-city"              //city name

WebServer server(80);

String getWeather() {
  HTTPClient http;
  String url = "http://api.openweathermap.org/data/2.5/weather?q="
                + String(CITY)
                + "&appid=" + String(API_KEY)
                + "&units=metric";
  http.begin(url);
  int httpCode = http.GET();

  if (httpCode == 200) {
    String payload = http.getString();
    http.end();
    return payload;
  }
  http.end();
  return "error";
}

void parseWeather(String json, float &temp, float &humidity, float &windSpeed, String &description) {
  JsonDocument doc;
  deserializeJson(doc, json);
  temp        = doc["main"]["temp"];
  humidity    = doc["main"]["humidity"];
  windSpeed   = doc["wind"]["speed"];
  description = doc["weather"][0]["description"].as<String>();

}

String buildHTML(float temp, float humidity, 
                 float windSpeed, String description) {
  String html = "<!DOCTYPE html><html><head>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<title>ESP32 Weather</title></head>";
  html += "<body style='font-family:sans-serif; text-align:center; background:#111; color:white; padding:2rem'>";
  html += "<h1>Delhi Weather</h1>";
  html += "<h2>" + description + "</h2>";
  html += "<p>Temp: " + String(temp) + " C</p>";
  html += "<p>Humidity: " + String(humidity) + "%</p>";
  html += "<p>Wind: " + String(windSpeed) + " m/s</p>";
  html += "</body></html>";
  return html;
}

void setup () {
  Serial.begin(115200);

  WiFi.begin(SSID, PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() !=WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Connected YIPPEEEEE");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  server.on("/", []() {
    String json = getWeather();
    if (json == "error") {
      server.send(200, "text/html", "<h1>Error fetching weather :(</h1>");
      return;
    }
    float temp, humidity, windSpeed;
    String description;
    parseWeather(json, temp, humidity, windSpeed, description);
    server.send(200, "text/html", buildHTML(temp, humidity, windSpeed, description));
  });

  server.begin();
  Serial.println("Server started :D");

}

void loop () {
  server.handleClient();
}