cmake_minimum_required(VERSION 3.7)

set(Q2PLUGIN adjustmentdemoplugin)

project(${Q2PLUGIN} LANGUAGES CXX)

message("-- Linking ${Q2PLUGIN}")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# CMAKE_PREFIX_PATH By default this is empty. It is intended to be set by the project.
#
#set(CMAKE_PREFIX_PATH "/home/cepek/Qt/5.15.0/gcc_64/lib/cmake/Qt5/") # example linux
#set(CMAKE_PREFIX_PATH "C:/Qt/5.15.0/msvc2019_64/lib/cmake/Qt5/")     # example windows

find_package(Qt6 COMPONENTS Core Gui Sql Widgets REQUIRED)

set(Q2PATH ../../qgama)
include_directories(${Q2PATH} ../../gama/lib)

add_library(${Q2PLUGIN} SHARED
  ${Q2PATH}/qgamainterfaces.h
  ${Q2PLUGIN}.cpp
  ${Q2PLUGIN}.h
)

target_link_libraries(${Q2PLUGIN} PRIVATE
  Qt6::Core Qt6::Gui Qt6::Sql Qt6::Widgets
)

target_compile_definitions(${Q2PLUGIN} PRIVATE ${Q2PLUGIN}_LIBRARY)

set_target_properties(${Q2PLUGIN} PROPERTIES LINKER_LANGUAGE CXX)
