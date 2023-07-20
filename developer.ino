/*
 * Graham Gillmore
 * Screen Developer Control Loop
 * 
 * Would probably be more maintainable if I wrote it as a FSM but this is just POC
 *
 */

#define DELAY 10

int pwm_l_pin = 5;
int pwm_r_pin = 6;
int reed_l_pin = 7;
int reed_r_pin = 8;

// Pin 2 & 3 are the only interrupt pins on the uno 
int push_l_pin = 2;
int push_r_pin = 3;

// Overkill but will make my code more readable
enum DIR { LEFT, RIGHT };
DIR dir = LEFT; // Just Start out going left cause why not

void ramp_up(){
  int source;
  int drain;
  
  if(dir == LEFT){
    source = pwm_l_pin;
    drain = pwm_r_pin;
  }
  else if(dir == RIGHT){
    source = pwm_r_pin;
    drain = pwm_l_pin;
  }

  analogWrite(drain, 0);
  for(int i = 0 ; i <= 255; i +=5) 
  {
    analogWrite(source, i); 
    delay(DELAY);                            
  }
}

void ramp_down(){
  int source;
  int drain;
  
  if(dir == LEFT){
    source = pwm_l_pin;
    drain = pwm_r_pin;
  }
  else if(dir == RIGHT){
    source = pwm_r_pin;
    drain = pwm_l_pin;
  }

  analogWrite(drain, 0);
  for(int i = 255 ; i >= 0; i -=5) 
  {
    analogWrite(source, i); 
    delay(DELAY);                            
  } 
}

void left_push_isr(){
  analogWrite(pwm_l_pin, 0);
  analogWrite(pwm_r_pin, 255); 
  dir = RIGHT;
}

void right_push_isr(){
  analogWrite(pwm_l_pin, 255);
  analogWrite(pwm_r_pin, 0); 
  dir = LEFT;
}

void setup(){
   pinMode(reed_l_pin, INPUT_PULLUP);
   pinMode(reed_r_pin, INPUT_PULLUP);

   pinMode(push_l_pin, INPUT_PULLUP);
   pinMode(push_r_pin, INPUT_PULLUP);

   attachInterrupt(digitalPinToInterrupt(push_l_pin), left_push_isr, RISING);
   attachInterrupt(digitalPinToInterrupt(push_r_pin), right_push_isr, RISING);
   
   ramp_up();
}

void loop(){
  switch(dir){
    case LEFT:
      if(digitalRead(reed_l_pin) == LOW)
      {
        ramp_down();
        dir = RIGHT;
        ramp_up();
      }
      break;

    case RIGHT:
      if(digitalRead(reed_r_pin) == LOW)
      {
        ramp_down();
        dir = LEFT;
        ramp_up();
      }
      break;

    default:
      analogWrite(pwm_l_pin, 0);
      analogWrite(pwm_r_pin, 0);
      break;
  }
}
