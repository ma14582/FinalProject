/* ============================================================================
This sketch collects raw data from a sound sensor LM393 and an accelerometer/
gyroscopy MPU6050 sensor to check sound and motion from the pet. All this data is 
processed in different ways. A threshold is set up in a way that every time the 
result of the data processed is bigger than the threshold an request is send to 
the server in order to insert a new event (barking or movement, shown by ******) 
and an email is sent to the user to inform about the new event.
===============================================================================
*/

#include<Wire.h>
#include <Bridge.h>
#include <HttpClient.h>
#include <Temboo.h>
#include "TembooAccount.h" // contains Temboo account information

#define STEP_MOV 5        // Run the movement code every 500 milliseconds
#define BARK_WITHIN 100      // Set the barking notification every 10 seconds
#define NUM_MOV 2   // Set the number of barking
#define SIZE 10     // Set the array size in 10
const int MPU = 0x68;                           // I2C address of the MPU-6050

// variables to get the three angles: yaw, pitch and roll
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;
double compAngleX, compAngleY, compAnglez;
double accXangle , accYangle, acczangle , gyroXrate , gyroYrate, gyroZrate;
double gyroXAngle, gyroYAngle, gyroZAngle;
int ap = 0.955;         // float rgyro,w;
double timer;     // To set up the time in milliseconds
double average2;
double prev_average;
double reading[SIZE];
double threshold2 = 20;          // Limit to check movement
int index2 = 0;
int count2 = 0;
int led = 13;

int threshold = 500;            // Limit to check barking
int volume;                     // data from the sound sensor
int barking = 0;                // To check when a barking happens the first time
int timer1 = 0;                 // To start the counter for the next barking notification

int moveCounter = 1;            // To start the counter for movement code to run

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

}

void loop() {
  // Initialize the client library
  HttpClient client1;
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);       // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 14, true); // request a total of 14 registers

  checkSound();

  if (moveCounter == STEP_MOV) {
    getValuesAccGy();
  }

  if (moveCounter < STEP_MOV) {
    moveCounter++;
  } else {
    moveCounter = 1;
  }

  delay(100);        // Delay the loop for one second
}

// Check if there is movement according to a threshold
void checkSound() {
  volume = analogRead(A0); // Reads the value from the Analog PIN A0

  // Check if the data is bigger than the threshold
  if (volume >= threshold) {
    digitalWrite(led, HIGH); //Turn ON Led
    Serial.print(volume); Serial.print("\t"); Serial.print("TIMER:");
    // send a request to the server with the new event and an email to the user
    if (barking == 0) {
      barking = 1;
      timer1++;         // start the timer until the next barking
      Serial.println(timer1);
      // Print the event
      Serial.println("********************************************************************");
      Serial.println("                           BARKING!!!                               ");
      Serial.println("********************************************************************");
      // Make a HTTP request:
      client2.get("http://192.168.0.6:3000/insertEvent?event_id=1&pet_id=1");
      //sendEmailToUser("cooper is barking");
    } else {              // Print an new barking but within the time set
      Serial.print(timer1); Serial.print("\t"); Serial.println("------BARKING WITHIN TIME-----");
      timer1++;
    }
  } else {
    digitalWrite(led, LOW); // Turn OFF Led
    Serial.print(volume); Serial.print("\t"); Serial.print("TIMER:"); Serial.println(timer1);
    if (barking == 1) {
      timer1++;
    }
  }
  // Reset the timer and barking
  if (timer1 == BARK_WITHIN) {
    timer1 = 0;
    barking = 0;
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
  timer = millis();

  //Angular position
  gyroXAngle += gyroXrate * (millis() - timer) / 1000;
  gyroYAngle += gyroYrate * (millis() - timer) / 1000;
  gyroZAngle += gyroZrate * (millis() - timer) / 1000;

  filter(accXangle, accYangle, acczangle, gyroXAngle, gyroYAngle, gyroZAngle, timer);
}

// Applying filter to get more precision in the three angles
void filter(double accXangle, double accYangle, double acczangle, double gyroXAngle, double gyroYAngle, double gyroZAngle, double timer) {
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

  getAverageReading(average2, prev_average, timer);
}

/* Fill the array with the difference of the current average and previous average for 5 secons
 and getting the jump between max and min absolute values*/
void getAverageReading(double average2,  double prev_average, double timer) {

  double diff = abs(average2 - prev_average);
  reading[index2++] = diff;       // insert in an array the difference of current average and previous average
  double avg_reading;
  // Get the maximun and minimum values in all the elements once 5 seconds pass.
  if (index2 == SIZE) {
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
  // Check if there is movement
  if (avg_reading > threshold2) {
    // Check number of movement in order to send the event
    if (count2 == NUM_MOV) {
      Serial.println("********************************************************************");
      Serial.println("                           MOVEMENT!!!                              ");
      Serial.println("********************************************************************");
      // Make a HTTP request:
      client2.get("http://192.168.0.6:3000/insertEvent?event_id=2&pet_id=1");
      //sendEmailToUser("cooper is moving");
      count2 = 0;
    } else {            // Start the number of movements
      count2++;
      Serial.println("-----------------------------------------------------");
      Serial.print("AVG: "); Serial.print(avg_reading); Serial.print("\t"); Serial.print("MOVEMENT N: "); Serial.println(count2);
      Serial.println("-----------------------------------------------------");
      
    }
  } else {
    Serial.println("-----------------------------------------------------");
    Serial.print("AVG: "); Serial.print(avg_reading); Serial.print("\t"); Serial.println("NO MOVEMENT DETECTED");
    Serial.println("-----------------------------------------------------");
    count2 = 0;
  }

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

