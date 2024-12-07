#include "cwl.h"
#include <stdio.h>

int main()
{
    if (CWL_INIT(8080) != CWL_SUCCESS_CODE) {
        printf("Failed to initialize server.\n");
        return 1;
    }

    // This would run indefinitely to accept connections
    // You can modify the test to use `CWL_SETREQFUNC` and test your handlers.
    
    return 0;
}




/*
    how it should be:
    
    #include "cwl.h"

    int main(void)
    {
        CWL_INIT(80);
        
    }

*/