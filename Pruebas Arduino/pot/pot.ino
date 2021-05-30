#define ANALOGPIN      A0  // For Circuit Playground Express

int delayval = 500;        // delay for half a second

void setup() {
   Serial.begin(9600);    // open the serial port at 9600 bps
}

void loop() {
   int value;

   value = analogRead(ANALOGPIN); // analog read of potentiometer

   Serial.println(value);   // print value

   delay(delayval);         // Delay for a period of time (in milliseconds).
}
