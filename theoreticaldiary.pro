# This file is part of Theoretical Diary.
#
# Theoretical Diary is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Theoretical Diary is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with Theoretical Diary.  If not, see <https://www.gnu.org/licenses/>.

include(external-libs/o2/src/src.pri)

INCLUDEPATH += external-libs/json/single_include/nlohmann \
    external-libs/cryptopp \

unix:LIBS += \
    -L/usr/lib/ -lcryptopp

win32:LIBS += \
    -l"external-libs/cryptopp/cryptlib"

QT += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/gui/aboutdialog.cpp \
    src/core/diaryholder.cpp \
    src/core/googlewrapper.cpp \
    src/core/theoreticaldiary.cpp \
    src/gui/diaryeditor.cpp \
    src/gui/diarymenu.cpp \
    src/gui/mainmenu.cpp \
    src/gui/mainwindow.cpp \
    src/gui/promptpassword.cpp \
    src/gui/updatepassword.cpp \
    src/main.cpp \
    src/util/encryptor.cpp \
    src/util/runguard.cpp \
    src/util/zipper.cpp

HEADERS += \
    src/gui/aboutdialog.h \
    src/core/diaryholder.h \
    src/core/googlewrapper.h \
    src/core/theoreticaldiary.h \
    src/gui/diaryeditor.h \
    src/gui/diarymenu.h \
    src/gui/mainmenu.h \
    src/gui/mainwindow.h \
    src/gui/promptpassword.h \
    src/gui/updatepassword.h \
    src/util/encryptor.h \
    src/util/runguard.h \
    src/util/zipper.h

FORMS += \
    src/gui/aboutdialog.ui \
    src/gui/diaryeditor.ui \
    src/gui/diarymenu.ui \
    src/gui/mainmenu.ui \
    src/gui/mainwindow.ui \
    src/gui/promptpassword.ui \
    src/gui/updatepassword.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:RC_ICONS = images/windows_icons/icon.ico
win32:VERSION = 1.0.0.0
win32:QMAKE_TARGET_COMPANY = someretical
win32:QMAKE_TARGET_PRODUCT = "Theoretical Diary"
win32:QMAKE_TARGET_DESCRIPTION = "Digital diary"
win32:QMAKE_TARGET_COPYRIGHT = 2021

RESOURCES += \
    fonts/fonts.qrc \
    images/images.qrc \
    styles/styles.qrc \
    text/text.qrc

DISTFILES += \
    styles/dark/unsavedchanges.qss
