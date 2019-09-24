/*
 * AARDVARK - A program for controlling a voltage regulator circuit.
 * Made for JEDL Project 1
 */

#include <EEPROM.h>

/**
 * location to store voltage state info in
 */
const int STATE_ADDR = 34;

/**
 * pin definitions
 */
const int PWM_PIN    = 3;
const int V_READ_PIN = A7;
const int I_READ_PIN = A5;
const int STATE_PIN  = A3;

/**
 * voltage range definitions
 */
const float V_MAX = 12.0;
const float V_MIN = 8.0;

/**
 * resistance values
 */
const int  R_VOUT_UPPER = 5050; // 4560;
const int  R_VOUT_LOWER = 3260; // 3243;
const int  R_SC         = 940; // in milli-ohms
const long R_IOUT_UPPER = 509000L;
const long R_IOUT_LOWER = 326000L;

/**
 * calibrating the arduino (it outputs different voltages when plugged into the computer or not)
 */

float ARDUINO_OUT;
int   calibration_count = 0;

/**
 * PWM range definitions
 */
int PWM_MAX;
int PWM_MIN;

/**
 * variables for keeping track of actual voltages, for averaging
 */
const int READ_COUNT = 20;
int       v_reads[READ_COUNT];
int       i_reads[READ_COUNT];
int       v_sum;
int       i_sum;
int       read_loc = 1;

/**
 * variables for keeping track of the intended voltage output
 */
float desired;
float pwm_out;

void calibrate() {
  /**
   * calibrate arduino outputs based on voltage from 3.3V pin
   */
  int reference = analogRead(STATE_PIN);
  ARDUINO_OUT = (3.3 * 1023) / reference;
  PWM_MAX = (V_MAX * R_VOUT_LOWER * 255) / ((R_VOUT_UPPER + R_VOUT_LOWER) * ARDUINO_OUT);
  PWM_MIN = (V_MIN * R_VOUT_LOWER * 255) / ((R_VOUT_UPPER + R_VOUT_LOWER) * ARDUINO_OUT);
  calibration_count = 0;
}

void setup() {
  /**
   * setup serial connection and pwm output
   */
  Serial.begin(9600); // starts the serial (over USB) connection
  pinMode(PWM_PIN, OUTPUT);
  
  // calibrate arduino based on 3.3V pin
  calibrate();
  
  // read saved voltage state from EEPROM and set initial voltage
  float desired = static_cast<float>(EEPROM.read(STATE_ADDR))/2;
  if(8 <= desired && desired <= 12) setDesired(desired);
  else                              setDesired(8.0);
  
  // setup voltage/current averaging tables
  for(int i = 1; i < READ_COUNT; i++){
    v_reads[i] = 0;
    i_reads[i] = 0;
  }
  v_reads[0] = analogRead(V_READ_PIN);
  i_reads[0] = analogRead(I_READ_PIN);
  v_sum = v_reads[0];
  i_sum = i_reads[0];
}

void setDesired(float voltage){
  /*
   * Sets the voltage on the regulator. Must be in the range 8-12 volts.
   */
   
  desired = voltage;
  pwm_out = PWM_MIN + ((desired - V_MIN) * (PWM_MAX - PWM_MIN)) / (V_MAX - V_MIN);
  EEPROM.update(STATE_ADDR, static_cast<byte>(desired*2));
  
}

float getCurrent(){
  /*
   * Gets the current drawn by the active load, in mA.
   */
   
  float measured = (ARDUINO_OUT * i_sum) / (1023 * READ_COUNT);
  float ref_voltage = (measured * (R_IOUT_UPPER + R_IOUT_LOWER)) / R_IOUT_LOWER;
  float actual_voltage = getActualVoltage();
  float total_current = 1000000 * (ref_voltage - actual_voltage) / R_SC;
  return total_current - (1000 * actual_voltage / (R_VOUT_UPPER + R_VOUT_LOWER));
  
}

float getActualVoltage(){
  /*
   * Gets the voltage that is actually across the active load.
   */

  float measured = (ARDUINO_OUT * v_sum) / (1023 * READ_COUNT);
  return (measured * (R_VOUT_UPPER + R_VOUT_LOWER)) / R_VOUT_LOWER;
  
}

void printStatus(){
  
  String msg = "DESIRED: "+ String(desired)+ "V ACTUAL: "+ String(getActualVoltage())+ "V DRAW: "+ String(getCurrent())+ "mA";
  Serial.println(msg);
  
}

String readStr;

void loop() {

  // set pwm duty cycle
  analogWrite(PWM_PIN, pwm_out);

  // read voltage and current
  v_sum -= v_reads[read_loc];
  v_reads[read_loc] = analogRead(V_READ_PIN);
  v_sum += v_reads[read_loc];
  i_sum -= i_reads[read_loc];
  i_reads[read_loc] = analogRead(I_READ_PIN);
  i_sum += i_reads[read_loc];
  read_loc = (read_loc+1)%READ_COUNT;

  // check if enough time has elapsed to calibrate
  calibration_count++;
  if(!(calibration_count%2000)) calibrate();

  // check serial bus for command
  if(Serial.available()){
    readStr = Serial.readString();

    while(readStr.length() > 0){
      if(readStr.startsWith("read")){
        printStatus();
        readStr.remove(0, 4);
      }
      else if(readStr.startsWith("set")){
        readStr.remove(0, 3);
        readStr.trim();
        float v_set = readStr.toFloat();
        if(v_set >= 8 && v_set <= 12){
          v_set = static_cast<float>(static_cast<int>(v_set*2))/2;
          setDesired(v_set);
        }
        else{
          Serial.println("Desired voltage must be between 8 and 12 volts.");
        }
        printStatus();
      }
      else{
        readStr.remove(0, 1);
      }
      
    }
  }
}
