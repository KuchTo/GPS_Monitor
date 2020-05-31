#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>
LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 20 chars and 4 line display

TinyGPSPlus gps;
SoftwareSerial ss(4, 3);



void setup()
{
  Serial.begin(9600);  
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
    if (gps.altitude.isUpdated()||gps.satellites.isUpdated()||gps.course.isUpdated() )
      {  
      lcd.setCursor(0,0);
      lcd.print("Alt:");
      lcd.print(gps.altitude.meters()); // Altitude in meters (double)
      lcd.print("m");
      lcd.print(" S:");
      lcd.print(gps.satellites.value()); // Number of satellites in use (u32)
      lcd.print(" C:");
      lcd.print(gps.course.deg()); // Course in degrees (double)
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
      lcd.print("LAT:");
      lcd.print(gps.location.lat(), 4); // Raw longitude in whole degrees
      lcd.print("LNG:");
      lcd.print(gps.location.lng(), 4); // Raw longitude in whole degrees
      }
    } else
    {
    lcd.setCursor(0,1);
    lcd.print("No valid location   ");
    }
if (gps.date.isValid() & gps.date.isValid())
    {    
    lcd.setCursor(0,2);
    lcd.print(gps.date.day());
    lcd.print(".");
    lcd.print(gps.date.month());
    lcd.print(".");
    lcd.print(gps.date.year());
    lcd.print(" ");
    lcd.print(gps.time.hour()+1);
    lcd.print(":");
    lcd.print(gps.time.minute());
    lcd.print("    ");        
    } else
    {
    lcd.setCursor(0,2);
    lcd.print("No valid Timestamp  ");
    }
if (gps.speed.isValid())
    { 
    lcd.setCursor(0,3);
    lcd.print("GS:");
    lcd.print(gps.speed.kmph());
    lcd.print("km/h ");
    lcd.print(gps.speed.knots()); // Speed in knots (double)
    lcd.print("kt ");
    } else
    {
    lcd.setCursor(0,3);
    lcd.print("No valid Speeddata  ");
    }

smartdelay(5000);
}

static void smartdelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}
