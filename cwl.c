#include "cwl.h"

static CWL_SERVER *serv;

int CWL_INIT(int port, CWL_SERVER *server)
{
    serv = server;

    #ifdef __linux__
        serv->ListenFD = socket(AF_INET, SOCK_STREAM, 0);
        if (serv->ListenFD < 0) {
            perror("Error opening socket");
            return CWL_ERROR;
        } else {
            printf("Socket created successfully\n");
        }

        serv->addr.sin_family = AF_INET;
        serv->addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        serv->addr.sin_port = htons(port);

        if (bind(serv->ListenFD, (struct sockaddr*)&serv->addr, sizeof(serv->addr)) < 0) {
            perror("Error binding socket");
            return CWL_ERROR;
        } else {
            printf("Socket bound successfully\n");
        }

        if (listen(serv->ListenFD, 20) < 0) {
            perror("Error listening on socket");
            return CWL_ERROR;
        } else {
            printf("Listening on socket\n");
        }

        // Accepting a client connection
        serv->ConnFD = accept(serv->ListenFD, NULL, NULL);
        if (serv->ConnFD < 0) {
            perror("Error accepting connection");
            return CWL_ERROR;
        } else {
            printf("Client connected\n");
        }
    #endif
    serv->port = port;
    return CWL_SUCCESS_CODE;
}


int CWL_SETREQFUNC(int (*func)())
{
    if (func) {
        func(); 
    }
    return CWL_SUCCESS_CODE;
}

int CWL_RESPONSE(int opt, char *htmlFileName)
{
    switch (opt)
    {
        case 0:
        {
            // Get the file into html variable
            FILE *fptr = fopen(htmlFileName, "r");
            if (!fptr) {
                perror("Error opening file");
                return CWL_ERROR;
            }

            fseek(fptr, 0, SEEK_END);
            long file_size = ftell(fptr);
            fseek(fptr, 0, SEEK_SET);

            char *html = (char *)malloc(file_size + 1);
            if (!html) {
                perror("Memory allocation error");
                fclose(fptr);
                return CWL_ERROR;
            }

            fgets(html, file_size + 1, fptr);
            fclose(fptr);

            // Escape double quotes in the html
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

            send(serv->ConnFD, html, strlen(html), 0);
            free(html);

            break;
        }

        case 1:
        {
            char *html = htmlFileName;
            while(1)
                send(serv->ConnFD, html, strlen(html), 0);
            break;
        }

        default:
            return CWL_ERROR;
    }

    return CWL_SUCCESS_CODE;
}

int CWL_CLOSE(CWL_SERVER *serv)
{
    #ifdef __linux__
        if (serv->ConnFD > 0) {
            close(serv->ConnFD);
        }
        if (serv->ListenFD > 0) {
            close(serv->ListenFD);
        }
    #endif
    return CWL_SUCCESS_CODE;
}
