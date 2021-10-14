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

INCLUDEPATH += external-libs/json/single_include/nlohmann \
    external-libs/cryptopp

unix:LIBS += \
    -l:libcryptopp.a

win32:LIBS += \
    -l"external-libs/cryptopp/cryptlib"

QT += core gui network networkauth

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    aboutwindow.cpp \
    autherrorwindow.cpp \
    confirmoverwrite.cpp \
    diaryholder.cpp \
    diarywindow.cpp \
    encryptor.cpp \
    flushwindow.cpp \
    googlewrapper.cpp \
    main.cpp \
    mainwindow.cpp \
    nodiaryfound.cpp \
    promptauth.cpp \
    promptpassword.cpp \
    runguard.cpp \
    settingsprovider.cpp \
    theoreticaldiary.cpp \
    unknowndiaryformat.cpp \
    zipper.cpp

HEADERS += \
    aboutwindow.h \
    autherrorwindow.h \
    confirmoverwrite.h \
    diaryholder.h \
    diarywindow.h \
    encryptor.h \
    flushwindow.h \
    googlewrapper.h \
    mainwindow.h \
    nodiaryfound.h \
    promptauth.h \
    promptpassword.h \
    runguard.h \
    settingsprovider.h \
    theoreticaldiary.h \
    unknowndiaryformat.h \
    zipper.h

FORMS += \
    aboutwindow.ui \
    autherrorwindow.ui \
    confirmoverwrite.ui \
    diarywindow.ui \
    flushwindow.ui \
    mainwindow.ui \
    nodiaryfound.ui \
    promptauth.ui \
    promptpassword.ui \
    unknowndiaryformat.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:RC_ICONS = images/icon.ico

RESOURCES += \
    theoretical-diary.qrc

DISTFILES += \
    LICENSE \
    README.md \
    styles/defaultwindow.qss \
    styles/mainwindow.qss \
    styles/promptpassword.qss \
    text/CONTRIBUTORS.txt \
    text/LICENSES.txt \
    text/VERSION.txt \
    fonts/Ubuntu-Bold.ttf \
    fonts/Ubuntu-BoldItalic.ttf \
    fonts/Ubuntu-Italic.ttf \
    fonts/Ubuntu-Light.ttf \
    fonts/Ubuntu-LightItalic.ttf \
    fonts/Ubuntu-Medium.ttf \
    fonts/Ubuntu-MediumItalic.ttf \
    fonts/Ubuntu-Regular.ttf \
    images/icon.ico \
    images/icon.png \
    images/icon.svg

OTHER_FILES += \
    .clang-format \
    .github/workflows/linux.yml \
    .github/workflows/windows.yml \
    .gitignore \
    .gitmodules
