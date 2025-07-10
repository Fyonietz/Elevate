#include "server.h"
#include "windows.h"

const char *get_mime_type(const char *path){
    const char *extension_types = strrchr(path,'.');
    if(!extension_types) return "text/plain";
    if(strcmp(extension_types,".html") ==0 ) return "text/html";
    if(strcmp(extension_types,".css") ==0 ) return "text/css";
    if (strcmp(extension_types, ".json") == 0) return "application/json";
    if(strcmp(extension_types,".js") ==0 ) return "application/javascript";
    if(strcmp(extension_types,".png") ==0 ) return "image/png";
    if(strcmp(extension_types,".jpg") ==0 || strcmp(extension_types,".jpeg") ==0 ) return "image/jpeg";
    if(strcmp(extension_types,".gif") ==0 ) return "image/gif";
    return "text/plain";
}

std::string decode(const std::string &un_decode){
    char decoded[1024];
    mg_url_decode(un_decode.c_str(),un_decode.length(),decoded,sizeof(decoded),1);
    return std::string(decoded);
};
int get_method(struct mg_connection *connection,const mg_request_info *info){
    const char *query = info->query_string;
    mg_printf(connection, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n");
    mg_printf(connection, "GET Request Received\nQuery: %s\n", query ? query : "none");

    return 200;
};

int post_method(struct mg_connection *connection,const mg_request_info *info){
    char post_data[1024] = {0};
    int post_data_lenght = mg_read(connection,post_data,sizeof(post_data));
    post_data[post_data_lenght]='\0';
    //Post List

    std::cout << post_data << std::endl;

    mg_printf(connection,
        "HTTP/1.1 302 Found\r\n"
        "Location: /\r\n"
        "Content-Length: 0\r\n"
        "\r\n");
    return 302;
}

int request_handler(struct mg_connection *connection, void *callback_data) {
    const struct mg_request_info *info = mg_get_request_info(connection);

    if (strcmp(info->request_method, "GET") == 0) {
        return get_method(connection, info);
    } else if (strcmp(info->request_method, "POST") == 0) {
        return post_method(connection, info);
    } else {
        mg_printf(connection, "HTTP/1.1 405 Method Not Allowed\r\nContent-Type: text/plain\r\n\r\nMethod Not Allowed\n");
        return 405;
    }
}
int static_file_handler(struct mg_connection *connection, void *callback_data) {
    const struct mg_request_info *request_info = mg_get_request_info(connection);
    const char *uri = request_info->local_uri;

    char file_path[1024];
    if (strcmp(uri, "/") == 0) {
        snprintf(file_path, sizeof(file_path), "Ui/index.html");
    } else {
        snprintf(file_path, sizeof(file_path), "Ui%s", uri);
    }

    FILE *file = fopen(file_path, "rb");
    if (!file) {
        mg_printf(connection, "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\nFile not found.");
        return 404;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    if (file_size < 0) {
        fclose(file);
        mg_printf(connection, "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/plain\r\n\r\nFile size error.");
        return 500;
    }

    const char *mime = get_mime_type(file_path);

    
    char buffer[1024];
    size_t n;
    while ((n = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        mg_write(connection, buffer, n);
    }
    fclose(file);
    return 200;
}

int time_handler(struct mg_connection *connection,void *callback_data){
    std::string time_string = get_local_system_time();

    mg_printf(connection,
              "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n%s",
              time_string.c_str());
    return 200;
};

int save_json_handler(struct mg_connection *connection,void *callback_data){
    char buffer_json[4096];
    int lenght_buffer_json = mg_read(connection,buffer_json,sizeof(buffer_json)-1);
    buffer_json[lenght_buffer_json]='\0';

    FILE *file_json = fopen("Ui/JSON/app.json","w");
    if(!file_json){
        mg_printf(connection,"HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/plain\r\n\r\nFailed to open file.");
        return 500;
    }
    fwrite(buffer_json,1,lenght_buffer_json,file_json);
    fclose(file_json);

    mg_printf(connection,"HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nSaved");
    return 200;
};

int save_file_path_handler(struct mg_connection *connection,void *callback_data){
    get_app_path();
   mg_printf(connection,
       "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 2\r\n"
        "\r\n"
        "OK");
    return 200;
}

int app_launcher_handler(struct mg_connection *connection,void *callback_data){
    char app_parameter[1020] = {0};
    int app_parameter_lenght = mg_read(connection, app_parameter, sizeof(app_parameter) - 1);
    app_parameter[app_parameter_lenght] = '\0';
    std::string param_str(app_parameter);
    std::string app_name;
    auto pos = param_str.find('=');
    if (pos != std::string::npos)
        app_name = param_str.substr(pos + 1);
    else
        app_name = param_str;

    // Try app.json first, then search.json
    std::vector<std::string> json_files = find_all_json_files("Ui/JSON");
    bool found = false;
    std::string app_that_running;
    for (const auto& json_file : json_files) {
        nlohmann::json app_lists_json;
        std::ifstream in(json_file);
        if (!in.is_open()) continue;
        try {
            in >> app_lists_json;
        } catch (...) {
            continue;
        }
        for (const auto& entry : app_lists_json["app_lists"]) {
            if (entry.contains(app_name)) {
                app_that_running = entry[app_name]["app_location"];
                found = true;
                break;
            }
        }
        if (found) break;
    }
    if (found) {
        std::cout << "Running: " << app_name << std::endl;
        create_process(app_name, app_that_running);
    }

    mg_printf(connection,
       "HTTP/1.1 303 OK\r\n"
        "Location : /\r\n"
        "Content-Length: 2\r\n"
        "\r\n"
        "OK");
    return true;
}

int delete_app_handler(struct mg_connection *connection, void *) {
    char post_data[256] = {0};
    mg_read(connection, post_data, sizeof(post_data));
    std::string name;
    // Parse "name=AppName" from post_data
    auto pos = std::string(post_data).find('=');
    if (pos != std::string::npos)
        name = std::string(post_data).substr(pos + 1);

    // Load JSON
    nlohmann::json app_lists_json;
    std::ifstream in("Ui/JSON/app.json");
    if (in.is_open()) {
        in >> app_lists_json;
        in.close();
    }
    if (app_lists_json.contains("app_lists") && app_lists_json["app_lists"].is_array()) {
        auto& arr = app_lists_json["app_lists"];
        for (auto it = arr.begin(); it != arr.end(); ++it) {
            if (it->contains(name)) {
                arr.erase(it);
                break;
            }
        }
    }
    // Save JSON
    std::ofstream out("Ui/JSON/app.json");
    out << app_lists_json.dump(4);
    out.close();
    mg_printf(connection,
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 2\r\n"
        "\r\n"
        "OK");
    return 200;
}

int command_handler(struct mg_connection *connection,void *callback_data){
    char command_parameter[1024]={0};
    int command_parameter_lenght = mg_read(connection,command_parameter,sizeof(command_parameter)-1);
    command_parameter[command_parameter_lenght]='\0';
    std::string string_command_parameter(command_parameter);

    cmd(decode(string_command_parameter));

    mg_printf(connection,
    "HTTP/1.1 200 OK\r\n"
    "Content-Type:text/plain \r\n"
    "Content-Lenght:2\r\n"
    "\r\n"
    "OK"
    );
    return 200;
}
int app_scanner_handler(struct mg_connection *connection,void *callback_data){
    char parameter_trigger[8]={0};
    int parameter_trigger_len = mg_read(connection,parameter_trigger,sizeof(parameter_trigger)-1);
    parameter_trigger[parameter_trigger_len]='\0';
    std::cout << parameter_trigger << std::endl;
    if(parameter_trigger){
        std::cout << parameter_trigger << std::endl;
        scan();
    }else{
        std::cout << "Error" << std::endl;
    }
    mg_printf(connection,
    "HTTP/1.1 200 OK\r\n"
    "Content-Type:text/plain \r\n"
    "Content-Lenght:2\r\n"
    "\r\n"    
    "File Scanner Running"    
);
    return 200;
}


int icon_converter_handler(struct mg_connection *connection,void *callback_data){
    char parameter_trigger[8]={0};
    int parameter_trigger_len = mg_read(connection,parameter_trigger,sizeof(parameter_trigger)-1);
    parameter_trigger[parameter_trigger_len]='\0';
    std::cout << parameter_trigger << std::endl;
    if(parameter_trigger){
        std::cout << parameter_trigger << std::endl;
        convert_icon();
    }else{
        std::cout << "Error" << std::endl;
    }
    mg_printf(connection,
    "HTTP/1.1 200 OK\r\n"
    "Content-Type:text/plain \r\n"
    "Content-Lenght:2\r\n"
    "\r\n"    
    "Icon Converter Running"    
);
    return 200;

}

int list_json_handler(struct mg_connection *connection,void *callback_data){
       nlohmann::json result = nlohmann::json::array();
    for (const auto& entry : std::filesystem::recursive_directory_iterator("Ui/JSON")) {
        if (entry.is_regular_file() && entry.path().extension() == ".json") {
            result.push_back(entry.path().filename().string());
        }
    }
    std::string body = result.dump();
    mg_printf(connection,
        "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: %zu\r\n\r\n%s",
        body.size(), body.c_str());
    return 200;
}