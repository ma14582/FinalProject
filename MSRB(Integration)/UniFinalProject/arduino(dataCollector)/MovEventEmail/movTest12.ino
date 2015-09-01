/* ============================================================================
This sketch collects raw data from a accelerometer/gyroscopy sensor MPU6050. 
The data from the accelerometer and gyroscopy are processed to get the three
angles (yaw, pitch and roll). The average from the current three angles is 
calculated alongside the previos average is set. The difference resulted of
substracting both is inserted in an array for 5 seconds. All the values
has been calculated with the absolute value because it meassures movmement, 
not directions of movement. After this timer the maximum and minimun of the 
array are calculated in order to get the bigger jump and set the array index 
in 0 again to start a new movement cicle. If the jump is bigger than the 
threshold set, start the movement with a counter. Once the countergets three, 
a request is send to the server in order to insert a new movement and an email
is sent to the user to inform about the new event.
===============================================================================
*/
#include<Wire.h>
#include <Bridge.h>
#include <HttpClient.h>
#include <Temboo.h>
#include "TembooAccount.h" // contains Temboo account information

const int MPU=0x68;                             // I2C address of the MPU-6050

// variables to get the three angles: yaw, pitch and roll
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
double compAngleX,compAngleY, compAnglez;
double accXangle ,accYangle,acczangle ,gyroXrate ,gyroYrate,gyroZrate;
double gyroXAngle, gyroYAngle, gyroZAngle;
int ap=0.955;           // float rgyro,w;
double timer, prev_timer;     // To set up the time and previous time in milliseconds
double average;
double prev_average;
double reading[10];
double threshold = 20;          // Limit to check movement
double start_mov = 0;
double last_update = 0;
int index = 0;
int count = 0;
int led = 13;

// your Gmail username, formatted as a complete email address, eg "bob.smith@gmail.com"
const String GMAIL_USER_NAME = "chiquiarenasalonso@gmail.com";

// your Gmail password
const String GMAIL_PASSWORD = "uouwqxmdwfajyrni";

// the email address you want to send the email to, eg "jane.doe@temboo.com"
const String TO_EMAIL_ADDRESS = "chiqui2arenasalonso@gmail.com";

boolean success = false; // a flag to indicate whether we've sent the email yet or not

void setup(){
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

void loop(){
 
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);       // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU,14,true);  // request a total of 14 registers
  
  getValuesAccGy();

  delay(500);        // Delay the loop for one second
}

// Get values from the accelerometer and gyroscopy
void getValuesAccGy(){
  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
  AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)

  calculateAngles(AcX, AcY, AcZ, GyX, GyY, GyZ);
}

// Calculate angles yaw, pitch and roll
void calculateAngles(int16_t AcX, int16_t AcY, int16_t AcZ, int16_t GyX, int16_t GyY, int16_t GyZ){
 accXangle = (atan2(AcY, AcZ) * RAD_TO_DEG);
 accYangle = (atan2(AcX, AcZ) * RAD_TO_DEG);
 acczangle = (atan2(AcX, AcY) * RAD_TO_DEG);
 gyroXrate = GyX / 16.5;
 gyroYrate = GyY / 16.5;
 gyroZrate = GyZ/ 16.5;

 // Set the current and previous time
 if (timer == 0) {
  prev_timer = timer;
  timer = millis();
 } else {
  prev_timer = timer;
  timer = millis();
 }

 //Angular position
 gyroXAngle += gyroXrate * (millis()-timer)/1000;
 gyroYAngle += gyroYrate * (millis()-timer)/1000;
 gyroZAngle += gyroZrate * (millis()-timer)/1000;

 filter(accXangle, accYangle, acczangle, gyroXAngle, gyroYAngle, gyroZAngle, timer, prev_timer);
}

// Applying filter to get more precision in the three angles
void filter(double accXangle, double accYangle, double acczangle, double gyroXAngle, double gyroYAngle, double gyroZAngle, double timer,  double prev_timer){
 compAngleX = ap * (compAngleX + gyroXAngle) + (1-ap) * accXangle;
 compAngleY = ap * (compAngleY + gyroYAngle) + (1-ap) * accYangle;
 compAnglez = ap * (compAnglez + gyroZAngle) + (1-ap) * acczangle; 

 // Get the average for the current three angles and setting the previous average (absolute values)
 if (timer == 0) {
  average = abs((compAngleX + compAngleY + compAnglez) / 3);
  prev_average = average;
 } else {
  prev_average = average;
  average = abs((compAngleX + compAngleY + compAnglez) / 3);
 }
 
 getAverageReading(average, prev_average, timer, prev_timer);
}

/* Fill the array with the difference of the current average and previous average for 5 secons
 and getting the jump between max and min absolute values*/
void getAverageReading(double average,  double prev_average, double timer, double prev_timer){
   
   double diff = abs(average - prev_average);
   reading[index++] = diff;       // insert in an array the difference of current average and previous average
   double avg_reading;
   // Get the maximun and minimum values in all the elements once 5 seconds pass.
   if (index == 10) {
    int i;
    double min = reading[0];
    double max = reading[0];
    for (i = 1; i < index; i++){
      if (reading[i] < min) {
        min = reading[i];
      }
      if (reading[i] > max) {
        max = reading[i];
      }
    }
    // Getting the jump between max and min absolute values
    avg_reading = max - min;
    index = 0; 
    checkMov(avg_reading, timer);
  }
}

// Check if there is movement during 15 seconds according to the threshold
void checkMov(double avg_reading, double timer) {
  // Initialize the client library
    HttpClient client;
    // Prints the average in the array for 5 seconds
    Serial.print("AVG: "); Serial.println(avg_reading); 
    
    if (avg_reading > threshold) {
      // Prints YES in case the movement occurs
      Serial.println("YESSSSSS");
      // Check when it was the last movement in order to start the counter
        if (last_update == 0){
          start_mov = timer;
          last_update = start_mov;
          count++;
          Serial.print("count: "); Serial.println(count);
        } else {
          // In case movement happens within 5 seconds increases the counter
          if ((timer - last_update) < 5100){
            count++;
            Serial.print("count: "); Serial.println(count);
            // if there is 3 movement in 15 seconds prints movement, send email to the user and a request to the server
            if (count == 3) {
              Serial.println("*******************************MOVEMENT******************************"); 
              count = 0;
              // Make a HTTP request:
              client.get("http://192.168.0.6:3000/insertEvent?event_id=2&pet_id=1");
              sendEmailToUser();
            }
            // Start the counter in case movement doesn't happens within 5 seconds
          } else {
            count = 1;  
            Serial.print("count: "); Serial.println(count);
          }
          last_update = timer;            // Update the timer for the last movement
        }
    }
    Serial.println("-----------------------------------------------------");
}

// Send an email to the user in order to inform when an event happens
void sendEmailToUser(){
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
        SendEmailChoreo.addInput("MessageBody", "Cooper is moving!!!");
    
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

