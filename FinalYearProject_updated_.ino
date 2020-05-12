#include <BearSSLHelpers.h>
#include <CertStoreBearSSL.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WiFiType.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WiFiClientSecureAxTLS.h>
#include <WiFiClientSecureBearSSL.h>
#include <WiFiServer.h>
#include <WiFiServerSecure.h>
#include <WiFiServerSecureAxTLS.h>
#include <WiFiServerSecureBearSSL.h>
#include <WiFiUdp.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>
#include <ESP8266wifi.h>

/********************************************************************************************************************/
/*
      Daenkai Boonrat
      Voice Control Home Automation

*/
/********************************************************************************************************************/


#define Relay1            D1 /*Red Light*/
#define Relay2            D2 /*Green Light*/
#define Relay3            D3 /*Fan Light*/

/*my home wifi*/
#define WLAN_SSID       "UPC604523"             
#define WLAN_PASS       "GUOZWGTD"        

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com" //Adafruit Server
#define AIO_SERVERPORT  1883  

/**/               
#define AIO_USERNAME    "dannygmit"            // Username
#define AIO_KEY         "aio_bHZY74VyzYnfHwEzVbafK2Aas6DR"   // Auth Key

//WIFI CLIENT
WiFiClient client;

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

//Red Light
Adafruit_MQTT_Subscribe red = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME"/feeds/Relay1");

//Green Light
Adafruit_MQTT_Subscribe green = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Relay2");

//Fan
Adafruit_MQTT_Subscribe fan = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Relay3");
void MQTT_connect();



void setup() {
  Serial.begin(115200);

  pinMode(Relay1, OUTPUT);
  pinMode(Relay2, OUTPUT);
  pinMode(Relay3, OUTPUT);
  
  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); 
  Serial.println(WiFi.localIP());
 
  mqtt.subscribe(&red);
  mqtt.subscribe(&green);
  mqtt.subscribe(&fan);
}

void loop() {
 
  MQTT_connect();
  

  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(20000))) {
    
    if (subscription == &red) {
      Serial.print(F("Got: "));
      Serial.println((char *)red.lastread);
      int red_State = atoi((char *)red.lastread);
      digitalWrite(Relay1, red_State);
      
    }
    if (subscription == &green) {
      Serial.print(F("Got: "));
      Serial.println((char *)green.lastread);
      int green_State = atoi((char *)green.lastread);
      digitalWrite(Relay2, green_State);
    }
     if (subscription == &fan) {
      Serial.print(F("Got: "));
      Serial.println((char *)fan.lastread);
      int fan_State = atoi((char *)fan.lastread);
      digitalWrite(Relay3, fan_State);
    }
   
  }

  
}

void MQTT_connect() {
  int8_t ret;

  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  
  while ((ret = mqtt.connect()) != 0) {
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000); 
    retries--;
    if (retries == 0) {
      while (1);
    }
  }
  Serial.println("MQTT Connected!");
  
}
