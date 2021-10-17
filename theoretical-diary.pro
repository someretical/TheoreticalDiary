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
    placeholder.cpp \
    promptauth.cpp \
    promptpassword.cpp \
    runguard.cpp \
    saveerror.cpp \
    settingsprovider.cpp \
    theoreticalcalendar.cpp \
    theoreticaldiary.cpp \
    unknowndiaryformat.cpp \
    unsavedchanges.cpp \
    updatepassword.cpp \
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
    placeholder.h \
    promptauth.h \
    promptpassword.h \
    runguard.h \
    saveerror.h \
    settingsprovider.h \
    theoreticalcalendar.h \
    theoreticaldiary.h \
    unknowndiaryformat.h \
    unsavedchanges.h \
    updatepassword.h \
    zipper.h

FORMS += \
    aboutwindow.ui \
    autherrorwindow.ui \
    confirmoverwrite.ui \
    diarywindow.ui \
    flushwindow.ui \
    mainwindow.ui \
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
    fonts/Roboto-Black.ttf \
    fonts/Roboto-BlackItalic.ttf \
    fonts/Roboto-Bold.ttf \
    fonts/Roboto-BoldItalic.ttf \
    fonts/Roboto-Italic.ttf \
    fonts/Roboto-Light.ttf \
    fonts/Roboto-LightItalic.ttf \
    fonts/Roboto-Medium.ttf \
    fonts/Roboto-MediumItalic.ttf \
    fonts/Roboto-Regular.ttf \
    fonts/Roboto-Thin.ttf \
    fonts/Roboto-ThinItalic.ttf \
    fonts/RobotoCondensed-Bold.ttf \
    fonts/RobotoCondensed-BoldItalic.ttf \
    fonts/RobotoCondensed-Italic.ttf \
    fonts/RobotoCondensed-Light.ttf \
    fonts/RobotoCondensed-LightItalic.ttf \
    fonts/RobotoCondensed-Regular.ttf \
    images/disabled/base.svg \
    images/disabled/branch-closed.svg \
    images/disabled/branch-end.svg \
    images/disabled/branch-more.svg \
    images/disabled/branch-open.svg \
    images/disabled/checkbox_checked.svg \
    images/disabled/checkbox_checked_invert.svg \
    images/disabled/checkbox_indeterminate.svg \
    images/disabled/checkbox_indeterminate_invert.svg \
    images/disabled/checkbox_unchecked.svg \
    images/disabled/checkbox_unchecked_invert.svg \
    images/disabled/checklist.svg \
    images/disabled/checklist_indeterminate.svg \
    images/disabled/checklist_indeterminate_invert.svg \
    images/disabled/checklist_invert.svg \
    images/disabled/close.svg \
    images/disabled/downarrow.svg \
    images/disabled/downarrow2.svg \
    images/disabled/float.svg \
    images/disabled/leftarrow.svg \
    images/disabled/leftarrow2.svg \
    images/disabled/radiobutton_checked.svg \
    images/disabled/radiobutton_unchecked.svg \
    images/disabled/rightarrow.svg \
    images/disabled/rightarrow2.svg \
    images/disabled/sizegrip.svg \
    images/disabled/slider.svg \
    images/disabled/splitter-horizontal.svg \
    images/disabled/splitter-vertical.svg \
    images/disabled/tab_close.svg \
    images/disabled/toolbar-handle-horizontal.svg \
    images/disabled/toolbar-handle-vertical.svg \
    images/disabled/uparrow.svg \
    images/disabled/uparrow2.svg \
    images/disabled/vline.svg \
    images/primary/base.svg \
    images/primary/branch-closed.svg \
    images/primary/branch-end.svg \
    images/primary/branch-more.svg \
    images/primary/branch-open.svg \
    images/primary/checkbox_checked.svg \
    images/primary/checkbox_checked_invert.svg \
    images/primary/checkbox_indeterminate.svg \
    images/primary/checkbox_indeterminate_invert.svg \
    images/primary/checkbox_unchecked.svg \
    images/primary/checkbox_unchecked_invert.svg \
    images/primary/checklist.svg \
    images/primary/checklist_indeterminate.svg \
    images/primary/checklist_indeterminate_invert.svg \
    images/primary/checklist_invert.svg \
    images/primary/close.svg \
    images/primary/downarrow.svg \
    images/primary/downarrow2.svg \
    images/primary/float.svg \
    images/primary/leftarrow.svg \
    images/primary/leftarrow2.svg \
    images/primary/radiobutton_checked.svg \
    images/primary/radiobutton_unchecked.svg \
    images/primary/rightarrow.svg \
    images/primary/rightarrow2.svg \
    images/primary/sizegrip.svg \
    images/primary/slider.svg \
    images/primary/splitter-horizontal.svg \
    images/primary/splitter-vertical.svg \
    images/primary/tab_close.svg \
    images/primary/toolbar-handle-horizontal.svg \
    images/primary/toolbar-handle-vertical.svg \
    images/primary/uparrow.svg \
    images/primary/uparrow2.svg \
    images/primary/vline.svg \
    styles/aboutwindow.qss \
    styles/defaultwindow.qss \
    styles/diarywindow.qss \
    styles/filedialog.qss \
    styles/mainwindow.qss \
    styles/material_cyan_dark.qss \
    styles/promptpassword.qss \
    styles/s_default.qss \
    styles/theoreticalcalendar.qss \
    styles/updatepassword.qss \
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
