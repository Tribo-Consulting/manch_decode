#define L Serial.print

#define VERBOSE_PREAMBLE 1

namespace Manch{
  
  #define PREAMBLE 0
  #define CHECK 1
  #define SYNC 2
  #define RESYNC 3
  
  int error;
  
  uint8_t i, p;
  byte buffer;
  
  char status;
  
  int T; //timing of a half bit
  
  bool w; //keep track of the 2T timing
  
  /** Called every falling or rising edge */
  void dec(bool s, int t){
    
    ++i;
    
    if(status==RESYNC){
        #if VERBOSE_PREAMBLE==1
         Serial.print("<RESYNC> ");
         Serial.println(error);
        #endif
        
        //reset everything and wait for a preamble and resync
        i=0;
        T=0;
        w=0;
        
        buffer=0x0;
        p=0;
        
        status=PREAMBLE;
    }
    
    //preamble
    if(status==PREAMBLE){
      
        #if VERBOSE_PREAMBLE==1
          Serial.println();
          Serial.print(s);
          Serial.print(" ");
          Serial.print(t);
          Serial.print(" preamble");  
        #endif
        
        if(i>3){
            
            if(!T){ //undefined T
              T = t;
              
              #if VERBOSE_PREAMBLE==1
                Serial.print(" t=");
                Serial.print(T);
              #endif
            }
            else if(t<T*0.75){ //found a much smaller T
                T=t;
                
                #if VERBOSE_PREAMBLE==1
                Serial.print(" smaller t=");
                Serial.print(T);
              #endif
            }
            else if(t>T*0.75 && t<=T*1.5){ //just T
            
            }
            else if(t>T*1.5 && t<T*2.5 ){ //first 2T is a sync signal
                   status = SYNC;
                   i = 0;
                   
                   p=4; //checksum start at the 4th bit
                   buffer = buffer | s<<7-p++;
                   
                   #if VERBOSE_PREAMBLE==1
                     Serial.println(" SYNC!\n");
                   #endif
            }
            else{ //found much longer T
              status = RESYNC;
              error = 99;
            }
        }
  
    }
    
    else{ // SYNCED start to decode to buffer
      
      if(t>T*2.5 || t<T*0.75){ //timming is very off
        status=RESYNC;
        error == 98;
      }
      
      //decode
      if(t>T*1.5){ //2t
        buffer = buffer | s<<7-p++; //L(s);
      }
      else{ //t
         if(w<1){ //first t
           w=1;
         }
         else{ //second t
           buffer = buffer | s<<7-p++; //L(s);
           w=0;
         }        
      }
      
      
      //do a CHECKSUM after sync signal
      if(status==SYNC){
        
        if(p>7){// wait for a complete byte  
           if(buffer==0b0100){ //checksum
             status = CHECK;
             
             #if VERBOSE_PREAMBLE==1
               Serial.println(" CHECK! \n");
             #endif
             
             //clear
             buffer=0x0;
             p=0;
           }
           else{ //invalid checksum
             status=RESYNC;
             
             #if VERBOSE_PREAMBLE==1
               Serial.println("<INVALID CHECKSUM>");
               error=97;
             #endif
             
             
          }
        } 
      }
      
      
      //start pushing bytes to out buffer
      else if(status==CHECK){
        //received a full byte
        if(p>7){
            
            //Serial.println();
            //Serial.println(buffer, BIN);
            //Serial.println(b, DEC);
            //Serial.write(b);
            
            enqueue(buffer);
            
            buffer = 0x0; //clear
            p=0;
        }
      }
      
      
    }
  
  }
  
  uint8_t available(){
      return queuelevel();
  }
  byte read(){
      return dequeue();
  }

}


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


