#include <Arduino.h>
#include <WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <MQTT.h>

const char* WIFI_SSID = "Zoker";
const char* WIFI_PASS = "gus ardhi";
const char* HOSTNAME = "FTI";
const char* IOTBROKER = "broker.hivemq.com";
#define PIN_RELAY 32

OneWire oneWire(4);
DallasTemperature sensors(&oneWire);
WiFiClient net;
MQTTClient iot;

void setup()
{
  pinMode(PIN_RELAY, OUTPUT);
  Serial.begin(115200);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  WiFi.setHostname(HOSTNAME);

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(1000);
  }
  Serial.println("");
  Serial.println("WiFi connected successfully.");

  sensors.begin();
  
  iot.begin(IOTBROKER, net);
  iot.onMessage(messageReceived);

   Serial.print("Connecting to Iot Broker");
  while(!iot.connect("ESP32", "public", "public"))
  {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("");
  Serial.println("Iot broker connected successfully.");

  iot.subscribe("undiknas/ti/kelompok4/relay");
  iot.subscribe("undiknas/ti/+/chatroom");
}
unsigned long intervalCounterRelay = 0;
unsigned long intervalCounterSensor = 0; 
void loop() {
  
  unsigned long now = millis();
  if( ( now - intervalCounterRelay) > 1000)
  {
    intervalCounterRelay = now;

    bool relayState = getRelay();
    iot.publish("undiknas/ti/kelompok4/relay/state", String(relayState));
  }
  
  if( ( now - intervalCounterSensor) > 5000)
  {
    intervalCounterSensor = now;

    Serial.println("Hello Gus Ardhi");
    iot.publish("undiknas/ti/kelompok4/sensor/serial", String(Serial));
  Serial.println("Hello Gus Ardhi");
iot.loop();
}



void setRelay(bool state)
{
  digitalWrite(PIN_RELAY, state);
  Serial.print("Relay state changed to: ");
  Serial.print(state);
  Serial.println();
}

bool getRelay()
{
  bool state = digitalRead(PIN_RELAY);
  Serial.print("Relay state is: ");
  Serial.print(state);
  Serial.println();
  return state;
}

void messageReceived(String &topic, String &payload)
{
  Serial.println("Incomming: " + topic + " - " + payload);

  if(topic == "undiknas/ti/kelompok4/relay")
  {
    if(payload == "on")
    {
      setRelay(1);
    }
    else
    {
    setRelay(0);
    }
  }
}