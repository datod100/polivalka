#include <LiquidCrystal_I2C.h>

const int SENSOR1 = 2;
const int VALVE = 2;
const unsigned long TIMEOUT = 10 * 1000;
const int WATER_ON_THESH = 30;
const int WATER_OFF_THESH = 60;

LiquidCrystal_I2C lcd(0x27, 20, 4);

void setup()
{
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

  pinMode(VALVE, OUTPUT);
}

void loop()
{
  int moitureInPercentage = readSensor();
  lcd.setCursor(0, 1);
  lcd.print(toLimitedString(moitureInPercentage) + String("%"));

  if (moitureInPercentage < WATER_ON_THESH)
  {
    releaseWater();
  }

  delay(500);
}

boolean releaseWater()
{
  unsigned long startTime = millis();
  waterON();
  int moitureInPercentage = 0;
  Serial.println("Water ON");

  while (millis() - startTime < TIMEOUT && moitureInPercentage < WATER_OFF_THESH)
  {
    moitureInPercentage = readSensor();

    delay(500);
  }
  if (moitureInPercentage < WATER_OFF_THESH){
    Serial.println("Err: TIMEOUT");
    return false;
  }else{
    waterOFF();
    Serial.println("Water ON completed");
  }
  return true;
}

void waterON(){
    digitalWrite(VALVE, true); //water ON
}

void waterOFF(){
    digitalWrite(VALVE, false); //water OFF
}

int readSensor()
{
  int rawSensorData = analogRead(SENSOR1);
  int moitureInPercentage = map(rawSensorData, 320, 620, 99, 0);
  String sensorValue = moitureInPercentage + String("% (") + rawSensorData + String(") ");

  Serial.println("Moiture : " + sensorValue);
  return moitureInPercentage;
}

String toLimitedString(int val)
{
  char buff[2];
  sprintf(buff, "%2d", val);
  return String(buff);
}