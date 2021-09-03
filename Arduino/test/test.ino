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
    duration = pulseIn(echo, HIGH, 100000);//紀錄echo電位從HIGH到low的時間，就是超音波來回的時間，若5秒內沒收到超音波則回傳0
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
    delay(200);
    digitalWrite(right3,HIGH);
    digitalWrite(right4,LOW);
    digitalWrite(left1,HIGH);
    digitalWrite(left2,LOW);
    delay(300);
    digitalWrite(right3,HIGH);
    digitalWrite(right4,LOW);
    digitalWrite(left2,HIGH);
    digitalWrite(left1,LOW);
    delay(200);
    /*gofront();*/
    
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
int turndis=30,i;
float result[3];

void loop() {
    result[0]= get_distance( leftTrig_Pins, leftEcho_Pins);
    result[1]= get_distance( frontTrig_Pins, frontEcho_Pins);
    result[2]= get_distance( rightTrig_Pins, rightEcho_Pins);
      if(result[0]==0||result[2]==0||result[0]>3000||result[2]>3000)
        return;
    /*Serial.print(result[0]);
    Serial.print("    ");
    Serial.print(result[1]);
    Serial.print("    ");
    Serial.println(result[2]);*/
    for(i=0;i<3;i++)
    {
      if(result[i]>turndis)
        break;
      }
      delay(100);
    switch (i){
      case 0:
      turnleft();
      gofront();
      delay(600);
      break;
      case 2:
      turnright();
      gofront();
      delay(600);
      break;
      case 1:
      gofront();
      break;
    }
      if(result[1]>2000||result[1]<10)
    {
      if(result[0]<30&&result[2]<30){
      back();delay(500);return;}
      /*else{
      if(result[0]>result[2]){
      turnleft();delay(700);return;}
      if(result[2]>result[0]){
      turnright();delay(700);return;}
      }*/}
      if(result[0]<7||result[2]<7)
      {
        if(result[0]<7)
        {
          digitalWrite(left1,HIGH);
          digitalWrite(left2,LOW);
        }
        if(result[2]<7)
        {
          digitalWrite(right4,HIGH);
          digitalWrite(right3,LOW);
        }
      }
    delay(100);

}

