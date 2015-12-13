/*-------------------------------------*\
|  Joystick Functions			|
|  Provides common functions to access	|
|  joysticks and gamepads with a simple	|
|  set of commands			|
\*-------------------------------------*/

#include "joystick.h"

int initPad(padData &pad, string dev)
{
  memset(&pad, 0x00, sizeof(pad) );
  pad.fd = open(dev.c_str(), O_RDONLY);
  
  if(pad.fd <= 0)
  {
    cout << "Could not open " << dev << endl;
  }
  else
  {
    ioctl(pad.fd, JSIOCGAXES, &pad.axisCount);
    ioctl(pad.fd, JSIOCGBUTTONS, &pad.buttonCount);
    fcntl(pad.fd, F_SETFL, O_NONBLOCK);
    ioctl(pad.fd, JSIOCGNAME(80), pad.padname);
  }
  return pad.fd;
}

void updatePad(padData &pad)
{
  pad.changed = false;
  for(int i = 0; i < 100; i++)
  {
    int result = read(pad.fd, &pad.ev, sizeof(pad.ev));
    if(result > 0)
    {
      if( pad.ev.type == JS_EVENT_AXIS)
      {
        pad.aPos[pad.ev.number] = pad.ev.value;
        pad.changed = true;
      }
      else if (pad.ev.type == JS_EVENT_BUTTON)
      {
        pad.bPos[pad.ev.number] = pad.ev.value;
        pad.changed = true;
      }
    }
  }
}
