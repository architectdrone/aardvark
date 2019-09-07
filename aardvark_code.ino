/*
 * AARDVARK - A program for controlling a voltage regulator circuit.
 * Made for JEDL Project 1
 */
using namespace std;

void setup() {
  // put your setup code here, to run once:

}

void sendMessage(String  msg){
  /*
   * Sends a message to a given COM port via the USB. If no USB is plugged in, this function does nothing.
   */
}

void setVoltage(float voltage) {
  /*
   * Sets the voltage on the regulator. Must be in the range 8-12 volts.
   */
}

float getCurrent(){
  /*
   * Gets the current drawn by the active load, in mA.
   */
   
   return 0.0;
}

float getActualVoltage() {
  /*
   * Gets the voltage that is actually across the active load.
   */

   return 0.0;
}

float getDesiredVoltage() {
  /*
   * Gets the voltage that the user wants to have across the load.
   */

   return 0.0;
}

void loop() {
  // put your main code here, to run repeatedly:
  float desiredVoltage = getDesiredVoltage();
  float actualVoltage  = getActualVoltage();
  float current        = getCurrent();

  String msg = "DESIRED: "+ String(desiredVoltage)+ "V ACTUAL: "+ String(actualVoltage)+ "V DRAW: "+ String(current)+ "mA";
  sendMessage(msg);

  setVoltage(desiredVoltage);
  
}
