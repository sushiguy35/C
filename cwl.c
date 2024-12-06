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

int CWL_RESPONSE(int opt, char *htmlFileName)
{
    switch (opt)
    {
        case 0:
        // Get the file into html variable
            FILE *fptr;
            fptr = fopen(htmlFileName, "r");

            fseek(fptr, 0, SEEK_END);
            long file_size = ftell(fptr);
            fseek(fptr, 0, SEEK_SET);

            char *html = (char *)malloc(file_size + 1);
            fgets(html, file_size + 1, fptr);
            fclose(fptr);

            for (int i = 0; i < strlen(html); i++) {
                if (html[i] == '"') {
                    // Shift the rest of the string to the right to make room
                    for (int j = strlen(html); j > i; j--) {
                        html[j] = html[j - 1];
                    }
                    html[i] = '\\';  // Insert backslash before the quote
                    i++;  // Skip the next quote, as it was already processed
                }
            }

            break;
        case 1:
            
            break;
        default:
            return CWL_ERROR;
    }
}

int CWL_CLOSE(CWL_SERVER *serv)
{
    
}