/*
 *
 */

const int PWM_PIN    = 3;
const int UP_BTN_PIN = 5;
const int DN_BTN_PIN = 6;

const int V_HIGH = 255;
const int V_LOW  = 170;

const int DEBOUNCE = 50;

int up_state = LOW;
int dn_state = LOW;
int up_last;
int dn_last;
int up_check = FALSE;
int dn_check = FALSE;

int v_state = 0;
int v_out = V_LOW;

void setup() {
  pinMode(PWM_PIN, OUTPUT);
  pinMode(UP_BTN_PIN, INPUT);
  pinMode(DN_BTN_PIN, INPUT);
}

void loop() {
  analogWrite(PWM_PIN, v_out);
  
  checkUp();
  checkDown();
  
}

void checkUp(){
  
  int cur_state = digitalRead(UP_BTN_PIN);
  if(cur_state == HIGH){
    if(up_check == TRUE){
      if(up_last+DEBOUNCE < millis()){
        inc_v_state();
        update_v_out();
      }
    }
    else{
      up_check = TRUE;
      up_last = millis();
    }
  }
  else{
    if(up_check == TRUE) up_check = FALSE;
  }
  
}

void checkDown(){
  
  int cur_state = digitalRead(DN_BTN_PIN);
  if(cur_state == HIGH){
    if(dn_check == TRUE){
      if(dn_last+DEBOUNCE < millis()){
        dec_v_state();
        update_v_out();
      }
    }
    else{
      dn_check = TRUE;
      dn_last = millis();
    }
  }
  else{
    if(dn_check == TRUE) dn_check = FALSE;
  }
  
}

void inc_v_state(){
  if(v_state < 8) v_state++;
}

void dec_v_state(){
  if(v_state > 0) v_state--;
}

void update_v_out(){
  v_out = V_LOW + (v_state * (V_HIGH-V_LOW))/8;
}
