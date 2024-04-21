#define BUFFER_SIZE 1024
#define MAX_REQUEST_SIZE 1024
#define DEFAULT_DIR "/var/www"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
int PORT = 8088;
char* requested_file;

// Method to load a HTML resource
char* load_html_resource(char* filename) {

}

//Method to build the file name
char* build_file_name(char* filename) {
    // Calculate the size of the variable required
    char dest[strlen(DEFAULT_DIR)+strlen(filename)];
    // Copy in default DIR
    strcpy( dest, DEFAULT_DIR );
    // Copy in Filename
    strcat( dest, filename );
    // Load the returned value into buffer
    requested_file = dest;
    // Return value
    return requested_file;
}

// Method to check if a requested file exists
int file_exists(char* filename) {
    // Check if the file exists by testing for read access
    char* dest = build_file_name(filename);
        if (access(dest, F_OK) != -1) {
            // File exists
            return 1;
        } else {
            // File doesn't exist
            return 0;
        }
}

// Method to extract the method & requested resource
void extract_method_and_url(const char *request, char *method, char *url) {
    // Example HTTP Request | GET /index.html HTTP/1.1\r\nHost: www.example.com\r\n\r\n
    // Find the first space to separate method and URL
    const char *space_pos = strchr(request, ' ');
    if (space_pos) {
        // Copy the method from the request
        int method_length = space_pos - request;
        strncpy(method, request, method_length);
        method[method_length] = '\0';
        // Find the end of the URL (next space or newline)
        const char *url_start = space_pos + 1;
        const char *url_end = strpbrk(url_start, " \r\n");
        if (url_end) {
            // Copy the URL from the request
            int url_length = url_end - url_start;
            strncpy(url, url_start, url_length);
            url[url_length] = '\0';
        }
    }
}

// Method to handle content served once connection established
void *connection_handler(void *socket_desc) {
    int sock = *(int*)socket_desc;
    char buffer[BUFFER_SIZE] = {0};
    ssize_t valread;
    char method[16], url[256];
    // Read the request
    valread = read(sock, buffer, BUFFER_SIZE);
    extract_method_and_url(buffer, method, url);
    char response[1024];
    if(file_exists(url))
    {
    // File exists so read the file into buffer
        char* filename = build_file_name(url);
        char buf[0x100];
        snprintf(buf, sizeof(buf), "%s", filename);
        char * buffer = 0;
        FILE *f;
        long length;
        f = fopen(buf, "r");
        if (f)
        {
        fseek (f, 0, SEEK_END);
        length = ftell (f);
        fseek (f, 0, SEEK_SET);
        buffer = malloc (length);
        if (buffer)
        {
            fread (buffer, 1, length, f);
        }
        fclose (f);
        }
        if (buffer)
        {
    // Calculate the size of the variable required
    char content[strlen("HTTP/1.1 200 OK\nContent-Type: text/html\n\n")+strlen(buffer)];
    // Copy in default DIR
    strcpy( content, "HTTP/1.1 200 OK\nContent-Type: text/html\n\n" );
    // Copy in Filename
    strcat( content, buffer );
    // Load the returned value into buffer
    snprintf(response, sizeof(response), content, method, url);

    buffer="";
        }

//snprintf(response, sizeof(response), "HTTP/1.1 200 OK\nContent-Type: text/html\n\n<html><body><h1>Hello, World!</h1>Method: %s<br /> URL %s <br /><a href=\"page.html\">Another page...</a></body></html>", method, url);

    }
    else
    {
    // File doesn't Exists.
    snprintf(response, sizeof(response), "HTTP/1.1 404 OK\nContent-Type: text/html\n\n<html><body><h1>404 Error!</h1>Page %s not found...</body></html>", url);
    }
    // Send the response
    write(sock, response, strlen(response));
    // Close the socket and free the socket descriptor
    close(sock);
    free(socket_desc);
    return NULL;
}

int main() {
    setvbuf (stdout, NULL, _IONBF, 0);
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    // Create socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        printf("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;

    // Got bored of the code not releasing the port on exit, so this loops through to find a new port and outputs to console
    int test_case = 0;
    while(test_case == 0) {
        address.sin_port = htons(PORT);
        // Bind the socket to localhost port 8088
        if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
                printf("Bind failed");
                //exit(EXIT_FAILURE);
                PORT = PORT + 1;
            }
            else
            {
                test_case = 1;  
                printf("PORT: %i",PORT); 
            }
        }

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0) {
        printf("Listen failed");
        exit(EXIT_FAILURE);
    }
    // Accept incoming connections and create a thread for each
    while ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))) {
        pthread_t thread_id;
        int *new_sock = malloc(sizeof(int));
        *new_sock = new_socket;

        if (pthread_create(&thread_id, NULL, connection_handler, (void*)new_sock) < 0) {
            printf("Thread creation failed");
            exit(EXIT_FAILURE);
        }
        // Detach the thread to avoid memory leaks
        pthread_detach(thread_id);
    }
    if (new_socket<0) {
        printf("Accept failed");
        exit(EXIT_FAILURE);
    }
    return 0;
}