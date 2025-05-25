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
    mg_printf(connection, "HTTP/1.1 200 OK\r\nContent-Type: %s\r\nContent-Length: %ld\r\n\r\n", mime, file_size);

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
    
    return 200;
}