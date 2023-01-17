#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <string>

#define DEV_MODE

//Inline ensures every instance of constant is using same place in memory
//Constants are seperated and collected in Namespaces

//Constants for Configuration of ROS Publisher System
namespace ros_consts {
    //Initializing constexpr string requires string_view
    //Initializing as const still sufficient
    inline const std::string pose_topic_name = "chatter";
}

//Constants for Configuration of camera Detection
//constexpr because "compile time constant" -> value can be assigned to other const: "const x = (constexpr)...;""
namespace camera_consts {
    //Size of Initial Camera Input -> "Camera input Resolution"
    inline constexpr int inputWidth = 1280; //640
    inline constexpr int inputHeight = 960; //480

    //Size of Final Detection Window and its "Resolution"
    inline constexpr int destWidth = 400;
    inline constexpr int destHeight = 400;


    //Min and Max Size of the Detected Squares and their Contours
    inline constexpr int minSizeSquare = 100;//100//1000
    inline constexpr int maxSizeSquare = 3500;
}

//Constants for Debugging Purposes
namespace debug_consts {
    inline constexpr bool singleCameraSteps = false;
    inline constexpr bool displayBuildInformations = false;
    
    inline constexpr bool drawDetectedCircles = true; 
}

#endif