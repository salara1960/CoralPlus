#-------------------------------------------------
#
# Project created by QtCreator 2019-01-28T20:13:34
#
#-------------------------------------------------

QMAKE_CXX += -std=gnu++11 -O2 -masm=att

QT = core gui network


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CoralPlusCli
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    itDialog.cpp \
    calc.cpp

HEADERS += \
        mainwindow.h \
    build_number.h \
    itDialog.h \
    calc.h \
    global.h

FORMS += \
        mainwindow.ui \
    itDialog.ui \
    calc.ui \
    itInfo.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    icons.qrc


