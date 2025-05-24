#include "windowsAPI.h"

std::string get_local_system_time(){
  SYSTEMTIME system_time;
    GetLocalTime(&system_time);

    std::ostringstream output_string_stream;
    output_string_stream << system_time.wHour << " : "
                                               << system_time.wMinute << " : "
                                               << system_time.wSecond; 

    return output_string_stream.str();
};