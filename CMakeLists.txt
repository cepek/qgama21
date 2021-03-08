cmake_minimum_required(VERSION 3.7)

project(qgama LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# CMAKE_PREFIX_PATH  By default this is empty. It is intended to be set by
# the project. If you use Qt Creator to build the project, you can use
# CMakeLists.txt as it is.
#
#set(CMAKE_PREFIX_PATH "~/Qt/6.0.1/gcc_64/")           # example linux
#
# or set up the environment variable CMAKE_PREFIX_PATH, for example
#
#   $ export CMAKE_PREFIX_PATH=~/Qt/6.0.1/gcc_64/
#   $ cmake .. -G Ninja
#   $ cmake --build .
#
#set(CMAKE_PREFIX_PATH "C:/Qt/6.0.1/msvc2019_64/")     # example windows

# QtCreator supports the following variables for Android, which are
# identical to qmake Android variables.
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
include_directories(gama/lib qgama)

find_package(Qt6 COMPONENTS
  Core Gui Sql Widgets PrintSupport SvgWidgets REQUIRED)

set(Q2SRC
    qgama/main.cpp
    qgama/adjustment.cpp
    qgama/adjustment.h
    qgama/adjustmentresults.cpp
    qgama/adjustmentresults.h
    qgama/constants.cpp
    qgama/constants.h
    qgama/dbconnectdialog.cpp
    qgama/dbconnectdialog.h
    qgama/dbfunctions.cpp
    qgama/dbfunctions.h
    qgama/drawsettings.cpp
    qgama/drawsettings.h
    qgama/qgamacontrolpanel.cpp
    qgama/qgamacontrolpanel.h
    qgama/qgamahelp.cpp
    qgama/qgamahelp.h
    qgama/qgamainterfaces.h
    qgama/importconfigurationfile.cpp
    qgama/importconfigurationfile.h
    qgama/insertclusterdialog.cpp
    qgama/insertclusterdialog.h
    qgama/insertobservationdialog.cpp
    qgama/insertobservationdialog.h
    qgama/lineeditdelegate.cpp
    qgama/lineeditdelegate.h
    qgama/networkadjustmentpanel.cpp
    qgama/networkadjustmentpanel.h
    qgama/networksvg.cpp
    qgama/networksvg.h
    qgama/observationeditor.cpp
    qgama/observationeditor.h
    qgama/observationtablemodel.cpp
    qgama/observationtablemodel.h
    qgama/parametereditor.cpp
    qgama/parametereditor.h
    qgama/pointeditor.cpp
    qgama/pointeditor.h
    qgama/pointtablemodel.cpp
    qgama/pointtablemodel.h
    qgama/pointtypecombobox.cpp
    qgama/pointtypecombobox.h
    qgama/selectadjresultslanguage.cpp
    qgama/selectadjresultslanguage.h
    qgama/selectconfiguration.cpp
    qgama/selectconfiguration.h
    qgama/showmessage.h
    qgama/shrinkbandwidth.cpp
    qgama/shrinkbandwidth.h
    qgama/tableviewstyle.cpp
    qgama/tableviewstyle.h
    qgama/xml2txt.cpp
    qgama/xml2txt.h
    qgama/qgama.qrc
    )

if(ANDROID)
  add_library(qgama SHARED
    ${Q2SRC}
    $<TARGET_OBJECTS:libgama>
  )
else()
  add_executable(qgama
    ${Q2SRC}
    $<TARGET_OBJECTS:libgama>
  )
endif()

target_link_libraries(qgama PRIVATE
  Qt6::Widgets Qt6::Sql Qt6::PrintSupport Qt6::SvgWidgets)

add_subdirectory(gama)  # git clone https://git.savannah.gnu.org/git/gama.git
