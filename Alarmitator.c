#define BLYNK_TEMPLATE_ID "TMPLgKu6L57u"
#define BLYNK_TEMPLATE_NAME "NODEMCU"
#define BLYNK_AUTH_TOKEN "gkeGlxnz1DsThKWGLk_YRxNh7bBbN-z8"//auth token of blynk


#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>  //calling the wifi library
#include <BlynkSimpleEsp8266.h> //calling the blynk library

#include <NTPClient.h> //calling the ntp client to get the OFFset
#include <WiFiUdp.h> 

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Lelo";  
char pass[] = "nidungahaan";  
BlynkTimer timer;


WiFiUDP ntpUDP;
const long utcOffsetInSeconds = 19800;
NTPClient timeClient(ntpUDP, "asia.pool.ntp.org", utcOffsetInSeconds);//OFFset of indian standard time compared to UTC

int button = D4;
int count = 0;//no of times pressed button
int t = 0;
int t1 = 0;
int key = 0;
int buzzer= D1;
int countSnooze = 0;
int startTimeInSecs;
int inputValue;

int randomNo;

unsigned long startMillis;
unsigned long currentMillis;
const unsigned long period = 10000; // alarm: 30 seconds
int currentButtonState;
int previousButtonState;
boolean counting;
byte bb;

void randomdata(){
    randomNo=random(7,12);//generating the number of times the button should be pressed || alarm: 10 - 27
    Blynk.virtualWrite(V1, randomNo);
}

BLYNK_WRITE(V0) {
  startTimeInSecs = param.asInt();//getting the time from blynk software
  if(startTimeInSecs == 0)
  {
    startTimeInSecs = 999999;
  }
  Serial.println(startTimeInSecs);
}

void setup(){
  Serial.begin(9600);//setting the baud rate
  Blynk.begin(auth, ssid, pass); //Connecting the blynk software
  delay(100);
  pinMode(button, INPUT_PULLUP);//Setting the button as input 
  pinMode(buzzer, OUTPUT);//Setting the buzzer/LED as output
  digitalWrite(buzzer, HIGH);
  randomdata();
  currentButtonState=HIGH;

  counting = true;

  timeClient.begin();

}


void snooze(){//snooze function
  countSnooze++;
  digitalWrite(buzzer, HIGH);
  if(countSnooze == 1){//first snooze for 10 seconds || alarm snooze: 5 min
    delay(10000);
    startTimeInSecs = startTimeInSecs+20;
  }
  else if(countSnooze == 2){//second snooze for 8 seconds || alarm snooze: 1 min
    delay(8000);
    startTimeInSecs = startTimeInSecs+18;
  }
  else if(countSnooze == 3){//third snooze for 5 seconds || alarm snooze: 30 sec
    delay(5000);
    startTimeInSecs = startTimeInSecs+15;
  }
  else if (countSnooze>3){
    delay(3000);//constant 3 seconds snooze after fourth snooze || alarm snooze: 10 sec
    startTimeInSecs = startTimeInSecs+14;
  }
}




void loop(){ 
  timeClient.update();
  int HH = timeClient.getHours();
  int MM = timeClient.getMinutes();
  int SS = timeClient.getSeconds();
  int server_time = 3600*HH + 60*MM + SS;//getting current time and converting to seconds
  randomdata();
  if(startTimeInSecs == server_time)
{
  key=0;
  digitalWrite(buzzer, LOW);//Alarm goes off
  startMillis=millis();
  while(key!=1){  
  currentMillis = millis();
  previousButtonState = currentButtonState;//checking the status of the button
  if (currentMillis - startMillis <= period)  //true until the period elapses.  Note that this is the reverse of BWOD
  {
    bb=digitalRead(D4);
    currentButtonState = bb;
    //Serial.println(currentButtonState);
    if (currentButtonState == LOW and previousButtonState == HIGH)  //the button has become pressed
    {
      count++;//count of the button increases
      Serial.println(count);      
    }
    delay(20);

  }
  else  //period has ended
  {
    if(count==randomNo){//checking if the number of count is equal to the random value displayed
    digitalWrite(buzzer, HIGH);//Turning the alarm off
    key=1;
    exit(0);//Exiting the program 
  }else{
    digitalWrite(buzzer, HIGH);//Snoozing the buzzer
    snooze();//calling the snooze function
    count=0;
    key=1;
  }
  }

  yield();//to manage the watchdogtimer
}

}
  Blynk.run();//For blynk to be active
}
