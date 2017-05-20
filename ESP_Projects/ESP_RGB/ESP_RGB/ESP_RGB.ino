
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Adafruit_NeoPixel.h>

#include <WiFiManager.h> 

#define PIN 4

#define NUM_LEDS 6

#define BRIGHTNESS 100

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRBW + NEO_KHZ800);
LiquidCrystal_I2C lcd(0x27, 16, 2);

int redPin = 16;
int greenPin = 13;
int orangePin = 14;

int gamma1[] = {
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
	1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
	2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
	5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
	10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
	17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
	25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
	37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
	51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
	69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
	90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
	115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
	144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
	177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
	215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 };

ESP8266WebServer server(80);
const int led = 14;

void handleRoot() {
	digitalWrite(led, 1);
	char temp[400];
	int sec = millis() / 1000;
	int min = sec / 60;
	int hr = min / 60;

	snprintf(temp, 400,

		"<html>\
  <head>\
    <meta http-equiv='refresh' content='5'/>\
    <title>ESP8266 Demo</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <h1>Hello from ESP8266!</h1>\
    <p>Uptime: %02d:%02d:%02d</p>\
    <img src=\"/test.svg\" />\
  </body>\
</html>",

hr, min % 60, sec % 60
);
	server.send(200, "text/html", temp);
	
}

void handleNotFound() {
	
	String message = "File Not Found\n\n";
	message += "URI: ";
	message += server.uri();
	message += "\nMethod: ";
	message += (server.method() == HTTP_GET) ? "GET" : "POST";
	message += "\nArguments: ";
	message += server.args();
	message += "\n";

	for (uint8_t i = 0; i < server.args(); i++) {
		message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
	}

	server.send(404, "text/plain", message);
	
}
void setFadeColor(int cPin1, int cPin2, int cPin3) {
	for (int i = 0; i <= 255; i++) {
		analogWrite(cPin1, i);
		analogWrite(cPin2, 255 - i);
		analogWrite(cPin3, 255);
		delay(5);
	}
}
// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
	for (uint16_t i = 0; i<strip.numPixels(); i++) {
		strip.setPixelColor(i, c);
		strip.show();
		delay(wait);
	}
}

void pulseWhite(uint8_t wait) {
	for (int j = 0; j < 256; j++) {
		for (uint16_t i = 0; i<strip.numPixels(); i++) {
			strip.setPixelColor(i, strip.Color(0, 0, 0, gamma1[j]));
		}
		delay(wait);
		strip.show();
	}

	for (int j = 255; j >= 0; j--) {
		for (uint16_t i = 0; i<strip.numPixels(); i++) {
			strip.setPixelColor(i, strip.Color(0, 0, 0, gamma1[j]));
		}
		delay(wait);
		strip.show();
	}
}


void rainbowFade2White(uint8_t wait, int rainbowLoops, int whiteLoops) {
	float fadeMax = 100.0;
	int fadeVal = 0;
	uint32_t wheelVal;
	int redVal, greenVal, blueVal;

	for (int k = 0; k < rainbowLoops; k++) {

		for (int j = 0; j<256; j++) { // 5 cycles of all colors on wheel

			for (int i = 0; i< strip.numPixels(); i++) {

				wheelVal = Wheel(((i * 256 / strip.numPixels()) + j) & 255);

				redVal = red(wheelVal) * float(fadeVal / fadeMax);
				greenVal = green(wheelVal) * float(fadeVal / fadeMax);
				blueVal = blue(wheelVal) * float(fadeVal / fadeMax);

				strip.setPixelColor(i, strip.Color(redVal, greenVal, blueVal));

			}

			//First loop, fade in!
			if (k == 0 && fadeVal < fadeMax - 1) {
				fadeVal++;
			}

			//Last loop, fade out!
			else if (k == rainbowLoops - 1 && j > 255 - fadeMax) {
				fadeVal--;
			}

			strip.show();
			delay(wait);
		}

	}



	delay(500);


	for (int k = 0; k < whiteLoops; k++) {

		for (int j = 0; j < 256; j++) {

			for (uint16_t i = 0; i < strip.numPixels(); i++) {
				strip.setPixelColor(i, strip.Color(0, 0, 0, gamma1[j]));
			}
			strip.show();
		}

		delay(2000);
		for (int j = 255; j >= 0; j--) {

			for (uint16_t i = 0; i < strip.numPixels(); i++) {
				strip.setPixelColor(i, strip.Color(0, 0, 0, gamma1[j]));
			}
			strip.show();
		}
	}

	delay(500);


}

void whiteOverRainbow(uint8_t wait, uint8_t whiteSpeed, uint8_t whiteLength) {

	if (whiteLength >= strip.numPixels()) whiteLength = strip.numPixels() - 1;

	int head = whiteLength - 1;
	int tail = 0;

	int loops = 3;
	int loopNum = 0;

	static unsigned long lastTime = 0;


	while (true) {
		for (int j = 0; j<256; j++) {
			for (uint16_t i = 0; i<strip.numPixels(); i++) {
				if ((i >= tail && i <= head) || (tail > head && i >= tail) || (tail > head && i <= head)) {
					strip.setPixelColor(i, strip.Color(0, 0, 0, 255));
				}
				else {
					strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
				}

			}

			if (millis() - lastTime > whiteSpeed) {
				head++;
				tail++;
				if (head == strip.numPixels()) {
					loopNum++;
				}
				lastTime = millis();
			}

			if (loopNum == loops) return;

			head %= strip.numPixels();
			tail %= strip.numPixels();
			strip.show();
			delay(wait);
		}
	}

}
void fullWhite() {

	for (uint16_t i = 0; i<strip.numPixels(); i++) {
		strip.setPixelColor(i, strip.Color(0, 0, 0, 255));
	}
	strip.show();
}


// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
	uint16_t i, j;

	for (j = 0; j<256 * 5; j++) { // 5 cycles of all colors on wheel
		for (i = 0; i< strip.numPixels(); i++) {
			strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
		}
		strip.show();
		delay(wait);
	}
}

void rainbow(uint8_t wait) {
	uint16_t i, j;

	for (j = 0; j<256; j++) {
		for (i = 0; i<strip.numPixels(); i++) {
			strip.setPixelColor(i, Wheel((i + j) & 255));
		}
		strip.show();
		delay(wait);
	}
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
	WheelPos = 255 - WheelPos;
	if (WheelPos < 85) {
		return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3, 0);
	}
	if (WheelPos < 170) {
		WheelPos -= 85;
		return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3, 0);
	}
	WheelPos -= 170;
	return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0, 0);
}

uint8_t red(uint32_t c) {
	return (c >> 8);
}
uint8_t green(uint32_t c) {
	return (c >> 16);
}
uint8_t blue(uint32_t c) {
	return (c);
}

void setup(void) {
	pinMode(greenPin, OUTPUT);
	pinMode(orangePin, OUTPUT);
	pinMode(redPin, OUTPUT);

	Serial.begin(115200);
	WiFiManager wifiManager;

	wifiManager.resetSettings();
	wifiManager.autoConnect("TEST CONNECT");
	Serial.println("connected...yeey :)");
	server.on("/", handleRoot);
	server.on("/inline", []() {
		server.send(200, "text/plain", "this works as well");
	});
	server.onNotFound(handleNotFound);
	server.begin();
	Serial.println(WiFi.localIP());
	Serial.println("HTTP server started");

	strip.setBrightness(BRIGHTNESS);
	
	strip.begin();

	strip.show(); // Initialize all pixels to 'off'
	colorWipe(strip.Color(0, 0, 0), 5);
}

void loop(void) {
	server.handleClient();
	/*setFadeColor(redPin, greenPin, orangePin);
	setFadeColor(greenPin, orangePin, redPin);
	setFadeColor(orangePin, redPin, greenPin);
	*/

	/*colorWipe(strip.Color(255, 0, 0), 10); // Red
	colorWipe(strip.Color(0, 255, 0), 10); // Green
	colorWipe(strip.Color(0, 0, 255), 10); // Blue
	colorWipe(strip.Color(0, 0, 0, 255), 10); // White
	*/
	whiteOverRainbow(20, 75, 5);

	//pulseWhite(10);

	 //fullWhite();
	// delay(2000);

	rainbowFade2White(3, 3, 1);


	/*for (int i = 0; i < 255; i++)
	{
		analogWrite(redPin, 255 -i);
		analogWrite(greenPin, i);
		delay(5);
	}
	for (int i = 0; i < 255; i++)
	{
		analogWrite(greenPin, 255 - i);
		analogWrite(orangePin, i);
		delay(5);
	}
	for (int i = 0; i < 255; i++)
	{
		analogWrite(orangePin, 255 - i);
		analogWrite(redPin, i);
		delay(5);
	}*/
}
