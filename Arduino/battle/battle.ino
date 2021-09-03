/* Receive the controlling message, turning on/off and pwm, and
 * than set the corresponding pin.
 */
#include <WiFiClient.h>
#include <LWiFi.h>
#include <task.h>

#define SSID "1scream2.4G"
#define PASSWD "2017scream"
#define TCP_IP "192.168.0.71"
#define TCP_PORT 5000
//#define TCP_IP_PHONE "192.168.208.232"
# define right4 13
# define right3 14
# define left1 16
# define left2 15
const int leftTrig_Pins = 5;
const int leftEcho_Pins = 4;
const int frontTrig_Pins = 3;
const int frontEcho_Pins = 2;
const int rightTrig_Pins = 10;
const int rightEcho_Pins = 11;
WiFiClient wifiClient;

WiFiClient wifiClientPh;
static char buf[128],bufPh[128],buf_send[128],buf_phsend[128],realname[128],sendplace[128]="(";
static char client_ID[] = "yeeee",Team[] = "B";
static int messageLen,phmessageLen;
int MyPosX=0,MyPosY=0,DstPosX,DstPosY,miposx,miposy;
static char *recv_ID,*recv_buf;
bool gorun=false;


IPAddress ip;

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

void setup()
{
    int status = WL_IDLE_STATUS;


    // set WiFi
   // WiFi.mode(WIFI_STA);
    WiFi.begin(SSID, PASSWD);
    while (status != WL_CONNECTED) {
        // Connect failed, blink 0.5 second to indicate
        // the board is retrying.
        delay(500);
        WiFi.begin(SSID, PASSWD);
        status =  WiFi.begin(SSID, PASSWD);   
        Serial.print("Attempting to connect to SSID: ");
        Serial.println(SSID);
        Serial.println(status);
    }
    
    // Conenct to AP successfully
   // wifiClient.connect(TCP_IP, TCP_PORT);
      while (!wifiClient.connect(TCP_IP, TCP_PORT)){
      delay(300);
      Serial.print("Attempting to connect to SERVER: ");
        Serial.println(TCP_IP);
    }
    /*
while (!wifiClientPh.connect(TCP_IP_PHONE, TCP_PORT)){
      delay(300);
      Serial.print("Attempting to connect to PHONE SERVER: ");
        Serial.println(TCP_IP_PHONE);
    }*/
    reg_ID();

    delay(1000);
    xTaskCreate(
                    askPos,          /* Task function. */
                    "askPos",        /* String with name of task. */
                    10000,            /* Stack size in words. */
                    NULL,             /* Parameter passed as input of the task */
                    1,                /* Priority of the task. */
                    NULL);            /* Task handle. */
}  
void send_mes(char ID[],char mes[])
{
    sprintf(buf,"%s|%s",ID,mes);
    wifiClient.write(buf, strlen(buf));
    wifiClient.flush();
}

void turnleft(int in){
    analogWrite(right4,255);
    analogWrite(right3,in);
    analogWrite(left2,255);
    analogWrite(left1,in);
    delay(200);
}
void  turnright(int in){
    analogWrite(right3,255);
    analogWrite(right4,in);
    analogWrite(left1,255);
    analogWrite(left2,in);
    delay(200);
}
/*
void turnleft(){
    analogWrite(right4,255);
    analogWrite(right3,150);
    analogWrite(left2,255);
    analogWrite(left1,150);
    delay(200);
}
void  turnright(){
    analogWrite(right3,255);
    analogWrite(right4,150);
    analogWrite(left1,255);
    analogWrite(left2,150);
    delay(200);
}*/

void gofront()
{
  analogWrite(right4,255);
    analogWrite(right3,0);
    analogWrite(left1,255);
    analogWrite(left2,0);
}

void runstop()
{
  analogWrite(right4,0);
    analogWrite(right3,0);
    analogWrite(left1,0);
    analogWrite(left2,0);
}
 
/*void send_phone(int x,int y)
{
    sprintf(buf_phsend,"(%d,%d)",x,y);
    wifiClientPh.write(buf_phsend, strlen(buf_phsend));
    wifiClientPh.flush();
}*/

bool firstsecond=true;
char base[2];
int towerx[3],towery[3],c,attack=0,tmpset=0,tmptr=0,blood;
void askPos( void * parameter )
{
  while (1)
  {
    if ((messageLen = wifiClient.available()) > 0) {
      int i = 0;
      do
      {
        buf[i++] = wifiClient.read();
      } while (i < 128 && buf[i - 1] != '\r');

      buf[i - 1] = '\0';
      recv_ID = strtok(buf, "|\0");
      Serial.print(recv_ID);
      Serial.print(":");
      recv_buf = strtok(NULL, "|\0");
      Serial.println(recv_buf);
      //與server溝通部份
      if (strcmp(recv_buf, "Start")==0) {
        gorun = true;
        delay(200);
        send_mes("Position","");
      }
      
      if(sscanf(recv_buf, "POS:(%d,%d)BaseA:%cBaseB:%cTowers:(%d,%d)(%d,%d)(%d,%d)Blood:%d", &MyPosX, &MyPosY, &base[0],&base[1],&towerx[0],&towery[0],&towerx[1],&towery[1],&towerx[2],&towery[2],&blood))
      {
    
          if(towerx[0]!=-1)
          {
            gorun=true;
            DstPosX=towerx[0];
            DstPosY=towery[0];
            }
            else
            {
              gorun=false;
              }
        delay(200);
        send_mes("Position", "");
      }
      /*if(Team[0]=='A'&&base[1]=='O')
      {
        attack=1;
        DstPosX=96;
        DstPosY=96;
        if(MyPosX<110&&MyPosX>85&&MyPosY<110&&MyPosY>85)
          {
            gorun=false;
            delay(3200);
            gorun=true;
            attack=0;
          }
        }    
      if(Team[0]=='B'&&base[0]=='O')
      {
        attack=1;
        DstPosX=480;
        DstPosY=480;
        if(MyPosX<500&&MyPosX>465&&MyPosY<500&&MyPosY>465)
          {
            gorun=false;
            delay(3200);
            gorun=true;
            attack=0;
          }
        }*/      
    }
    //delay(100);
  }
  
//  Serial.println("Ending task 1");

  vTaskDelete( NULL );
}


void reg_ID()
{
    strcpy(buf,"Register");
    strcat(buf,Team);
    strcat(buf,"|");
    strcat(buf,client_ID);
    wifiClient.write(buf, strlen(buf));
    wifiClient.flush();
}

float finishv=0;
float miv=0;
float turndir=0;
float distance=0;
int dischange=0;
float totaldistance=hypot(abs(MyPosY-DstPosY),abs(MyPosX-DstPosY));
float result[3];
void loop()
{   
    /*result[0]= get_distance( leftTrig_Pins, leftEcho_Pins);
    result[1]= get_distance( frontTrig_Pins, frontEcho_Pins);
    result[2]= get_distance( rightTrig_Pins, rightEcho_Pins);
    Serial.print(result[0]);
    Serial.print(result[1]);
    Serial.println(result[2]);*/
    if(gorun) 
    {
      if(firstsecond) //要先走一點才能獲得行走目標方向
        {
          miposx=MyPosX;
          miposy=MyPosY;
          gofront();
          delay(2000);
          firstsecond=false;
          return;
          }
     //finishv是目的方向 miv是車子行走方向
      finishv=atan2(DstPosY-MyPosY,DstPosX-MyPosX)*180/PI;
      miv=atan2(MyPosY-miposy,MyPosX-miposx)*180/PI;
      miposx=MyPosX;
      miposy=MyPosY; 
      distance=hypot(abs(MyPosY-DstPosY),abs(MyPosX-DstPosY));
      dischange=map(distance,0,totaldistance,0,150);
      turndir=finishv-miv;
        if(turndir>180) turndir=turndir-360;
        if(turndir<-180) turndir=turndir+360;
        if(turndir<180&&turndir>15)
        {
          turnright(dischange);
          gofront();
          delay(300);
          }
        if(turndir>-180&&turndir<-15)
        {
          turnleft(dischange);
          gofront();
          delay(300);
          }
        if(turndir<15&&turndir>-15)
        {
          gofront();
          delay(300);
          }
      }

    else  runstop();
    //delay(100);
} 
