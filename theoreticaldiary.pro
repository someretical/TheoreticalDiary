include(external-libs/someretical-o2/src/src.pri)
include(external-libs/asyncfuture/asyncfuture.pri)

INCLUDEPATH += external-libs/json/single_include/nlohmann \
    external-libs/cryptopp \

unix:LIBS += \
    -L/usr/lib/ -lcryptopp

macx:LIBS += \
    -L/usr/lib/

QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.15

ICON = images/osx_icons/icons.icns

win32:LIBS += \
    -l"external-libs/cryptopp/cryptlib"

QT += core gui charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/core/diaryholder.cpp \
    src/core/googlewrapper.cpp \
    src/core/internalmanager.cpp \
    src/core/theoreticaldiary.cpp \
    src/gui/aboutdialog.cpp \
    src/gui/apiresponse.cpp \
    src/gui/diaryeditor.cpp \
    src/gui/diaryentryviewer.cpp \
    src/gui/diarymenu.cpp \
    src/gui/diarypixels.cpp \
    src/gui/diarystats.cpp \
    src/gui/mainmenu.cpp \
    src/gui/mainwindow.cpp \
    src/gui/optionsmenu.cpp \
    src/gui/standaloneoptions.cpp \
    src/gui/styles/base/basestyle.cpp \
    src/gui/styles/base/phantomcolor.cpp \
    src/gui/styles/dark/darkstyle.cpp \
    src/gui/styles/light/lightstyle.cpp \
    src/main.cpp \
    src/util/alertlabel.cpp \
    src/util/custommessageboxes.cpp \
    src/util/encryptor.cpp \
    src/util/eventfilters.cpp \
    src/util/misc.cpp \
    src/util/runguard.cpp \
    src/util/zipper.cpp

HEADERS += \
    src/core/diaryholder.h \
    src/core/googlewrapper.h \
    src/core/internalmanager.h \
    src/core/theoreticaldiary.h \
    src/gui/aboutdialog.h \
    src/gui/apiresponse.h \
    src/gui/diaryeditor.h \
    src/gui/diaryentryviewer.h \
    src/gui/diarymenu.h \
    src/gui/diarypixels.h \
    src/gui/diarystats.h \
    src/gui/mainmenu.h \
    src/gui/mainwindow.h \
    src/gui/optionsmenu.h \
    src/gui/standaloneoptions.h \
    src/gui/styles/base/basestyle.h \
    src/gui/styles/base/phantomcolor.h \
    src/gui/styles/dark/darkstyle.h \
    src/gui/styles/light/lightstyle.h \
    src/util/alertlabel.h \
    src/util/custommessageboxes.h \
    src/util/diarycalendarbutton.h \
    src/util/diarycomparisonlabel.h \
    src/util/diarypixellabel.h \
    src/util/encryptor.h \
    src/util/eventfilters.h \
    src/util/hashcontroller.h \
    src/util/misc.h \
    src/util/passwordlineedit.h \
    src/util/runguard.h \
    src/util/zipper.h

FORMS += \
    src/gui/aboutdialog.ui \
    src/gui/apiresponse.ui \
    src/gui/diaryeditor.ui \
    src/gui/diaryentryviewer.ui \
    src/gui/diarymenu.ui \
    src/gui/diarypixels.ui \
    src/gui/diarystats.ui \
    src/gui/mainmenu.ui \
    src/gui/mainwindow.ui \
    src/gui/optionsmenu.ui \
    src/gui/standaloneoptions.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /usr/bin
!isEmpty(target.path): INSTALLS += target

win32:RC_ICONS = images/windows_icons/icon.ico
win32:VERSION = 1.0.0.0
win32:QMAKE_TARGET_COMPANY = someretical
win32:QMAKE_TARGET_PRODUCT = "TheoreticalDiary"
win32:QMAKE_TARGET_DESCRIPTION = "Digital diary."
win32:QMAKE_TARGET_COPYRIGHT = 2022

RESOURCES += \
    images/images.qrc \
    styles/styles.qrc \
    text/text.qrc
