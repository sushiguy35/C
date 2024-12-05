#include "cwl.h"

int CWL_INIT(int port, CWL_SERVER *serv)
{
    #ifdef __linux__
        serv->LSockfd = socket(AF_INET, SOCK_STREAM, 0);
        serv->LSockfd = 
    #endif
    serv->port = port;
    return CWL_SUCCESS_CODE
}


int CWL_CLOSE(CWL_SERVER *serv)
{
    
}