const int  watchPin = A0;    // the pin that the pushbutton is attached to
const int ledPin = 13;       // the pin that the LED is attached to

void setup() {
  // initialize the button pin as a input:
  pinMode(watchPin, INPUT);
  // initialize the LED as an output:
  pinMode(ledPin, OUTPUT);
  // initialize serial communication:
  Serial.begin(9600);
}

void loop() {
    edgeDetect();
}

void edgeDetect(){
   int pinState = 0;                // current state of the button
   static int lastPinState = 0;     // previous state of the button
   
   pinState = digitalRead(watchPin);

  if (pinState != lastPinState) { //changed
    pinChanged(pinState);
    //delay(50);
  }
  
  lastPinState = pinState;
}

void pinChanged(bool state){
    static long previousMillis = 0; 
    unsigned long currentMillis = millis();
    
    int d = currentMillis - previousMillis;
    
    previousMillis = currentMillis;
    
    
    digitalWrite(ledPin, !state);
    dec(state, d);
}


