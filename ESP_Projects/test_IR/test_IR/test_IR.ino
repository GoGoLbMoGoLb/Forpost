/*
#include <Ticker.h>
#include <Adafruit_NeoPixel.h>
#include <IRremoteESP8266.h>
#define PIXEL_COUNT 6
#define PIXEL_PIN	4    // Digital IO pin connected to the NeoPixels.
#define ONE 600
#define ZERO 0
#define SEND_PIN 5
#define RECV_PIN 12

uint32_t previusMicros = 0;
uint32_t currentMicros = 0;
const long interval = 600;
uint8_t buff[25];
volatile unsigned long next = 0;
//int next;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);
Ticker flipper;
IRrecv irrecv(RECV_PIN);
decode_results results;
uint8_t count = 0;
uint32_t rad = 0xA000E8;
void dump(decode_results *results) {
	// Dumps out the decode_results structure.
	// Call this after IRrecv::decode()
	int count = results->rawlen;
	if (results->decode_type == UNKNOWN) {
		Serial.print("Unknown encoding: ");
	}
	else if (results->decode_type == NEC) {
		Serial.print("Decoded NEC: ");

	}
	else if (results->decode_type == SONY) {
		Serial.print("Decoded SONY: ");
	}
	else if (results->decode_type == RC5) {
		Serial.print("Decoded RC5: ");
	}
	else if (results->decode_type == RC6) {
		Serial.print("Decoded RC6: ");
	}
	else if (results->decode_type == PANASONIC) {
		Serial.print("Decoded PANASONIC - Address: ");
		Serial.print(results->panasonicAddress, HEX);
		Serial.print(" Value: ");
	}
	else if (results->decode_type == LG) {
		Serial.print("Decoded LG: ");
	}
	else if (results->decode_type == JVC) {
		Serial.print("Decoded JVC: ");
	}
	else if (results->decode_type == AIWA_RC_T501) {
		Serial.print("Decoded AIWA RC T501: ");
	}
	else if (results->decode_type == WHYNTER) {
		Serial.print("Decoded Whynter: ");
	}
	Serial.print(results->value, HEX);
	Serial.print(" (");
	Serial.print(results->bits, DEC);
	Serial.println(" bits)");
	Serial.print("Raw (");
	Serial.print(count, DEC);
	Serial.print("): ");

	for (int i = 1; i < count; i++) {
		if (i & 1) {
			Serial.print(results->rawbuf[i] * USECPERTICK, DEC);
		}
		else {
			Serial.write('-');
			Serial.print((unsigned long)results->rawbuf[i] * USECPERTICK, DEC);
		}
		Serial.print(" ");
	}
	Serial.println();
}
void flip()
{
	++count;
	Serial.println(count);
	if (count == 1)
	{
		strip.setPixelColor(1, strip.Color(255, 0, 0));
		strip.show();
	}

	if (count == 5)
	{
		strip.setPixelColor(1, strip.Color(0, 255, 0));
		strip.show();
		count = 0;
	}

	
}
void IR_interrupt()
{

	if (buff[count] != 0)
	{
		count--;
		//buff[count]--;
		//analogWrite(SEND_PIN,ONE);
		digitalWrite(SEND_PIN, HIGH);
		Serial.println("one");
	}
	else
	{
		count++;
		//analogWrite(SEND_PIN, ZERO);
		digitalWrite(SEND_PIN, LOW);
		Serial.println("zero");

	}
	if (count == sizeof(buff))
	{
		flipper.detach();
	}
}
void parseHEX(uint32_t signal)
{
	
	buff[0] = 4;
	buff[1] = 2;
	uint32_t mask = 0x02;
	for (uint8_t i = 2; i < sizeof(buff); i++)
	{
		if (signal & mask == mask)
		{ 
			buff[i] = 2;
		}
		else if(signal & mask == 0)
		{
			buff[i] = 1;
		}
		mask *= 0x02;
	}
}
void setup()
{
	Serial.begin(115200);

	//currentMicros = micros();
	flipper.attach(0.1, IR_interrupt);
	irrecv.enableIRIn();
	pinMode(SEND_PIN, OUTPUT);
	strip.begin();
	strip.show(); // Initialize all pixels to 'off'

}
void loop()
{
	parseHEX(rad);
	//noInterrupts();
	//interrupts();
	currentMicros = millis();
	if (irrecv.decode(&results)) {
		Serial.println(results.value);

		dump(&results);
		irrecv.resume(); // Receive the next value
	}
	

	delay(1000);

}*/





#include <IRremoteESP8266.h>
#define Duty_Cycle 56  //in percent (10->50), usually 33 or 50
//TIP for true 50% use a value of 56, because of rounding errors
//TIP for true 40% use a value of 48, because of rounding errors
//TIP for true 33% use a value of 40, because of rounding errors

#define Carrier_Frequency 56000   //usually one of 38000, 40000, 36000, 56000, 33000, 30000


#define PERIOD    (1000000+Carrier_Frequency/2)/Carrier_Frequency
#define HIGHTIME  PERIOD*Duty_Cycle/100
#define LOWTIME   PERIOD - HIGHTIME
#define txPinIR   5   //IR carrier output
#define RECV_PIN 12


IRrecv irrecv(RECV_PIN);
decode_results results;



unsigned long sigTime = 0; //use in mark & space functions to keep track of time

						   //RAW NEC signal -32 bit with 1 repeat - make sure buffer starts with a Mark
unsigned int NEC_RAW[] = { 9000, 4500, 560, 560, 560, 560, 560, 1690, 560, 560, 560, 560, 560, 560, 560, 560, 560, 560, 560, 1690, 560, 1690, 560, 560, 560, 1690, 560, 1690, 560, 1690, 560, 1690, 560, 1690, 560, 560, 560, 560, 560, 1690, 560, 560, 560, 560, 560, 560, 560, 1690, 560, 560, 560, 1690, 560, 1690, 560, 560, 560, 1690, 560, 1690, 560, 1690, 560, 560, 560, 1690, 560, 39980, 9000, 2232, 560 }; //AnalysIR Batch Export (IRremote) - RAW

																																																																																																					  //RAW Mitsubishi 88 bit signal  - make sure buffer starts with a Mark
unsigned int Mitsubishi_RAW[] = { 3172, 1586, 394, 394, 394, 1182, 394, 394, 394, 394, 394, 1182, 394, 394, 394, 1182, 394, 394, 394, 394, 394, 1182, 394, 1182, 394, 1182, 394, 394, 394, 1182, 394, 394, 394, 1182, 394, 1182, 394, 1182, 394, 394, 394, 394, 394, 394, 394, 394, 394, 1182, 394, 1182, 394, 394, 394, 1182, 394, 1182, 394, 394, 394, 394, 394, 1182, 394, 394, 394, 394, 394, 1182, 394, 394, 394, 394, 394, 1182, 394, 1182, 394, 394, 394, 1182, 394, 1182, 394, 1182, 394, 1182, 394, 1182, 394, 1182, 394, 1182, 394, 1182, 394, 1182, 394, 1182, 394, 394, 394, 394, 394, 394, 394, 394, 394, 394, 394, 394, 394, 394, 394, 394, 394, 1182, 394, 1182, 394, 1182, 394, 1182, 394, 1182, 394, 1182, 394, 394, 394, 1182, 394, 394, 394, 394, 394, 394, 394, 394, 394, 394, 394, 394, 394, 1182, 394, 394, 394, 394, 394, 1182, 394, 1182, 394, 1182, 394, 1182, 394, 1182, 394, 394, 394, 1182, 394, 1182, 394, 394, 394, 394, 394, 394, 394, 394, 394, 394, 394, 1182, 394, 394, 394 }; //AnalysIR Batch Export (IRremote) - RAW

unsigned int warning[] = { 2400, 600, 600,600,600,600,600,600,1200,600,  600,600,1200,600,1200,600,1200,600,     600,600,600,600,600,600,600,600,		600,600,600,600,600,600,600,600,		600,600,600,600,600,600,600,600,	600,600,1200,600,600,600,1200,600 };
void dump(decode_results *results) {
	// Dumps out the decode_results structure.
	// Call this after IRrecv::decode()
	int count = results->rawlen;
	if (results->decode_type == UNKNOWN) {
		Serial.print("Unknown encoding: ");
	}
	else if (results->decode_type == NEC) {
		Serial.print("Decoded NEC: ");

	}
	else if (results->decode_type == SONY) {
		Serial.print("Decoded SONY: ");
	}
	else if (results->decode_type == RC5) {
		Serial.print("Decoded RC5: ");
	}
	else if (results->decode_type == RC6) {
		Serial.print("Decoded RC6: ");
	}
	else if (results->decode_type == PANASONIC) {
		Serial.print("Decoded PANASONIC - Address: ");
		Serial.print(results->panasonicAddress, HEX);
		Serial.print(" Value: ");
	}
	else if (results->decode_type == LG) {
		Serial.print("Decoded LG: ");
	}
	else if (results->decode_type == JVC) {
		Serial.print("Decoded JVC: ");
	}
	else if (results->decode_type == AIWA_RC_T501) {
		Serial.print("Decoded AIWA RC T501: ");
	}
	else if (results->decode_type == WHYNTER) {
		Serial.print("Decoded Whynter: ");
	}
	Serial.print(results->value, HEX);
	Serial.print(" (");
	Serial.print(results->bits, DEC);
	Serial.println(" bits)");
	Serial.print("Raw (");
	Serial.print(count, DEC);
	Serial.print("): ");

	for (int i = 1; i < count; i++) {
		if (i & 1) {
			Serial.print(results->rawbuf[i] * USECPERTICK, DEC);
		}
		else {
			Serial.write('-');
			Serial.print((unsigned long)results->rawbuf[i] * USECPERTICK, DEC);
		}
		Serial.print(" ");
	}
	Serial.println();
}

void setup() {
	//  Serial.begin(57600);
	pinMode(txPinIR, OUTPUT);
	irrecv.enableIRIn();
	pinMode(10, OUTPUT);
	digitalWrite(10, LOW);
}

void loop() {
	if (irrecv.decode(&results)) {
		Serial.println(results.value);

		dump(&results);
		irrecv.resume(); // Receive the next value
	}

				 //Next send the Mitsubishi AC RAW signal above
	sigTime = micros(); //keeps rolling track of signal time to avoid impact of loop & code execution delays
	for (int i = 0; i < sizeof(warning) / sizeof(warning[0]); i++) {
		mark(warning[i++]);  //also move pointer to next position
		if (i < sizeof(warning) / sizeof(warning[0])) space(warning[i]); //pointer will be moved by for loop
	}
	delay(5000); //wait 5 seconds between each signal (change to suit)
				 //Serial.println("******");

}

void mark(unsigned int mLen) { //uses sigTime as end parameter
	sigTime += mLen; //mark ends at new sigTime
	unsigned long now = micros();
	unsigned long dur = sigTime - now; //allows for rolling time adjustment due to code execution delays
	if (dur == 0) return;
	while ((micros() - now) < dur) { //just wait here until time is up
		digitalWrite(txPinIR, HIGH);
		delayMicroseconds(HIGHTIME - 5);
		digitalWrite(txPinIR, LOW);
		delayMicroseconds(LOWTIME - 6);
	}
}

void space(unsigned int sLen) { //uses sigTime as end parameter
	sigTime += sLen; //space ends at new sigTime
	unsigned long now = micros();
	unsigned long dur = sigTime - now; //allows for rolling time adjustment due to code execution delays
	if (dur == 0) return;
	while ((micros() - now) < dur); //just wait here until time is up
}