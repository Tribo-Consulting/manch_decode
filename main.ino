const int watchPin = A0;    // the pin that it is toggled using manchester encoding
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
    
    edgeDetect(); //runs on loop
    
    //print data to serial port whenever something is avaliable
    while (Manch::available() > 0) {
        Serial.write(Manch::read());
    }
    
}

//check if the state of the pin changed
void edgeDetect(){
    bool pinState = 0;                 // current state
    static bool lastPinState = 0;      // previous state
   
    pinState = !digitalRead(watchPin); //invert for pullup config

    if (pinState != lastPinState) {     //changed
        pinChanged(pinState);
    }
  
    lastPinState = pinState;
}


/** When the pin changed this is called passing the current logic state, and it keeps track of the time since last change */
void pinChanged(bool state){
    static long previousMillis = 0; 
    unsigned long currentMillis = millis();
    int duration = currentMillis - previousMillis;
    previousMillis = currentMillis;
    
    digitalWrite(ledPin, state);
    
    //calls decode passing the current logic state and duration since last change.
    Manch::decode(state, duration);
}


