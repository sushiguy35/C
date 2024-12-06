#include "cwl.h"
#include <stdio.h>

void onReq()
{
    CWL_RESPONSE(1, "IT WORKS");
    return;
}

int main()
{
    CWL_SERVER server;

    CWL_INIT(6969, &server);
while (1)
    CWL_SETREQFUNC(onReq);

    CWL_CLOSE(&server);
}