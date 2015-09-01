#include<Wire.h>
#include <Bridge.h>
#include <HttpClient.h>
#include <Temboo.h>
#include "TembooAccount.h" // contains Temboo account information

const int MPU = 0x68;                           // I2C address of the MPU-6050

// variables to get the three angles: yaw, pitch and roll
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;
double compAngleX, compAngleY, compAnglez;
double accXangle , accYangle, acczangle , gyroXrate , gyroYrate, gyroZrate;
double gyroXAngle, gyroYAngle, gyroZAngle;
int ap = 0.955;         // float rgyro,w;
double timer, prev_timer;     // To set up the time and previous time in milliseconds
double average2;
double prev_average;
double reading[10];
double threshold2 = 20;          // Limit to check movement
int index2 = 0;
int count2 = 0;
int led = 13;

const int numReadings = 10;     // max numbers of element in the array
int readings[numReadings];      // the readings from the analog input
int index1 = 0;                  // the index of the current reading
int total = 0;                  // the running total
int average1 = 0;                // the average
int count1 = 0;                  // Counter for the numbers of high averages
int inputPin = A0;              // Analogic input
int threshold1 = 100;            // threshold for the volume
int volume;

int moveCounter = 0;

// your Gmail username, formatted as a complete email address, eg "bob.smith@gmail.com"
const String GMAIL_USER_NAME = "chiquiarenasalonso@gmail.com";

// your Gmail password
const String GMAIL_PASSWORD = "uouwqxmdwfajyrni";

// the email address you want to send the email to, eg "jane.doe@temboo.com"
const String TO_EMAIL_ADDRESS = "chiqui2arenasalonso@gmail.com";

boolean success = false; // a flag to indicate whether we've sent the email yet or not

void setup() {
  // Bridge takes about two seconds to start up
  // it can be helpful to use the on-board LED
  // as an indicator for when it has initialized
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);
  Bridge.begin();
  digitalWrite(led, HIGH);

  // initialize serial communication with computer:
  Serial.begin(9600);
  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);

  // initialize all the readings to 0:
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }

}

void loop() {
  // Initialize the client library
  HttpClient client1;
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);       // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 14, true); // request a total of 14 registers

  checkSound();

  if (moveCounter == 0) {
    getValuesAccGy();
  }

  if (moveCounter < 4) {
    moveCounter++;
  } else {
    moveCounter = 0;
  }

  delay(100);        // Delay the loop for one second
}


void checkSound() {
  // Calculate the average in sound array
  average1 = calculateAverage(index1);
  index1 = index1 + 1;   // advance to the next position in the array

  // if we're at the end of the array...
  if (index1 >= numReadings)    {
    index1 = 0;          // ...wrap around to the beginning
  }
  average1 = total / numReadings;    // calculate the average

  if (average1 >= threshold1) {
    if (count1 == 0) {
      Serial.println(average1);
      Serial.println("YESSSSSS   BARKING");
      // Make a HTTP request and send an email to the user:
      //client1.get("http://192.168.0.6:3000/insertEvent?event_id=1&pet_id=1");
      //sendEmailToUser("coper is barking");
      count1 = 1;        // Set the counter to 1 in order to avoid more sound until the last one finishes
    }
    digitalWrite(led, HIGH); //Turn ON Led

    // set the counter to 0 to start a new sound
  } else {
    digitalWrite(led, LOW); // Turn OFF Led
    Serial.println(average1);
    count1 = 0;
  }

}

// Get values from the accelerometer and gyroscopy
void getValuesAccGy() {
  AcX = Wire.read() << 8 | Wire.read(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
  AcY = Wire.read() << 8 | Wire.read(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ = Wire.read() << 8 | Wire.read(); // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  GyX = Wire.read() << 8 | Wire.read(); // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY = Wire.read() << 8 | Wire.read(); // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ = Wire.read() << 8 | Wire.read(); // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)

  calculateAngles(AcX, AcY, AcZ, GyX, GyY, GyZ);
}

// Calculate angles yaw, pitch and roll
void calculateAngles(int16_t AcX, int16_t AcY, int16_t AcZ, int16_t GyX, int16_t GyY, int16_t GyZ) {
  accXangle = (atan2(AcY, AcZ) * RAD_TO_DEG);
  accYangle = (atan2(AcX, AcZ) * RAD_TO_DEG);
  acczangle = (atan2(AcX, AcY) * RAD_TO_DEG);
  gyroXrate = GyX / 16.5;
  gyroYrate = GyY / 16.5;
  gyroZrate = GyZ / 16.5;

  // Set the current and previous time
  if (timer == 0) {
    prev_timer = timer;
    timer = millis();
  } else {
    prev_timer = timer;
    timer = millis();
  }

  //Angular position
  gyroXAngle += gyroXrate * (millis() - timer) / 1000;
  gyroYAngle += gyroYrate * (millis() - timer) / 1000;
  gyroZAngle += gyroZrate * (millis() - timer) / 1000;

  filter(accXangle, accYangle, acczangle, gyroXAngle, gyroYAngle, gyroZAngle, timer, prev_timer);
}

// Applying filter to get more precision in the three angles
void filter(double accXangle, double accYangle, double acczangle, double gyroXAngle, double gyroYAngle, double gyroZAngle, double timer,  double prev_timer) {
  compAngleX = ap * (compAngleX + gyroXAngle) + (1 - ap) * accXangle;
  compAngleY = ap * (compAngleY + gyroYAngle) + (1 - ap) * accYangle;
  compAnglez = ap * (compAnglez + gyroZAngle) + (1 - ap) * acczangle;

  // Get the average for the current three angles and setting the previous average (absolute values)
  if (timer == 0) {
    average2 = abs((compAngleX + compAngleY + compAnglez) / 3);
    prev_average = average2;
  } else {
    prev_average = average2;
    average2 = abs((compAngleX + compAngleY + compAnglez) / 3);
  }

  getAverageReading(average2, prev_average, timer, prev_timer);
}

/* Fill the array with the difference of the current average and previous average for 5 secons
 and getting the jump between max and min absolute values*/
void getAverageReading(double average2,  double prev_average, double timer, double prev_timer) {

  double diff = abs(average2 - prev_average);
  reading[index2++] = diff;       // insert in an array the difference of current average and previous average
  double avg_reading;
  // Get the maximun and minimum values in all the elements once 5 seconds pass.
  if (index2 == 10) {
    int i;
    double min = reading[0];
    double max = reading[0];
    for (i = 1; i < index2; i++) {
      if (reading[i] < min) {
        min = reading[i];
      }
      if (reading[i] > max) {
        max = reading[i];
      }
    }
    // Getting the jump between max and min absolute values
    avg_reading = max - min;
    index2 = 0;
    checkMov(avg_reading, timer);
  }
}

// Check if there is movement during 15 seconds according to the threshold
void checkMov(double avg_reading, double timer) {
  // Initialize the client library
  HttpClient client2;
  // Prints the average in the array for 5 seconds
  Serial.print("AVG: "); Serial.println(avg_reading);

  if (avg_reading > threshold2) {
    // Prints YES in case the movement occurs
    Serial.println("YESSSSSS   MOVEMENT");
    // Check when it was the last movement in order to start the counter
    if (count2 == 2) {
      Serial.println("*******************************MOVEMENT******************************");
      // Make a HTTP request:
      //client2.get("http://192.168.0.6:3000/insertEvent?event_id=2&pet_id=1");
      //sendEmailToUser("cooper is moving");
      count2 = 0;
    } else {
      count2++;
    }
  } else {
    count2 = 0;
  }
  Serial.println("-----------------------------------------------------");
}

// Calculate the average for the elements in the sound array every time a new one is inserted
int calculateAverage(int index1) {
  int result;
  // subtract the last reading:
  total = total - readings[index1];

  // read from the sensor:
  volume = analogRead(inputPin); // Reads the value from the Analog PIN A0
  readings[index1] = volume;     // Introduce the volume value into the array

  // add the reading to the total:
  total = total + readings[index1];

  // calculate the average:
  result = total / numReadings;

  return result;
}

// Send an email to the user in order to inform when an event happens
void sendEmailToUser(String text) {
  Serial.println("Running SendAnEmail...");
  TembooChoreo SendEmailChoreo;

  // invoke the Temboo client
  SendEmailChoreo.begin();

  // set Temboo account credentials
  SendEmailChoreo.setAccountName(TEMBOO_ACCOUNT);
  SendEmailChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
  SendEmailChoreo.setAppKey(TEMBOO_APP_KEY);

  // identify the Temboo Library choreo to run (Google > Gmail > SendEmail)
  SendEmailChoreo.setChoreo("/Library/Google/Gmail/SendEmail");

  // the first input is your Gmail email address
  SendEmailChoreo.addInput("Username", GMAIL_USER_NAME);
  // next is your Gmail password.
  SendEmailChoreo.addInput("Password", GMAIL_PASSWORD);
  // who to send the email to
  SendEmailChoreo.addInput("ToAddress", TO_EMAIL_ADDRESS);
  // then a subject line
  SendEmailChoreo.addInput("Subject", "Cooper");

  // next comes the message body, the main content of the email
  SendEmailChoreo.addInput("MessageBody", text);

  // tell the Choreo to run and wait for the results. The
  // return code (returnCode) will tell us whether the Temboo client
  // was able to send our request to the Temboo servers
  unsigned int returnCode = SendEmailChoreo.run();

  // a return code of zero (0) means everything worked
  if (returnCode == 0) {
    Serial.println("Success! Email sent!");
  } else {
    // a non-zero return code means there was an error
    // read and print the error message
    while (SendEmailChoreo.available()) {
      char c = SendEmailChoreo.read();
      Serial.print(c);
    }
  }
  SendEmailChoreo.close();
}

