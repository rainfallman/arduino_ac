# define right4 12
# define right3 13
# define left1 15
# define left2 14

const int leftTrig_Pins = 5;
const int leftEcho_Pins = 4;
const int frontTrig_Pins = 3;
const int frontEcho_Pins = 2;
const int rightTrig_Pins = 10;
const int rightEcho_Pins = 11;

float get_distance(int trig, int echo){//計算距離
    float duration;
    pinMode(trig, OUTPUT);
    pinMode(echo, INPUT);
    digitalWrite(trig, HIGH);
    delayMicroseconds(10);//給予trig 10us TTL pulse,讓模組發射聲波
    digitalWrite(trig, LOW);
    duration = pulseIn(echo, HIGH, 5000000);//紀錄echo電位從high到low的時間，就是超音波來回的時間，若5秒內沒收到超音波則回傳0
    return duration / 29 / 2;// 聲速340m/s ，換算後約每29微秒走一公分，超音波來回所以再除2
}
void turnleft(){
    digitalWrite(right4,HIGH);
    digitalWrite(right3,LOW);
    digitalWrite(left2,HIGH);
    digitalWrite(left1,LOW);
    delay(400);
}
void  turnright(){
    digitalWrite(right3,HIGH);
    digitalWrite(right4,LOW);
    digitalWrite(left1,HIGH);
    digitalWrite(left2,LOW);
    delay(400);
}

void back(){
  
    digitalWrite(right3,HIGH);
    digitalWrite(right4,LOW);
    digitalWrite(left2,HIGH);
    digitalWrite(left1,LOW);
    delay(500);
    digitalWrite(right3,HIGH);
    digitalWrite(right4,LOW);
    digitalWrite(left1,HIGH);
    digitalWrite(left2,LOW);
    delay(400);
    gofront();
    
}
void gofront()
{
  digitalWrite(right4,HIGH);
    digitalWrite(right3,LOW);
    digitalWrite(left1,HIGH);
    digitalWrite(left2,LOW);
}
  void setup() {

  
 Serial.begin(9600);
 pinMode(right4,OUTPUT);
 pinMode(right3,OUTPUT);
 pinMode(left1,OUTPUT);
 pinMode(left2,OUTPUT);
}

void loop() {
    
    float lresult= get_distance( leftTrig_Pins, leftEcho_Pins);
    float fresult= get_distance( frontTrig_Pins, frontEcho_Pins);
    float rresult= get_distance( rightTrig_Pins, rightEcho_Pins);
      if(lresult==0||rresult==0||lresult>3000||rresult>3000)
        return;
        Serial.print(lresult);
    Serial.print("    ");
    Serial.print(fresult);
    Serial.print("    ");
    Serial.println(rresult);
    if(lresult>38)
    {
      turnleft();
      gofront();
      delay(700);
      return;
    }
    if(rresult>38)
    {
      turnright();
      gofront();
      delay(700);
      return;
    }
    if(fresult>2000)
    {
      if(lresult<30&&rresult<30){
      back();delay(500);return;}
      else{
      if(lresult>rresult){
      turnleft();delay(700);return;}
      if(rresult>lresult){
      turnright();delay(700);return;}
      }}
    gofront();
    delay(200);

}

