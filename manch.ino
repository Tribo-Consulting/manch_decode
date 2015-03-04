int i=0;

int p;
int bu[64];

int synced = false;

int T;

int w;

//11111111 00001111
void dec(int s, int t){
  
    ++i;
    Serial.print("\n");
    Serial.print(s);
    Serial.print(" ");
    Serial.print(t);
    
  
  //preamble
  if(synced==false){
      Serial.print(" preamble");  
      
      if(i>3){
          /* todo make sure T is not a 2T */
          if(!T){
            T = t;
            
            Serial.print(" t=");
            Serial.print(T);
          }
          else if(t>T*1.5){ //sync signal
                 synced = true;
                 i = 0;
                 
                 Serial.println(" sync");
          }
      }
      
      
      
    
  }
  
  
  
  //decode data
  if(synced){
    
    if(t>T*1.5){ //2t
      bu[p++] = s;
      
      //Serial.print(" (");
      //Serial.print(!s);
      //Serial.print(") 2T");
    }
    else{ //t
      
       if(w<1){ //first t
         w++;
       }
       else{ //second t
         bu[p++] = s;
         w=0;
         
         //Serial.print(" (");
        //Serial.print(!s);
        //Serial.print(") T");
       }        
    }
    
    
    if(p>7){
        Serial.print("\nBits: ");
        for(int j=0; j<8; j++){
          Serial.print(!bu[j]);
        }
        Serial.print("\n");
        p=0;
    }
    
    
  }

}

