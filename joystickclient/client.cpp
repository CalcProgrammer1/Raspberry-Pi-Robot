#include <cstdlib>
#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <pthread.h>
#include "joystick.h"

using namespace std;

#define PORT "3400"
#define TRUE 1
#define FALSE 0

void * open_camera( void *arg )
{
    system( "nc -lu -p 5001 | mplayer -fps 40 -demuxer h264es -" );
}

int main(int argc, char *argv[])
{
    //Network variables
    addrinfo            hints;
    addrinfo *          result;
    int                 fd;
    int                 yes;
    sockaddr_storage    serv_addr;

    //Joystick pad data
    padData pad;

    //Joystick device name
    string padName = "/dev/input/js0";

    //Camera stream status
    char                camera_streaming;

    //Camera streaming thread ID
    pthread_t			thread_id;

    //Initialize Variables
    yes               = 1;
    camera_streaming  = FALSE;

    //Clear the hints struct
    memset(&hints, 0, sizeof hints);

    //Set up hints struct
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    //Check for port number passed in on command line
    if( argc == 3 )
    {
        getaddrinfo(argv[1], argv[2], &hints, &result);
    }
    else
    {
        getaddrinfo(argv[1], PORT, &hints, &result);
    }

    fd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

    connect(fd, result->ai_addr, result->ai_addrlen);

    //Initialize joystick
    initPad(pad,padName);

    while( 1 )
    {
        //Update joystick data
        updatePad(pad);

        //Pressing button 1 starts video stream
        if( ( pad.bPos[1] != 0 ) && ( camera_streaming == FALSE ) )
        {
            //Start camera streaming thread
            int error = 0;
            error = pthread_create( &thread_id, NULL, &open_camera, NULL );

            if( error != 0 )
            {
                camera_streaming = FALSE;
                cout << "Error opening camera" << endl;
            }
            else
            {
                camera_streaming = TRUE;
                cout << "Camera thread started" << endl;
            }
        }

        //Pressing button 2 kills video stream
        if( pad.bPos[2] != 0 )
        {
            system( "killall -9 mplayer" );
            camera_streaming = FALSE;
        }

        //Send joystick data to server
        send(fd,&pad,sizeof pad, 0);

        //Delay loop
        usleep(50000);
    }

    close(fd);

    return 0;
}
