# This file is part of theoretical-diary.
#
# theoretical-diary is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# theoretical-diary is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with theoretical-diary.  If not, see <https://www.gnu.org/licenses/>.

INCLUDEPATH += external-libs/json/single_include/nlohmann

# The lib version of networkauth is installed by default.
# To install the dev version needed for compiling, run $ sudo apt install libqt5networkauth5-dev
QT += core gui network networkauth

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11 ordered

include(external-libs/qt-secret/src/Qt-Secret.pri)
include(external-libs/qtkeychain/qtkeychain.pri)

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    aboutwindow.cpp \
    autherrorwindow.cpp \
    diaryholder.cpp \
    encryptor.cpp \
    flushwindow.cpp \
    googlewrapper.cpp \
    main.cpp \
    mainwindow.cpp \
    runguard.cpp \
    settingsprovider.cpp \
    theoreticaldiary.cpp

HEADERS += \
    aboutwindow.h \
    autherrorwindow.h \
    diaryholder.h \
    encryptor.h \
    flushwindow.h \
    googlewrapper.h \
    mainwindow.h \
    runguard.h \
    settingsprovider.h \
    theoreticaldiary.h

FORMS += \
    aboutwindow.ui \
    autherrorwindow.ui \
    flushwindow.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RC_ICONS = images/icon.ico

RESOURCES += \
    theoretical-diary.qrc

DISTFILES += \
    .gitmodules \
    LICENSE \
    README.md

OTHER_FILES += \
    .clang-format \
    .github/workflows/build.yml \
    .gitignore \
    images/icon.ico \
    images/icon.png \
    images/icon.svg
