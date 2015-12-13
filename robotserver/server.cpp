#include <cstdlib>
#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "joystick.h"

using namespace std;

#define PORT "3400"

int main(int argc, char *argv[])
{
    padData pad;
    float camera_x = 50.0f;
    float camera_y = 50.0f;

    addrinfo hints, *result;
    int servfd, clientfd;
    sockaddr_storage their_addr;
    int yes = 1;
    //Clear the hints struct
    memset(&hints, 0, sizeof hints);

    //Set up hints struct
    hints.ai_family = AF_UNSPEC; //ipv4 or ipv6, doesn't matter
    hints.ai_socktype = SOCK_STREAM; //use socket streams
    hints.ai_flags = AI_PASSIVE; //use my ip

    getaddrinfo(NULL, PORT, &hints, &result);

    servfd = socket(result->ai_family,result->ai_socktype,result->ai_protocol);
    setsockopt(servfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

    bind(servfd, result->ai_addr, result->ai_addrlen);

    freeaddrinfo(result);
    listen(servfd, 10);

    cout << "Waiting for connections..." << endl;

    socklen_t addrsize = sizeof their_addr;

    clientfd = accept(servfd, (struct sockaddr*)&their_addr,&addrsize);

    cout << "Connected!" << endl;

    int numbytes = recv(clientfd,&pad,sizeof pad,0);

    while(numbytes == sizeof(pad))
    {
        char  servo_str[128];
        float axis_0;
        float axis_1;
        float axis_2;
        float axis_3;

        int   axis_4;
        int   axis_5;

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

        if( pad.bPos[0] != 0 )
        {
            camera_x = 50.0f;
            camera_y = 50.0f;
        }

       // cout << axis_0 << "		" << axis_1 << "		" << axis_2 << "		" << axis_3 << endl;

        sprintf( servo_str, "echo 0=%d\% > /dev/servoblaster", (int)( 100 - axis_1 ) );
        system( servo_str );

        sprintf( servo_str, "echo 1=%d\% > /dev/servoblaster", (int)axis_3 );
        system( servo_str );

        sprintf( servo_str, "echo 3=%f\% > /dev/servoblaster", 100.0f - camera_x );
        system( servo_str );

        sprintf( servo_str, "echo 4=%f\% > /dev/servoblaster", 100.0f - camera_y );
        system( servo_str );

        numbytes = recv(clientfd,&pad,sizeof pad,0);
    }

    close(clientfd);
}



#if 0
  addrinfo hints, *result;
  int fd;

  sockaddr_storage serv_addr;
  int yes = 1;
  
  memset(&hints, 0, sizeof hints);

  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  getaddrinfo(argv[1], PORT, &hints, &result);

  fd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
  
  connect(fd, result->ai_addr, result->ai_addrlen);

  int numbytes = 0;

  numbytes = recv(fd,&pad,sizeof pad,0);

  while(numbytes > 0)
  {
    if(numbytes > 0)
    {
      char  servo_str[128];
      float axis_0;
      float axis_1;
      float axis_2;
      float axis_3;

      int   axis_4;
      int   axis_5;

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

      if( pad.bPos[0] != 0 )
      {
        camera_x = 50.0f;
        camera_y = 50.0f;
      }

      //cout << axis_0 << "		" << axis_1 << "		" << axis_2 << "		" << axis_3 << endl;

      sprintf( servo_str, "echo 0=%d\% > /dev/servoblaster", (int)( 100 - axis_1 ) );
      system( servo_str );

      sprintf( servo_str, "echo 1=%d\% > /dev/servoblaster", (int)axis_3 );
      system( servo_str );

      sprintf( servo_str, "echo 2=%f\% > /dev/servoblaster", 100.0f - camera_x );
      system( servo_str );

      sprintf( servo_str, "echo 3=%f\% > /dev/servoblaster", 100.0f - camera_y );
      system( servo_str );


      usleep(25000);
    }
    numbytes = recv(fd,&pad,sizeof pad,0);
  }
  close(fd);
  return 0;
}

  #endif
