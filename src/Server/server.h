#ifndef SERVER_CONFIG_H
#define SERVER_CONFIG_H
#include "civetweb.h"
#include <cstring>
#include <cstdio>
#include <cstdlib>
//Manage File By Extension
const char *get_mime_type(const char *path);
//Handling File 
int static_file_handler(struct mg_connection *connection,void *callback_data);
#endif // !SERVER_CONFIG