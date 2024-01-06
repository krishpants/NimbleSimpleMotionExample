#ifndef NCMMotion_h
#define NCMMotion_h
#include <Arduino.h>

class NCMMotion {
public:
  NCMMotion(); // Constructor

  void begin(); // Initialize the library

  // Setters for values
  void setMinPosition(long position);
  void setMaxPosition(long position);
  void setSpeed(int speed);
  void setMaxFrequency(float maxHz);

  // Main functions
  void generateSineWave(const String& overlay = "");
  void easeToBasePosition();

  // Reset functions
  void resetLoopCounter();
  void resetFirstCall();

  // Getters
  int getLoopCount();
  bool isInBasePosition();
  long getPositionCommand();
  long timeSinceLastMovement();

private:
  // Set variables
  long targetMinPosition;
  long targetMaxPosition;
  float frequency; // Set After Converstion map 0-100 > 0 - maxFrequency HZ

  // Eased Variables
  long minPosition;
  long maxPosition;
  
  // Data Variables
  int loopCount;
  bool isFirstCall;
  bool inBasePosition;
  long positionCommand;
  long lastMovement;
  
  //Map Speed Variables
  float maxFrequency; // Maximum frequency in Hz

  // Helper functions
  float mapSpeedToFrequency(int speed);
  void easeMinMaxValues();

  //Vibration Overlay
  float calculateVibrationOverlay(const String& overlay, float phase, float timeElapsed); // Declaration of calculateVibrationOverlay
  float calculateVibrationWave(float vibrationAmplitude, float vibrationFrequency, float timeElapsed);
  bool phaseFallsBetweenClockHours(float phase, int start_clock, int end_clock);
  float convertHourToRadians(int hour_on_a_clock);
  float normalizeRadians(float radians);
  float interpolateValuesAcrossClockTimes(float phase, int start_hour, int end_hour, float minValue, float maxValue);
};

#endif
