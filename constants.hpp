#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>

namespace ros_consts {
    //Initializing constexpr string requires string_view
    //Initializing as const still sufficient
    //Inline ensures every instance of constant is using same place in memory
    inline const std::string pose_PubSub_name = "/talker/pose";
   
}

namespace camera_consts {
    
}

#endif