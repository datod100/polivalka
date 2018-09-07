int SENSOR1 = 2;
int VALVE = 2;

int rawSensorData = 0;
int moitureInPercentage = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  pinMode(VALVE, OUTPUT); 
  //establishContact();
}

void loop() {
    rawSensorData = analogRead(SENSOR1);
    moitureInPercentage = map(rawSensorData,320,619,100,0);
    Serial.write("Moiture : ");
    Serial.print(moitureInPercentage);
    Serial.print(" %");
    Serial.write(" (");
    Serial.print(rawSensorData);
    Serial.println(")");
    if (moitureInPercentage>60){
      digitalWrite(VALVE,true);
    }else{
      digitalWrite(VALVE,false);
    }

    
    delay(500);
}

void establishContact() {  
  while (Serial.available() <= 0) {
    Serial.print('A');   // send a capital A
    delay(300);
  }  
}

