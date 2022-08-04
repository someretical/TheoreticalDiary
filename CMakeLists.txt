cmake_minimum_required(VERSION 3.5)

project("Theoretical Diary" LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets Charts REQUIRED)

add_executable(theoreticaldiary
        src/styling/BaseStyle.cpp
        src/styling/BaseStyle.h
        src/styling/PhantomColour.cpp
        src/styling/PhantomColour.h
        src/styling/DarkStyle.cpp
        src/styling/DarkStyle.h
        src/styling/LightStyle.cpp
        src/styling/LightStyle.h
        src/util/Util.cpp
        src/util/Util.h
        src/core/RunGuard.cpp
        src/core/RunGuard.h
        src/main.cpp
        src/MainWindow.cpp
        src/MainWindow.h
        src/MainWindow.ui
        src/resources.qrc
        src/util/Constants.h
        src/styling/StyleManager.cpp
        src/styling/StyleManager.h src/util/AsyncTask.h src/core/Icons.cpp src/core/Icons.h src/util/Font.cpp src/util/Font.h)

add_subdirectory(external-libs/CuteLogger)

target_include_directories(theoreticaldiary PRIVATE src/)

target_link_libraries(theoreticaldiary
        PRIVATE
            Qt5::Widgets
            Qt5::Charts
            CuteLogger)