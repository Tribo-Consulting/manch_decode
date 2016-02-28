/*
 manch - 1.0
 
 created Mar 4, 2015
 by Victor N. Borges -
 
 http://www.vitim.us
 https://github.com/victornpb/manch_decode/
 
 This arduino sketch provides software to decode manchester encoding.
 The code is abstract and can be used independently of the hardware used.
 You just need to provide the logic levels and the interval between transitions to the decode function.
 
 The decoding runs assyncronously and fills a buffer to be used later, using the functions avaliable() and read().
 The buffer size is 8 bytes but there' no hard limit, you can change it from 1 to watever you want.
 
 No hardware interrupt is required, but you can capture transisions using one if you want.
 
 You need to send a 11110100 byte before your data stream.
 The 1111 part is used as a PREAMBLE and needed to calculate the correct timing used,
 the first 0 bit is used as a SYNC signal and a checksum is performed on 0100,
 to check it the PREAMBLE and SYNC occurred on the correct timing. If the CHECKSUM is passed then the next bytes are decoded and pushed to the buffer.
 If the timing is off or SYNC is lost, or checksum is invalid, it will set the RESYNC flag and wait for PREABMLE-CHECK byte (0b11110100) to start decoding again.
 This avoids the buffer being filled with garbage.

 The streams is MSB-first (most significant bit first).
                                           
 The preamble can be as long as you want eg:
                   checksum (0100)
 ....preamble........VVVV
 111111111111111111110100
                    /\
                   sync (first transision)
                     
This is meant to be a simple implementation, it is not a protocol, it does not have CRC. It can be implemented on top of it.
If your data does not pass CRC, you can set Manch.status = RESYNC to force a resync. 

*/


namespace Manch{

    #define VERBOSE 0 //change to 1 to print everything

    char status;
    enum {
        PREAMBLE, //waiting for checksum
        SYNC,     //synced signal
        CHECK,    //passed checksum
        RESYNC    //lost sync
    };


    /** Called every falling or rising edge. This function performs the decoding and fills the buffer.
    bool s -> current logic state
    uint16_t t -> time in milliseconds since last transition
    */
    void decode(bool s, uint16_t t){

        static uint16_t T; //timing of a half bit

        static uint8_t i; //count edges on the preamble stage

        static bool w; //keep track of the 2T timing

        static struct { //used to store bits until a full byte is received
            byte buffer;
            uint8_t pos;
        } bits;

        ++i;

        if(status==RESYNC){ //if the RESYNC flag is set reset everything and wait for a preamble and resync
            i=0;
            T=0;
            w=0;

            bits.buffer=0x0; //clear bits buffer
            bits.pos=0;

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

                if(!T){ //if T is undefined, use the first t as T
                    T = t;

                    #if VERBOSE==1
                    Serial.print(" t=");
                    Serial.print(T);
                    #endif
                }
                else if(t<T*0.75){ //found a much smaller T, use that instead
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

                    bits.pos=4; //checksum start at the 4th bit
                    bits.buffer = bits.buffer | s<<7-bits.pos++;

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
            if(t>T*1.5){ //2T
                bits.buffer = bits.buffer | s<<7-bits.pos++; 
            }
            else{ //T
                if(w<1){ //first t
                    w=1;
                }
                else{ //second t
                    bits.buffer = bits.buffer | s<<7-bits.pos++;
                    w=0;
                }        
            }


            //do a CHECKSUM after sync signal
            if(status==SYNC){

                if(bits.pos>7){// wait for a complete byte  
                    if(bits.buffer==0b0100){ //checksum
                        status = CHECK;

                        #if VERBOSE==1
                        Serial.println(F(" CHECK!\n"));
                        #endif

                        //clear
                        bits.buffer=0x0;
                        bits.pos=0;
                    }
                    else{ //invalid checksum
                        status=RESYNC;

                        #if VERBOSE==1
                        Serial.println(F("<INVALID CHECKSUM>"));
                        #endif
                    }
                } 
            }

            //start pushing bytes to out buffer
            else if(status==CHECK){
                //received a full byte
                if(bits.pos>7){
                    //Serial.println();
                    //Serial.println(buffer, BIN);
                    //Serial.println(b, DEC);
                    //Serial.write(b);

                    Buffer::enqueue(bits.buffer);

                    bits.buffer = 0x0; //clear
                    bits.pos = 0;
                }
            }

        }

    }
      
    /** Return the number of bytes in the buffer */
    uint8_t available(){
        return Buffer::queuelevel();
    }

    /** Used to read the buffer */
    byte read(){
        return Buffer::dequeue();
    }

}
