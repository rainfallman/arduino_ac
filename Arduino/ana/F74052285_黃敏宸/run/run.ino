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
    duration = pulseIn(echo, HIGH, 5000000);//紀錄echo電位從HIGH到LOW的時間，就是超音波來回的時間，若5秒內沒收到超音波則回傳0
    return duration / 29 / 2;// 聲速340m/s ，換算後約每29微秒走一公分，超音波來回所以再除2
}
void turnleft(){
    analogWrite(right4,255);
    analogWrite(right3,0);
    analogWrite(left2,255);
    analogWrite(left1,0);
    delay(400);
}
void  turnright(){
    analogWrite(right3,255);
    analogWrite(right4,0);
    analogWrite(left1,255);
    analogWrite(left2,0);
    delay(400);
}

void back(){
  
    analogWrite(right3,255);
    analogWrite(right4,0);
    analogWrite(left2,255);
    analogWrite(left1,0);
    delay(200);
    analogWrite(right3,200);
    analogWrite(right4,0);
    analogWrite(left1,200);
    analogWrite(left2,0);
    delay(300);
    analogWrite(right3,200);
    analogWrite(right4,0);
    analogWrite(left2,200);
    analogWrite(left1,0);
    delay(200);
    /*gofront();*/
    
}
void gofront()
{
  analogWrite(right4,255);
    analogWrite(right3,0);
    analogWrite(left1,255);
    analogWrite(left2,0);
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
        if(result[0]<8||result[2]<8)
      {
        if(result[0]<8)
        {
          analogWrite(left1,255);
          analogWrite(left2,0);
          analogWrite(right4,220);
          analogWrite(right3,0);
          delay(150);
        }
        if(result[2]<8)
        {
          analogWrite(right4,255);
          analogWrite(right3,0);
          analogWrite(left2,0);
          analogWrite(left1,220);
          delay(150);
        }
      }
    Serial.print(result[0]);
    Serial.print("    ");
    Serial.print(result[1]);
    Serial.print("    ");
    Serial.println(result[2]);
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
      /*if(result[0]<7||result[2]<7)
      {
        if(result[0]<7)
        {
          analogWrite(left1,255);
          analogWrite(left2,0);
          analogWrite(right4,150);
          analogWrite(right3,0);
        }
        if(result[2]<7)
        {
          analogWrite(right4,255);
          analogWrite(right3,0);
          analogWrite(left2,0);
          analogWrite(left1,150);
        }
      }*/
    delay(100);

}

