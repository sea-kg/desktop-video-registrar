cmake_minimum_required(VERSION 3.15.3)

set(PROJECT_NAME webcam-video-registrar)

project(${PROJECT_NAME})

include(${CMAKE_CURRENT_SOURCE_DIR}/src.wsjcpp/CMakeLists.txt)

set(CMAKE_CXX_STANDARD 17)
set(EXECUTABLE_OUTPUT_PATH ${${PROJECT_NAME}_SOURCE_DIR})

# include header dirs
list (APPEND WSJCPP_INCLUDE_DIRS "src")
list (APPEND WSJCPP_INCLUDE_DIRS "src/app")
list (APPEND WSJCPP_INCLUDE_DIRS "src/ArgumentsProcessors")
list (APPEND WSJCPP_INCLUDE_DIRS "src/Downloaders")


# source code
list (APPEND WSJCPP_SOURCES "src/main.cpp")

find_package(PkgConfig REQUIRED)
pkg_search_module(gstreamer REQUIRED IMPORTED_TARGET gstreamer-1.0>=1.4)
pkg_search_module(gstreamer-sdp REQUIRED IMPORTED_TARGET gstreamer-sdp-1.0>=1.4)
pkg_search_module(gstreamer-app REQUIRED IMPORTED_TARGET gstreamer-app-1.0>=1.4)
pkg_search_module(gstreamer-video REQUIRED IMPORTED_TARGET gstreamer-video-1.0>=1.4)


include_directories(${WSJCPP_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} ${WSJCPP_SOURCES})

target_link_libraries(${PROJECT_NAME}
    PkgConfig::gstreamer
    PkgConfig::gstreamer-sdp
    PkgConfig::gstreamer-app
    PkgConfig::gstreamer-video
)

# # enable testing functionality
# enable_testing()
# add_subdirectory(tests)