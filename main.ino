#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>

#define MAX_DISTANCE 9 

#define FIREBASE_HOST "smart-home-224ab-default-rtdb.firebaseio.com" // Firebase host
#define FIREBASE_AUTH "MzOWnrdK46mPPYAS5AfDF40u9Tol7TaHr2m5LHbd" // Firebase Auth code
#define WIFI_SSID "DD" // Enter your WiFi name
#define WIFI_PASSWORD "23456789" // Enter your WiFi password

#define TRIG_PIN D1 // GPIO 5 (D1)
#define ECHO_PIN D2 // GPIO 4 (D2)

int fireStatus = 0;

void setup() {
  // Initialize serial communication at 9600 bits per second
  Serial.begin(9600); // Ensure this baud rate matches the Serial Monitor's baud rate

  // Connect to WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.println("Connected.");
  Serial.println(WiFi.localIP());
  
  // Initialize Firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  // Initialize the sensor pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(D4, OUTPUT);
  pinMode(D3, OUTPUT);
}

void loop() {
  doorLock();
  bulbOnOff();

  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  // Set the TRIG_PIN high for 10 microseconds
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Read the echo pin, returns the sound wave travel time in microseconds
  long duration = pulseIn(ECHO_PIN, HIGH);

  // Calculate the distance
  // Speed of sound wave divided by 2 (there and back)
  float distance = duration * 0.034 / 2;

  // Map the distance to a percentage value (0 to 100)
  int percentage = map(distance, 0, MAX_DISTANCE, 100, 0);
  if (percentage > 100) {
    percentage = 100;
  } else if (percentage < 0) {
    percentage = 0;
  }

  // Print the percentage on the serial monitor
  Serial.print("Percentage: ");
  Serial.print(percentage);
  Serial.println(" %");

  // Pause for a moment before the next reading
  //delay(500);
  // Send percentage to Firebase
  Firebase.setInt("/percentage", percentage);
    
  // Pause for a moment before the next reading
  //delay(500);
}


void doorLock(){
  if (Firebase.failed()) {
    Serial.print("Firebase connection failed: ");
    Serial.println(Firebase.error());
    delay(5000); // Wait before retrying
    return;
  }

  fireStatus = Firebase.getInt("door_lock");
  if (Firebase.failed()) {
    Serial.print("Failed to get value: ");
    Serial.println(Firebase.error());
    delay(5000); // Wait before retrying
    return;
  }
  
  if (fireStatus == 0) {
    Serial.println("LED Turned ON");
    digitalWrite(D4, HIGH);
  } else if (fireStatus == 1) {
    Serial.println("LED Turned OFF");
    digitalWrite(D4, LOW);
  } else {
    Serial.println("Command Error! Please send 0/1");
  }
  Serial.println(fireStatus);
}

void bulbOnOff(){
  fireStatus = Firebase.getInt("bulb");
  if (fireStatus == 0) {
    Serial.println("Led Turned ON");
    digitalWrite(D3, HIGH);
  }
  else if (fireStatus == 1) {
    Serial.println("Led Turned OFF");
    digitalWrite(D3, LOW);
  }
  else {
    Serial.println("Command Error! Please send 0/1");
  }
}