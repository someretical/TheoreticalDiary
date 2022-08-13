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
        src/gui/styling/BaseStyle.cpp
        src/gui/styling/BaseStyle.h
        src/gui/styling/PhantomColour.cpp
        src/gui/styling/PhantomColour.h
        src/gui/styling/DarkStyle.cpp
        src/gui/styling/DarkStyle.h
        src/gui/styling/LightStyle.cpp
        src/gui/styling/LightStyle.h
        src/core/Util.cpp
        src/core/Util.h
        src/core/RunGuard.cpp
        src/core/RunGuard.h
        src/main.cpp
        src/gui/MainWindow.cpp
        src/gui/MainWindow.h
        src/gui/MainWindow.ui
        src/resources.qrc
        src/core/Constants.h
        src/gui/styling/StyleManager.cpp
        src/gui/styling/StyleManager.h
        src/core/AsyncTask.h
        src/gui/Icons.cpp
        src/gui/Icons.h
        src/gui/Font.cpp
        src/gui/Font.h
        src/gui/MainMenu.cpp
        src/gui/MainMenu.h
        src/gui/MainMenu.ui
        src/gui/DiaryTabWidget.cpp
        src/gui/DiaryTabWidget.h
        src/gui/DiaryTabWidget.ui
        src/gui/DiaryWidget.cpp
        src/gui/DiaryWidget.h
        src/gui/DiaryWidget.ui
        src/gui/KMessageWidget.h
        src/gui/KMessageWidget.cpp
        src/gui/MessageWidget.cpp
        src/gui/MessageWidget.h 
        src/core/Config.cpp 
        src/core/Config.h)

add_subdirectory(external-libs/CuteLogger)

target_include_directories(theoreticaldiary PRIVATE src/)

target_link_libraries(theoreticaldiary
        PRIVATE
            Qt5::Widgets
            Qt5::Charts
            CuteLogger)
