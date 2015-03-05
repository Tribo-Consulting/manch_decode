namespace Manch{
  
  #define VERBOSE 0 //change to 1 to print everything
  
  char status;
  enum {
    PREAMBLE, //waiting for checksum
    SYNC,     //synced signal
    CHECK,    //passed checksum
    RESYNC    //lost sync
  };
  

  
  /** Called every falling or rising edge */
  void dec(bool s, uint16_t t){
    
    static uint16_t T; //timing of a half bit
    
    static uint8_t i; //count edges on the preamble stage
    
    static bool w; //keep track of the 2T timing
    
    static byte buffer; //used to store bits until a full byte is received
    static uint8_t p;   //position of current bit
    
    ++i;
    
    if(status==RESYNC){
        //reset everything and wait for a preamble and resync
        i=0;
        T=0;
        w=0;
        
        buffer=0x0;
        p=0;
        
        status=PREAMBLE;
        
        #if VERBOSE==1
         Serial.print("<RESYNC> ");
        #endif
    }
    
    //preamble
    if(status==PREAMBLE){
      
        #if VERBOSE==1
          Serial.println();
          Serial.print(s);
          Serial.print(" ");
          Serial.print(t);
          Serial.print(" preamble");  
        #endif
        
        if(i>3){
            
            if(!T){ //undefined T
              T = t;
              
              #if VERBOSE==1
                Serial.print(" t=");
                Serial.print(T);
              #endif
            }
            else if(t<T*0.75){ //found a much smaller T
                T=t;
                
                #if VERBOSE==1
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
                   
                   #if VERBOSE==1
                     Serial.println(" SYNC!\n");
                   #endif
            }
            else{ //found much longer T
              status = RESYNC;
            }
        }
  
    }
    
    else{ // SYNCED start to decode to buffer
      
      if(t>T*2.5 || t<T*0.75){ //timming is very off
        status=RESYNC;
      }
      
      //decode
      if(t>T*1.5){ //2t
        buffer = buffer | s<<7-p++; 
      }
      else{ //t
         if(w<1){ //first t
           w=1;
         }
         else{ //second t
           buffer = buffer | s<<7-p++;
           w=0;
         }        
      }
      
      
      //do a CHECKSUM after sync signal
      if(status==SYNC){
        
        if(p>7){// wait for a complete byte  
           if(buffer==0b0100){ //checksum
             status = CHECK;
             
             #if VERBOSE==1
               Serial.println(" CHECK! \n");
             #endif
             
             //clear
             buffer=0x0;
             p=0;
           }
           else{ //invalid checksum
             status=RESYNC;
             
             #if VERBOSE==1
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
            
            Buffer::enqueue(buffer);
            
            buffer = 0x0; //clear
            p=0;
        }
      }
      
      
    }
  
  }
  
  uint8_t available(){
      return Buffer::queuelevel();
  }
  byte read(){
      return Buffer::dequeue();
  }

}
