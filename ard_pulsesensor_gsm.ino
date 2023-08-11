
#include <SoftwareSerial.h>
SoftwareSerial mySerial(10, 11);  // RX, TX


int PulseSensorPurplePin = A0;

int Signal, Signal_old;  

char flag2 = 0;

int ms10 = 0; 


const int RunningAverageCount1 = 128;
float RunningAverageBuffer1[RunningAverageCount1];
int NextRunningAverage1;

float RunningAverageVolt1 = 0;

int i1;


char at_flag, echo_flag, net_flag;

void gsm_init(void);
void send_sms(char ch, int sig);


void setup() 
{
  Serial.begin(9600);

  mySerial.begin(9600);

  delay(100); 

  gsm_init();
  
  send_sms(3, Signal);
}


void loop() 
{
  RunningAverageBuffer1[NextRunningAverage1++] = analogRead(PulseSensorPurplePin);
  if(NextRunningAverage1 >= RunningAverageCount1)NextRunningAverage1 = 0;
  RunningAverageVolt1 = 0;
  for(i1=0; i1< RunningAverageCount1; ++i1)RunningAverageVolt1 += RunningAverageBuffer1[i1];
  RunningAverageVolt1 /= RunningAverageCount1;


  Signal_old = (int)RunningAverageVolt1;

       if(Signal_old <  505)Signal = map(Signal_old, 0, 505, 0, 59);
  else if(Signal_old >= 505 && Signal_old <= 515)Signal = 0;
  else Signal = map(Signal_old, 515, 700, 60, 80); 

  
  if( !(Signal_old >= 505 && Signal_old <= 515) )
  {
   if(Signal < 60)
   {
    if(flag2 == 0)
    {
     send_sms(1, Signal);
      
     flag2 = 1; 
    }
   }
   else if(Signal > 80)
   {
    if(flag2 == 0)
    {
     send_sms(2, Signal);
      
     flag2 = 1; 
    }
   }
   else flag2 = 0;
  }

   ++ms10;

   if(ms10 > 300)
   {
    Serial.write('%');
    Serial.write(Signal/100%10+48);
    Serial.write(Signal/10%10+48);
    Serial.write(Signal/1%10+48);
    Serial.write('#');
    Serial.println();
      
    ms10 = 0;
   }
    
   delay(10); 
}

void gsm_init(void)
{
  at_flag = 1; 
  
  while(at_flag)
  {
   mySerial.println("AT");
   while(mySerial.available()>0)
   {
    if(mySerial.find("OK"))at_flag=0;
   }
   delay(1000); 
  }
    
  delay(500);

  echo_flag = 1;  
  
  while(echo_flag)
  {
   mySerial.println("ATE0");
   while(mySerial.available()>0)
   {
    if(mySerial.find("OK"))echo_flag=0;
   }
   delay(1000);
  }  
  
  delay(500);
  
  net_flag = 1; 
  
  while(net_flag)
  {
   mySerial.println("AT+CPIN?");
   while(mySerial.available()>0)
   {
    if(mySerial.find("+CPIN: READY"))net_flag = 0; 
   }
   delay(1000);
  }

  delay(500);
}

void send_sms(char ch, int sig)
{
 Serial.write('%');
 Serial.write(Signal/100%10+48);
 Serial.write(Signal/10%10+48);
 Serial.write(Signal/1%10+48);
 Serial.write('#');
 Serial.println();

 ms10 = 0;
 
     
 mySerial.println("AT"); delay(500);
 
 mySerial.println("AT+CMGF=1"); delay(500);
 
 mySerial.print("AT+CMGS="); mySerial.write('"'); mySerial.print("9790700580"); mySerial.write('"'); mySerial.println(); delay(500);

      if(ch == 1)mySerial.println("Low heart rate.."); 
 else if(ch == 2)mySerial.println("High heart rate.."); 

 else if(ch == 3)mySerial.println("Welcome.."); 

 if(ch != 3)
 {
  mySerial.print(sig); mySerial.println(" BPM"); 
 }
 
 delay(500); 

 mySerial.write(26); 

 delay(2000); 
}
