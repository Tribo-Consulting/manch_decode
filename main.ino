const int watchPin = 3;    // the pin that it is toggled using manchester encoding
const int ledPin = 9;       // the pin that the LED is attached to

uint32_t time = 0;
volatile bool printReady = false;
volatile uint32_t microsEdge = 0;
bool printDone = false;

void isr(void)
{
    edgeDetect();
    //microsEdge = micros();
    printReady = true;
}

void setup() {
    // initialize the button pin as a input:
    pinMode(watchPin, INPUT);
    attachInterrupt(digitalPinToInterrupt(watchPin), isr, CHANGE);
    // initialize the LED as an output:
    pinMode(ledPin, OUTPUT);
    // initialize serial communication:
    Serial.begin(921600);
}

void loop() {
    
    //edgeDetect(); //runs on loop

    /*
    uint32_t microsNow = micros();
    //defer printing until n ms past the final edge.
    if(printReady && printDone && (microsNow - microsEdge >= 10000))*/

    uint32_t millisNow = millis();

    if(printReady && printDone && (millisNow - time >= 500))
    {
        printReady = false;
        printDone = false;
    }

    uint8_t buf[32];
    
    //Deferred print to console.
    if(!printDone)
    {
        //print data to serial port whenever something is avaliable
        if(Manch::available() > 8)
        {
            //noInterrupts();

            while (Manch::available() > 0) {
                
                Serial.write(Manch::read());
            }
            Serial.println();
            
            //interrupts();
        }
        
        printDone = true;

        time = millis();
    }
}

//check if the state of the pin changed
void edgeDetect(){
    bool pinState = 0;                 // current state
    static bool lastPinState = 0;      // previous state
   
    pinState = digitalRead(watchPin); //invert for idle=LOW config

    if (pinState != lastPinState) {     //changed
        pinChanged(pinState);
    }
  
    lastPinState = pinState;
}


/** When the pin changed this is called passing the current logic state, and it keeps track of the time since last change */
void pinChanged(bool state){
    static long previousMicros = 0; 
    unsigned long currentMicros = micros();
    int duration = currentMicros - previousMicros;
    previousMicros = currentMicros;
    
    digitalWrite(ledPin, state);
    
    //calls decode passing the current logic state and duration since last change.
    Manch::decode(state, duration);
}


