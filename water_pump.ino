#include <LiquidCrystal_I2C.h>

const int SENSOR1 = 2;
const int SENSOR2 = 3;
const int VALVE = 2;
const unsigned long TIMEOUT = 20 * 1000;
const unsigned long TIMEOUT_DELAY = 40 * 1000;
const int OFFSET = -47;
const int WATER_ON_THESH = 40;
const int WATER_OFF_THESH = 60;
unsigned long generalTimeCounter;

LiquidCrystal_I2C lcd(0x27, 20, 4);

void setup()
{
  pinMode(VALVE, OUTPUT);
  Serial.begin(9600);

  initLCD();
  while (!Serial)
  {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  waterOFF();
  generalTimeCounter = millis();
}

void loop()
{
  int moitureInPercentage = readSensor();

  if (moitureInPercentage < WATER_ON_THESH)
  {
    irrigate();
    generalTimeCounter = millis();
  }else{
    setLCDStatus("Stand by");
    delay(500);
    setLCDTime(millsToTime(millis() - generalTimeCounter));
  }

}

void irrigate()
{
  setLCDStatus("Irrigate");
  Serial.println("Irrigate");
  waterON();
  int moitureInPercentage = 0;

  unsigned long startTime = millis();
  unsigned long deltaTime = 0;
  while (deltaTime < TIMEOUT && moitureInPercentage < WATER_OFF_THESH)
  {
    moitureInPercentage = readSensor();
    delay(500);
    deltaTime = millis() - startTime;
    
    setLCDTime(millsToTime(deltaTime));
  }

  waterOFF();
  if (moitureInPercentage < WATER_OFF_THESH)
  {
    timeOUT();
  }
}

void timeOUT()
{
  Serial.println("Delay");
  setLCDStatus("Delay");

  unsigned long startTime = millis();
  unsigned long timeLeft = millis() - startTime;
  while (timeLeft < TIMEOUT_DELAY)
  {
    delay(1000);
    timeLeft = millis() - startTime;
    readSensor();

    setLCDTime(millsToTimeTimeout(timeLeft, TIMEOUT_DELAY));
  }
}

String millsToTimeTimeout(unsigned long mills, unsigned long timeout)
{
  unsigned long runMillis = timeout - mills;
  unsigned long allSeconds = runMillis / 1000;
  int runHours = allSeconds / 3600;
  int secsRemaining = allSeconds % 3600;
  int runMinutes = secsRemaining / 60;
  int runSeconds = secsRemaining % 60;

  char buf[8];
  sprintf(buf, "%02d:%02d:%02d", runHours, runMinutes, runSeconds);
  return String(buf);
}

String millsToTime(unsigned long mills)
{
  unsigned long runMillis = mills;
  unsigned long allSeconds = runMillis / 1000;
  int runHours = allSeconds / 3600;
  int secsRemaining = allSeconds % 3600;
  int runMinutes = secsRemaining / 60;
  int runSeconds = secsRemaining % 60;

  char buf[8];
  sprintf(buf, "%02d:%02d:%02d", runHours, runMinutes, runSeconds);
  return String(buf);
}

void waterON()
{
  digitalWrite(VALVE, true); //water ON
  Serial.println("Water ON");
}

void waterOFF()
{
  digitalWrite(VALVE, false); //water OFF
  Serial.println("Water OFF");
}

int readSensor()
{
  int rawSensorData = analogRead(SENSOR1);
  int moitureInPercentage = map(rawSensorData, 320 + OFFSET, 620 + OFFSET, 100, 0);
  setLCDSensorValue(moitureInPercentage);

  String sensorValue = moitureInPercentage + String("% (") + rawSensorData + String(") ");
  Serial.println("Sensor 1 : " + sensorValue);

  int rawSensorData2 = analogRead(SENSOR2);
  int moitureInPercentage2 = map(rawSensorData2, 320 + OFFSET, 620 + OFFSET, 100, 0);
  String sensorValue2 = moitureInPercentage2 + String("% (") + rawSensorData2 + String(") ");
  Serial.println("Sensor 2 : " + sensorValue2);

  return moitureInPercentage;
}

void setLCDStatus(String status)
{
  lcd.setCursor(8, 0);
  char buff[8];
  sprintf(buff, "%-8s", status);
  lcd.print(buff);
}

void setLCDTime(String time)
{
  lcd.setCursor(8, 1);
  lcd.print(time);
}

void setLCDSensorValue(int value)
{
  lcd.setCursor(3, 0);
  if (value > 99)
    value = 99;
  if (value < 0)
    value = 0;
  char buff[2];
  sprintf(buff, "%2d", val);
  lcd.print(buff);
}

void setLCDThreasholdValue(int value)
{
  lcd.setCursor(3, 1);
  char buff[2];
  sprintf(buff, "%2d", val);
  lcd.print(buff);
}

void initLCD()
{
  lcd.init();
  lcd.backlight();
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("SN: 0% Stand by");
  lcd.setCursor(0, 1);
  lcd.print("TH: 0%  00:00:00");

  setLCDThreasholdValue(WATER_ON_THESH);
}