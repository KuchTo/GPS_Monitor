// GPS Alarmanlage 2020 Tobias Kuch GPL 3.0
#define SwitchPIN 9
#define RGBLED_Red 8
#define RGBLED_Green 7
#define RGBLED_Blue 6
#define Relais_Alarm 2
#include <SoftwareSerial.h>
#include <TinyGPS++.h>

TinyGPSPlus gps;
SoftwareSerial ss(4, 3);


float ActualGpsLAT = 0; 
float ActualGpsLON = 0;
bool DistanceInKM = false;
bool Button1Pressed = false;
double Distance = 0;
bool systemarmed = false;
bool initsystemarmed = false;
double GeofenceinMeters = 30;

bool Engage_Alarm = false;
byte SmartdelayLoop = 0;

void setup()
{
  systemarmed = false;
  Serial.begin(9600);
  pinMode(SwitchPIN, INPUT_PULLUP); // Setzt den Digitalpin 13 als Outputpin
  pinMode(RGBLED_Red,OUTPUT);
  pinMode(RGBLED_Green,OUTPUT);
  pinMode(RGBLED_Blue,OUTPUT);
  pinMode(Relais_Alarm,OUTPUT);
  digitalWrite(Relais_Alarm, HIGH);
  digitalWrite(RGBLED_Red, LOW);
  digitalWrite(RGBLED_Green, LOW);
  digitalWrite(RGBLED_Blue, HIGH);
  delay(100);
  digitalWrite(RGBLED_Red, HIGH);
  digitalWrite(RGBLED_Green, LOW);
  digitalWrite(RGBLED_Blue, LOW);
  delay(100);
  digitalWrite(RGBLED_Red, LOW);
  digitalWrite(RGBLED_Green, HIGH);
  digitalWrite(RGBLED_Blue, LOW);

  ss.begin(9600);
  smartdelay(1000);
}

void loop()
{

if (gps.location.isValid()& ActualGpsLAT < 90 & ActualGpsLON < 180 )
    {   
    if (gps.location.isUpdated())
      {  
      if ((systemarmed) && (initsystemarmed) )
        {
        initsystemarmed = false;
        digitalWrite(RGBLED_Red, HIGH);
        digitalWrite(RGBLED_Green, LOW);
        digitalWrite(RGBLED_Blue, LOW);
        Serial.println (gps.location.lat());
        Serial.println (gps.location.lng());
        Serial.println("ValidGPS Coordinates transferred to Memory");
        ActualGpsLAT = gps.location.lat();  // Location is stored in Memory
        ActualGpsLON = gps.location.lng();  // Location is stored in Memory
        Serial.println (ActualGpsLAT);
        Serial.println (ActualGpsLON);
        }        
      double Getlat = gps.location.lat();
      double Getlng = gps.location.lng();
      
      Distance = gps.distanceBetween(Getlat,Getlng,ActualGpsLAT,ActualGpsLON);
      Serial.print("Distance Calculated: "); 
      Serial.print(Distance); 
      Serial.println(" m"); 
      if ((systemarmed) && (!(initsystemarmed)) )
        {
        if ((Distance > GeofenceinMeters) && ((ActualGpsLAT != gps.location.lat()) || (ActualGpsLON != gps.location.lng()) ) ) 

          {
            // engage time Limited Alarm !
             Engage_Alarm = true;
             SmartdelayLoop = 0;
             Serial.println("Alarm Engaged, transferring new GPS Coordinates to Memory.");
             ActualGpsLAT = gps.location.lat();  // Location is stored in Memory
             ActualGpsLON = gps.location.lng();  // Location is stored in Memory 
             Serial.println (ActualGpsLAT);
             Serial.println (ActualGpsLON);
          }            
        }
      }    
    } 

Alarm_Control();
smartdelay(5000);
}


void Alarm_Control()
{
if (Engage_Alarm)
  {
  if (SmartdelayLoop < 7)
    {
    digitalWrite(Relais_Alarm, LOW);  

    } else
    {
    Engage_Alarm = false;
    digitalWrite(Relais_Alarm, HIGH);  
    } 
  } else
  {
  Engage_Alarm = false;
  digitalWrite(Relais_Alarm, HIGH);  
  }
}
static void smartdelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    if (!(digitalRead(SwitchPIN)))   // read the input pin
      {
      delay(100);   //debouncing  
      if (!Button1Pressed)
        {
        Button1Pressed = true;
        systemarmed = !systemarmed;
        if (systemarmed)
           {
           digitalWrite(RGBLED_Red, HIGH);
           digitalWrite(RGBLED_Green, LOW);
           digitalWrite(RGBLED_Blue, HIGH);
           initsystemarmed = true;
           Serial.println("Alarm System Activated with Init Coordinates:"); 
           Serial.println (gps.location.lat());
           Serial.println (gps.location.lng());
           
           } else
           {
           Serial.println("Alarm System DeActivated");  
           digitalWrite(RGBLED_Red, LOW);
           digitalWrite(RGBLED_Green, HIGH);
           digitalWrite(RGBLED_Blue, LOW);
           initsystemarmed = false;
           Engage_Alarm = false;
           SmartdelayLoop = 0;
           digitalWrite(Relais_Alarm, HIGH); 
           }         
        } 
      }  else
      {
      Button1Pressed = false;  
      }
      
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
  SmartdelayLoop = SmartdelayLoop + 1;
}
