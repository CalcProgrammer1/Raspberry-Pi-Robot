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
    string padName = "/dev/input/js0";

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

    initPad(pad,padName);
  
    while(1)
    {
        updatePad(pad);
        send(fd,&pad,sizeof pad, 0);
        usleep(50000);
    } 
    close(fd);

    return 0;
}
