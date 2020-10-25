#-------------------------------------------------
#
# Project created by QtCreator 2019-04-14T08:36:23
#
#-------------------------------------------------

QT       += core gui
QT       += axcontainer
QT       += core gui multimedia
QT       += widgets gui
QT       += charts      #chart绘图必备
QT       += serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = VeinPunctureSystem
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

INCLUDEPATH += \
C:\opencv\opencv\build\include\opencv2\
C:\opencv\opencv\build\include\opencv\
C:\opencv\opencv\build\include

LIBS += \
C:\opencv\opencv\build\x64\vc15\lib\opencv_world400d.lib
C:\opencv\opencv\build\x64\vc15\lib\opencv_world400.lib

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    triopc.cpp \
    myalgorithm.cpp \
    safethread.cpp \
    flatui.cpp \
    loginform.cpp \
    myaxesdriver.cpp \
    tozerothread.cpp \
    recordthread.cpp \
    updatepicthread.cpp

HEADERS += \
        mainwindow.h \
    triopc.h \
    myalgorithm.h \
    safethread.h \
    flatui.h \
    loginform.h \
    loginform.h \
    myaxesdriver.h \
    tozerothread.h \
    recordthread.h \
    updatepicthread.h

FORMS += \
        mainwindow.ui \
    loginform.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=

RESOURCES += \
    image.qrc
