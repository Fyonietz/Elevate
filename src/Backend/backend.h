#ifndef WIN_API_H
#define WIN_API_H
#include <windows.h>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <filesystem>
#include "tinyfiledialogs.h"
#include "json.hpp"
#include <shellapi.h>
#include <gdiplus.h>
#include <thread>
#include <future>
//API For Time 
std::string get_local_system_time();
//API For App Path
std::string get_app_path();
//API For Running App
std::future<DWORD> create_process(const std::string& app_name,const std::string& app);
//API For Inner Running App
std::future<DWORD> create_process_inner(const std::string &app_name,const std::string &app);
//API For Get File Icon
class GDIPlusManager{
    public:
        GDIPlusManager();
        ~GDIPlusManager();
    private:
        ULONG_PTR gdiplusToken;
};
//API For Terminate Process
void shutdown(DWORD pid);
//Thread
void extract_icon_async(const std::string& exe_path,const std::wstring& icon_save_path);

void get_file_icon(const char* input_path,const wchar_t* output_path);
//Converter API
wchar_t* ConvertToWChar(const std::string& str);
//JSON Loader
std::vector<std::string> find_all_json_files(const std::string &root_dir);
//Terminal
void cmd(const std::string &command);
//URL DECODED
std::string decode(const std::string &un_decode);
//Scanner Function
void scan();
//Icon Converter
void convert_icon();
#endif // !WIN_API_H