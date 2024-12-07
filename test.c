#include "cwl.h"

// Request handler function
void onreq(int sockfd)
{
    // For simplicity, you can send a 200 OK response with a static file
    CWL_RESPONSE(sockfd, "index.html");
}

int main(void)
{
    CWL_INIT(988);               // Initialize the server on port 988
    CWL_SETREQFUNC(onreq);       // Set the request handler function
    CWL_LISTEN();                // Start listening for incoming requests
    CWL_CLOSE();                 // Close the server after handling requests
}
