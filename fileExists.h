int file_exists(char* filename) {
    // Check if the file exists by testing for read access
char* str = "/var/www/";
char dest[strlen(DEFAULT_DIR)+strlen(filename)];
strcpy( dest, DEFAULT_DIR );
strcat( dest, filename );
    if (access(dest, F_OK) != -1) {
        // File exists
        return 1;
    } else {
        // File doesn't exist
        return 0;
    }
}