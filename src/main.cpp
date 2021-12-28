#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <keys.h>
#include <FastLED.h>

#define LED_PIN 16
#define NUM_LED 20
CRGBArray<NUM_LED> leds;

#define SOLID 0
#define PCMR 1
#define SMALL 2
#define MED 3
#define LARGE 4
#define XL 5
#define XXL 6
#define COLOR_FADE 7
#define PRIDE 8
#define XMAS 9
#define SOLID2 10
#define MURICA 11
#define XYLON 12
#define FX_TOTAL 13
int fx = PCMR;
bool isXmasSet = false;
int fadeHue = 100;

int hue = 255;
int hue2 = 100;
int sat = 255;
int sat2 = 255;
int val = 255;
int val2 = 255;
int pcmrHue = 0;
int bgt = 200;

WiFiClient wiFiClient;
PubSubClient pubSubClient(wiFiClient);
long lastMessage = 0;
char msg[100];
int value = 0;

const char* LB = "-------------------------------";

int roller(int item) {
  if (item > 255) {
    item = 0;
  } else if (item < 0) {
    item = 255;
  }
  return item;
}

void lightSection(int blockSize) {
  for (size_t i = 0; i < NUM_LED; i+=blockSize*2) {
      for (size_t n = 0; n < blockSize; n++) {
        int indexA = i+n;
        int indexB = i+n+blockSize;
        if (indexA < NUM_LED) {
          leds[indexA] = CRGB().setHSV(hue, sat, val);
        }
        if (indexB < NUM_LED) {
          leds[indexB] = CRGB().setHSV(hue2, sat2, val2);;
        }
      } 
    }
}

void maybeBuildXmas() {
  if (!isXmasSet) {
    for (size_t i = 0; i < NUM_LED; i++) {
      leds[i] = CRGB().setHSV(random(0, 255), random(200, 255), random(200, 255));
    }
    isXmasSet = true;
  }
}

void displayLights() {
  FastLED.setBrightness(bgt);
  switch (fx) {

    case PCMR:
        fill_rainbow(leds, NUM_LED, pcmrHue, 2);
        pcmrHue++;
        if (pcmrHue > 255) {
          pcmrHue = 0;
        }
        
      break;

    case SMALL:
      lightSection(8);
      break;
    
    case MED:
      lightSection(12);
      break;

    case LARGE:
      lightSection(18);
      break;

    case XL:
      lightSection(26);
      break;

    case XXL:
      lightSection(34);
      break;
    
    case MURICA:
      for (size_t i = 0; i < NUM_LED; i+=27) {
        for (size_t n = 0; n < 9; n++) {
          int indexA = i+n;
          int indexB = i+n+9;
          int indexC = i+n+9+9;
          if (indexA < NUM_LED) {
            leds[indexA] = CRGB::Red;
          }
          if (indexB < NUM_LED) {
            leds[indexB] = CRGB::White;
          }
          if (indexC < NUM_LED) {
            leds[indexC] = CRGB::Blue;
          }
        }
      }
      break;

    case PRIDE:
      fill_rainbow( leds, NUM_LED, hue, 2);
      break;

    case XMAS:
      maybeBuildXmas();
      break;

    case SOLID2:
      for(int i = 0; i < NUM_LED; i++) {
        leds[i] = CRGB().setHSV(hue2, sat2, val2);
      }
      break;

    case COLOR_FADE:
      for(int i = 0; i < NUM_LED; i++) {
        leds[i] = CRGB().setHSV(fadeHue, 250, 250);
      }
      break;

    case XYLON:
      for(int i = 0; i < NUM_LED/2; i++) {   
        // fade everything out
        leds.fadeToBlackBy(40);

        // let's set an led value
        leds[i] = CHSV(hue++,255,255);

        // now, let's first 20 leds to the top 20 leds, 
        leds(NUM_LED/2,NUM_LED-1) = leds(NUM_LED/2 - 1 ,0);
        FastLED.delay(33);
      }
      break;
    
    default:

      for(int i = 0; i < NUM_LED; i++) {
        leds[i] = CRGB().setHSV(hue, sat, val);
      }
      break;
  }

  FastLED.show();
}


// _________________________________________________________________

void callback(char* topic, byte* message, unsigned int lenght) {
  String messageTemp;
  for (size_t i = 0; i < lenght; i++) {
    messageTemp += (char) message[i];
  }

  Serial.print("Message on Topic: ");
  Serial.println(topic);
  Serial.print("Message: ");
  Serial.println(messageTemp);
}

void setupWifi() {
  delay(12);
  Serial.println(LB);
  Serial.print("Connecting to: ");
  Serial.println(SSID);

  WiFi.begin(SSID, PWD);

  while (WiFi.status() != WL_CONNECTED){
    delay(420);
    Serial.print(">");
  }

  Serial.println();
  Serial.println("Connected to WiFi!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void setupPubSub() {
  pubSubClient.setServer(MQTT_SERVER, 1883);
  pubSubClient.setCallback(callback);
}

void reconnectToPubSub() {
  while(!pubSubClient.connected()) {
    Serial.println("MQTT Connecting...");
    if (pubSubClient.connect("ESP32Client")) {
      Serial.println("Connected to MQTT!");
      pubSubClient.subscribe("esp32/output");
    } else {
      Serial.println("Failed to connect to MQTT");
      Serial.print("State: ");
      Serial.println(pubSubClient.state());
      Serial.println("Trying again in 5 seconds...");
      delay(5000);
    }
  }
}

void setupLeds() {
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LED);
  FastLED.setBrightness(bgt);
}

void setup() {
  Serial.begin(115200);

  setupWifi();
  setupPubSub();
  setupLeds();

  delay(1500);
}

void loop() {

  if (!pubSubClient.connected()){
    reconnectToPubSub();
  }
  pubSubClient.loop();

  EVERY_N_SECONDS(15) { pubSubClient.publish("test", "I made cool LEDS!"); }
  EVERY_N_MILLISECONDS(20) { displayLights(); }
  
}