/* *************************************************************
   Encoder definitions
   
   Add an "#ifdef" block to this file to include support for
   a particular encoder board or library. Then add the appropriate
   #define near the top of the main ROSArduinoBridge.ino file.
   
   ************************************************************ */
   
#ifdef USE_BASE

#ifdef ROBOGAIA
  /* The Robogaia Mega Encoder shield */
  #include "MegaEncoderCounter.h"

  /* Create the encoder shield object */
  MegaEncoderCounter encoders = MegaEncoderCounter(4); // Initializes the Mega Encoder Counter in the 4X Count mode
  
  /* Wrap the encoder reading function */
  long readEncoder(int i) {
    if (i == LEFT) return encoders.YAxisGetCount();
    else return encoders.XAxisGetCount();
  }

  /* Wrap the encoder reset function */
  void resetEncoder(int i) {
    if (i == LEFT) return encoders.YAxisReset();
    else return encoders.XAxisReset();
  }
#elif defined(ARDUINO_ENC_COUNTER)
  volatile long left_enc_pos = 0L;
  volatile long right_enc_pos = 0L;
  static const int8_t ENC_STATES [] = {0,1,-1,0,-1,0,0,1,1,0,0,-1,0,-1,1,0};  //encoder lookup table
    
  /* Interrupt routine for LEFT encoder, taking care of actual counting */
  ISR (PCINT2_vect){
  	static uint8_t enc_last=0;
        
	enc_last <<=2; //shift previous state two places
	enc_last |= (PIND & (3 << 2)) >> 2; //read the current state into lowest 2 bits
  
  	left_enc_pos += ENC_STATES[(enc_last & 0x0f)];
  }
  
  /* Interrupt routine for RIGHT encoder, taking care of actual counting */
  ISR (PCINT1_vect){
        static uint8_t enc_last=0;
          	
	enc_last <<=2; //shift previous state two places
	enc_last |= (PINC & (3 << 4)) >> 4; //read the current state into lowest 2 bits
  
  	right_enc_pos += ENC_STATES[(enc_last & 0x0f)];
  }
  
  /* Wrap the encoder reading function */
  long readEncoder(int i) {
    if (i == LEFT) return left_enc_pos;
    else return right_enc_pos;
  }

  /* Wrap the encoder reset function */
  void resetEncoder(int i) {
    if (i == LEFT){
      left_enc_pos=0L;
      return;
    } else { 
      right_enc_pos=0L;
      return;
    }
  }
#elif defined ARDUINO_MY_COUNTER
  //實現：左右電機脈衝計數
  //1.定義計數器
  volatile long left_count = 0L;
  volatile long right_count = 0L;
  //2.初始化
  void initEncoder(){
    pinMode(LEFT_A,INPUT); // 21  --- 2
    pinMode(LEFT_B,INPUT); // 20  --- 3
    pinMode(RIGHT_A,INPUT);// 18  --- 5
    pinMode(RIGHT_B,INPUT);// 19  --- 4

    attachInterrupt(2,leftEncoderEventA,CHANGE);
    attachInterrupt(3,leftEncoderEventB,CHANGE);
    attachInterrupt(5,rightEncoderEventA,CHANGE);
    attachInterrupt(4,rightEncoderEventB,CHANGE);
  }
  //3.編寫中斷回調函數
  void leftEncoderEventA(){
    if(digitalRead(LEFT_A) == HIGH){
      if(digitalRead(LEFT_B) == HIGH){//A高B高
        left_count++;
      } else {
        left_count--;
      }
    } else {
      if(digitalRead(LEFT_B) == LOW){//A低B低
        left_count++;
      } else {
        left_count--;
      }
    }
  };
  
  void leftEncoderEventB(){
    if(digitalRead(LEFT_B) == HIGH){
      if(digitalRead(LEFT_A) == LOW){//B高A低
        left_count++;
      } else {
        left_count--;
      }
    } else {
      if(digitalRead(LEFT_A) == HIGH){//B低A高
        left_count++;
      } else {
        left_count--;
      }
    }
  };
  
  void rightEncoderEventA(){
   if(digitalRead(RIGHT_A) == HIGH){
      if(digitalRead(RIGHT_B) == HIGH){//A高B高
        right_count++;
      } else {
        right_count--;
      }
    } else {
      if(digitalRead(RIGHT_B) == LOW){//A低B低
        right_count++;
      } else {
        right_count--;
      }
    }  
  };
  
  void rightEncoderEventB(){
     if(digitalRead(RIGHT_B) == HIGH){
      if(digitalRead(RIGHT_A) == LOW){//B高A低
        right_count++;
      } else {
        right_count--;
      }
    } else {
      if(digitalRead(RIGHT_A) == HIGH){//B低A高
        right_count++;
      } else {
        right_count--;
      }
    }  
  };
  
  //4.實現編碼器數據和重置函數
  // i 取值是LEFT或RIGHT，是左右輪的標記
  long readEncoder(int i){
    if (i == LEFT) return left_count;
    else return right_count;
  };
  void resetEncoder(int i){
    if (i == LEFT){
      left_count=0L;
      return;
    } else { 
      right_count=0L;
      return;
    }
  };
 
#else
  #error A encoder driver must be selected!
#endif

/* Wrap the encoder reset function */
void resetEncoders() {
  resetEncoder(LEFT);
  resetEncoder(RIGHT);
}

#endif
