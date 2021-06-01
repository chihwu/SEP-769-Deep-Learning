/* Water Level Meter
*/

int trigpin = 7;
int echopin = 5;
const int height=1000;//height of distance sensor above water bowl

void setup()
{
  Serial.begin(9600);
  pinMode(trigpin, OUTPUT);
  pinMode(echopin, INPUT);
}

void loop()
{
  int duration, distance, waterLevel;
  digitalWrite(trigpin, HIGH);
   
  delayMicroseconds(1000);  
  digitalWrite(trigpin, LOW);
   
  duration = pulseIn(echopin,HIGH);
   
  distance = ( duration / 2) / 29;
  waterLevel = height-distance;
  Serial.print("distance:"); 
  Serial.print(distance);
  Serial.println(" cm")
  Serial.print("water level:"); 
  Serial.print(waterLevel);
  Serial.println(" cm")
  Serial.print("\n");
    
  delay(1000);
}
