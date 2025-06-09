#include "backend.h"
using json = nlohmann::json;

// JSON setup
json app_lists_json;
json app_name_json;
json app_attributes_json;


std::string get_local_system_time(){
  SYSTEMTIME system_time;
    GetLocalTime(&system_time);

    std::ostringstream output_string_stream;
    output_string_stream << system_time.wHour << " : "
                                               << system_time.wMinute << " : "
                                               << system_time.wSecond; 

    return output_string_stream.str();
};
std::string get_app_path() {
    const char *filter[] = {"*.exe","*.cmd","*.bat"};
    const char* app_path = tinyfd_openFileDialog(
        "Choose An App",
        "",
        3,
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

    // Load existing JSON
    json app_lists_json;
    app_lists_json["app_lists"]={};
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
        std::cout << "\nApp name: " << app_name << std::endl;
        std::cout << "App Path: " << app_path_to_string << std::endl;

}

    return app_path_to_string;
}