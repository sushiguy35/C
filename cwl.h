#ifndef CWL_H
#define CWL_H

#ifdef __WIN32

// TODO, FIX IT LATER

#elif __APPLE__

// TODO, FIX IT LATER

#elif __linux__

#include <sys/socket.h>

#endif


// Error codes
#define CWL_SUCCESS_CODE 0
#define CWL_ERROR 1

// Server Structure
typedef struct {
    #ifdef __linux__
    // Listening socket
    int LSockfd;
    // Connected socket
    int CSockfd;
    #endif
    int port;
} CWL_SERVER;

int CWL_INIT(int port, CWL_SERVER *serv);
int CWL_CLOSE(CWL_SERVER *serv);

#endif