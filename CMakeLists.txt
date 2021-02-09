cmake_minimum_required(VERSION 3.7)

project(gama-q2 LANGUAGES CXX)

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

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check http://doc.qt.io/qt-5/deployment-android.html for more information.
# They need to be set before the find_package(Qt5 ...) call.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

add_definitions(-DGNU_gama_expat_1_1)  # if we use local copy of expat sources
include_directories(gama/lib gama-q2)

find_package(Qt6 COMPONENTS Core Gui Sql Widgets PrintSupport Core5Compat SvgWidgets REQUIRED)

set(Q2SRC
    gama-q2/main.cpp
    gama-q2/adjustment.cpp
    gama-q2/adjustment.h
    gama-q2/adjustmentresults.cpp
    gama-q2/adjustmentresults.h
    gama-q2/constants.cpp
    gama-q2/constants.h
    gama-q2/dbconnectdialog.cpp
    gama-q2/dbconnectdialog.h
    gama-q2/dbfunctions.cpp
    gama-q2/dbfunctions.h
    gama-q2/drawsettings.cpp
    gama-q2/drawsettings.h
    gama-q2/gamaq2controlpanel.cpp
    gama-q2/gamaq2controlpanel.h
    gama-q2/gamaq2help.cpp
    gama-q2/gamaq2help.h
    gama-q2/gamaq2interfaces.h
    gama-q2/importconfigurationfile.cpp
    gama-q2/importconfigurationfile.h
    gama-q2/insertclusterdialog.cpp
    gama-q2/insertclusterdialog.h
    gama-q2/insertobservationdialog.cpp
    gama-q2/insertobservationdialog.h
    gama-q2/lineeditdelegate.cpp
    gama-q2/lineeditdelegate.h
    gama-q2/networkadjustmentpanel.cpp
    gama-q2/networkadjustmentpanel.h
    gama-q2/networksvg.cpp
    gama-q2/networksvg.h
    gama-q2/observationeditor.cpp
    gama-q2/observationeditor.h
    gama-q2/observationtablemodel.cpp
    gama-q2/observationtablemodel.h
    gama-q2/parametereditor.cpp
    gama-q2/parametereditor.h
    gama-q2/pointeditor.cpp
    gama-q2/pointeditor.h
    gama-q2/pointtablemodel.cpp
    gama-q2/pointtablemodel.h
    gama-q2/pointtypecombobox.cpp
    gama-q2/pointtypecombobox.h
    gama-q2/selectadjresultslanguage.cpp
    gama-q2/selectadjresultslanguage.h
    gama-q2/selectconfiguration.cpp
    gama-q2/selectconfiguration.h
    gama-q2/showmessage.h
    gama-q2/shrinkbandwidth.cpp
    gama-q2/shrinkbandwidth.h
    gama-q2/tableviewstyle.cpp
    gama-q2/tableviewstyle.h
    gama-q2/xml2txt.cpp
    gama-q2/xml2txt.h
    gama-q2/gama-q2.qrc
    )

if(ANDROID)
  add_library(gama-q2 SHARED
    ${Q2SRC}
    $<TARGET_OBJECTS:libgama>
  )
else()
  add_executable(gama-q2
    ${Q2SRC}
    $<TARGET_OBJECTS:libgama>
  )
endif()

target_link_libraries(gama-q2 PRIVATE Qt6::Widgets Qt6::Sql Qt6::PrintSupport Qt6::Core5Compat Qt6::SvgWidgets)

add_subdirectory(gama)
