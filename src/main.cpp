#include <iostream>
#include "civetweb.h"
#include <cstring>
#include <cstdio>
#include <cstdlib>

// Serve the index.html file
int serve(struct mg_connection *conn) {
    FILE *file = fopen("Ui/index.html", "rb"); // Open HTML File
    if (!file) {
        mg_printf(conn, "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\nFile not found");
        return 404;
    }

    fseek(file, 0, SEEK_END);              // Move to the end of file
    long size = ftell(file);               // Get file size
    rewind(file);                          // Rewind to beginning

    char *buffer = (char *)malloc(size);   // Allocate memory
    if (!buffer) {
        fclose(file);
        mg_printf(conn, "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/plain\r\n\r\nMemory allocation failed");
        return 500;
    }

    fread(buffer, 1, size, file);          // Read file into memory
    fclose(file);                          // Close file

    // Send HTTP response
    mg_printf(conn, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n");
    mg_write(conn, buffer, size);          // Write response body
    free(buffer);                          // Free memory

    return 200;
}

// Handles HTTP requests
int request_handler(struct mg_connection *conn, void *callback_data) {
    const struct mg_request_info *request_info = mg_get_request_info(conn);
    if (strcmp(request_info->local_uri, "/") == 0) {
        return serve(conn);
    }

  
    return 0;
}

int main() {
    const char *config[] = {
        "document_root", "Ui",
        "listening_ports", "9090",
        0
    };

    struct mg_callbacks callbacks = {};
    struct mg_context *context = mg_start(&callbacks, 0, config);

    if (!context) {
        std::cerr << "Failed to start Civetweb server." << std::endl;
        return 1;
    }

    mg_set_request_handler(context, "/", request_handler, 0);

    std::cout << "Server running on http://localhost:9090" << std::endl;
    getchar();  // Wait for user to press Enter

    mg_stop(context);
    return 0;
}
