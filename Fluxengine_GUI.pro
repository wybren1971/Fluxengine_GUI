QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
unix {QMAKE_LFLAGS += -no-pie} #added so mimetype on linux is an executable and not a shared library

VERSION = 0.7
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

APP_NAME = "%{FLuxengine_GUI}"
COMPANY_NAME = "Wybren van Duinen"

# add defines
DEFINES += AppName='"\\\\\\"$$APP_NAME\\\\\\""'
DEFINES += CompanyName='"\\\\\\"COMPANY_NAME\\\\\\""'

#DEFINES += "BUILDDATE=$$system(date -R)"
DEFINES += "BUILDDATE=$$system(date +%Y-%m-%d)"
#DEFINES += BuildDate=\\\\\\"$$BUILD_DATE\\\\\\"
#CONFIG += static

#static { #//Everything below takes effect with CONFIG += static

#    CONFIG += static
    #QTPLUGIN += qsqloci qgif

#    DEFINES += STATIC #// Equivalent to "#define STATIC" in source code
#    message("Static build.")
#}
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    dialogpreferences.cpp \
    fluxengine.cpp \
    main.cpp \
    mainwindow.cpp \
    showlayout.cpp \
    wizard.cpp

HEADERS += \
    dialogpreferences.h \
    fluxengine.h \
    mainwindow.h \
    showlayout.h \
    wizard.h

FORMS += \
    dialogpreferences.ui \
    mainwindow.ui \
    showlayout.ui

# Default rules for deployment.

#unix:{
    # suppress the default RPATH if you wish
    #QMAKE_LFLAGS_RPATH=

    #QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN/lib\'"
#    QMAKE_LFLAGS += -Wl,-rpath,"'\$$ORIGIN'"
#}

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
