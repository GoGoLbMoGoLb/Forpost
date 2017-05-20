#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#define DS3231_SDA 4
#define DS3231_SCL 5
LiquidCrystal_I2C lcd(0x3F, 16, 2);  // Changed for LCD1602 0x27 I2C address and columns by rows. 
//LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7);
uint8_t heart[8] = { 0x0,0xa,0x1f,0x1f,0xe,0x4,0x0 }; // Bitmap character example

void setup() {
	//Wire.begin(DS3231_SDA, DS3231_SCL);
	lcd.begin(DS3231_SDA, DS3231_SCL);                     // changed instead lcd.init() method in old library version
	lcd.clear();
	delay(100);
	lcd.backlight();
	lcd.createChar(1, heart);        //store character in LCD1602 memory
	lcd.home();
	lcd.print("ESP8266 with");
	lcd.setCursor(10, 0);              // columns, rows !! care the order.
	lcd.write(byte(1));               // write sprite character code at Cursor location
	lcd.setCursor(0, 1);
	lcd.print("LiquidCrystalI2C");
}

void loop() {
	
}