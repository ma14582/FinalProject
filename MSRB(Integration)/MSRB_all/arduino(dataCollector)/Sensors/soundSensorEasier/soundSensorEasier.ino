int led = 13;
int threshold = 500; //Change This
int volume;
int barking = 0;
int timer = 0;

void setup() {
  Serial.begin(9600); // For debugging
  pinMode(led, OUTPUT);
}

void loop() {

  volume = analogRead(A0); // Reads the value from the Analog PIN A0

  Serial.println(volume);

  if (volume >= threshold) {
    digitalWrite(led, HIGH); //Turn ON Led
    Serial.println("YESSSSSSS");
    if (barking == 0) {
      barking = 1;
      timer++;
      Serial.println("BARKING*****************************");
    } else {
      timer++;
    }
  } else {
    digitalWrite(led, LOW); // Turn OFF Led
    if (barking == 1) {
      timer++;
    }
  }

  if (timer == 100) {
   timer = 0;
   barking = 0;
  }
  Serial.print("TIMER:"); Serial.println(timer);
  
  delay(100);
}
