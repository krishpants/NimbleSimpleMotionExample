# NimbleSimpleMotionExample

## Description
A simple example code base for the nimble control module with a bit of expansion on the main SDK which is also included here.

## Motion Library
- Motion library handles all the sin wave generation.
- Easing is handled automaticaly with the used only supplying speed/min/max values which can be updated at any time.
- Motion lib also counts strokes which can be accessed by the user.
- Speed is capped at 5hz by default but can be changed
- User supplied speed is supplied as a 0-100 value which is quadraticaly mapped to frequency. (A linear mapping would feel non linear when considered as stroke speed) 

## Also included
- Simple example of encoder handling
- Simple example of button handling
- Simple example of LED handling

## Texture
Some early experiments with texture (vibration) are included.
Vibration is addes by calling generateSineWave('vibrationModeString')
Included vibrations are:

generateSineWave('strong') - a strong constant vibration
generateSineWave('purr') - a vibration with a freqency fall off, at the bottom part of the stroke

## Clock Analogy
- To help visualise the stroke pattern, a set of clock based helper functions are included
- These convert the phase of the stroke to a clock value with 6 being the bottom of the down stroke.
- This allows you to stipulate where certain actions will happen.
- eg: the 'purr' vibration pattern happens between 5 & 7 o' clock.


