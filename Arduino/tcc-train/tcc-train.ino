// for I2C Communication
#include <time.h>
#include <sys/time.h>

#include <Wire.h>
#include <PN532_I2C.h>
#include <PN532.h>
#include <NfcAdapter.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ArduinoMqttClient.h>
#include <WiFi.h>
#include <Servo.h>

#include "arduino_secrets.h"

// I2C bus
const int sdapin = 16;  // PICO_DEFAULT_I2C_SDA_PIN // 4
const int sclpin = 17;  // PICO_DEFAULT_I2C_SCL_PIN // 5

// screen
#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 32  // OLED display height, in pixels
#define BUTTON_PIN BOOTSEL
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// On an arduino UNO:       A4(SDA), A5(SCL)
#define OLED_RESET -1        // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C  ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
int count = 0;

// NFC
PN532_I2C pn532_i2c = PN532_I2C(Wire);
NfcAdapter nfc = NfcAdapter(pn532_i2c);

const int knownTags = 20;
const String tagIds[] = { "04 65 98 FA C1 1C 91", "04 5A 98 FA C1 1C 91", "04 59 98 FA C1 1C 91", "04 4E 98 FA C1 1C 91", "04 4D 98 FA C1 1C 91",
                          "04 5E 99 FA C1 1C 91", "04 69 99 FA C1 1C 91", "04 6A 99 FA C1 1C 91", "04 75 99 FA C1 1C 91", "04 76 99 FA C1 1C 91",
                          "1D 83 74 53 08 10 80", "1D 84 74 53 08 10 80", "1D 85 74 53 08 10 80", "1D 86 74 53 08 10 80", "1D 87 74 53 08 10 80",
                          "1D 88 74 53 08 10 80", "1D 89 74 53 08 10 80", "1D 8A 74 53 08 10 80", "1D 8B 74 53 08 10 80", "1D 8C 74 53 08 10 80" };

String tagId = "";
String displayLine[] = { "", "", "", "", "" };
char line[32];
time_t now;
time_t timeLastTrain;
char timeDisplayed[80];
char time_buff[80];

boolean invertedDisplay = false;

// MQTT
const char ssid[] = SECRET_SSID;  // your network SSID (name)
const char pass[] = SECRET_PASS;  // your network password (use for WPA, or use as key for WEP)

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = "192.168.178.232";
int port = 1883;
const char topic[] = "tcc/train";

const long interval = 1000;
unsigned long previousMillis = 0;

// Servo
const int nrOfServos = 2;
const int servoPin[nrOfServos] = { 0, 1 };
// Create a servo objects
Servo myservo[nrOfServos];

void setup(void) {
  Serial.begin(115200);
  if (!Serial) delay(1000);
  Serial.println("System ready");

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // IC2 bus
  Wire.setSDA(sdapin);
  Wire.setSCL(sclpin);

  setupTime();
  setupDisplay();
  setupNFC();
  setupWIFI();
  setupMQTT();
  setupServo();

  delay(2000);
}

void loop() {
  // toggleLed(LED_BUILTIN);
  togglePixel(127, 0);
  // checkInvertDisplay();
  readTrain();
  mqttClient.poll();
  count++;
}

void setupTime() {
  struct timeval tv;
  tv.tv_sec = 1726250839;  // Fri Sep 13 2024 20:07:19 GMT+0200 (Midden-Europese zomertijd)
  tv.tv_usec = 0;
  settimeofday(&tv, nullptr);
  time(&now);
  timeLastTrain = now;
}

void setupDisplay() {
  // DISPLAY SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;) {  // Don't proceed, loop forever
      toggleLed(LED_BUILTIN);
      delay(100);
    }
  }
  display.clearDisplay();
  display.setTextSize(1);               // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);  // Draw white text
  display.setCursor(0, 0);
  display.invertDisplay(invertedDisplay);
}

void setupNFC() {
  nfc.begin(true);
  display.println("NFC initialized");
  display.display();
}

void setupWIFI() {
  // WIFI: attempt to connect to WiFi network:
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    // failed, retry
    display.print(".");
    display.display();
    delay(5000);
  }
  display.print("WIFI: ");
  display.println(ssid);
  display.display();
}

void setupMQTT() {
  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT failed: ");
    Serial.println(mqttClient.connectError());
    for (;;) {  // Don't proceed, loop forever
      toggleLed(LED_BUILTIN);
      delay(1000);
    }
  }
  display.print("MQTT: ");
  display.println(broker);
  display.display();

  mqttClient.onMessage(onMqttMessage);
  // subscribe to tcc/switch topic
  mqttClient.subscribe("tcc/switch");
}

void setupServo() {
  for (int s = 0; s < nrOfServos; s++) {
    myservo[s].attach(servoPin[s]);
    myservo[s].write(30);
  }
}

void checkInvertDisplay() {
  // check if usr button is pressed
  boolean newState = digitalRead(BUTTON_PIN);
  if (newState == LOW) {
    // Short delay to debounce button.
    delay(20);
    newState = digitalRead(BUTTON_PIN);
    if (newState == LOW) {
      invertedDisplay = !invertedDisplay;
      display.invertDisplay(invertedDisplay);
    }
  }
}

void readTrain() {
  if (readTag()) {
    bool tagIdentified = false;
    for (int i = 0; i < knownTags && !tagIdentified; i++) {
      if (tagId == tagIds[i]) {
        sprintf(line, "%s  Trein %2i", getTime(), i + 1);
        tagIdentified = true;
        displayNewLine(line);
        // send message, the Print interface can be used to set the message contents
        mqttClient.beginMessage(topic);
        mqttClient.print(line);
        mqttClient.endMessage();
      }
    }
    if (!tagIdentified) {
      displayNewLine("Onbekende Trein:");
      displayNewLine(tagId);
      Serial.println(tagId);
    }
    timeLastTrain = now;
  } else {
    time(&now);
    if (now > timeLastTrain + 10) {
      displayTime();
    } else {
      // reset timeDisplayed
      timeDisplayed[0] = 0;
    }
  }
}

bool readTag() {
  bool newTagFound = false;
  if (nfc.tagPresent(100)) {
    NfcTag tag = nfc.read();
    if (tagId != tag.getUidString()) {
      tagId = tag.getUidString();
      newTagFound = true;
      // tag.print();
    }
  } else {
    tagId = "";
  }
  return newTagFound;
}

void togglePixel(int16_t x, int16_t y) {
  int16_t c = not display.getPixel(x, y);
  display.drawPixel(x, y, c);
  // display.drawPixel(x - 1, y, c);
  // display.drawPixel(x, y, c);
  // display.drawPixel(x, y + 1, c);
  display.display();
}

void toggleLed(const int led) {
  if (digitalRead(led) == LOW) {
    digitalWrite(led, HIGH);
  } else {
    digitalWrite(led, LOW);
  }
}

void displayNewLine(const String& i_line) {
  displayLine[4] = i_line;

  display.clearDisplay();
  display.setTextSize(1);               // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);  // Draw white text
  display.setCursor(0, 0);

  // scroll up
  for (int8_t l = 0; l < 4; l++) {
    displayLine[l] = displayLine[l + 1];
    display.println(displayLine[l]);
  }
  display.display();
}

void displayTime() {
  if (strcmp(timeDisplayed, getTime()) != 0) {
    strcpy(timeDisplayed, getTime());
    for (int8_t l = 0; l < 5; l++) {
      displayLine[l] = "";
    }
    display.clearDisplay();
    display.setTextSize(2);               // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);  // Draw white text
    display.setCursor(40, 10);
    display.println(timeDisplayed);
    display.display();
  }
}

String removeSpaces(const String& txt) {
  String result;
  for (int8_t l = 0; l < txt.length(); l++) {
    if (txt[l] != ' ') {
      result += txt[l];
    }
  }
  return result;
}

char* getTime() {
  time(&now);
  strftime(time_buff, sizeof(time_buff), "%H:%M", localtime(&now));
  return time_buff;
}

char* getDateTime() {
  time(&now);
  strftime(time_buff, sizeof(time_buff), "%F %T", localtime(&now));
  return time_buff;
}

void setTurnout(int turnout, bool straight) {
  myservo[turnout].write(straight ? 30 : 150);
  Serial.print("set turnout:");
  Serial.print(turnout);
  Serial.print(" straight:");
  Serial.println(straight);
}

void onMqttMessage(int messageSize) {
  char message[64];
  int c = 0;
  String topic = mqttClient.messageTopic();
  // use the Stream interface to print the contents
  while (mqttClient.available()) {
    message[c++] = (char)mqttClient.read();
  }
  message[c] = '\0';

  // we received a message, print out the topic and contents
  Serial.println("Received a message with topic '");
  Serial.print(topic);
  Serial.print("', length ");
  Serial.print(messageSize);
  Serial.println(" bytes:");
  Serial.println(message);

  if (topic == "tcc/switch") {
    int turnout = atoi(message);
    bool straight = (message[strlen(message)-1] == 's');
    setTurnout(turnout, straight);
  }
}
