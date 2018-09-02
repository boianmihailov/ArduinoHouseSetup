// Boian Mihailov <info@boian.me>
// Boiler temp sensor with OLED display 128x64
// DHT22 and ds18b20 sensors 
// Outputs to influxdb

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include <InfluxDb.h>
#include "DHT.h"

#define OLED_RESET LED_BUILTIN
Adafruit_SSD1306 display(OLED_RESET);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2



#define ONE_WIRE_BUS D3
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

const char* WIFI_SSID = "SSID";
const char* WIFI_PASSWORD = "password";

#define INFLUXDB_HOST "192.168.88.XXX"

#define DHTPIN 14
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);
WiFiClient wifiClient;
Influxdb influx(INFLUXDB_HOST);


void setup()   {                
  Serial.begin(115200);
  sensors.begin();
  dht.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  
  
  display.clearDisplay();
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
  display.clearDisplay();
  sensors.requestTemperatures(); 
  
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float tb = sensors.getTempCByIndex(0);
  if (isnan(h) || isnan(t)) {
    Serial.println("ERROR: Failed to read from DHT sensor!");
    delay(500);
    return;
  } else {
  
  const char* device = "boiler";
  
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

  InfluxData rowtb("btemp");
  rowtb.addTag("device", device);
  rowtb.addTag("sensor", "ds");
  rowtb.addTag("mode", "temp");
  rowtb.addValue("loopCount", loopCount);
  rowtb.addValue("value", tb);
  influx.write(rowtb);

      
  }
  
  Serial.println();
  Serial.print("Celsius temperature: ");
  Serial.print(tb); 

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Boiler temperature:");

  display.setCursor(0,16);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.println(tb);// display.println(0xDEADBEEF, HEX);

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.print("Air temp:");
  display.print(t);
  display.write((char)7);
  display.println("C");
  display.print("Air humidity:");
  display.print(h); 
  display.println("%");
  display.display();

  
  delay(15000);
}
