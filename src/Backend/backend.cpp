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
std::string get_app_path(){
  const char *filter[] = {"*.exe"};

  const char* app_path = tinyfd_openFileDialog(
    "Choose An App",
    nullptr,
    1,
    filter,
    "Executeable File",
    0
  );
 std::string app_path_to_string(app_path);
 std::filesystem::path app_path_to_name(app_path);
 std::string app_name = app_path_to_name.stem().string();
  if(app_path){
   
    //Register
    app_lists_json["app_lists"].clear();
    app_name_json[app_name]=app_name;
    app_attributes_json["app_location"]=app_path_to_string;
    app_name_json[app_name]=app_attributes_json;
    app_lists_json["app_lists"].push_back(app_name_json);
  
    //Save
    std::ofstream app_json("Ui/JSON/app.json");
    if(!app_json.is_open()){
       std::cout << "\n[ERROR] Failed to open 'JSON/app.json'\n";
       std::cout << "Check if the 'JSON' folder exists in: " << std::filesystem::current_path() << "\n";
    }else{
      app_json << app_lists_json.dump(4);
      app_json.close();
    }
    std::cout << "\nApp name: " << app_name << std::endl;
    std::cout << "App Path: " << app_path_to_string << std::endl; 
  }else{
    std::cout << "\nCanceled" << std::endl;
  }
  return app_path_to_string;
};