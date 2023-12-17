#define sensor D5 //2
#define led_war D1//3
#define relay_1 D7 //gpio13
#define ledFlip D4
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
  pinMode(ledFlip,OUTPUT);
  pinMode(relay_1, OUTPUT);
  digitalWrite(relay_1,LOW);
}

void loop() {
  cekLogic();
  runWar();
  indikator();
  if(Time >= 122)
  {
    stateOut = 0;
    trigg = 0;
    Time = 0;
  }
    digitalWrite(relay_1,stateOut);
   
 Serial.println(String() + "stateOut:" + stateOut);
// Serial.println(String() + "trigg   :"+ trigg);
 Serial.println(String() + "Time   :" + Time);
} 

void cekLogic()
{
  int data = digitalRead(sensor);
  Serial.println(String() + "data:" + data);
  //flag = data;
  if(data == HIGH){stateOut=1; trigg=1; flag=0; Time=0;  }
  else{flag=1;}
}

void runWar(){
  unsigned long timer = millis();
  if(trigg==1 && flag == 1 && timer - saveTimer > 1000)
  {
    saveTimer = timer;
    stateWar = !stateWar;
    Time++;
    
  }
  else
  {
    stateWar = 0;
  }
   digitalWrite(led_war,stateWar);
}

void indikator()
{
  unsigned long tmr = millis();
  if(tmr - saveFlip > 1000)
  {
    saveFlip = tmr;
    stateFlip = !stateFlip;
    digitalWrite(ledFlip,stateFlip);
  }
}
