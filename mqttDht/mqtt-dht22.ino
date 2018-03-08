#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "DHT.h"

#define WLAN_SSID       "Tante Meier"
#define WLAN_PASS       "giNGFDu9fKU4"

#define MQTT_SERVER      "192.168.1.3"
#define MQTT_SERVERPORT  1883                   // use 8883 for SSL
#define MQTT_USERNAME    "admin"
#define MQTT_KEY         "lkwpeter"

DHT dht(D5, DHT22);

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, MQTT_SERVERPORT, MQTT_USERNAME, MQTT_KEY);

Adafruit_MQTT_Publish temp = Adafruit_MQTT_Publish(&mqtt, "bathroom/temp");
Adafruit_MQTT_Publish humid = Adafruit_MQTT_Publish(&mqtt, "bathroom/humid");

// Bug workaround for Arduino 1.6.6, it seems to need a function declaration
// for some reason (only affects ESP8266, likely an arduino-builder bug).
void MQTT_connect();

void setup() {
  dht.begin();

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void loop() {
  MQTT_connect();

  temp.publish(dht.readTemperature());
  humid.publish(dht.readHumidity());
  
  if(! mqtt.ping()) {
    mqtt.disconnect();
  }

  delay(60000);
}

void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
}
