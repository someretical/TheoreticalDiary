cmake_minimum_required(VERSION 3.5)

project("Theoretical Diary" LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ZLIB REQUIRED)
if(ZLIB_VERSION_STRING VERSION_LESS "1.2.0")
    message(FATAL_ERROR "zlib 1.2.0 or higher is required to use the gzip format")
endif()
include_directories(SYSTEM ${ZLIB_INCLUDE_DIR})

find_package(Qt5 COMPONENTS Widgets Charts REQUIRED)

add_executable(theoreticaldiary
        src/core/Application.cpp
        src/core/Application.h
        src/core/AsyncTask.h
        src/core/Config.cpp
        src/core/Config.h
        src/core/Constants.h
        src/core/Diary.cpp
        src/core/Diary.h
        src/core/RunGuard.cpp
        src/core/RunGuard.h
        src/core/TimeDelta.cpp
        src/core/TimeDelta.h
        src/core/Util.cpp
        src/core/Util.h
        src/crypto/Cipher.cpp
        src/crypto/Cipher.h
        src/crypto/DiaryKey.cpp
        src/crypto/DiaryKey.h
        src/crypto/Hash.cpp
        src/crypto/Hash.h
        src/crypto/Random.cpp
        src/crypto/Random.h
        src/crypto/botan_all.cpp
        src/crypto/botan_all.h
        src/gui/CategoryListWidget.cpp
        src/gui/CategoryListWidget.h
        src/gui/CategoryListWidget.ui
        src/gui/ContextMenuOverride.h
        src/gui/DiaryTabWidget.cpp
        src/gui/DiaryTabWidget.h
        src/gui/DiaryTabWidget.ui
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
        src/gui/SettingsWidget.cpp
        src/gui/SettingsWidget.h
        src/gui/SettingsWidget.ui
        src/gui/diary_menu/DiaryMainMenuWidget.cpp
        src/gui/diary_menu/DiaryMainMenuWidget.h
        src/gui/diary_menu/DiaryMainMenuWidget.ui
        src/gui/diary_menu/DiaryUnlockWidget.cpp
        src/gui/diary_menu/DiaryUnlockWidget.h
        src/gui/diary_menu/DiaryUnlockWidget.ui
        src/gui/diary_menu/DiaryWidget.cpp
        src/gui/diary_menu/DiaryWidget.h
        src/gui/diary_menu/DiaryWidget.ui
        src/gui/diary_menu/calendar/DiaryCalendar.cpp
        src/gui/diary_menu/calendar/DiaryCalendar.h
        src/gui/diary_menu/calendar/DiaryCalendar.ui
        src/gui/diary_menu/calendar/MonthTableWidget.cpp
        src/gui/diary_menu/calendar/MonthTableWidget.h
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
        src/resources.qrc
        src/icons/icons.qrc
        src/streams/LayeredStream.cpp
        src/streams/LayeredStream.h
        src/streams/StoreDataStream.cpp
        src/streams/StoreDataStream.h
        src/streams/SymmetricCipherStream.cpp
        src/streams/SymmetricCipherStream.h
        src/streams/qtiocompressor.cpp
        src/streams/qtiocompressor.h
        src/gui/diary_menu/EntryEditor.cpp
        src/gui/diary_menu/EntryEditor.h
        src/gui/diary_menu/EntryEditor.ui
        src/gui/diary_menu/text_editor/markdownhighlighter.h
        src/gui/diary_menu/text_editor/qmarkdowntextedit.cpp
        src/gui/diary_menu/text_editor/markdownhighlighter.cpp
        src/gui/diary_menu/text_editor/qplaintexteditsearchwidget.h
        src/gui/diary_menu/text_editor/qplaintexteditsearchwidget.ui
        src/gui/diary_menu/text_editor/qplaintexteditsearchwidget.cpp
        src/gui/diary_menu/text_editor/qmarkdowntextedit.h
        src/gui/diary_menu/text_editor/qownlanguagedata.h
        src/gui/diary_menu/text_editor/qownlanguagedata.cpp
        src/gui/diary_menu/text_editor/linenumberarea.h
        src/gui/diary_menu/text_editor/MarkdownTextEditor.cpp
        src/gui/diary_menu/text_editor/MarkdownTextEditor.h
        src/gui/diary_menu/text_editor/MarkdownTextEditor.ui)

add_subdirectory(external-libs/CuteLogger)

target_include_directories(theoreticaldiary PRIVATE src/)

target_link_libraries(theoreticaldiary
        PRIVATE
        Qt5::Widgets
        CuteLogger
        ${ZLIB_LIBRARIES})
