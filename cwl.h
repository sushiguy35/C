//----------------------------------------------------------------------------
//              CWL - A C Web Library
//  CWL is a basic c library to make websites with C backend code
//  Maker: Sushiguy35. May be used or changed by anyone if you give me credits 
//----------------------------------------------------------------------------



#ifndef CWL_H
#define CWL_H

#ifdef __WIN32

// TODO, FIX IT LATER

#elif __APPLE__

// TODO, FIX IT LATER

#elif __linux__

#include <sys/socket.h>
#include <netinet/in.h>

#endif

#include <stdlib.h>
#include <stdio.h>

// Error codes
#define CWL_SUCCESS_CODE 0
#define CWL_ERROR 1

// Server Structure
typedef struct {
    #ifdef __linux__
    int sockfd;

    // server Address
    struct sockaddr_in addr;
    // Length of server address
    int addrLen;

    #endif
    int port;
} CWL_SERVER;

int CWL_INIT(int port);
int CWL_CLOSE(CWL_SERVER *serv);
int CWL_RESPONSE(int opt, char *html);
int CWL_SETREQFUNC(int (*func)());

#endif