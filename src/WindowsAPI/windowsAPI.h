#ifndef WIN_API_H
#define WIN_API_H
#include <windows.h>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include "tinyfiledialogs.h"
#include "json.hpp"
std::string get_local_system_time();
void get_app_path();
#endif // !WIN_API_H