#include "cwl.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

static CWL_SERVER *serv;
static const char *defaultResponse = "HTTP/1.0 200 OK\r\n"
                                     "Server: webserver-c\r\n"
                                     "Content-type: text/html\r\n\r\n";

#define DEFAULT_RESPONSE "<html><body>Hello, world!</body></html>"

// Function to escape double quotes in HTML content (simplified version)
char *fixHtmlFile(char *file) {
    return file;  // For simplicity, directly return the file as is.
}

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

    // Set socket option SO_REUSEADDR
    int opt = 1;
    if (setsockopt(serv->sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt(SO_REUSEADDR) failed");
        close(serv->sockfd);
        return CWL_ERROR;
    }

    // Init address for socket to be bound to
    serv->addrLen = sizeof(serv->addr);

    serv->addr.sin_family = AF_INET;
    serv->addr.sin_port = htons(serv->port);
    serv->addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Bind the socket to the address
    if (bind(serv->sockfd, (struct sockaddr *)&serv->addr, serv->addrLen) != 0) {
        perror("webserver (bind)");
        close(serv->sockfd);
        return CWL_ERROR;
    }
    printf("socket successfully bound to address\n");

    return CWL_SUCCESS_CODE;
}


int (*request_handler)(int);  // Declare a function pointer for request handler

int CWL_SETREQFUNC(int (*func)(int))
{
    request_handler = func;  // Store the function pointer
    return CWL_SUCCESS_CODE;
}

int CWL_LISTEN()
{
    if (listen(serv->sockfd, SOMAXCONN) != 0) {
        perror("webserver (listen)");
        return CWL_ERROR;
    }
    printf("server listening for connections\n");

    for (;;) {
        int newsockfd = accept(serv->sockfd, (struct sockaddr *)&serv->addr, (socklen_t *)&serv->addrLen);
        if (newsockfd < 0) {
            perror("webserver (accept)");
            continue;
        }
        printf("connection accepted\n");

        // Call the request handler when a new request is received
        if (request_handler != NULL) {
            request_handler(newsockfd);  // Pass the socket to the handler
        }

        close(newsockfd);  // Close the client socket after handling
    }

    return CWL_SUCCESS_CODE;
}

int CWL_RESPONSE(int sockfd, char *htmlFile)
{
    FILE *fptr = fopen(htmlFile, "r");
    if (!fptr) {
        perror("Error opening file");
        return CWL_ERROR;
    }

    // Move the file pointer to the end of the file to determine the file size
    fseek(fptr, 0, SEEK_END);
    long file_size = ftell(fptr);
    fseek(fptr, 0, SEEK_SET);  // Move the file pointer back to the beginning

    // Allocate memory for the entire file content (+1 for null-terminator)
    char *html = (char *)malloc(file_size + 1);
    if (!html) {
        perror("Memory allocation error");
        fclose(fptr);
        return CWL_ERROR;
    }

    // Read the entire file into the buffer
    size_t bytes_read = fread(html, 1, file_size, fptr);
    html[bytes_read] = '\0';  // Null-terminate the string

    fclose(fptr);

    // Process the HTML content (escape quotes, etc.) and get the final response HTML
    char *newHtml = fixHtmlFile(html);  

    // If fixHtmlFile does not allocate new memory (i.e., newHtml == html), we shouldn't free it twice
    if (newHtml != html) {
        free(html);  // Free the original html only if newHtml is a new allocation
    }

    // Send the default header followed by the HTML content
    char *response = malloc(strlen(defaultResponse) + strlen(newHtml) + 1);
    strcpy(response, defaultResponse);
    strcat(response, newHtml);

    // Send the response to the client over the socket connection
    int valwrite = write(sockfd, response, strlen(response));
    if (valwrite < 0) {
        perror("webserver (write)");
        free(response);
        return CWL_ERROR;
    }

    // Free the memory
    free(response);  // Free the memory for the final response string
    free(newHtml);   // Free the memory allocated by fixHtmlFile (if newHtml is different from html)

    return CWL_SUCCESS_CODE;
}

int CWL_CLOSE()
{
    if (serv->sockfd >= 0) {
        close(serv->sockfd);
    }
    free(serv);
    return CWL_SUCCESS_CODE;
}
