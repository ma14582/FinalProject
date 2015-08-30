#include<Wire.h>

const int MPU=0x68;                             // I2C address of the MPU-6050

// variables to get the three angles: yaw, pitch and roll
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
double compAngleX,compAngleY, compAnglez,timer, prev_timer;
double accXangle ,accYangle,acczangle ,gyroXrate ,gyroYrate,gyroZrate;
double gyroXAngle, gyroYAngle, gyroZAngle;
// float rgyro,w;
int ap=0.955;
double average;
double prev_average;
double reading[10];
double threshold = 20;
double start_mov = 0;
double last_update = 0;
int index = 0;
int count = 0;

void setup(){
  Serial.begin(9600);
  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
}
void loop(){
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU,14,true);  // request a total of 14 registers
  
  // Get values from the accelerometer and gyroscopy
  getValuesAccGy();

  delay(500);        // Delay the loop for one second
}
void getValuesAccGy(){
  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
  AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)

  // Calculate angles yaw, pitch and roll
  calculateAngles(AcX, AcY, AcZ, GyX, GyY, GyZ);
}

void calculateAngles(int16_t AcX, int16_t AcY, int16_t AcZ, int16_t GyX, int16_t GyY, int16_t GyZ){
 accXangle = (atan2(AcY, AcZ) * RAD_TO_DEG);
 accYangle = (atan2(AcX, AcZ) * RAD_TO_DEG);
 acczangle = (atan2(AcX, AcY) * RAD_TO_DEG);/* my attempt to calculate yaw but not correct*/
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

 //angular position
 gyroXAngle += gyroXrate * (millis()-timer)/1000;
 gyroYAngle += gyroYrate * (millis()-timer)/1000;
 gyroZAngle += gyroZrate * (millis()-timer)/1000;/* my attempt to calculate yaw but not correct*/

 // Applying filter
 filter(accXangle, accYangle, acczangle, gyroXAngle, gyroYAngle, gyroZAngle, timer, prev_timer);
}

void filter(double accXangle, double accYangle, double acczangle, double gyroXAngle, double gyroYAngle, double gyroZAngle, double timer,  double prev_timer){
 compAngleX = ap * (compAngleX + gyroXAngle) + (1-ap) * accXangle;
 compAngleY = ap * (compAngleY + gyroYAngle) + (1-ap) * accYangle;
 compAnglez = ap * (compAnglez + gyroZAngle) + (1-ap) * acczangle; /*yaw but not correct*/
 if (timer == 0) {
  average = (compAngleX + compAngleY + compAnglez) / 3;
  prev_average = average;
 } else {
  prev_average = average;
  average =(compAngleX + compAngleY + compAnglez) / 3;
 }

 // Printing the angles (yaw, pitch and roll), average, previous average and timer
 printAngles(compAngleX, compAngleY, compAnglez, average, prev_average, timer, prev_timer);
}

void printAngles(double compAngleX, double compAngleY, double compAnglez, double average,  double prev_average, double timer, double prev_timer){
   //Serial.print("ypr\t"); Serial.print(compAnglez);
   //Serial.print("\t"); Serial.print(compAngleY);
   //Serial.print("\t"); Serial.print(compAngleX);
   //Serial.print("\t"); Serial.print("time: "); Serial.println(timer);
  // Serial.print("average: "); Serial.println(abs(average));
   //Serial.print("previous average: "); Serial.println(prev_average);
   //double difference = timer - prev_timer;
   //Serial.print("\t"); Serial.print("prev: "); Serial.println(prev_timer);
   //Serial.print("\t"); Serial.print("difference: "); Serial.println(difference);
   double diff = abs(average) - prev_average;
   //Serial.print("difference: "); Serial.println(diff);
   reading[index++] = abs(diff);
   double avg_reading;
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
    avg_reading = max - min;
    index = 0; 
    Serial.print("AVG: "); Serial.println(avg_reading); 
    //Serial.print("time: "); Serial.println(timer);
    if (avg_reading > threshold) {
      Serial.println("YESSSSSS");
        if (last_update == 0){
          start_mov = timer;
          last_update = start_mov;
          count++;
          Serial.print("count: "); Serial.println(count);
        } else {
          if ((timer - last_update) < 5100){
            count++;
            Serial.print("count: "); Serial.println(count);
          } else {
            count = 1;  
            Serial.print("count: "); Serial.println(count);
          }
          last_update = timer;
        }
    }
   }
   if (count == 3) {
      Serial.println("MOVEMENT******************************"); 
      count = 0;
   }
   //Serial.println("-----------------------------------------------------");
}

















