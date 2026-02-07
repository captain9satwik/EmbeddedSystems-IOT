#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // Change 0x27 to 0x3F if needed

void setup() {
  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("BOOTING.....");
  delay(2000);

  // Loading bar animation
 lcd.setCursor(0, 1);
  for (int i = 0; i < 16; i++) {
    lcd.print((char)255); // full block character
    delay(150);
  }

  delay(1000);
  lcd.clear();

  // Display a quote
  lcd.setCursor(0, 0);
  lcd.print("  Stay curious!");
  lcd.setCursor(0, 1);
  lcd.print("  Keep building.");
}

void loop() {
  // Nothing here
}
