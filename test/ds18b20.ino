#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS D3
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void setup(void)
{
  Serial.begin(115200);
  sensors.begin();
}

void loop(void){ 
  sensors.requestTemperatures(); 
  Serial.println();
  Serial.print("Celsius temperature: ");
  Serial.print(sensors.getTempCByIndex(0)); 
  delay(5000);
}
