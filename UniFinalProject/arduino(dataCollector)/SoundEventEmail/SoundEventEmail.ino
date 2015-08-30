/* ============================================================================
This sketch collects raw data from a sound sensor LM393. All this data is 
inserted in an array and the average is calculated every time a new dat is 
inserted. A threshold is set up in a way that every time the average is bigger
than the threshold an request is send to the server in order to insert a new
bark and an email is sent to the user to inform about the new event.
===============================================================================
*/

#include <Bridge.h>
#include <HttpClient.h>
#include <Temboo.h>
#include "TembooAccount.h" // contains Temboo account information

const int numReadings = 10;     // max numbers of element in the array
int readings[numReadings];      // the readings from the analog input
int index = 0;                  // the index of the current reading
int total = 0;                  // the running total
int average = 0;                // the average
int count = 0;                  // Counter for the numbers of high averages
int inputPin = A0;              // Analogic input
int led = 13;     
int threshold = 100;            // threshold for the volume
int volume;

// your Gmail username, formatted as a complete email address, eg "bob.smith@gmail.com"
const String GMAIL_USER_NAME = "chiquiarenasalonso@gmail.com";

// your Gmail password
const String GMAIL_PASSWORD = "uouwqxmdwfajyrni";

// the email address you want to send the email to, eg "jane.doe@temboo.com"
const String TO_EMAIL_ADDRESS = "chiqui2arenasalonso@gmail.com";

boolean success = false; // a flag to indicate whether we've sent the email yet or not
 
void setup()  {
  // Bridge takes about two seconds to start up
  // it can be helpful to use the on-board LED
  // as an indicator for when it has initialized
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);
  Bridge.begin();
  digitalWrite(led, HIGH);
  // initialize serial communication with computer:
  Serial.begin(9600);
  
  // initialize all the readings to 0:
  for (int thisReading = 0; thisReading < numReadings; thisReading++){     
    readings[thisReading] = 0;   
  } 
} 
    
void loop() {   
// Initialize the client library
  HttpClient client;
 
  // Calculate the average
  average = calculateAverage(index);
  
  index = index + 1;   // advance to the next position in the array  
  
  // if we're at the end of the array...   
  if (index >= numReadings)    {   
    index = 0;          // ...wrap around to the beginning
  }
   average = total / numReadings;    // calculate the average
    
    if(average>=threshold){
      if (count == 0) {
        Serial.println(average);       
        // Make a HTTP request:
        client.get("http://192.168.0.6:3000/insertEvent?event_id=1&pet_id=1");
        sendEmailToServer();
        count = 1;
      }
      digitalWrite(led, HIGH); //Turn ON Led
    } else{
      digitalWrite(led, LOW); // Turn OFF Led
      Serial.println(average);
      count = 0;
    }
 
  delay(100);        // delay in between reads for stability
}

int calculateAverage(int index){
  int result;
  // subtract the last reading:   
  total= total - readings[index];   
  
  // read from the sensor:   
  volume = analogRead(inputPin); // Reads the value from the Analog PIN A0
  readings[index] = volume;     // Introduce the volume value into the array
  
  // add the reading to the total:   
  total= total + readings[index];   
 
  // calculate the average:
    result = total / numReadings;

    return result;
}

void sendEmailToServer(){
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
        SendEmailChoreo.addInput("MessageBody", "Cooper is barking!!!");
    
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
