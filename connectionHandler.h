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
        // File exists
        snprintf(response, sizeof(response), "HTTP/1.1 200 OK\nContent-Type: text/html\n\n<html><body><h1>Hello, World!</h1>Method: %s<br /> URL %s <br /><a href=\"page.html\">Another page...</a></body></html>", method, url);
    
    }
    else
    {
        // Filedoesnt Exists.
        snprintf(response, sizeof(response), "HTTP/1.1 404 OK\nContent-Type: text/html\n\n<html><body><h1>404 Error!</h1>Page %s not found...</body></html>", url);
    
    }  
    // Print the extracted method and URL
    //snprintf(response, sizeof(response), "HTTP/1.1 200 OK\nContent-Type: text/html\n\n<html><body><h1>Hello, World!</h1>Method: %s<br /> URL %s</body></html>", method, url);
    // Send the response
    write(sock, response, strlen(response));
    // Close the socket and free the socket descriptor
    close(sock);
    free(socket_desc);
    return NULL;
}