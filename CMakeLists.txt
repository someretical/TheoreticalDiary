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
        src/core/Application.cpp
        src/core/Application.h
        src/core/AsyncTask.h
        src/core/Config.cpp
        src/core/Config.h
        src/core/Constants.h
        src/core/RunGuard.cpp
        src/core/RunGuard.h
        src/core/Util.cpp
        src/core/Util.h
        src/gui/DiaryTabWidget.cpp
        src/gui/DiaryTabWidget.h
        src/gui/DiaryTabWidget.ui
        src/gui/editor/DiaryMainMenuWidget.cpp
        src/gui/editor/DiaryMainMenuWidget.h
        src/gui/editor/DiaryMainMenuWidget.ui
        src/gui/editor/DiaryWidget.cpp
        src/gui/editor/DiaryWidget.h
        src/gui/editor/DiaryWidget.ui
        src/gui/editor/DiaryUnlockWidget.cpp
        src/gui/editor/DiaryUnlockWidget.h
        src/gui/editor/DiaryUnlockWidget.ui
        src/gui/Font.cpp
        src/gui/Font.h
        src/gui/Icons.cpp
        src/gui/Icons.h
        src/gui/KMessageWidget.cpp
        src/gui/KMessageWidget.h
        src/gui/MainMenu.cpp
        src/gui/MainMenu.h
        src/gui/MainMenu.ui
        src/gui/MainWindow.cpp
        src/gui/MainWindow.h
        src/gui/MainWindow.ui
        src/gui/MessageWidget.cpp
        src/gui/MessageWidget.h
        src/gui/PasswordLineEdit.cpp
        src/gui/PasswordLineEdit.h
        src/gui/styling/BaseStyle.cpp
        src/gui/styling/BaseStyle.h
        src/gui/styling/DarkStyle.cpp
        src/gui/styling/DarkStyle.h
        src/gui/styling/LightStyle.cpp
        src/gui/styling/LightStyle.h
        src/gui/styling/PhantomColour.cpp
        src/gui/styling/PhantomColour.h
        src/gui/styling/StyleManager.cpp
        src/gui/styling/StyleManager.h
        src/main.cpp
        src/resources.qrc)

add_subdirectory(external-libs/CuteLogger)

target_include_directories(theoreticaldiary PRIVATE src/)

target_link_libraries(theoreticaldiary
        PRIVATE
        Qt5::Widgets
        Qt5::Charts
        CuteLogger)
