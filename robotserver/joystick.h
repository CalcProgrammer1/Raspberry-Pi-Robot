/*-------------------------------------*\
|  Joystick Functions			|
|  Provides common functions to access	|
|  joysticks and gamepads with a simple	|
|  set of commands			|
\*-------------------------------------*/

#include <iostream>
#include <linux/joystick.h>
#include <string>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#define MAX_AXIS 16
#define MAX_BUTTONS 32

using namespace std;

struct padData
{
  unsigned char axisCount;
  unsigned char buttonCount;
  char padname[80];
  int fd;
  int aPos[MAX_AXIS];
  int bPos[MAX_BUTTONS];
  bool changed;
  js_event ev;
};

//Initialize Pad
//You must provide a padData to open and the name of the device
//Returns the file descriptor of the pad if successful
//Returns -1 if unsuccessful
int initPad(padData &pad, string dev);

//Update Pad
//You must provide a padData to update
void updatePad(padData &pad);
