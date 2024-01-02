# NimbleSimpleMotionExample

## Description
A simple example code base for the nimble control module with a bit of expansion on the main SDK which is also included here.

## Motion Library
- Motion library handles all the sin wave generation.
- Easing is handled automaticaly with the used only supplying speed/min/max values which can be updated at any time.
- Motion lib also counts strokes which can be accessed by the user.
- Texture is not yet implemented, this stroke pattern is similar to a venus or tremblr style.
- Speed is capped at 5hz by default but can be changed
- User supplied speed is supplied as a 0-100 value which is quadraticaly mapped to frequency. (A linear mapping would feel non linear when considered as stroke speed) 

## Also included
- Simple example of encoder handling
- Simple example of button handling
- Simple example of LED handling