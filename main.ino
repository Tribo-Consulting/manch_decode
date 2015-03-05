const int watchPin = A0;    // the pin that the pushbutton is attached to
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
    
    while (Manch::available() > 0) {
        Serial.write(Manch::read());
    }
    
}


void edgeDetect(){
   bool pinState = 0;                // current state
   static bool lastPinState = 0;     // previous state
   
   pinState = !digitalRead(watchPin); //invert for pullup config

  if (pinState != lastPinState) { //changed
    pinChanged(pinState);
    //delay(50);
  }
  
  lastPinState = pinState;
}


void pinChanged(bool state){
    static long previousMillis = 0; 
    unsigned long currentMillis = millis();
    int duration = currentMillis - previousMillis;
    previousMillis = currentMillis;
    
    digitalWrite(ledPin, state);
    Manch::dec(state, duration);
}


