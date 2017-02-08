#-------------------------------------------------
#
# Project created by QtCreator 2016-11-20T22:59:02
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets \
    multimedia \
    multimediawidgets

RC_ICONS = robocenter.ico

TARGET = rov-control
TEMPLATE = app
win32{
    INCLUDEPATH = C:/SDL-VC/SDL-1.2.15/include/
    LIBS += -LC:/SDL-VC/SDL-1.2.15/lib/x64 -lSDL
    CONFIG += console
}
unix {
    LIBS += -lSDL2
}

SOURCES += main.cpp\
    mainwindow.cpp \
    messages.cpp \
    startwindow.cpp \
    robocamera.cpp \
    calcwindow.cpp \
    graphicsscene.cpp \
    basetool.cpp \
    figure.cpp \
    communicator.cpp \
    joystick.cpp

HEADERS  += mainwindow.h \
    messages.h \
    startwindow.h \
    robocamera.h \
    calcwindow.h \
    graphicsscene.h \
    basetool.h \
    figure.h \
    communicator.h \
    joystick.h

FORMS    += mainwindow.ui \
    startwindow.ui \
    calcwindow.ui
