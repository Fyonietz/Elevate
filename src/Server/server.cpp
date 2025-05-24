#include "server.h"
#include "windows.h"
const char *get_mime_type(const char *path){
    const char *extension_types = strrchr(path,'.');
    if(!extension_types) return "text/plain";
    if(strcmp(extension_types,".html") ==0 ) return "text/html";
    if(strcmp(extension_types,".css") ==0 ) return "text/css";
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
int static_file_handler(struct mg_connection *connection,void *callback_data){
    const struct mg_request_info *request_info = mg_get_request_info(connection);
    const char *universal_request_info = request_info->local_uri;

    char file_path[1024];
       if (strcmp(universal_request_info, "/") == 0) {
        snprintf(file_path, sizeof(file_path), "Ui/index.html");
    } else {
        snprintf(file_path, sizeof(file_path), "Ui%s", universal_request_info);
    }

    //Open File
    FILE *file = fopen(file_path,"rb");
    if(!file){
        mg_printf(connection,"HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\nFile not found\n");
        return 404;
    }

    //Get File Size
    fseek(file,0,SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    //Allocate Memory
    char *buffer_static_into_memory = (char*)malloc(file_size);
    if(!buffer_static_into_memory){
        fclose(file);
        mg_printf(connection, "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/plain\r\n\r\nMemory error\n");
        return 500;
    }

    fread(buffer_static_into_memory,1,file_size,file);
    fclose(file);

    const char *mime_types = get_mime_type(file_path);

    //Cache Control
    const char *cache_control = "Cache-Control: max-age=3600\r\n";
    if(strcmp(mime_types,"text/css")==0 || strcmp(mime_types,"application/javascript")==0){
        cache_control = "Cache-Control: no-cache, no-store, must-revalidate\r\nPragma: no-cache\r\nExpires: 0\r\n";
    }

    //Send Response
    mg_printf(
        connection,
         "HTTP/1.1 200 OK\r\n"
          "Content-Type: %s\r\n"
            "%s"
           "\r\n",
           mime_types,cache_control
    );
  
    mg_write(connection,buffer_static_into_memory,file_size);
    free(buffer_static_into_memory);
    
    return 200;

};

int time_handler(struct mg_connection *connection,void *callback_data){
    std::string time_string = get_local_system_time();

    mg_printf(connection,
              "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n%s",
              time_string.c_str());
    return 200;
};