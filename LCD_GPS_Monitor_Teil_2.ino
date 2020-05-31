// GPS Empänger mit Navigation 2020 Tobias Kuch GPL 3.0
#define SwitchPIN 7
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>
LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 20 chars and 4 line display

TinyGPSPlus gps;
SoftwareSerial ss(4, 3);


float TargetGpsLAT = 48.85826;  // Coordinates EIFFEL_TOWER in France
float TargetGpsLON = 2.294516;
bool DistanceInKM = false;
bool Button1Pressed = false;
double Distance = 0;
double courseToT = 0;

void setup()
{
  Serial.begin(9600);
  pinMode(SwitchPIN, INPUT_PULLUP); // Setzt den Digitalpin 13 als Outputpin
  lcd.init();                      // initialize theI2C lcd 
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.clear(); 
  ss.begin(9600);
  smartdelay(1);
}

void loop()
{
if (gps.altitude.isValid())
    {
    if (gps.altitude.isUpdated()||gps.satellites.isUpdated()||gps.speed.isValid() )
      {  
      lcd.setCursor(0,0);
      lcd.print("A:");
      lcd.print(gps.altitude.meters(),0); // Altitude in meters (double)
      lcd.print("m ");
      lcd.print("Gs:");
      lcd.print(gps.speed.kmph(),1);
      lcd.print("km/h");
      }
    } else
    {
    lcd.setCursor(0,0);
    lcd.print("No valid Data       ");
    }
if (gps.location.isValid())
    {
    if (gps.location.isUpdated())
      {  
      lcd.setCursor(0,1);
      lcd.print("P W:");
      lcd.print(gps.location.lat(), 4); // Raw longitude in whole degrees
      lcd.print(" L:");
      lcd.print(gps.location.lng(), 4); // Raw longitude in whole degrees
      }
    } else
    {
    lcd.setCursor(0,1);
    lcd.print("No valid location   ");
    }

if (TargetGpsLAT < 90 & TargetGpsLON < 180  )
    {
    lcd.setCursor(0,2);
    lcd.print("T W:");
    lcd.print(TargetGpsLAT, 4); // Raw longitude in whole degrees
    lcd.print(" L:");
    lcd.print(TargetGpsLON, 4); // Raw longitude in whole degrees
    } else
    {
    lcd.setCursor(0,2);
    lcd.print("No valid Target     ");
    }

if (gps.location.isValid() & TargetGpsLAT < 90 & TargetGpsLON < 180 )
    { 
      DistanceInKM = false;
      Distance = gps.distanceBetween(gps.location.lat(),gps.location.lng(),TargetGpsLAT,TargetGpsLON);
      courseToT = gps.courseTo(gps.location.lat(),gps.location.lng(),TargetGpsLAT,TargetGpsLON);
      if (Distance > 1000)
        {
        Distance = Distance / 1000;
        DistanceInKM = true;
        }
      lcd.setCursor(0,3);
      lcd.print("Dst:");
      lcd.print(Distance);
      if(DistanceInKM)  {lcd.print("km ");} else { lcd.print("m "); }
      lcd.print("Dir:"); 
      lcd.print(gps.cardinal(courseToT));
    } else
    {
    lcd.setCursor(0,3);
    lcd.print("No valid Way calc.  ");
    }
 
smartdelay(5000);
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
        TargetGpsLAT = gps.location.lat();
        TargetGpsLON = gps.location.lng();
        lcd.setCursor(0,2);
        lcd.print("T W:");
        lcd.print(TargetGpsLAT, 4); // Raw longitude in whole degrees
        lcd.print(" L:");
        lcd.print(TargetGpsLON, 4); // Raw longitude in whole degrees
        } 
      }  else
      {
      Button1Pressed = false;  
      }
      
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}
