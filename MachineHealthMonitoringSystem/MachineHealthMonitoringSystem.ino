// ====== LIBRARIES ======
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <math.h>

// ====== LCD SETUP ======
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ====== DS18B20 TEMP SENSOR ======
#define ONE_WIRE_BUS 4
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature tempSensor(&oneWire);

// ====== MPU6050 ======
Adafruit_MPU6050 mpu;

// ====== UART SETUP FOR CURRENT SENSOR ======
#define RXD2 16     // Connects to Arduino TX (via voltage divider)
#define TXD2 17     // Not used, but required for UART setup
HardwareSerial SerialFromArduino(2);

float currentFromArduino = 0.0;

// ====== TIMING ======
unsigned long lastUpdate = 0;
const int displayInterval = 1000;

// ====== FUNCTION PROTOTYPES ======
float calculateVibration();
String predictStatus(float temp, float vib, float current);

void setup() {
  Serial.begin(9600);
  SerialFromArduino.begin(9600, SERIAL_8N1, RXD2, TXD2);

  Wire.begin(21, 22);
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.print("Initializing...");
  delay(1500);
  lcd.clear();

  tempSensor.begin();

  if (!mpu.begin()) {
    lcd.print("MPU6050 FAIL");
    while (1);
  }
  mpu.setAccelerometerRange(MPU6050_RANGE_4_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
}

void loop() {
  // === Read UART current from Arduino ===
  if (SerialFromArduino.available()) {
    String rawString = SerialFromArduino.readStringUntil('\n');
    rawString.trim();  // Remove extra whitespaces/newlines
    currentFromArduino = rawString.toFloat();
  }

  tempSensor.requestTemperatures();
  float tempC = tempSensor.getTempCByIndex(0);
  float vibration = calculateVibration();

  // === Embedded ML Decision Tree ===
  String status = predictStatus(tempC, vibration, currentFromArduino);

  // === Debug Serial Output ===
  Serial.print("Temp: "); Serial.print(tempC); Serial.print(" °C | ");
  Serial.print("Vib: "); Serial.print(vibration, 2); Serial.print(" g | ");
  Serial.print("I: "); Serial.print(currentFromArduino, 2); Serial.print(" A | ");
  Serial.print("Status: "); Serial.println(status);

  // === LCD Output ===
  if (millis() - lastUpdate > displayInterval) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("T:"); lcd.print(tempC, 1);
    lcd.print(" I:"); lcd.print(currentFromArduino, 2);
    lcd.setCursor(0, 1);
    lcd.print("V:"); lcd.print(vibration, 2);
    lcd.print(" "); lcd.print(status);
    lastUpdate = millis();
  }
}

// ====== MODEL/ML LOGIC (Decision Tree, edit as per your export) ======
String predictStatus(float temp, float vib, float current) {
  // Example logic – replace thresholds/rules with the export_text() output from Python
  if (current <= 0.06) {
    if (vib <= 0.2) {
      return "off";
    } else {
      return "fault";
    }
  } else {
    if (temp <= 30.0) {
      return "normal";
    } else {
      return "fault";
    }
  }
}

// ====== VIBRATION FEATURE ======
float calculateVibration() {
  float minAccel = 100.0;
  float maxAccel = -100.0;
  unsigned long start = millis();
  while (millis() - start < 1000) {
    sensors_event_t a, g, t;
    mpu.getEvent(&a, &g, &t);

    float totalAccel = sqrt(
      a.acceleration.x * a.acceleration.x +
      a.acceleration.y * a.acceleration.y +
      a.acceleration.z * a.acceleration.z
    );

    if (totalAccel > maxAccel) maxAccel = totalAccel;
    if (totalAccel < minAccel) minAccel = totalAccel;

    delay(10);
  }
  return maxAccel - minAccel;  // g peak-to-peak
}
