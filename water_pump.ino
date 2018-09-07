#include <LiquidCrystal_I2C.h>

const int SENSOR1 = 2;
const int VALVE = 2;
const unsigned long TIMEOUT = 10 * 1000;
const unsigned long TIMEOUT_DELAY = 20 * 1000;
const int OFFSET = -47;
const int WATER_ON_THESH = 30;
const int WATER_OFF_THESH = 60;

LiquidCrystal_I2C lcd(0x27, 20, 4);

void setup()
{
  pinMode(VALVE, OUTPUT);
  lcd.init();
  lcd.backlight();

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Sensors :");

  Serial.begin(9600);
  while (!Serial)
  {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  waterOFF();
}

void loop()
{
  int moitureInPercentage = readSensor();

  if (moitureInPercentage < WATER_ON_THESH)
  {
    releaseWater();
  }

  delay(500);
}

boolean releaseWater()
{
  waterON();
  int moitureInPercentage = 0;
  Serial.println("Water ON");

  unsigned long startTime = millis();
  unsigned long timeLeft = millis() - startTime;
  while (timeLeft < TIMEOUT && moitureInPercentage < WATER_OFF_THESH)
  {
    moitureInPercentage = readSensor();
    delay(500);
    timeLeft = millis() - startTime;
    
    lcd.setCursor(11, 1);
    lcd.print(millsToTime(timeLeft));
  }
  
  lcd.setCursor(11, 1);
  lcd.print("     ");
  if (moitureInPercentage < WATER_OFF_THESH)
  {
    Serial.println("Err: TIMEOUT");
    timeOUT();
    return false;
  }

  waterOFF();
  Serial.println("Water ON completed");
  return true;
}

void timeOUT()
{
  lcd.setCursor(12, 1);
  lcd.print("TOUT");
  waterOFF();

  unsigned long startTime = millis();
  unsigned long timeLeft = millis() - startTime;
  while (timeLeft < TIMEOUT_DELAY)
  {
    delay(1000);
    timeLeft = millis() - startTime;

    lcd.setCursor(11, 0);
    lcd.print(millsToTimeTimeout(timeLeft,TIMEOUT_DELAY));
  }

  lcd.setCursor(11, 0);
  lcd.print("     ");
  waterOFF();

  lcd.setCursor(12, 1);
  lcd.print("    ");
}

String millsToTimeTimeout(unsigned long mills, unsigned long timeout){  
    int runMinutes = (timeout - mills) / 1000 / 60;
    int runSeconds = ((timeout - mills) / 1000) % 60;
    char buf[5];
    sprintf(buf, "%02d:%02d", runMinutes, runSeconds);
    return String(buf);
}

String millsToTime(unsigned long mills){  
    int runMinutes = mills / 1000 / 60;
    int runSeconds = (mills / 1000) % 60;
    char buf[5];
    sprintf(buf, "%02d:%02d", runMinutes, runSeconds);
    return String(buf);
}

void waterON()
{
  lcd.setCursor(13, 0);
  lcd.print(" ON");
  digitalWrite(VALVE, true); //water ON
}

void waterOFF()
{
  lcd.setCursor(13, 0);
  lcd.print("OFF");
  digitalWrite(VALVE, false); //water OFF
}

int readSensor()
{
  int rawSensorData = analogRead(SENSOR1);
  int moitureInPercentage = map(rawSensorData, 320 + OFFSET, 620 + OFFSET, 100, 0);
  String sensorValue = moitureInPercentage + String("% (") + rawSensorData + String(") ");

  lcd.setCursor(0, 1);
  //lcd.print(toLimitedString(moitureInPercentage) + String("%"));
  lcd.print(sensorValue);

  Serial.println("Moiture : " + sensorValue);
  return moitureInPercentage;
}

String toLimitedString(int val)
{
  char buff[2];
  sprintf(buff, "%2d", val);
  return String(buff);
}