
#include <IRMitsubishiAC.h>
#include <IRKelvinator.h>
#include <IRDaikinESP.h>
#include <IRremoteESP8266.h>
#include <ESP8266WiFi.h>

#include <SPI.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include <Adafruit_NeoPixel.h>
#define BUTTON_PIN	14    // Digital IO pin connected to the button.  This will be
#define BUZER_PIN	13
#define PIXEL_PIN	4    // Digital IO pin connected to the NeoPixels.
#define RECV_PIN 12
#define SEND_PIN D4 //ESP8266 UART Serial1 Tx pin D4...also ... GPIO2/TXD1

#define ESP8266PLATFORM true    //set to true for ESP8266 platform
#define PIXEL_COUNT 6

#define NEW_GAME	  0x8305E8
#define RADIATION	  0xA000E8
#define KILL_EVERYONE 0x8300E8

#define BIT_COUNT 24
#define ONE_SEC 1000
#define TWO_SEC 2000
#define THREE_SEC 3000
#define FOUR_SEC 4000
#define half_SEC 350

#define DutyCycle50 50
#define DutyCycle40 40
#define DutyCycle30 30
#define DutyCycle20 20
#define DutyCycle10 10

#define PROTOCOL_BUFFER_SIZE 24
#define B(bit_no)         (1 << (bit_no))
#define CB(reg, bit_no)   (reg) &= ~B(bit_no)    //clear bit
#define SB(reg, bit_no)   (reg) |= B(bit_no)

#define ESP8266PLATFORM true    //set to true for ESP8266 platform

// Parameter 1 = number of pixels in strip,  neopixel stick has 8
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream, correct for neopixel stick
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip), correct for neopixel stick
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);
IRrecv irrecv(RECV_PIN);
decode_results results;
ESP8266WebServer server(80);
MDNSResponder mdns;

unsigned char carrierFreq = 0; //default
unsigned char DUTY = 0xF0; //50% default
unsigned int cycleCount = 0;

unsigned long sigTime = 0; //used in mark & space functions to keep track of time
unsigned long sigStart = 0; //used to calculate correct length of existing signal, to handle some repeats

bool oldState = HIGH;
uint8_t showType = 0;
uint8_t led = 12;

const char* serverIndex = "<form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form><form method = 'POST' action = '/find'><input type = 'submit' value = 'FIND ME!'></form>";

const char* host = "esp";
const char* ssid = "Forpost-24";
const char* password = "For123Post";

unsigned long previousMillis = 0;
unsigned long prevMinusmillis = 0;
unsigned long sendMillis = 0;
unsigned long buzzerBlinkMillis = 0;
unsigned long buzzermillis = 0;


unsigned long firstSecMillis = 0;
unsigned long secondSecMillis = 0;
unsigned long thirddSecMillis = 0;
unsigned long fourthSecMillis = 0;
const uint interval = 100;

uint8_t WScount = 0;
uint16_t Wstime = 0;
uint16_t BrightimeUp = 0;
uint16_t BrightimeDown = 0;
uint32_t WScolor;
uint8_t WScolorCounter = 0;
uint8_t ledState = LOW;
uint8_t buzerState = LOW;
uint8_t ProtocolBuff[PROTOCOL_BUFFER_SIZE];

uint8_t buzerBlinkCount = 0;

uint8_t buz = 1;

bool boom = false;
bool flagBright = true;
//bool buzerEnable = false;

volatile bool btnPressed = false;
volatile bool btnReleased = false;

unsigned long pressTime = 0, releaseTime = 0, FindGranataMillis = 0;
unsigned long tiime;
unsigned long buzerOnMillis = 0;
uint8_t c_MinBrightness = 0;

void buzzerBlink(uint8_t interval)
{
	if (millis() - buzzerBlinkMillis > interval) {
		buzzerBlinkMillis = millis();

		//if (buzerState == LOW)
		//{
		//	buzerState = HIGH;
		//}
		//else
		//{
		//	buzerState = LOW;
		//}
		digitalWrite(BUZER_PIN, !digitalRead(BUZER_PIN));
	}
}


void handleInterruptCHANGE() {//нажатие

	if (digitalRead(BUTTON_PIN) == LOW)
	{
		btnPressed = true;
		buzerState = true;
		btnReleased = false;
		pressTime = millis();
	}
	else if(digitalRead(BUTTON_PIN) == HIGH) {
		btnReleased = true;
		releaseTime = millis();
	}
	
}
int checkbit(const int value, const int position) {
	int result;
	if ((value & (1 << position)) == 0) {
		result = 0;
	}
	else {
		result = 1;
	}
	return result;
}
int setbit(const int value, const int position) {
	return (value | (1 << position));
}
int unsetbit(const int value, const int position) {
	return (value & ~(1 << position));
}
uint8_t decodeID(decode_results *res)
{
	uint8_t mask = 0x01;
	uint8_t ID = res->value;
	
	for (uint8_t i = 0; i < 8; i++)
	{
		if (ProtocolBuff[i] != 0)
		{
			//D |= mask;
			SB(ID, mask);
			//mask++;
		}
		else {
			//ID &= mask;
			CB(ID, mask);		
		}
		//ID = 1 << buff[i];
		mask++;
	}
	
	return ID;
}
uint8_t DECdecode()
{
	uint8_t chek;
	uint8_t set;
	uint8_t unset;
	uint8_t result;
	uint8_t tempbuff[8] = {0,0,0,0,1,0,1,0};// 10 DEC
	for (uint8_t i = 0; i < 8; i++)
	{
		chek = checkbit(tempbuff[i],i);
		if (chek == 1)
		{
			set = setbit(tempbuff[i], i);
		}
		else 
		{
			unset = unsetbit(tempbuff[i], i);
		}
		
	}
	Serial.println(set);
	Serial.println(unset);
}
void dump(decode_results *results) {
	// Dumps out the decode_results structure.
	// Call this after IRrecv::decode()
	int count = results->rawlen;
	if (results->decode_type == SONY) {
		Serial.print("Decoded Miles: ");
	}
	

	Serial.print(results->value, HEX);
	Serial.print(" (");
	Serial.print(results->bits, DEC);
	Serial.println(" bits)");
	Serial.print("Raw (");
	Serial.print(count);
	Serial.print("): ");

	uint8_t secondCounter = 0;
	for (int i = 1; i < count; i++) {
		if (i & 1) {
			Serial.print(results->rawbuf[i] * USECPERTICK, DEC);
			if (results->rawbuf[i] * USECPERTICK >= 1050 && results->rawbuf[i] * USECPERTICK <= 1400)
			{
				ProtocolBuff[secondCounter] = 1;
				Serial.print("(");
				Serial.print(ProtocolBuff[secondCounter]);
				Serial.println(")");
				secondCounter++;
			}
			else if (results->rawbuf[i] * USECPERTICK >= 500 && results->rawbuf[i] * USECPERTICK <= 700)
			{
				ProtocolBuff[secondCounter] = 0;
				Serial.print("(");
				Serial.print(ProtocolBuff[secondCounter]);
				Serial.println(")");
				secondCounter++;
			}
		}
		else {
			Serial.write('-');
			Serial.print((unsigned long)results->rawbuf[i] * USECPERTICK, DEC);
		}
		Serial.print(" ");
	}
	Serial.println();
	
}
void WebUpdate(void)
{
	// Удаляем предидущие конфигурации WIFI сети
	WiFi.disconnect(); // обрываем WIFI соединения
	WiFi.softAPdisconnect(); // отключаем отчку доступа(если она была
	WiFi.mode(WIFI_OFF); // отключаем WIFI
	delay(500);

	// присваиваем статичесий IP адрес
	WiFi.mode(WIFI_AP_STA); // режим клиента
	WiFi.config(IPAddress(192, 168, 1, 222), IPAddress(192, 168, 1, 1), IPAddress(255, 255, 255, 0), IPAddress(192, 168, 1, 1));



	Serial.println("Booting Sketch...");
	WiFi.begin(ssid, password);
	if (WiFi.waitForConnectResult() == WL_CONNECTED) {
		
		server.on("/", HTTP_GET, []() {
			server.sendHeader("Connection", "close");
			server.sendHeader("Access-Control-Allow-Origin", "*");
			server.send(200, "text/html", serverIndex);
		});
		server.on("/update", HTTP_POST, []() {
			server.sendHeader("Connection", "close");
			server.sendHeader("Access-Control-Allow-Origin", "*");
			server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
			ESP.restart();
		}, []() {
			HTTPUpload& upload = server.upload();
			if (upload.status == UPLOAD_FILE_START) {
				Serial.setDebugOutput(true);
				WiFiUDP::stopAll();
				Serial.printf("Update: %s\n", upload.filename.c_str());
				uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
				if (!Update.begin(maxSketchSpace)) {//start with max available size
					Update.printError(Serial);
				}
			}
			else if (upload.status == UPLOAD_FILE_WRITE) {
				if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
					Update.printError(Serial);
				}
			}
			else if (upload.status == UPLOAD_FILE_END) {
				if (Update.end(true)) { //true to set the size to the current progress
					Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
				}
				else {
					Update.printError(Serial);
				}
				Serial.setDebugOutput(false);
			}
			yield();
		});
		//server.on("/find", FindGranata);
		server.begin();
		Serial.printf("Ready! Open http://%s.local in your browser\n", host);
		Serial.println(WiFi.localIP());
	}
	else {
		Serial.println("WiFi Failed");
	}
	
	if (mdns.begin("esp", WiFi.localIP())) {
		Serial.println(" setting up MDNS responder!");
	}
	MDNS.addService("http", "tcp", 80);
}
void FindGranata()
{
	buzzerBlink(500);
	server.send(204, "find process ....","<p>find process....</p>");
}
void setup() {
	Serial.begin(115200);

	WebUpdate();
	pinMode(BUTTON_PIN, INPUT_PULLUP);
	pinMode(BUZER_PIN, OUTPUT);
	pinMode(SEND_PIN, OUTPUT);
	
	digitalWrite(SEND_PIN, HIGH);
	attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), handleInterruptCHANGE, CHANGE);
	
	initDutyCycle(DutyCycle50);
	irrecv.enableIRIn();

	//strip.setBrightness(100);
	strip.begin();
	strip.show(); // Initialize all pixels to 'off'
	/*digitalWrite(BUZER_PIN, HIGH);
	delay(500);
	digitalWrite(BUZER_PIN, LOW);*/
}
void buzerRepeat(uint8_t interval,uint8_t repeat,bool buzerEnable)
{
	if (millis() - buzerOnMillis > interval && buzerEnable)
	{
		if (buzerBlinkCount < repeat*2)
		{
			buzzerBlink(100);
		}
		buzerBlinkCount++;
		if (buzerBlinkCount == repeat*2)
		{
			buzerEnable = false;
			//buzerBlinkCount = 0;
		}
		buzerOnMillis = millis();
	}
}
void loop() {
	ESP.wdtFeed(); //avoid watchdog issues
	server.handleClient();
	//if (millis() - firstSecMillis > ONE_SEC)
	//{
	//	firstSecMillis = millis();
	//	//buz++; 
	//	Serial.println(buz);
	//}
	
	//	buzerRepeat(1000, 5, true);
		

	if (!flagBright) {
		
		if (millis() - previousMillis > 3)
		{
			previousMillis = millis();
			for (uint8_t i = 0; i < strip.numPixels(); i++)
			{
				strip.setPixelColor(i, strip.Color(0, 0, c_MinBrightness));
			}
			strip.show();
			c_MinBrightness++;
			if (c_MinBrightness == 255)
			{
				flagBright = false;
			}
		}
	}
	//startShow(2);
	if (!flagBright)
	{
		
		if (millis() - prevMinusmillis > 3)
		{
			prevMinusmillis = millis();
			for (uint8_t i = 0; i < strip.numPixels(); i++)
			{
				strip.setPixelColor(i, strip.Color(0, 0, c_MinBrightness));
			}
			strip.show();
			c_MinBrightness--;
			if (c_MinBrightness == 0)
			{
				flagBright = true;
			}
		}
	}
	if (irrecv.decode(&results)) {
		Serial.println(results.value);
		
		dump(&results);
		Serial.print("ID = ");
		uint8_t ID = decodeID(&results);
		Serial.println(ID);
		irrecv.resume(); // Receive the next value
	}

	if (btnPressed && btnReleased)//подсчет времени нажатия и отпускания
	{
		tiime = releaseTime - pressTime;
		btnPressed = false;
		btnReleased = false;
		
		Serial.print("Time  ");
		Serial.println(tiime);
	}	
	if (btnPressed)
	{
		startShow(7);	
	
	}
	if (btnPressed && !btnReleased && (millis() - pressTime >= 2000))
	{
		
		digitalWrite(BUZER_PIN, HIGH);
		previousMillis = millis();
		delay(100);
		digitalWrite(BUZER_PIN, LOW);
		boom = true;
		buzerOnMillis = millis();
		btnPressed = false;
		btnReleased = true;
	}
	if (boom)
	{
		startShow(5);
		
		if (millis() - buzerOnMillis > interval) {
			
			digitalWrite(BUZER_PIN, HIGH);
			Serial.println("ON");
					
			//buzzer();
		}
		if (millis() - buzerOnMillis > interval+600) {
			buzerOnMillis = millis();
			digitalWrite(BUZER_PIN, LOW);
			buzzermillis = millis();
			Serial.println("OFF");
			//boom = false;
		}

	}
	
	if(tiime != 0 && tiime < 2000 && !btnReleased)
	{
		Serial.println("<2000");
		startShow(10);
		btnPressed = false;
		btnReleased = true;
	}
	

	//if (millis() - sendMillis > 1000)
	//{
	//	sendMiles(RADIATION, BIT_COUNT, 1);

	//}

	//sendMiles(KILL_EVERYONE, BIT_COUNT, 1, 56);

	//startShow(1);
	/*if (millis() > Wstime)
	{
		if (WScount >= strip.numPixels())
		{
			WScount = 0;
			if (WScolorCounter % 3 == 0)
			{
				WScolor = strip.Color(255, 0, 0);
			}
			else if (WScolorCounter % 3 == 1)
			{
				WScolor = strip.Color(0, 255, 0);
			}
			else if (WScolorCounter % 3 == 2)
			{
				WScolor = strip.Color(0, 0, 255);
				WScolorCounter = 0;
			}
			WScolorCounter++;
		}
		strip.setPixelColor(WScount, WScolor);
		WScount++;
		Wstime = millis() + ONE_SEC;
			strip.show();
	}*/
	//colorWipe(strip.Color(255, 0, 0), 1000);
}

void startShow(uint8_t i) {
	switch (i) {
	case 0: colorWipe(strip.Color(0, 0, 0), 50);    // Black/off
		break;
	case 1: colorWipe(strip.Color(255, 0, 0), 1000);  // Red
		break;
	case 2: colorWipe(strip.Color(0, 255, 0), 50);  // Green
		break;
	case 3: colorWipe(strip.Color(0, 0, 255), 50);  // Blue
		break;
	case 4: theaterChase(strip.Color(127, 127, 127), 50); // White
		break;
	case 5: theaterChase(strip.Color(127, 0, 0), 100); // Red
		break;
	case 6: theaterChase(strip.Color(0, 0, 127), 300); // Blue
		break;
	case 7: 
		//rainbowWOdelay(20);
		rainbow(500);
		break;
	case 8: rainbowCycle(300);
		break;
	case 9: theaterChaseRainbow(50);
		break;
	case 10: minusRainbow(20);
		break;
	case 11: colorWipe(strip.Color(120, 127, 127), 10);  // white
		break;
	}
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
	for (uint16_t i = 0; i < strip.numPixels(); i++) {

		strip.setPixelColor(i, c);
		strip.show();
	}
	delay(wait);
}

//void colorWipe(uint32_t c, uint16_t wait) {
//	//uint8_t i;
//	for (uint8_t i = 0; i < strip.numPixels(); i++)
//	{
//		if (millis() - prevMinusmillis > wait)
//		{
//			prevMinusmillis = millis();
//			strip.setPixelColor(i, c);
//			strip.show();
//		}
//	}
//}
void rainbow(uint8_t wait) {
	uint16_t i, j;
	for (j = 128; j < 256; j++) {
		if (millis() - prevMinusmillis > wait)
		{
		for (i = 0; i < strip.numPixels(); i++) {
			strip.setPixelColor(i, Wheel((i + j) & 255));
		}
		
			strip.show();
			prevMinusmillis = millis();
		}
		//delay(wait);
	}
}
void minusRainbow(uint8_t wait) {
	uint16_t i, j;
		for (j = 230; j > 129; j--) {
			if (millis() - prevMinusmillis > wait)
			{
			for (i = 0; i < strip.numPixels(); i++) {
				strip.setPixelColor(i, Wheel((i + j) & 255));
			}
			
				strip.show();
				prevMinusmillis = millis();
			}
			//delay(wait);
		}
		
	}


// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
	uint16_t i, j;

	for (j = 0; j<256 * 5; j++) { // 5 cycles of all colors on wheel
		if (millis() - prevMinusmillis > wait)
		{	
			prevMinusmillis = millis();		
			for (i = 0; i< strip.numPixels(); i++) {
			strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
		}
		strip.show();
		
	}
		//delay(wait);
	}
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
	for (int j = 0; j<10; j++) {  //do 10 cycles of chasing
		for (int q = 0; q < 3; q++) {
			for (int i = 0; i < strip.numPixels(); i = i + 3) {
				strip.setPixelColor(i + q, c);    //turn every third pixel on
			}
			strip.show();

			delay(wait);

			for (int i = 0; i < strip.numPixels(); i = i + 3) {
				strip.setPixelColor(i + q, 0);        //turn every third pixel off
			}
		}
	}
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
	for (int j = 0; j < 256; j++) {     // cycle all 256 colors in the wheel
		for (int q = 0; q < 3; q++) {
			for (int i = 0; i < strip.numPixels(); i = i + 3) {
				strip.setPixelColor(i + q, Wheel((i + j) % 255));    //turn every third pixel on
			}
			strip.show();

			delay(wait);

			for (int i = 0; i < strip.numPixels(); i = i + 3) {
				strip.setPixelColor(i + q, 0);        //turn every third pixel off
			}
		}
	}
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
	WheelPos = 255 - WheelPos;
	if (WheelPos < 85) {
		return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
	}
	if (WheelPos < 170) {
		WheelPos -= 85;
		return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
	}
	WheelPos -= 170;
	return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}


void sendRawBuf(unsigned int *sigArray, unsigned int sizeArray, unsigned char kHz) {
	//digitalWrite(LED,HIGH); //blink LED for every signal - not needed
	ESP.wdtFeed(); //avoid watchdog issues
	initUPWM(kHz); //we only need to re-initialise if it has changed from last signal sent
	sigTime = micros(); //keeps rolling track of signal time to avoid impact of loop & code execution delays
	for (int i = 0; i < sizeArray; i++) {
		mark(sigArray[i++]); //also move pointer to next position
		if (i < sizeArray) { //check we have a space remaining before sending it
			space(sigArray[i]); //pointer will be moved by for loop
		}
	}
	//digitalWrite(LED,LOW); //blink LED for every signal - not needed
}

void sendMiles(unsigned long sigCode, byte numBits, unsigned char repeats) {
#define HEADER_MARK 2400
#define HEADER_SPACE 600
#define ONE_MARK 1200
#define ZERO_MARK 600
#define ONE_SPACE 600
#define ZERO_SPACE 600

	ESP.wdtFeed(); //avoid watchdog issues
				   //digitalWrite(LED,HIGH); //blink LED for every signal - not needed;

	unsigned long bitMask = (unsigned long)1 << (numBits - 1); //allows for signal from 1 bit up to 32 bits
															   //
	sigTime = micros(); //keeps rolling track of signal time to avoid impact of loop & code execution delays
	sigStart = sigTime; //remember for calculating first repeat gap (space), must end 108ms after signal starts
						// First send header Mark & Space
	mark(HEADER_MARK);
	space(HEADER_SPACE);

	while (bitMask) {
		if (bitMask & sigCode) { //its a One bit
			mark(ONE_MARK);
			space(ONE_SPACE);
		}
		else { // its a Zero bit
			mark(ZERO_MARK);
			space(ZERO_SPACE);
		}
		bitMask = (unsigned long)bitMask >> 1; // shift the mask bit along until it reaches zero & we exit the while loop
	}	
}
void initUPWM(unsigned char carrier) { // Assumes standard 8-bit Arduino, running at 16Mhz
									   //supported values are 30, 33, 36, 38, 40, 56 kHz, any other value defaults to 38kHz
	Serial1.begin(560000);
	carrierFreq = carrier;
}

void initDutyCycle(unsigned char dutyCycle) {
	//now do Duty cycle - we simply set the character to be sent, which creates the duty cycle for us.
	switch (dutyCycle) {
	case 50: //50%
		DUTY = 0xF0;
		break;

	case 40: // 40%
		DUTY = 0xF8;
		break;

	case 30: // 30%
		DUTY = 0xFC;
		break;

	case 20: // 20%
		DUTY = 0xFE;
		break;

	case 10: // 10%
		DUTY = 0xFF;
		break;

	default: // 50% for any invalid values
		DUTY = 0xF0;
		break;
	}
}

void mark(unsigned int mLen) { //uses sigTime as end parameter
	sigTime += mLen; //mark ends at new sigTime
	unsigned long startTime = micros();
	unsigned long dur = sigTime - startTime; //allows for rolling time adjustment due to code execution delays

	if (dur == 0) return;

	unsigned int cycleCount = dur / ((1000 + carrierFreq / 2) / carrierFreq); // get number of cycles & do rounding with integer maths
	ESP.wdtFeed(); //avoid watchdog issues
	while (cycleCount) {
		// while (true) { //send continuous carrier, for testing, signal generator or just generic PWM
		Serial1.write(DUTY); //write a character to emulate carrier, character value determines duty cycle.
		--cycleCount;
		if ((cycleCount ^ 0x1F) == 0)ESP.wdtFeed(); //avoid watchdog issues
	}

	while ((micros() - startTime) < dur) {} //just wait here until time is up

}

void space(unsigned int sLen) { //uses sigTime as end parameter
	sigTime += sLen; //space ends at new sigTime

	unsigned long startTime = micros();
	unsigned long dur = sigTime - startTime; //allows for rolling time adjustment due to code execution delays
	if (dur == 0) return;
	unsigned int cycleCount = 0; //
	ESP.wdtFeed();  //avoid watchdog issues
	while ((micros() - startTime) < dur) { //just wait here until time is up
		if ((cycleCount++ ^ 0x1f) == 0)ESP.wdtFeed(); //avoid watchdog issues
	}
}