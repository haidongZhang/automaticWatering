#include <Event.h>
#include <Timer.h>

Timer timer;                //正常浇水定时器
Timer safeTimer;            //安全定时器
int pin = 4;                //水泵继电器引脚
int sensorValue = 0;         //土壤湿度传感器采样值
const int sampleCount = 10; //采样次数
const double maxVal = 800;     //计算上限
const int threshold = 600;  //浇水的传感器阈值
bool  isWatered = false;    //一段时间内最多浇一次水,防止因为传感器损坏而过浇
long  waterInerval = 43200000; //正常浇水时间间隔 12*60*60*1000
long  safeWaterInterval = 86400000;  //安全浇水时间间隔,默认一天最多浇一次水 24*60*60*1000
long  waitTime = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(pin, OUTPUT);
  Serial.begin(9600);
  timer.every(waitTime, water, 1);
  safeTimer.every(safeWaterInterval, changeFlag);
}

void loop() {
  // put your main code here, to run repeatedly:
  timer.update();
  safeTimer.update();
}

void water(){

  int sum = 0;
  int val = 0;
  for(int i=0; i < sampleCount; i++){
    val = analogRead(A0);
    Serial.print("Sample Moisture Sensor Value:");
    Serial.print(val);
    Serial.print("\n");
    sum += val;
  }
  
  sensorValue = sum / sampleCount;

  //如果大于浇水阈值,浇水
  if(sensorValue > threshold){
    if(!isWatered){
      digitalWrite(pin, HIGH);
      delay(10000);
      digitalWrite(pin, LOW);
      isWatered = true;
    }
    sensorValue = threshold;
  }

  //根据土壤湿度设定下次检测浇水的时间间隔
  waitTime = (long)2*(maxVal-sensorValue)/maxVal*waterInerval;
  
  Serial.print("Waiting water time:");
  Serial.print(waitTime);
  Serial.print("\n");
  timer.every(waitTime, water, 1);
}

void changeFlag(){
  isWatered = false;  
}
