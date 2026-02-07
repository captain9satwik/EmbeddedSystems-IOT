#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);  // Change 0x27 to 0x3F if needed

const int pulsePin = A0;     // Pulse Sensor signal wire connected to A0
const int blinkPin = 13;     // Onboard LED blinks on heartbeat

int signal;                  // Raw pulse signal
int BPM;                     // Calculated Beats Per Minute
int IBI = 600;               // Inter-Beat Interval (ms)
boolean Pulse = false;
boolean firstBeat = true;
boolean secondBeat = false;

unsigned long sampleCounter = 0;
unsigned long lastBeatTime = 0;
int rate[10];
int P = 512;  // Peak
int T = 512;  // Trough
int thresh = 530;
int amp = 100;
boolean QS = false;

void setup() {
  pinMode(blinkPin, OUTPUT);
  Serial.begin(115200);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Initializing...");

  // Timer2 interrupt setup for 2ms
  TCCR2A = 0x02;
  TCCR2B = 0x06;
  OCR2A = 0x7C;
  TIMSK2 = 0x02;
  sei();

  delay(1000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Place finger...");
}

void loop() {
  if (QS) {
    QS = false;

    lcd.setCursor(0, 0);
    lcd.print("Heart Rate:     ");

    lcd.setCursor(0, 1);
    lcd.print("BPM: ");
    lcd.print(BPM);
    lcd.print("      ");
  }

  delay(20);
}

ISR(TIMER2_COMPA_vect) {
  cli();

  signal = analogRead(pulsePin);
  sampleCounter += 2;
  int N = sampleCounter - lastBeatTime;

  if (signal < thresh && N > (IBI / 5) * 3) {
    if (signal < T) {
      T = signal;
    }
  }

  if (signal > thresh && signal > P) {
    P = signal;
  }

  if (N > 250) {
    if ((signal > thresh) && (Pulse == false) && (N > (IBI / 5) * 3)) {
      Pulse = true;
      digitalWrite(blinkPin, HIGH);
      IBI = sampleCounter - lastBeatTime;
      lastBeatTime = sampleCounter;

      if (secondBeat) {
        secondBeat = false;
        for (int i = 0; i <= 9; i++) {
          rate[i] = IBI;
        }
      }

      if (firstBeat) {
        firstBeat = false;
        secondBeat = true;
        sei();
        return;
      }

      unsigned int runningTotal = 0;
      for (int i = 0; i < 9; i++) {
        rate[i] = rate[i + 1];
        runningTotal += rate[i];
      }

      rate[9] = IBI;
      runningTotal += rate[9];
      runningTotal /= 10;
      BPM = 60000 / runningTotal;
      QS = true;
    }
  }

  if (signal < thresh && Pulse == true) {
    digitalWrite(blinkPin, LOW);
    Pulse = false;
    amp = P - T;
    thresh = amp / 2 + T;
    P = thresh;
    T = thresh;
  }

  if (N > 2500) {
    thresh = 512;
    P = 512;
    T = 512;
    lastBeatTime = sampleCounter;
    firstBeat = true;
    secondBeat = false;
  }

  sei();
}
