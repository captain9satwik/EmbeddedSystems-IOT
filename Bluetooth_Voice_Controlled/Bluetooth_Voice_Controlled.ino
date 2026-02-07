#include <SoftwareSerial.h>
SoftwareSerial BTserial(11, 10); // RX, TX

#define red   4
#define blue  5
#define light 7

String s;

void setup() {
  Serial.begin(9600);
  BTserial.begin(9600);
  pinMode(red  , OUTPUT);
  pinMode(blue , OUTPUT);
  pinMode(light, OUTPUT);
  Serial.println("Ready");
}

void loop() {
  // read everything that arrived since last loop
  while (BTserial.available()) {
    char c = BTserial.read();
    s += c;
    delay(2);                 // give uart time to buffer next char
  }

  if (s.length()) {           // something received?
    s.trim();                 // remove CR/LF and spaces
    s.toLowerCase();          // make comparison robust
    Serial.println("Cmd: " + s);
    
    if      (s.startsWith("turn on red led")|| s.startsWith("turn on red"))    digitalWrite(red  , HIGH);
    else if (s.startsWith("turn off red led"))   digitalWrite(red  , LOW);
    else if (s.startsWith("turn on blue led"))   digitalWrite(blue , HIGH);
    else if (s.startsWith("turn off blue led"))  digitalWrite(blue , LOW);
    else if (s.startsWith("turn off light"))      digitalWrite(light, HIGH);
    else if (s.startsWith("turn off light"))     digitalWrite(light, LOW);
    
    s = "";                 // clear for next command
  }
}
