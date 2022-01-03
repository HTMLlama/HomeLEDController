#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <keys.h>
#include <FastLED.h>
#include <soc/soc.h>
#include <soc/rtc_cntl_reg.h>

#define LED_PIN 16
#define NUM_LED 270
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
#define BOXES 13
#define FX_TOTAL 14
int fx = 0;
bool isXmasSet = false;
int fadeHue = 100;

int pcmrHue = 0;
int bgt = 200;
String rgbStr1 = "FFFFFF";
String rgbStr2 = "00FF00";
CRGB backColor = CRGB::SlateBlue;
CRGB box1Color = CRGB::Tomato;
CRGB box2Color = CRGB::Aqua;
CRGB box3Color = CRGB::Green;
CRGB box4Color = CRGB::Indigo;
CRGB box5Color = CRGB::Lavender;
CRGB box6Color = CRGB::LemonChiffon;
CRGB box7Color = CRGB::Salmon;
CRGB box8Color = CRGB::SeaGreen;
CRGB box9Color = CRGB::SkyBlue;
CRGB box10Color = CRGB::Violet;

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

unsigned long toColorCode(String colorHex) {
  int len = colorHex.length() + 1;
  char hexArray[len];
  colorHex.toCharArray(hexArray, len);
  unsigned long color = strtoul(hexArray, NULL, 16);
  return color;
}

void lightSection(int blockSize) {
  for (size_t i = 0; i < NUM_LED; i+=blockSize*2) {
      for (size_t n = 0; n < blockSize; n++) {
        int indexA = i+n;
        int indexB = i+n+blockSize;
        if (indexA < NUM_LED) {
          leds[indexA] = CRGB().setColorCode(toColorCode(rgbStr1));
        }
        if (indexB < NUM_LED) {
          leds[indexB] = CRGB().setColorCode(toColorCode(rgbStr2));
        }
      } 
    }
}

void lightBoxes() {
  for (size_t i = 0; i < 90; i++) {
    leds[i] = backColor;
  }
  for (size_t i = 90; i < 105; i++) {
    leds[i] = box1Color;
  }
  for (size_t i = 105; i < 120; i++) {
    leds[i] = box2Color;
  }
  for (size_t i = 120; i < 135; i++) {
    leds[i] = box3Color;
  }
  for (size_t i = 135; i < 150; i++) {
    leds[i] = box4Color;
  }
  for (size_t i = 150; i < 165; i++) {
    leds[i] = box5Color;
  }
  for (size_t i = 165; i < 180; i++) {
    leds[i] = box6Color;
  }
  for (size_t i = 180; i < 195; i++) {
    leds[i] = box7Color;
  }
  for (size_t i = 195; i < 210; i++) {
    leds[i] = box8Color;
  }
  for (size_t i = 210; i < 240; i++) {
    leds[i] = box9Color;
  }
  for (size_t i = 240; i < 270; i++) {
    leds[i] = box10Color;
  }
}

void setFx(String selectedFx) {
  if (selectedFx == "solid") {
    fx = SOLID;
  } else if(selectedFx == "pcmr") {
    fx = PCMR;
  } else if(selectedFx == "small") {
    fx = SMALL;
  } else if(selectedFx == "med") {
    fx = MED;
  } else if(selectedFx == "large") {
    fx = LARGE;
  } else if(selectedFx == "xl") {
    fx = XL;
  } else if(selectedFx == "xxl") {
    fx = XXL;
  } else if(selectedFx == "color_fade") {
    fx = COLOR_FADE;
  } else if(selectedFx == "pride") {
    fx = PRIDE;
  } else if(selectedFx == "xmas") {
    fx = XMAS;
  } else if(selectedFx == "solid2") {
    fx = SOLID2;
  } else if(selectedFx == "murica") {
    fx = MURICA;
  } else if(selectedFx == "xylon") {
    fx = XYLON;
  } else if(selectedFx == "boxes") {
    fx = BOXES;
  } else {
    pubSubClient.publish("error/livingroom/leds", "Invalid Effect", 2);
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
      lightSection(5);
      break;
    
    case MED:
      lightSection(10);
      break;

    case LARGE:
      lightSection(15);
      break;

    case XL:
      lightSection(30);
      break;

    case XXL:
      lightSection(90);
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
      fill_rainbow(leds, NUM_LED, pcmrHue, 2);
      break;

    case XMAS:
      maybeBuildXmas();
      break;

    case SOLID2:
      for(int i = 0; i < NUM_LED; i++) {
        leds[i] = CRGB().setColorCode(toColorCode(rgbStr2));
      }
      break;

    case COLOR_FADE:
      for(int i = 0; i < NUM_LED; i++) {
        leds[i] = CRGB().setHSV(fadeHue, 250, 250);
      }
      break;

    case XYLON:
      for(int i = 0; i < NUM_LED/2; i++) {   
        leds.fadeToBlackBy(40);
        leds[i] = CHSV(pcmrHue++,255,255);
        leds(NUM_LED/2,NUM_LED-1) = leds(NUM_LED/2 - 1 ,0);
        FastLED.delay(33);
      }
      break;

    case BOXES:
      lightBoxes();
      break;
    
    default:
      for(int i = 0; i < NUM_LED; i++) {
        leds[i] = CRGB().setColorCode(toColorCode(rgbStr1));
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

  String topicStr(topic);
  String category = topicStr.substring(16);

  if (category == "fx") {
    setFx(messageTemp);
  } else if (category == "rgb1") {
    rgbStr1 = messageTemp;
  } else if (category == "rgb2") {
    rgbStr2 = messageTemp;
  } else if (category == "bgt") {
    bgt = messageTemp.toInt();
  } else if (category == "box0") {
    backColor = CRGB().setColorCode(toColorCode(messageTemp));
  } else if (category == "box1") {
    box1Color = CRGB().setColorCode(toColorCode(messageTemp));
  } else if (category == "box2") {
    box2Color = CRGB().setColorCode(toColorCode(messageTemp));
  } else if (category == "box3") {
    box3Color = CRGB().setColorCode(toColorCode(messageTemp));
  } else if (category == "box4") {
    box4Color = CRGB().setColorCode(toColorCode(messageTemp));
  } else if (category == "box5") {
    box5Color = CRGB().setColorCode(toColorCode(messageTemp));
  } else if (category == "box6") {
    box6Color = CRGB().setColorCode(toColorCode(messageTemp));
  } else if (category == "box7") {
    box7Color = CRGB().setColorCode(toColorCode(messageTemp));
  } else if (category == "box8") {
    box8Color = CRGB().setColorCode(toColorCode(messageTemp));
  } else if (category == "box9") {
    box9Color = CRGB().setColorCode(toColorCode(messageTemp));
  } else if (category == "box10") {
    box10Color = CRGB().setColorCode(toColorCode(messageTemp));
  }
  
  delay(100);
  Serial.print("Topic in: ");
  Serial.println(topic);
  Serial.print("Message in: ");
  Serial.println(messageTemp);
  // delay(200);
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
      pubSubClient.subscribe("livingroom/leds/#");
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
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

  Serial.begin(115200);

  setupWifi();
  setupPubSub();
  setupLeds();

  delay(1500);

  pubSubClient.publish("livingroom/leds/boot", "1", 2);
}

void loop() {

  if (!pubSubClient.connected()){
    reconnectToPubSub();
  }
  pubSubClient.loop();

  EVERY_N_MILLISECONDS(20) { displayLights(); }
  EVERY_N_MINUTES(10) { fadeHue = roller(fadeHue); }
  
}