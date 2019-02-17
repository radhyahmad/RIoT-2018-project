#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <Adafruit_Sensor.h>
//#include <DHT.h>
#include <Wire.h>
#include <BH1750.h>
#include<OneWire.h>
#include<DallasTemperature.h>

//library JSON

//#define DHTPIN D5
//#define DHTTYPE DHT22
#define ONE_WIRE_BUS D4

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

const char* ssid = "";
const char* password = "";
const char* mqtt_server = "";


WiFiClient espClient;
PubSubClient client(espClient);
long lastData = 0;
//DHT dht(DHTPIN, DHTTYPE);
BH1750 lightMeter;


void setup_wifi() {

  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  //WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {

  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  for (int i = 0; i < length; i++) {

    Serial.print((char)payload[i]);
  }
  Serial.println();


  if ((char)payload[0] == '1') {
    //digitalWrite(D2, HIGH);

  }

  if((char)payload[0] == '0'){
    
    //digitalWrite(D2, LOW);
  }

}

void reconnect() {

  while (!client.connected()) {

    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX); 

    if (client.connect(clientId.c_str())) { 

      Serial.println("connected");
      //client.publish("output/bacaSensor", "sensor value");
      client.subscribe("input/kendaliAktuator");

    } else {
      
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");

      delay(5000);
      
    }
  }
}

void setup() {

  //pinMode(D5, OUTPUT);
  Serial.begin(9600);
  //dht.begin();
  Wire.begin();
  lightMeter.begin();
  sensors.begin();
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    
    reconnect();
  }

  client.loop();

  float h = 50.67;
  float t = 30.67;;
  
  //if(isnan(h) || isnan(t)){

    //Serial.println("Failed to read from DHT sensor");
    //return;
  //} 
  
  float tS = sensors.getTempCByIndex(0);
  float l = lightMeter.readLightLevel();
  int adcSoil = analogRead(A0);
  sensors.requestTemperatures();
  float soilHumidity = 0;
  soilHumidity = ((1023.0 - adcSoil)/1023.0)*100;
  
  String temperature = String(t);
  String humidity = String(h);
  String intensity = String(l);
  String soilTemp = String(tS);
  String soilMoisture = String(soilHumidity);
  
  
  String payload = "{\"airTemperature\":";
  payload += temperature;
  payload += ",\"airHumidity\":";
  payload += humidity;
  payload += ",\"intensity\":";
  payload += intensity;
  payload += ",\"soilTemperature\":";
  payload += soilTemp;
  payload += ",\"soilMoisture\":";
  payload += soilMoisture;
  payload += "}";
  
  char dataSensor[250];
  
  long now = millis();

  if (now - lastData > 15000) {

    lastData = now;
    payload.toCharArray(dataSensor, 250);
    client.publish("output/bacaSensor", dataSensor);
    Serial.println(dataSensor);

   
  }
}
