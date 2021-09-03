/* Receive the controlling message, turning on/off and pwm, and
 * than set the corresponding pin.
 */
#include <WiFiClient.h>
#include <LWiFi.h>
#include <task.h>

#define SSID "2scream"
#define PASSWD "2017scream"
#define TCP_IP "192.168.0.101"
#define TCP_PORT 5000
//#define TCP_IP_PHONE "192.168.0.38"
# define right4 13
# define right3 14
# define left1 16
# define left2 15
WiFiClient wifiClient;

WiFiClient wifiClientPh;
static char buf[48],bufPh[48],buf_send[32],buf_phsend[32];
static char client_ID[] = " yeeeeeeeeeeeeeeee",Team[] = "B";
static int messageLen,phmessageLen;
static int MyPosX,MyPosY,DstPosX,DstPosY,miposx,miposy;
static char *recv_ID,*recv_buf;
bool gorun=false;


IPAddress ip;
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
void askPos( void * parameter )
{
  while (1)
  {
    if ((messageLen = wifiClient.available()) > 0) {
      int i = 0;
      do
      {
        buf[i++] = wifiClient.read();
      } while (i < 32 && buf[i - 1] != '\r');

      buf[i - 1] = '\0';
      recv_ID = strtok(buf, "|\0");
      Serial.print(recv_ID);
      Serial.print(":");
      recv_buf = strtok(NULL, "|\0");
      Serial.println(recv_buf);
      if (strcmp(recv_buf, "Start")==0) {
        gorun = true;
      }
      if(strcmp(recv_buf, "Done")==0){
        gorun = false;
      }
      sscanf(recv_buf, "POS:(%d,%d)(%d,%d)", &MyPosX, &MyPosY, &DstPosX, &DstPosY);
//    send_phone(MyPosX, MyPosY);
      send_mes("Position", "");
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

void loop()
{   
    if(gorun) 
    {
      if(firstsecond)
        {
          miposx=MyPosX;
          miposy=MyPosY;
          gofront();
          delay(3500);
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
          delay(1000);
          }
        if(turndir>-180&&turndir<-15)
        {
          turnleft(dischange);
          gofront();
          delay(1000);
          }
        if(turndir<15&&turndir>-15)
        {
          gofront();
          delay(1000);
          }
      }

    else  runstop();
    //delay(100);
} 
