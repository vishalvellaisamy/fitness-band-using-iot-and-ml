/***************************************************
  Adafruit MQTT Library ESP8266 Example

  Must use ESP8266 Arduino from:
    https://github.com/esp8266/Arduino

  Works great with Adafruit's Huzzah ESP board & Feather
  ----> https://www.adafruit.com/product/2471
  ----> https://www.adafruit.com/products/2821

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Tony DiCola for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "iphone 12 pro max"
#define WLAN_PASS       "vishal123"

//#define WLAN_SSID       "Abu king of Ajantha" // User ID
//#define WLAN_PASS       "abuzar1344"          // Pass

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "sumproject74"
#define AIO_KEY         "aio_sxmy444pA0NdYJ1Q01v5gxxOyDyA"

/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiClientSecure for SSL
//WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/

// Setup a feed called 'photocell' for publishing.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
Adafruit_MQTT_Publish photocell1 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/pulse");

Adafruit_MQTT_Publish photocell2 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/pulse_status");

Adafruit_MQTT_Publish photocell3 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/temp");

// Setup a feed called 'onoff' for subscribing to changes.
//Adafruit_MQTT_Subscribe onoffbutton = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/onoff");

/*************************** Sketch Code ************************************/

// Bug workaround for Arduino 1.6.6, it seems to need a function declaration
// for some reason (only affects ESP8266, likely an arduino-builder bug).
void MQTT_connect();


int Signal;

int randNumber;

int cnt1 = 0, cnt2 = 0, cnt3 = 0;

char arr[10], inByte, ii, start_flag = 0, over_flag = 0; 


void setup() 
{
  Serial.begin(9600);

  delay(10);

  Serial.println(F("Adafruit MQTT demo"));

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
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  // Setup MQTT subscription for onoff feed.
  //mqtt.subscribe(&onoffbutton);
}

void loop() 
{
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();

  // this is our 'wait for incoming subscription packets' busy subloop
  // try to spend your time here

//  Adafruit_MQTT_Subscribe *subscription;
//  while ((subscription = mqtt.readSubscription(3000))) {
//    if (subscription == &onoffbutton) {
//      Serial.print(F("Got: "));
//      Serial.println((char *)onoffbutton.lastread);
//    }
//  }

   
   while(Serial.available() > 0) 
   {
    inByte = Serial.read();

    if(start_flag == 0)
    {
     if(inByte == '%')
     {
      ii = 0;

      start_flag = 1;
     }  
    }
    else if(start_flag == 1)
    {
     if(inByte == '#')
     {
      over_flag = 1;

      start_flag = 0;
     }
     else 
     {
      arr[ii++] = inByte;
     }  
    }
   }

   if(over_flag == 1)
   {
    Signal = (((int)arr[0] - 48) * 100) + (((int)arr[1] - 48) * 10) + (((int)arr[2] - 48) * 1);
      
    randNumber = random(99, 101);

    if(Signal == 0)
    {
     ++cnt1; if(cnt1 > 3)cnt1 = 1;

          if(cnt1 == 1)photocell1.publish(Signal);
     else if(cnt1 == 2)photocell2.publish("NOT DETECTED");
     else if(cnt1 == 3)photocell3.publish(randNumber); 

     cnt2 = 0; cnt3 = 0;
    }
    else if(Signal >= 60 && Signal <= 80)
    {
     ++cnt2; if(cnt2 > 3)cnt2 = 1;

          if(cnt2 == 1)photocell1.publish(Signal);
     else if(cnt2 == 2)photocell2.publish("NORMAL");
     else if(cnt2 == 3)photocell3.publish(randNumber);

     cnt1 = 0; cnt3 = 0;
    }
    else if(Signal != 0)
    {
     ++cnt3; if(cnt3 > 3)cnt3 = 1;
     
          if(cnt3 == 1)photocell1.publish(Signal);
     else if(cnt3 == 2)
     {
           if(Signal < 60)photocell2.publish("LOW");
      else if(Signal > 80)photocell2.publish("HIGH");
     }
     else if(cnt3 == 3)photocell3.publish(randNumber);
    
     cnt1 = 0; cnt2 = 0;
    }
    
    over_flag = 0;
   }


//  // Now we can publish stuff!
//  Serial.print(F("\nSending photocell val "));
//  Serial.print(x);
//  Serial.print("...");
//  if (! photocell.publish(x++)) {
//    Serial.println(F("Failed"));
//  } else {
//    Serial.println(F("OK!"));
//  }

  // ping the server to keep the mqtt connection alive
  // NOT required if you are publishing once every KEEPALIVE seconds
  /*
  if(! mqtt.ping()) {
    mqtt.disconnect();
  }
  */
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}
