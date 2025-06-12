#ifndef SERVER_CONFIG_H
#define SERVER_CONFIG_H
#include "civetweb.h"
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include "../Backend/backend.h"

//Manage File By Extension
const char *get_mime_type(const char *path);
//POST Function
int post_method(struct mg_connection *connection,const mg_request_info *info);
//GET Function
int get_method(struct mg_connection *connection,const mg_request_info *info);
//Handling POST and GET
int request_handler(struct mg_connection *connection,void *callback_data);
//Handling File 
int static_file_handler(struct mg_connection *connection,void *callback_data);
//Handling Time From Windows API
int time_handler(struct mg_connection *connection,void *callback_data);
//JSON Save Handling
int save_json_handler(struct mg_connection *connection,void *callback_data);
//SAVE File Path Handling
int save_file_path_handler(struct mg_connection *connection,void *callback_data);
//App Running
int app_launcher_handler(struct mg_connection *connection,void *callback_data);
//Delete An App
int delete_app_handler(struct mg_connection *connection,void *);


#endif // !SERVER_CONFIG