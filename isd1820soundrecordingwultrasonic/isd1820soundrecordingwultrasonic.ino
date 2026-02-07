// Pin Definitions
const int playPin = 6;        // Pin to control playback on ISD1820
const int trigPin = 9;        // Pin for Trigger on HC-SR04
const int echoPin = 10;       // Pin for Echo on HC-SR04

long duration;                // Duration for the sound wave to return
int distance;                 // Calculated distance

void setup() {
  // Set the play pin as OUTPUT
  pinMode(playPin, OUTPUT);
  
  // Set the HC-SR04 pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  // Start with playback turned off
  digitalWrite(playPin, LOW);
  
  Serial.begin(9600); // Initialize serial communication for debugging
}

void loop() {
  // Send a pulse to the ultrasonic sensor to trigger the measurement
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Measure the time it takes for the pulse to return
  duration = pulseIn(echoPin, HIGH);
  
  // Calculate the distance (in cm)
  distance = duration * 0.0343 / 2;
  
  // Print the distance to the Serial Monitor (for debugging)
  Serial.print("Distance: ");
  Serial.println(distance);
  
  // Check if the distance is less than a threshold (e.g., 10 cm)
  if (distance < 10) {
    // Trigger playback if the object is within 10 cm
    digitalWrite(playPin, HIGH);
    
    // Wait for playback to finish (5 seconds in this case)
    delay(5000);
    
    // Stop playback
    digitalWrite(playPin, LOW);
    
    // Wait a little to avoid multiple triggers quickly
    delay(2000);
  }
  
  // Short delay before the next reading
  delay(100);
}