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