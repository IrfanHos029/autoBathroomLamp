#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <WiFiManager.h>
#include <EEPROM.h>

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
#define pinJum D3

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
bool wm_nonblocking = false;
bool stateWifi;
bool stateMode;
WiFiManager wifi;

  
void setup() {
  Serial.begin(115200);
  pinMode(sensor, INPUT_PULLUP);
  pinMode(pinJum,INPUT_PULLUP);
  pinMode(led_war, OUTPUT);
  pinMode(ledFlip, OUTPUT);
  pinMode(relay_1, OUTPUT);
  digitalWrite(relay_1, LOW);
  EEPROM.begin(12);
//  EEPROM.write(0,1); 
//     EEPROM.commit();
  if(wm_nonblocking) wifi.setConfigPortalBlocking(false);
stateWifi = EEPROM.read(0);
  stateMode = EEPROM.read(0);
  Serial.println(String()+"stateWifisetup=" + stateWifi + "stateModesetup=" + stateMode);
  pinMode(dimmer_pin, OUTPUT);
  pinMode(led_pin, OUTPUT);
  digitalWrite(led_pin, LOW);
  if(stateWifi==1){
  wifi.setConfigPortalTimeout(60);
  bool connectWIFI = wifi.autoConnect("JAM DIGITAL", "00000000");
  //keluarkan tulisan RTC
  if (!connectWIFI) {
    stateWifi=0;
    Serial.println("NOT CONNECTED TO AP");
    Serial.println("Pindah ke mode offlien");
    //showRTC();
    delay(1000);
    EEPROM.write(0,stateWifi);
    EEPROM.commit();
digitalWrite(led_pin, LOW);
    ESP.restart();
  }
  else
  {
    Serial.println("CONNECTED");
    /* switch on led */
 // pinMode(led_pin, OUTPUT);
  digitalWrite(led_pin, HIGH);

  Serial.println("Booting");
 // WiFi.mode(WIFI_STA);

 // WiFi.begin(ssid, password);

//  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
//    WiFi.begin(ssid, password);
//    Serial.println("Retrying connection...");
//  }
  /* switch off led */
  digitalWrite(led_pin, LOW);

  /* configure dimmers, and OTA server events */
  analogWriteRange(1000);
  //analogWrite(led_pin, 2);

  
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

  delay(1000);
 // Serial.println(String()+"NTP in the setup:"+ Clock.getHours()+":"+ Clock.getMinutes()+":"+Clock.getSeconds());
   }
  }
  

 //digitalWrite(dimmer_pin, LOW);
  /* setup the OTA server */


}

void loop() {
  ArduinoOTA.handle();
  if(wm_nonblocking) wifi.process();
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

//  Serial.println(String() + "stateOut:" + stateOut);
//  // Serial.println(String() + "trigg   :"+ trigg);
//  Serial.println(String() + "Time   :" + Time);
if(digitalRead(pinJum)==LOW){  
  stateWifi = !stateWifi; 
  delay(2000); 
  digitalWrite(dimmer_pin, HIGH);
  digitalWrite(led_pin, HIGH);
  EEPROM.write(0,stateWifi); 
     EEPROM.commit();
  delay(1000);
  ESP.restart();}
}

void cekLogic()
{
  int data = digitalRead(sensor);
 // Serial.println(String() + "data:" + data);
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
