#include "cwl.h"
#include <stdio.h>

void onReq()
{
    printf("it works");
    CWL_RESPONSE(1, "");
    return;
}

int main()
{
    CWL_SERVER server;

    CWL_INIT(80, &server);

    CWL_SETREQFUNC(onReq);

    CWL_CLOSE(&server);
}