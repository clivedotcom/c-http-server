//C HTTP Server
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 8088
char *getPathOfGetRequest(const char *buf) 
   {
   const char *start = buf;
   const char *end;
   char       *path=NULL;
   size_t      pathLen;

   /* Verify that there is a 'GET ' at the beginning of the string. */
   if(strncmp("GET ", start, 4))
      {
      fprintf(stderr, "Parse error: 'GET ' is missing.\n");
      goto CLEANUP;
      }

   /* Set the start pointer at the first character beyond the 'GET '. */
   start += 4;

   /* From the start position, set the end pointer to the first white-space character found in the string. */
   end=start;
   while(*end && !isspace(*end))
      ++end;

   /* Calculate the path length, and allocate sufficient memory for the path plus string termination. */
   pathLen = (end - start);
   path = malloc(pathLen + 1);
   if(NULL == path)
      {
      fprintf(stderr, "malloc() failed. \n");
      goto CLEANUP;
      }

   /* Copy the path string to the path storage. */
   memcpy(path, start, pathLen);

   /* Terminate the string. */
   path[pathLen] = '\0';

CLEANUP:

   /* Return the allocated storage, or NULL in the event of an error, to the caller. */
   return(path);
   }

int main(int argc, char const* argv[])
{
    int server_fd, new_socket;
    ssize_t valread;
    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);
    char buffer[1024] = { 0 };
    
 
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
 
    // Forcefully attaching socket to the port 8088
    if (setsockopt(server_fd, SOL_SOCKET,
                   SO_REUSEADDR | SO_REUSEPORT, &opt,
                   sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
 
    // Forcefully attaching socket to the port 8088
    if (bind(server_fd, (struct sockaddr*)&address,
             sizeof(address))
        < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    for (;;) {
    if ((new_socket
         = accept(server_fd, (struct sockaddr*)&address,
                  &addrlen))
        < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    valread = read(new_socket, buffer,
                   1024 - 1); // subtract 1 for the null
                              // terminator at the end
    //printf("%s\n", buffer);
   char *url =  getPathOfGetRequest(buffer);
    char hello[] = "HTTP/1.0 404 OK\r\n"
"Server: webserver-c\r\n"
"Content-type: text/html\r\n\r\n"
"<html><h1>hello, world</h1><b>This is a website</b><br />You have requested: %s</html>\r\n";

char* name_with_extension;
name_with_extension = malloc(strlen(hello)+strlen(url));
strcpy(name_with_extension, hello); /* copy name into the new var */
strcat(name_with_extension, url); /* add the extension */
printf("%s\n", name_with_extension);

//printf("%s\n",url);
    send(new_socket, name_with_extension, strlen(name_with_extension), 0);
    //printf("Hello message sent\n");
 
    // closing the connected socket
    close(new_socket);
    }
    // closing the listening socket
    close(server_fd);
    return 0;
}
