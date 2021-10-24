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
    changepanealert.cpp \
    confirmdelete.cpp \
    confirmoverwrite.cpp \
    diaryentrylist.cpp \
    diaryholder.cpp \
    diarywindow.cpp \
    encryptor.cpp \
    flushwindow.cpp \
    googlewrapper.cpp \
    main.cpp \
    mainwindow.cpp \
    missingpermissions.cpp \
    nodiaryfound.cpp \
    placeholder.cpp \
    promptauth.cpp \
    promptpassword.cpp \
    runguard.cpp \
    saveerror.cpp \
    theoreticalcalendar.cpp \
    theoreticaldiary.cpp \
    unknowndiaryformat.cpp \
    unsavedchanges.cpp \
    updatepassword.cpp \
    zipper.cpp

HEADERS += \
    aboutwindow.h \
    autherrorwindow.h \
    changepanealert.h \
    confirmdelete.h \
    confirmoverwrite.h \
    diaryentrylist.h \
    diaryholder.h \
    diarywindow.h \
    encryptor.h \
    flushwindow.h \
    googlewrapper.h \
    mainwindow.h \
    missingpermissions.h \
    nodiaryfound.h \
    placeholder.h \
    promptauth.h \
    promptpassword.h \
    runguard.h \
    saveerror.h \
    theoreticalcalendar.h \
    theoreticaldiary.h \
    unknowndiaryformat.h \
    unsavedchanges.h \
    updatepassword.h \
    zipper.h

FORMS += \
    aboutwindow.ui \
    autherrorwindow.ui \
    changepanealert.ui \
    confirmdelete.ui \
    confirmoverwrite.ui \
    diaryentrylist.ui \
    diarywindow.ui \
    flushwindow.ui \
    mainwindow.ui \
    missingpermissions.ui \
    nodiaryfound.ui \
    placeholder.ui \
    promptauth.ui \
    promptpassword.ui \
    saveerror.ui \
    theoreticalcalendar.ui \
    unknowndiaryformat.ui \
    unsavedchanges.ui \
    updatepassword.ui

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
    styles/aboutwindow.qss \
    styles/confirmoverwrite.qss \
    styles/day_label_bad.qss \
    styles/day_label_base.qss \
    styles/day_label_good.qss \
    styles/day_label_ok.qss \
    styles/day_label_starred.qss \
    styles/day_label_very_bad.qss \
    styles/day_label_very_good.qss \
    styles/diaryeditor.qss \
    styles/diaryentrylist.qss \
    styles/diarywindow.qss \
    styles/filedialog.qss \
    styles/mainwindow.qss \
    styles/material_cyan_dark.qss \
    styles/promptpassword.qss \
    styles/s_bad.qss \
    styles/s_base.qss \
    styles/s_default.qss \
    styles/s_good.qss \
    styles/s_ok.qss \
    styles/s_selected.qss \
    styles/s_star_black.qss \
    styles/s_star_white.qss \
    styles/s_very_bad.qss \
    styles/s_very_good.qss \
    styles/theoreticalcalendar.qss \
    styles/updatepassword.qss \
    text/CONTRIBUTORS.txt \
    text/LICENSES.txt \
    text/VERSION.txt \

OTHER_FILES += \
    .clang-format \
    .github/workflows/linux.yml \
    .github/workflows/windows.yml \
    .gitignore \
    .gitmodules
