#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#ifndef STASSID
#define STASSID "Wifi"
#define STAPSK  "00000000"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;
const char* host = "OTA-LEDS";

int led_pin = 4;
#define N_DIMMERS 1
int dimmer_pin = 16;

#define sensor D5 //14
#define led_war D1//5
#define relay_1 D7 //gpio13
#define ledFlip D4//2
bool stateOut;
bool trigg;
bool stateWar;
uint16_t Time = 0;
bool flag;
bool stateFlip;
unsigned long saveTimer = 0;
unsigned long saveFlip = 0;

void setup() {
  Serial.begin(115200);
  pinMode(sensor, INPUT_PULLUP);
  pinMode(led_war, OUTPUT);
  pinMode(ledFlip, OUTPUT);
  pinMode(relay_1, OUTPUT);
  digitalWrite(relay_1, LOW);

  /* switch on led */
  pinMode(led_pin, OUTPUT);
  digitalWrite(led_pin, HIGH);

  Serial.println("Booting");
  WiFi.mode(WIFI_STA);

  WiFi.begin(ssid, password);

  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    WiFi.begin(ssid, password);
    Serial.println("Retrying connection...");
  }
  /* switch off led */
  digitalWrite(led_pin, LOW);

  /* configure dimmers, and OTA server events */
  analogWriteRange(1000);
  //analogWrite(led_pin, 2);

  pinMode(dimmer_pin, OUTPUT);
  analogWrite(dimmer_pin, 50);


  ArduinoOTA.setHostname(host);
  ArduinoOTA.onStart([]() {
    analogWrite(dimmer_pin, 0);
    analogWrite(led_pin, 990);
  });

  ArduinoOTA.onEnd([]() { // do a fancy thing with our board led at end
    for (int i = 0; i < 30; i++) {
      analogWrite(led_pin, (i * 100) % 1001);
      delay(50);
    }
  });

  ArduinoOTA.onError([](ota_error_t error) {
    (void)error;
    ESP.restart();
  });

  /* setup the OTA server */
  ArduinoOTA.begin();
  Serial.println("Ready");

}

void loop() {
  ArduinoOTA.handle();
  cekLogic();
  runWar();
  indikator();
  if (Time >= 122)
  {
    stateOut = 0;
    trigg = 0;
    Time = 0;
  }
  digitalWrite(relay_1, stateOut);

  Serial.println(String() + "stateOut:" + stateOut);
  // Serial.println(String() + "trigg   :"+ trigg);
  Serial.println(String() + "Time   :" + Time);
}

void cekLogic()
{
  int data = digitalRead(sensor);
  Serial.println(String() + "data:" + data);
  //flag = data;
  if (data == HIGH) {
    stateOut = 1;
    trigg = 1;
    flag = 0;
    Time = 0;
  }
  else {
    flag = 1;
  }
}

void runWar() {
  unsigned long timer = millis();
  if (trigg == 1 && flag == 1 && timer - saveTimer > 1000)
  {
    saveTimer = timer;
    stateWar = !stateWar;
    Time++;

  }
  else
  {
    stateWar = 0;
  }
  digitalWrite(led_war, stateWar);
}

void indikator()
{
  unsigned long tmr = millis();
  if (tmr - saveFlip > 1000)
  {
    saveFlip = tmr;
    stateFlip = !stateFlip;
    digitalWrite(ledFlip, stateFlip);
  }
}
