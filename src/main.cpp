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
    mg_set_request_handler(context,"/api",request_handler,0);
    //Sys Info
    mg_set_request_handler(context,"/sys_info/time",time_handler,0);

    //Sys Act
    mg_set_request_handler(context,"/sys_act/save",save_json_handler,0);
    mg_set_request_handler(context,"/sys_act/add",save_file_path_handler,0);
    mg_set_request_handler(context,"/sys_act/run",app_launcher_handler,0);
    mg_set_request_handler(context,"/sys_act/delete",delete_app_handler,0);
    mg_set_request_handler(context,"/sys_act/cmd",command_handler,0);
    //Server Info
    std::cout << "Server running on http://localhost:9090" << std::endl;
    std::cout << "Press Enter To Stop Server"<< std::endl;
    Sleep(1000);
    auto start_ui = create_process_inner("Ui","NWeb.exe");
    DWORD pid = start_ui.get();
    //After Enter
    getchar();  
    std::cout <<"PID CODE: "<< pid << std::endl;
    shutdown(pid);
    
    mg_stop(context);

    return 0;
}
