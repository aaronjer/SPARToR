
#ifndef SPARTOR_INPUT_H_
#define SPARTOR_INPUT_H_

#include "SDL.h"
#include "SDL_net.h"


#define INP_KEYB 1 //keyboard
#define INP_JBUT 2 //joystick button
#define INP_JAXP 3 //joystick axis, positive
#define INP_JAXN 4 //joystick axis, negative


void kbinput(int press,SDL_keysym keysym);
void joyinput(int press,SDL_JoyButtonEvent jbutton);
void axisinput(SDL_JoyAxisEvent jaxis);
void readinput();

#endif

