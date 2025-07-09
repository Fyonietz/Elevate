#include "backend.h"
using json = nlohmann::json;
wchar_t* ConvertToWChar(const std::string& str){
     int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
    wchar_t* wideStr = new wchar_t[size_needed];
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, wideStr, size_needed);
    return wideStr;
}
// JSON setup
json app_lists_json;
json app_name_json;
json app_attributes_json;


std::string get_local_system_time(){
  SYSTEMTIME system_time;
    GetLocalTime(&system_time);

    std::ostringstream output_string_stream;
    output_string_stream << system_time.wHour << " : "
                                               << system_time.wMinute << ": "
                                               << system_time.wSecond;

    return output_string_stream.str();
};
std::string get_app_path() {
    const char *filter[] = {"*.exe","*.cmd","*.bat","*.url"};
    const char* app_path = tinyfd_openFileDialog(
        "Choose An App",
        "",
        4,
        filter,
        "Executable File",
        0
    );
    if (!app_path) {
        std::cout << "Canceled" << std::endl;
        return "";
    }

    std::string app_path_to_string(app_path);
    std::filesystem::path app_path_to_name(app_path);
    std::string app_name = app_path_to_name.stem().string();
      std::string app_path_without_ext = "Ui/Assets/icon/" +app_path_to_name.stem().string() + ".PNG";
      std::string icon_json_path = "/Assets/icon/" + app_path_to_name.stem().string() + ".PNG";
    // Load existing JSON
    json app_lists_json;
    std::ifstream in("Ui/JSON/app.json");
    if (in.is_open()) {
        in.seekg(0,std::ios::end);
        if(in.tellg() > 0){
            in.seekg(0,std::ios::beg);
             in >> app_lists_json;
        }
               in.close();
    }
    if (!app_lists_json.contains("app_lists") || !app_lists_json["app_lists"].is_array()) {
        app_lists_json["app_lists"] = json::array();

    }

    // Prepare new entry
    json app_attributes_json;
    app_attributes_json["app_location"] = app_path_to_string;
    app_attributes_json["app_image"] = icon_json_path;
    json app_name_json;
    app_name_json[app_name] = app_attributes_json;
    // Add to array
    app_lists_json["app_lists"].push_back(app_name_json);
    // Save
    std::ofstream app_json("Ui/JSON/app.json");
    if (!app_json.is_open()) {
        std::cout << "\n[ERROR] Failed to open 'Ui/JSON/app.json'\n";
        std::cout << "Check if the 'Ui/JSON' folder exists in: " << std::filesystem::current_path() << "\n";
        std::cout << "Check if the 'Ui/JSON' folder exists in: " << std::filesystem::current_path() << "\n";
}else{
       app_json << app_lists_json.dump(4);
        app_json.close();
        std::filesystem::path app_path_to_name(app_path);
        if (app_path_to_name.extension() == ".exe" && !std::filesystem::exists(app_path_without_ext)) {
            extract_icon_async(app_path_to_string, ConvertToWChar(app_path_without_ext));
        }
        std::cout << "\nApp name: " << app_name << std::endl;
        std::cout << "App Path: " << app_path_to_string << std::endl;
        std::wcout <<  ConvertToWChar(app_path_without_ext) << std::endl;

}

    return app_path_to_string;
}
std::future<DWORD> create_process(const std::string& app_name,const std::string& app){
    std::promise<DWORD> pid_promise;
    std::future<DWORD> pid_future = pid_promise.get_future();
   
    std::thread([app_name, app, prom = std::move(pid_promise)]() mutable {
        STARTUPINFOA si = {sizeof(si)};
        PROCESS_INFORMATION pi;
        std::string shellCmd = "cmd.exe /c start \"\" \"" + app + "\"";
        std::vector<char> cmd(shellCmd.begin(), shellCmd.end());
        
        //Working dir
        std::string::size_type last_position = app.find_last_of("\\/");
        std::string working_direction = (last_position != std::string::npos) ? app.substr(0, last_position) : "";

        // std::vector<char> cmd(app.begin(),app.end());
        cmd.push_back('\0');
        if (!CreateProcessA(
            nullptr,
            cmd.data(),
            nullptr,
            nullptr,
            FALSE,
            CREATE_NEW_CONSOLE,
            nullptr,
            working_direction.empty() ? nullptr : working_direction.c_str(),
            &si,
            &pi
        )) {
            DWORD err = GetLastError();
            std::cerr << "Failed To Launch: " << app << " " << err << std::endl;
            return;
        } else {
            std::cout << "PID: " << pi.dwProcessId << std::endl;
            prom.set_value(pi.dwProcessId);
        }
        WaitForSingleObject(pi.hProcess, INFINITE);
        // std::cout << app_name << " has closed "<< std::endl;
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

   }).detach();
   return pid_future;
}
std::future<DWORD> create_process_inner(const std::string &app_name,const std::string &app){ std::promise<DWORD> pid_promise;
    std::future<DWORD> pid_future = pid_promise.get_future();
   
    std::thread([app_name, app, prom = std::move(pid_promise)]() mutable {
        STARTUPINFOA si = {sizeof(si)};
        PROCESS_INFORMATION pi;
        std::string shellCmd = "\"" + app + "\"";
        std::vector<char> cmd(shellCmd.begin(), shellCmd.end());
        
        //Working dir
        std::string::size_type last_position = app.find_last_of("\\/");
        std::string working_direction = (last_position != std::string::npos) ? app.substr(0, last_position) : "";

        // std::vector<char> cmd(app.begin(),app.end());
        cmd.push_back('\0');
        if (!CreateProcessA(
            nullptr,
            cmd.data(),
            nullptr,
            nullptr,
            FALSE,
            CREATE_NEW_CONSOLE,
            nullptr,
            working_direction.empty() ? nullptr : working_direction.c_str(),
            &si,
            &pi
        )) {
            DWORD err = GetLastError();
            std::cerr << "Failed To Launch: " << app << " " << err << std::endl;
            return;
        } else {
            std::cout << "PID: " << pi.dwProcessId << std::endl;
            prom.set_value(pi.dwProcessId);
        }
        WaitForSingleObject(pi.hProcess, INFINITE);
        // std::cout << app_name << " has closed "<< std::endl;
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

   }).detach();
   return pid_future;
}
//API For Get File Icon

        GDIPlusManager::GDIPlusManager(){
            Gdiplus::GdiplusStartupInput gdiplus_startup_input;
            GdiplusStartup(&gdiplusToken,&gdiplus_startup_input,nullptr);
        };
          GDIPlusManager::~GDIPlusManager(){
            ULONG_PTR gdiplusToken;
        };

        ULONG_PTR gdiplusToken;

void get_file_icon(const char* input_path, const wchar_t* output_path) {
    GDIPlusManager gdiplus;

    // Only try to extract icon from .exe files
    std::filesystem::path p(input_path);
    if (p.extension() != ".exe") {
        std::cerr << "Icon extraction only supported for .exe files.\n";
        return;
    }

    HICON hIcon = nullptr;
    if (ExtractIconExA(input_path, 0, &hIcon, nullptr, 1) <= 0 || hIcon == nullptr) {
        std::cerr << "Failed To Extract Icon. \n";
        return;
    }

    Gdiplus::Bitmap* bmp = Gdiplus::Bitmap::FromHICON(hIcon);
    if (!bmp) {
        std::cerr << "Failed To Convert Icon To Bitmap \n";
        DestroyIcon(hIcon);
        return;
    }

    CLSID clsid;
    UINT num = 0, size = 0;
    Gdiplus::GetImageEncodersSize(&num, &size);
    if (size == 0) {
        std::cerr << "GetImageEncodersSize Failed.\n";
        delete bmp;
        DestroyIcon(hIcon);
        return;
    }

    Gdiplus::ImageCodecInfo* codecs = (Gdiplus::ImageCodecInfo*)(malloc(size));
    Gdiplus::GetImageEncoders(num, size, codecs);

    bool found = false;
    for (UINT i = 0; i < num; ++i) {
        if (wcsstr(codecs[i].FilenameExtension, L"*.PNG" ) != nullptr){
            clsid = codecs[i].Clsid;
            found = true;
            break;
        }
    }
    free(codecs);

    if (!found) {
        std::cerr << "ICO Encoder Not Found.\n";
        delete bmp;
        DestroyIcon(hIcon);
        return;
    }

    if (bmp->Save(output_path, &clsid, nullptr) != Gdiplus::Ok) {
        std::cerr << "Failed To Save .ico file.\n";
    } else {
        std::cout << "Icon Saved\n";
    }
    delete bmp;
    DestroyIcon(hIcon);
};


void extract_icon_async(const std::string& exe_path, const std::wstring& icon_save_path) {
    std::thread([exe_path, icon_save_path]() {
        get_file_icon(exe_path.c_str(), icon_save_path.c_str());
    }).detach();
}

std::vector<std::string> find_all_json_files(const std::string &root_dir){
    std::vector<std::string> json_files;
    for(const auto &entry : std::filesystem::recursive_directory_iterator(root_dir)){
        if(entry.is_regular_file() && entry.path().extension() == ".json"){
            json_files.push_back(entry.path().string());
        }
    }
    return json_files;
}

void cmd(const std::string &command){
    std::unordered_map<std::string,std::string> command_key={
        {"::sys::terminate","shutdown /s /f /t 0"},
        {"::sys::sleep","cant_sleep"}
    };
    //command
    std::string command_after_parser = command.substr(command.find('=')+1);

    std::cout << command_after_parser << std::endl;
    if(command_key.count(command_after_parser)){
        std::cout<< command_key[command_after_parser] << std::endl;
    }else{
        std::cout << "command doesnt exist" <<std::endl;
    }
};
void shutdown(DWORD pid){
   std::string cmd = "taskkill /PID " + std::to_string(pid) + " /F";
   system(cmd.c_str());
}
