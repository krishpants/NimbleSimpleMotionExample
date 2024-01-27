// Use "ESP32 Dev Module" as board
#include "NCMMotion.h"
// This goes last or will error
#include "nimbleCon.h"
#include <Arduino.h>

// Make an instance of Widget's motion lib
NCMMotion motion;

// The only global var used in this example code
int runningStage = 0;

void setup() {
  // Setup runs once on boot
  initNimbleSDK();
  // Set in hz of the max speed value of 100(%)
  // wave freqncy will be then mapped from 0 - MAX hz, for values 0-100
  motion.setMaxFrequency(5);//HZ
  Serial.begin(115200);
  delay(500);
}


void loop() {
  // Loop goes round and round!
  runMainOperation(); // Example
  doSomeLedStuff(); // Example
  checkPhysicalInputs(); // Example
  sendValuesToNimble(); // Important!
}

// Do not call! For reference only!
// These are the main functions, once you understand how they work you can effectivly delete everything bellow and start your own project.
// !!! EXCEPT sendValuesToNimble(); < Leave that in

void exampleLibraryFunctions(){
  return;
  // Set Motion Variables
  motion.setMinPosition(-200); // -1000 to 1000
  motion.setMaxPosition(800); // -1000 to 1000
  motion.setSpeed(60); // 0 - 100
  // Call continiously to ease back to base
  motion.easeToBasePosition();
  // Reset loop counter to 0;
  motion.resetLoopCounter();
  // Call immediatly before starting motion after any pause
  motion.resetFirstCall();
  // Call continiously to generate a sin wave with set perametrs
  motion.generateSineWave();
  // Get the loop count
  motion.getLoopCount();
  // Get the millis value since generateSineWave(); was last called (pause time)
  motion.timeSinceLastMovement();
  // Called in setup(), this is the max hz of motion lib
  motion.setMaxFrequency(5);
}

// ***************************************************
// ****************** Example ************************
// ***************************************************

// This example loops round and round a case statement. Performing a simple run/pause sequence, case 1-4 cucle automaticaly, 0 is stopped.
// IMPORTANT: The button code bellow toggles running stage between 0 and 1 on push as an example

void runMainOperation() {
  switch (runningStage) {
    case 0:
      // This is effectivly the pause state, motion.easeToBasePosition() is being continuously called to move the reciever back to fully sucked on if it is paused in a half cycle.
      // When you want to start, you would change running stage to 1 to move to the next step.
      // When you want to pause, you would change running stage back to 0;
      // You might do this using a button push as in the example code later on.
      motion.easeToBasePosition();

      break;
    case 1:
      // These are the 3 commands used to change the stroke perameters. They can be changed at any time. The library will automaticaly ease between values to prevent jumpy changes.
      // This step sets the stroke peramteters and then moves directly on.
      // You might link speed to the encoder, or perhaps randomise values here.
      motion.setMinPosition(-200); // -1000 to 1000
      motion.setMaxPosition(800); // -1000 to 1000
      motion.setSpeed(30); // 0 - 100
      runningStage ++;
      break;
    case 2:
      // This step advances automaticaly when reciever is in base position
      motion.easeToBasePosition();
      if (motion.isInBasePosition()){
        // Optionaly reset the loop counter here if you want back to 0
        motion.resetLoopCounter();
        // !! it is imporant to call motion.resetFirstCall() when the actuator has been paused for any period of time. this it to make sure the sin wave starts in a correct position. 
        // not calling this after a pause will resunt in a jumpy start.
        motion.resetFirstCall();
        runningStage ++;
      }
      break;
    case 3:
      // Here is where the motion is generated.
      // generateSineWave() constantly updates the position in a sin wave, this is then extracted in sendValuesToNimble() and sent to the actuator
      // Check if the loop count is even or odd
      // if (motion.getLoopCount() % 2 == 0) {
      //     // On even loop counts, apply the "purr" texture overlay
      //     motion.generateSineWave("purr");
      // } else {
      //     // On odd loop counts, don't apply any overlay
      //     motion.generateSineWave();
      // }
      motion.generateSineWave("purr");
      // The library keeps a loop count that increments at the bottom of every stroke.
      // In this example we wait until the counter gets to 20 and then move to the next step.
      // because the counter updates at the bottom of the stroke, moving on as soon as it changes stopped the unit in an optimal posiiton to prevent fall off.
      if (motion.getLoopCount() > 5){
        runningStage ++;
      }
      break;
    case 4:
      // for safety we still call easeToBasePosition() here to make sure the reciever moves to its propper base position.
      // due to the reasons above, this techncialy should not be neccecary.
      motion.easeToBasePosition();
      // lastMovement is only updated in generateSineWave(), as such we can use motion.timeSinceLastMovement() to see how long we have been paused for.
      // in this example we wait 10000ms (10 seconds) before settign running stage back to 1 to start the cycle again.
      if (motion.timeSinceLastMovement() >= 10000){
        runningStage = 1;
      }
      break;
    default:
      break;
  }
}


// ********************************************************
// ****************** Input Controlls *********************
// ********************************************************

void functionForButtonPushed(){
 // Will run once on push
  if (runningStage == 0){
    runningStage = 1;
  } else {
    runningStage = 0;
  }
}

void functionForButtonReleased(){
  // Will run once on release
}

void functionForEncoderClockwise(){
  // Will run once on clockwise encoder click
}

void functionForEncoderAntiClockwise(){
  // Will run once on anti clockwise encoder click
}

// ****************************************************************
// ****************** Input Controlls Helpers *********************
// ****************************************************************

void checkPhysicalInputs(){
  checkEncoderForInput(); 
  checkButtonForInput(); 
}

void checkButtonForInput() {
  static int lastButtonState = HIGH;
  static int buttonValue = 0;
  static unsigned long lastDebounceTime = 0;
  // Low when pushed, High when released
  int reading = digitalRead(ENC_BUTT);
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > 50) {
    // Update button value only if it has been stable for debounceDelay time
    if (reading != buttonValue) {
      buttonValue = reading;
      if (buttonValue == 0){
        // button has been released < Do Something here
        functionForButtonPushed();
      } else {
        // button has been pushed < Do Something here
        functionForButtonReleased();
      }
    }
  }
  lastButtonState = reading;
}

void checkEncoderForInput() {
    // Monitor the encoder for clicks
    static unsigned long lastUpdateTime = 0;
    static unsigned long lastEncoderPosition = 0;
    long currentEncoderPosition = encoder.getCount();
    if (millis() - lastUpdateTime > 50) {  // 50ms debounce interval
        if (currentEncoderPosition > lastEncoderPosition) {
            // Encoder has clicked once cockwise < Do Something here
            functionForEncoderClockwise();
        } else if (currentEncoderPosition < lastEncoderPosition) {
            // Encoder has clicked once anti-cockwise < Do Something here
            functionForEncoderAntiClockwise();
        }
        lastEncoderPosition = currentEncoderPosition;
        lastUpdateTime = millis();
    }
}


// An example of how to play with the leds
void doSomeLedStuff(){
  // Do some things with the lights!

  // Set various lights to 50 brightness (254 max) under various conditions.
  int airInValue = pendant.airIn ? 50 : 0;
  int airOutValue = pendant.airOut ? 50 : 0;
  int runningValue = runningStage > 0 ? 50 : 0;
  ledcWrite(2, runningValue);
  ledcWrite(5, airInValue);
  ledcWrite(7, airOutValue);


  pendant.present ? ledcWrite(PEND_LED, 50) : ledcWrite(PEND_LED, 0);  // Display pendant connection status on LED.
  actuator.present ? ledcWrite(ACT_LED, 50) : ledcWrite(ACT_LED, 0);  // Display actuator connection status on LED.
}



// you should not need to touch anything here unless you want to do more advanced things.
void sendValuesToNimble(){
  // this function sets values to the actuator construct...
  readFromPend(); // Used to get pendant values like airIn so they can be passed through
  actuator.forceCommand = 1023; // Leave it as it is normaly
  if (runningStage == 0){
    actuator.forceCommand = 300;
  }
  actuator.positionCommand = motion.getPositionCommand(); // Get postion from motion library
  actuator.airIn = pendant.airIn; // Pass thhrough pendant values
  actuator.airOut = pendant.airOut; // Pass thhrough pendant values
  //... and the sends them to the actuator at a fixed interval.
  if(checkTimer()) sendToAct();
}
