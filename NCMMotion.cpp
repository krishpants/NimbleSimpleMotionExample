#include "NCMMotion.h"
#include <Arduino.h>

NCMMotion::NCMMotion() {
  loopCount = 0;
  isFirstCall = true;
  inBasePosition = false;
  positionCommand = 0;
  lastMovement = 0;
  setMaxFrequency(5);
}

void NCMMotion::begin() {
  // Initialize the library, if needed
}


//************************************************************
//********************** Setting Setters *********************
//************************************************************

void NCMMotion::setMinPosition(long position) {
  targetMinPosition = constrain(position, -1000, 1000);
}

void NCMMotion::setMaxPosition(long position) {
  targetMaxPosition = constrain(position, -1000, 1000);
}

void NCMMotion::setSpeed(int speed) {
  frequency = mapSpeedToFrequency(speed);
}

//***********************************************************
//********************** Main Functions *********************
//***********************************************************

void NCMMotion::generateSineWave(const String& overlay) {
  static float prevSineWave = 2.0; // Initialize to a value outside the sine wave range
  const float minThreshold = -0.99; // Threshold close to the minimum value
  static unsigned long lastGenUpdateTime;
  static float phase;

  unsigned long currentTime = millis();
  easeMinMaxValues();
  if (isFirstCall) {
    lastGenUpdateTime = millis();
    phase = 3 * PI / 2; // Set phase to 3PI/2 to start at the minimum position
    isFirstCall = false;
  }
  float timeElapsed = (currentTime - lastGenUpdateTime) / 1000.0; // Time in seconds
  lastGenUpdateTime = currentTime;
  lastMovement = currentTime;
  float phaseIncrement = 2 * PI * frequency * timeElapsed;
  phase += phaseIncrement;
  if (phase > 2 * PI) {
    phase -= 2 * PI; // Keep phase within a 0-2PI range
  }
  float sineVal = sin(phase);
  long output = minPosition + (sineVal + 1) * (maxPosition - minPosition) / 2;
  if (prevSineWave < minThreshold && sineVal >= minThreshold) {
    loopCount++;
  }
  prevSineWave = sineVal;
  // Get the vibration overlay value
  float vibrationOverlay = calculateVibrationOverlay(overlay, phase, timeElapsed);
  // Add the overlay to the primary wave output
  positionCommand = output + vibrationOverlay;
  // Clamp positionCommand to the range [-1000, 1000]
  positionCommand = max(static_cast<long int>(-1000), min(static_cast<long int>(1000), positionCommand));
}

void NCMMotion::easeToBasePosition() {
  minPosition = targetMinPosition;
  maxPosition = targetMaxPosition;
  static unsigned long lastUpdateTime = 0; // Static to maintain state across function calls
  unsigned long updateInterval = 10; // Time interval in ms between position updates
  int maxTravelDistance = abs(maxPosition - minPosition);
  int stepSize = maxTravelDistance / (1000 / updateInterval); // Calculate step size
  // Ensure step size is at least 1
  if (stepSize < 1) stepSize = 1;
  if (millis() - lastUpdateTime > updateInterval) {
      inBasePosition = false;
      if (positionCommand > minPosition + 10) {
          // Decrease positionCommand if it is more than 10 above minPosition (added some lee way to avoid bouncing)
          positionCommand -= stepSize;
          if (positionCommand < minPosition) {
              positionCommand = minPosition; // Ensure not to go below minPosition
          }
      } else if (positionCommand < minPosition - 10) {
          // Increase positionCommand if it is more than 10 below minPosition (added some lee way to avoid bouncing)
          positionCommand += stepSize;
          if (positionCommand > minPosition) {
              positionCommand = minPosition; // Ensure not to go above minPosition
          }
      } else {
        inBasePosition = true;
      }
      lastUpdateTime = millis(); // Update the last update time
  }
}

//**************************************************************
//********************** Vibration Modes ***********************
//**************************************************************

float NCMMotion::calculateVibrationOverlay(const String& overlay, float phase, float timeElapsed) {
    
    // A simple strong vibration
    if (overlay == "strong") {
          // Vibration calculation within the specified phase range
          float vibrationAmplitude = 200;
          float vibrationFrequency = 50;
          return calculateVibrationWave(vibrationAmplitude, vibrationFrequency, timeElapsed);
    }

    // A more advanced modulated purr at the end of the down stroke
    if (overlay == "purr") {
        // Helper function for better visualisation of stroke maped to a clock. Only vibrating briefly between 5 & 7
        if (phaseFallsBetweenClockHours(phase,5,7)) {
            // Vibration calculation within the specified phase range
            float vibrationAmplitude = 100;
            float maxFrequency = 50;
            float minFrequency = 5;
            float currentFrequency = minFrequency; // Default to minFrequency

            // Create purr effect by reducing frequency.
            // Helper function interpolates 2 values between clock positions 
            currentFrequency = interpolateValuesAcrossClockTimes(phase,5,7,minFrequency,maxFrequency);

            // Return the vibration wave
            return calculateVibrationWave(vibrationAmplitude, currentFrequency, timeElapsed);
        }
    }

    return 0; // No vibration outside the specified phase range, or for unknown overlay
}

//********************************************************
//********************** Get & Reset *********************
//********************************************************

int NCMMotion::getLoopCount() {
  return loopCount;
}

long NCMMotion::getPositionCommand() {
  return positionCommand;
}

bool NCMMotion::isInBasePosition() {
  return inBasePosition;
}

long NCMMotion::timeSinceLastMovement() {
  //lastMovement is updated every cycle of generateSineWave, timeSinceLastMovement can be used to track how long we have been paused
  return millis() - lastMovement;
}

void NCMMotion::resetLoopCounter() {
  loopCount = 0;
}

void NCMMotion::resetFirstCall() {
  isFirstCall = true;
}

//**************************************************************
//********************** Frequency Helpers *********************
//**************************************************************

float NCMMotion::mapSpeedToFrequency(int speed) {
    if (speed <= 0) {
        return 0.0; // Off
    } else if (speed >= 100) {
        return maxFrequency; // Maximum frequency
    } else {
        // Quadratic mapping using user-specified coefficients
        float rtn_speed = (0.0001 * maxFrequency) * speed * speed;
        return rtn_speed < 0.1 ? 0.1 : rtn_speed;
    }
}

void NCMMotion::easeMinMaxValues() {
    static int lastEaseTime = 0;
    int easeInterval = 10; //Ease Every 10ms
    if(millis() - lastEaseTime >= easeInterval) {
        minPosition += (targetMinPosition > minPosition) ? 10 : (targetMinPosition < minPosition) ? -10 : 0;
        maxPosition += (targetMaxPosition > maxPosition) ? 10 : (targetMaxPosition < maxPosition) ? -10 : 0;
        lastEaseTime = millis();
    }
}

void NCMMotion::setMaxFrequency(float maxHz) {
    if (maxHz > 0.0) {
        maxFrequency = maxHz;
    }
}

//**************************************************************
//********************** Vibration Helpers *********************
//**************************************************************

float NCMMotion::calculateVibrationWave(float vibrationAmplitude, float vibrationFrequency, float timeElapsed) {
    // Calculate the phase increment for the vibration
    float vibrationPhaseIncrement = 2 * PI * vibrationFrequency * timeElapsed;
    static float vibrationPhase = 0;
    vibrationPhase += vibrationPhaseIncrement;
    if (vibrationPhase > 2 * PI) {
        vibrationPhase -= 2 * PI; // Normalize phase
    }
    return vibrationAmplitude * sin(vibrationPhase);
}

// This function helps visualise the stroke phase as the hours on a clock where 6 is the bottom of each stroke
bool NCMMotion::phaseFallsBetweenClockHours(float radian_value, int start_clock, int end_clock) {
    if (start_clock == end_clock) {
        return true;
    }
    float start_rad = convertHourToRadians(start_clock);
    float end_rad = convertHourToRadians(end_clock);
    if (start_rad <= end_rad) {
        return radian_value >= start_rad && radian_value <= end_rad;
    } else {
        return radian_value >= start_rad || radian_value <= end_rad;
    }
}

float NCMMotion::convertHourToRadians(int hour_on_a_clock) {
    return (2 * PI / 12) * ((hour_on_a_clock - 9) % 12);
}

float NCMMotion::interpolateValuesAcrossClockTimes(float phase, int start_hour, int end_hour, float minValue, float maxValue) {
    float start_rad = normalizeRadians(convertHourToRadians(start_hour));
    float end_rad = normalizeRadians(convertHourToRadians(end_hour));
    phase = normalizeRadians(phase);
    // Adjust for wrap-around
    if (start_rad > end_rad) {
        if (phase < end_rad) phase += 2 * PI;
        end_rad += 2 * PI;
    }
    float interpolationFactor = (phase - start_rad) / (end_rad - start_rad);
    // Apply interpolation factor to interpolate between min and max values supplied
    return maxValue - interpolationFactor * (maxValue - minValue);
}

float NCMMotion::normalizeRadians(float radians) {
    while (radians < 0) radians += 2 * PI;
    while (radians >= 2 * PI) radians -= 2 * PI;
    return radians;
}


//************************************************
//********************** End *********************
//************************************************
