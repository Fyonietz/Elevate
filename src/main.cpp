#include "Server/server.h"



int main() {
    //Server Setup
const char *config[] = {
    "document_root", "Ui",
    "listening_ports", "9090",
     "num_threads", "64",
    "enable_keep_alive", "yes",
    0
};
    struct mg_callbacks callbacks = {};
    struct mg_context *context = mg_start(&callbacks, 0, config);
    if (!context) {
        std::cerr << "Failed to start Civetweb server." << std::endl;
        return 1;
    }

    //Routes Register
    // mg_set_request_handler(context, "/", static_file_handler, 0);
    mg_set_request_handler(context,"/api",request_handler,0);
    mg_set_request_handler(context,"/time",time_handler,0);
    mg_set_request_handler(context,"/api/save",save_json_handler,0);
    mg_set_request_handler(context,"/os/path",save_file_path_handler,0);
    mg_set_request_handler(context,"/run",app_launcher_handler,0);
    mg_set_request_handler(context,"/delete",delete_app_handler,0);
    //Server Info
    std::cout << "Server running on http://localhost:9090" << std::endl;
    std::cout << "Press Enter To Stop Server"<< std::endl;
    Sleep(1000);
    create_process("UI","NWeb.exe");
    getchar();  
    
    mg_stop(context);

    return 0;
}
