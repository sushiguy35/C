#include "cwl.h"

int CWL_INIT(int port, CWL_SERVER *serv)
{
    #ifdef __linux__
        serv->ListenFD = socket(AF_INET, SOCK_STREAM, 0);

        serv->addr.sin_family = AF_INET;
        serv->addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        serv->addr.sin_port = htons(port);

        bind(serv->ListenFD, (struct sockaddr*)&serv->addr, sizeof(serv->addr));
        listen(serv->ListenFD, 20);
    #endif
    serv->port = port;
    return CWL_SUCCESS_CODE;
}

int CWL_SETREQFUNC(int (*func)())
{
    func();
}

int CWL_CLOSE(CWL_SERVER *serv)
{
    
}