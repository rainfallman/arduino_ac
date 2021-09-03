/* Receive the controlling message, turning on/off and pwm, and
 * than set the corresponding pin.
 */
#include <WiFiClient.h>
#include <LWiFi.h>
#include <task.h>

#define SSID "1scream2.4G"
#define PASSWD "2017scream"
#define TCP_IP "192.168.0.50"
#define TCP_PORT 5000
//#define TCP_IP_PHONE "192.168.0.38"
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
static char buf[64],bufPh[64],buf_send[64],buf_phsend[64],realname[64],sendplace[64]="(";
static char client_ID[] = "yeeee",Team[] = "B";
static int messageLen,phmessageLen;
static int MyPosX,MyPosY,DstPosX,DstPosY,miposx,miposy;
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
int tx[4],ty[4],c,tmpx,tmpy,tmpset=0,tmptr=0;
void askPos( void * parameter )
{
  while (1)
  {
    if ((messageLen = wifiClient.available()) > 0) {
      int i = 0;
      do
      {
        buf[i++] = wifiClient.read();
      } while (i < 64 && buf[i - 1] != '\r');

      buf[i - 1] = '\0';
      recv_ID = strtok(buf, "|\0");
      Serial.print(recv_ID);
      Serial.print(":");
      recv_buf = strtok(NULL, "|\0");
      Serial.println(recv_buf);
      if (strcmp(recv_buf, "Start")==0) {
        gorun = true;
        delay(200);
        send_mes("Treasure","");
      }
      if(sscanf(recv_buf, "Treasure:(%d,%d)(%d,%d)(%d,%d)(%d,%d)", &tx[0], &ty[0], &tx[1], &ty[1], &tx[2], &ty[2], &tx[3], &ty[3])){
        for(c=0;c<4;c++){
                if(tx[c]>=0&&tx[c]<=200&&ty[c]>=0&&ty[c]<=200)
                {
                  DstPosX=tx[c];
                  DstPosY=ty[c];
                }
        }
        /*DstPosX=tx[0];
        DstPosY=ty[0];*/
        gorun=true;
        send_mes("Position", "");
      }
      if(strcmp(recv_buf, "Done")==0){
        gorun = false;
        tmpset=0;
        tmptr=0;
      }
      if(sscanf(recv_buf, "POS:(%d,%d)", &MyPosX, &MyPosY)){
        delay(300);
        send_mes("Position", "");
      }
      if(sscanf(recv_buf, "False:%s",realname)){
        sprintf(sendplace,"(%d,%d)",MyPosX,MyPosY);
        Serial.print(sendplace);
        send_mes(realname,sendplace);
        gorun=false;
        tmptr=1;
      }
      if(sscanf(recv_buf,"(%d,%d)",&tmpx,&tmpy))
      {
        tmpset=1;
        }
      if(tmpset==1&&tmptr==1)
      {
        DstPosY=tmpy;
        DstPosX=tmpx;
        gorun=true;
      }
      

      
    }
    //delay(100);
  }
  
//  Serial.println("Ending task 1");
  vTaskDelete( NULL );
}


void reg_ID()
{
    strcpy(buf,"Register|");
    strcat(buf,client_ID);
    wifiClient.write(buf, strlen(buf));
    wifiClient.flush();
}


bool firstsecond=true;
float finishv=0;
float miv=0;
float turndir=0;
float distance=0;
int dischange=0;
//float totaldistance=sqrt(pow(MyPosY-DstPosY,2)+pow(MyPosX-DstPosY,2));
float totaldistance=hypot(abs(MyPosY-DstPosY),abs(MyPosX-DstPosY));
float result[3];
void loop()
{   
    result[0]= get_distance( leftTrig_Pins, leftEcho_Pins);
    result[1]= get_distance( frontTrig_Pins, frontEcho_Pins);
    result[2]= get_distance( rightTrig_Pins, rightEcho_Pins);
    Serial.print(result[0]);
    Serial.print(result[1]);
    Serial.println(result[2]);
    if(gorun) 
    {
      if(firstsecond)
        {
          miposx=MyPosX;
          miposy=MyPosY;
          gofront();
          delay(500);
          firstsecond=false;
          return;
          }
     
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
