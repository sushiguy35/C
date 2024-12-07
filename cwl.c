#include "cwl.h"

static CWL_SERVER *serv;
static char defResp[1024]; // Making this a global/static buffer
static const char *defaultResponse = "HTTP/1.0 200 OK\r\n"
                                     "Server: webserver-c\r\n"
                                     "Content-type: text/html\r\n\r\n";

int CWL_INIT(int port)
{
    // Ensure serv is properly allocated or initialized
    serv = malloc(sizeof(CWL_SERVER));
    if (serv == NULL) {
        perror("Failed to allocate memory for server");
        return CWL_ERROR;
    }
    serv->port = port; // Ensure port is set

    // Create a socket
    serv->sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (serv->sockfd == -1) {
        perror("webserver (socket)");
        return CWL_ERROR;
    }
    printf("socket created successfully\n");

    // Init address for socket to be bound to
    serv->addrLen = sizeof(serv->addr);

    serv->addr.sin_family = AF_INET;
    serv->addr.sin_port = htons(serv->port);
    serv->addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Bind the socket to the address
    if (bind(serv->sockfd, (struct sockaddr *)&serv->addr, serv->addrLen) != 0) {
        perror("webserver (bind)");
        return CWL_ERROR;
    }
    printf("socket successfully bound to address\n");

    if (listen(serv->sockfd, SOMAXCONN) != 0) {
        perror("webserver (listen)");
        return CWL_ERROR;
    }
    printf("server listening for connections\n");

    for (;;) {
        // Accept incoming connections
        int newsockfd = accept(serv->sockfd, (struct sockaddr *)&serv->addr,
                               (socklen_t *)&serv->addrLen);
        if (newsockfd < 0) {
            perror("webserver (accept)");
            continue;
        }
        printf("connection accepted\n");

        // Read from the socket (make sure not to overflow)
        int valread = read(newsockfd, defResp, sizeof(defResp) - 1);  // Leave space for null-termination
        if (valread < 0) {
            perror("webserver (read)");
            close(newsockfd);
            continue;
        }
        defResp[valread] = '\0';  // Null-terminate the response string

        // Write to the socket
        int valwrite = write(newsockfd, defaultResponse, strlen(defaultResponse));
        if (valwrite < 0) {
            perror("webserver (write)");
            close(newsockfd);
            continue;
        }

        close(newsockfd);
    }

    return CWL_SUCCESS_CODE;
}

int CWL_SETREQFUNC(int (*func)())
{
    // Implement your custom request handler function
    return CWL_SUCCESS_CODE;
}

int CWL_RESPONSE(int opt, char *htmlFileName)
{
    switch (opt) {
        case 0: {
            /* Example to serve an HTML file */
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

            write(serv->sockfd, html, strlen(html));  // Send the response
            free(html);
            break;
        }
        default:
            return CWL_ERROR;
    }

    return CWL_SUCCESS_CODE;
}

int CWL_CLOSE(CWL_SERVER *serv)
{
    if (serv->sockfd >= 0) {
        close(serv->sockfd);
    }
    free(serv);
    return CWL_SUCCESS_CODE;
}
