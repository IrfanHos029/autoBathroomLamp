#define sensor D5 //2
#define led_war D1//3
#define relay_1 D4
bool stateOut;
int trigg;
int stateWar = 0;
int Time;
int flag;
unsigned long saveTimer = 0;

void setup() {
  Serial.begin(115200);
  pinMode(sensor, INPUT_PULLUP);
  pinMode(led_war, OUTPUT);
  pinMode(relay_1, OUTPUT);
  digitalWrite(relay_1,LOW);
}

void loop() {
  cekLogic();
  runWar();
  if(Time >= 122)
  {
    stateOut = 0;
    trigg = 0;
    Time = 0;
  }
    digitalWrite(relay_1,stateOut);
    digitalWrite(led_war,stateWar);
// Serial.println(String() + "statewar:" + stateWar);
// Serial.println(String() + "trigg   :"+ trigg);
 Serial.println(String() + "Time   :" + Time);
} 

void cekLogic()
{
  int data = digitalRead(sensor);
  Serial.println(String() + "data:" + data);
  //flag = data;
  if(data == LOW){stateOut=1; trigg=1; flag=0; Time=0;  }
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
  
}
