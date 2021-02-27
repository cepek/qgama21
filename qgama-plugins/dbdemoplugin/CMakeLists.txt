cmake_minimum_required(VERSION 3.5)

project(dbdemoplugin LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# CMAKE_PREFIX_PATH By default this is empty. It is intended to be set by the project.
#
#set(CMAKE_PREFIX_PATH "/home/cepek/Qt/5.15.0/gcc_64/lib/cmake/Qt5/") # example linux
#set(CMAKE_PREFIX_PATH "C:/Qt/5.15.0/msvc2019_64/lib/cmake/Qt5/")     # example windows

find_package(Qt5 COMPONENTS Core Gui Sql Widgets REQUIRED)

set(Q2PATH "../../gama-q2")
include_directories(${Q2PATH})

add_library(dbdemoplugin SHARED
  ${Q2PATH}/gamaq2interfaces.h
  dbdemoplugin.cpp
  dbdemoplugin.h
)

target_link_libraries(dbdemoplugin PRIVATE
  Qt5::Core Qt5::Gui Qt5::Sql Qt5::Widgets
)

target_compile_definitions(dbdemoplugin PRIVATE DBDEMOPLUGIN_LIBRARY)
