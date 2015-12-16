#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include "joystick.h"

using namespace std;

#define PORT "3400"
#define TRUE 1
#define FALSE 0

int main(int argc, char *argv[])
{
    //Network variables
    addrinfo            hints;
    addrinfo *          result;
    int                 servfd;
    int                 clientfd;
    int                 numbytes;
    int                 yes;
    sockaddr_storage    their_addr;

    //Joystick pad data
    padData             pad;

    //Camera servo positions
    float               camera_x;
    float               camera_y;

    //Camera stream status
    char                camera_streaming;

    //Initialize Variables
    yes               = 1;
    camera_x          = 50.0f;
    camera_y          = 50.0f;
    camera_streaming  = FALSE;

    //Clear the hints struct
    memset(&hints, 0, sizeof hints);

    //Set up hints struct
    hints.ai_family = AF_UNSPEC; //ipv4 or ipv6, doesn't matter
    hints.ai_socktype = SOCK_STREAM; //use socket streams
    hints.ai_flags = AI_PASSIVE; //use my ip

    //Check for port number passed in on command line
    if( argc == 2 )
    {
        cout << "Starting on port " << argv[1] << "." << endl;
        getaddrinfo(NULL, argv[1], &hints, &result);
    }
    else
    {
        getaddrinfo(NULL, PORT, &hints, &result);
    }

    servfd = socket(result->ai_family,result->ai_socktype,result->ai_protocol);
    setsockopt(servfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

    bind(servfd, result->ai_addr, result->ai_addrlen);

    freeaddrinfo(result);
    listen(servfd, 10);

    cout << "Waiting for connections..." << endl;

    socklen_t addrsize = sizeof their_addr;

    clientfd = accept(servfd, (struct sockaddr*)&their_addr,&addrsize);

    char *their_ip = inet_ntoa(((sockaddr_in*)&their_addr)->sin_addr);

    cout << "Connection from " << their_ip << "!" << endl;

    numbytes = recv(clientfd,&pad,sizeof pad,0);

    while( numbytes == sizeof( pad ) )
    {
        //Loop variables
        char  servo_str[128];
        float axis_0;
        float axis_1;
        float axis_2;
        float axis_3;

        axis_0 = pad.aPos[0];
        axis_1 = pad.aPos[1];
        axis_2 = pad.aPos[2];
        axis_3 = pad.aPos[3];

        axis_0 = axis_0 / 32767.0f;
        axis_1 = axis_1 / 32767.0f;
        axis_2 = axis_2 / 32767.0f;
        axis_3 = axis_3 / 32767.0f;

        axis_0 = ( axis_0 * 50.0f ) + 50.0f;
        axis_1 = ( axis_1 * 20.0f ) + 50.0f;
        axis_2 = ( axis_2 * 50.0f ) + 50.0f;
        axis_3 = ( axis_3 * 20.0f ) + 50.0f;

        //Check D-pad to control camera pan/tilt
        if( pad.aPos[4] < 0 )
        {
            camera_x -= 0.5f;
            if( camera_x < 0.0f )
            {
                camera_x = 0.0f;
            }
        }
        else if( pad.aPos[4] > 0 )
        {
            camera_x += 0.5f;
            if( camera_x > 100.0f )
            {
                camera_x = 100.0f;
            }
        }

        if( pad.aPos[5] < 0 )
        {
            camera_y -= 0.5f;
            if( camera_y < 0.0f )
            {
                camera_y = 0.0f;
            }
        }
        else if( pad.aPos[5] > 0 )
        {
            camera_y += 0.5f;
            if( camera_y > 100.0f )
            {
                camera_y = 100.0f;
            }
        }

        //Pressing button 0 resets camera to center
        if( pad.bPos[0] != 0 )
        {
            camera_x = 50.0f;
            camera_y = 50.0f;
        }

        //Pressing button 1 starts video stream
        if( ( pad.bPos[1] != 0 ) && ( camera_streaming == FALSE ) )
        {
            //Fork process and call camera stream
            camera_streaming = TRUE;
            pid_t process;
            process = fork();

            if( process == 0 )
            {
                sprintf( servo_str, "sh `raspivid -t 0 -fps 25 -hf -vf -w 1280 -h 720 -b 10000000 -o - | nc -u %s 5001 &`", their_ip );
                system( servo_str );
            }
        }

        //Pressing button 2 kills video stream
        if( pad.bPos[2] != 0 )
        {
            system( "killall -9 raspivid" );
            camera_streaming = FALSE;
        }

        //Write out servo strings
        sprintf( servo_str, "echo 0=%d\% > /dev/servoblaster", (int)( 100 - axis_1 ) );
        system( servo_str );

        sprintf( servo_str, "echo 1=%d\% > /dev/servoblaster", (int)axis_3 );
        system( servo_str );

        sprintf( servo_str, "echo 3=%f\% > /dev/servoblaster", 100.0f - camera_x );
        system( servo_str );

        sprintf( servo_str, "echo 4=%f\% > /dev/servoblaster", 100.0f - camera_y );
        system( servo_str );

        //Receive the next packet
        numbytes = recv(clientfd,&pad,sizeof pad,0);
    }

    printf("numbytes = %d\r\n", numbytes);

    system( "echo 0=50\% > /dev/servoblaster" );
    system( "echo 1=50\% > /dev/servoblaster" );
    system( "echo 3=50\% > /dev/servoblaster" );
    system( "echo 4=50\% > /dev/servoblaster" );

    close(clientfd);
}
