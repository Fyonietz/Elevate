#include <iostream>
#include "Server/server.h"



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

    mg_set_request_handler(context, "/", static_file_handler, 0);

    std::cout << "Server running on http://localhost:9090" << std::endl;
    getchar();  // Wait for user to press Enter
    
    mg_stop(context);
    return 0;
}
