#include <DHT.h>
#include <LiquidCrystal_I2C.h>

// Define pin and type
#define DHTPIN 8
#define DHTTYPE DHT11

// Create DHT object (class name is DHT, not dht)
DHT dht(DHTPIN, DHTTYPE);

LiquidCrystal_I2C lcd(0x27, 16, 2); // if your LCD is at 0x3F


void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  dht.begin(); // Initialize DHT sensor
}

void loop() {
  float t = dht.readTemperature();
  float h = dht.readHumidity();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return; // Stop here if readings failed
  }

  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(t);
  lcd.print((char)223); // degree symbol
  lcd.print("C");

  lcd.setCursor(0, 1);
  lcd.print("Humidity: ");
  lcd.print(h);
  lcd.print("%");

  delay(2000); // 2-second delay between updates
}
