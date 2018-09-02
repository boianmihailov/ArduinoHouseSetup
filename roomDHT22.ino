// Boian Mihailov <info@boian.me> 
// DHT22 room sensors 
// outputs to influxdb

#include <ESP8266WiFi.h>
#include <InfluxDb.h>
#include "DHT.h"

#include <OneWire.h>
#include <DallasTemperature.h>

// Wifi: SSID and password
const char* WIFI_SSID = "SSID";
const char* WIFI_PASSWORD = "password";

#define INFLUXDB_HOST "192.168.88.xxx"
#define DHTPIN 2
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);
WiFiClient wifiClient;
Influxdb influx(INFLUXDB_HOST);


void setup() {
  // init the serial
  Serial.begin(115200);

  dht.begin();

  // init the WiFi connection
  Serial.println();
  Serial.println();
  Serial.print("INFO: Connecting to ");
  WiFi.mode(WIFI_STA);
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("INFO: WiFi connected");
  Serial.println("INFO: IP address: ");
  Serial.println(WiFi.localIP());
  influx.setDb("iot");
}

int loopCount = 0;

void loop() {
  loopCount++;
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("ERROR: Failed to read from DHT sensor!");
    return;
  } else {


  const char* device = "kitchen";
  
  InfluxData rowt("temperature");
  rowt.addTag("device", device);
  rowt.addTag("sensor", "dht22");
  rowt.addTag("mode", "temp");
  rowt.addValue("loopCount", loopCount);
  rowt.addValue("value", t);
  influx.write(rowt);

  InfluxData rowh("humidity");
  rowh.addTag("device", device);
  rowh.addTag("sensor", "dht22");
  rowh.addTag("mode", "hum");
  rowh.addValue("loopCount", loopCount);
  rowh.addValue("value", h);

  influx.write(rowh);
  
  }
  delay(30000); 
}

