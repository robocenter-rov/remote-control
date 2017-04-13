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
    INCLUDEPATH = $$PWD/SDL-1.2.15/include/
    LIBS += -L$$PWD/SDL-1.2.15/lib/x64 -lSDL
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
    joystick.cpp \
    remote-control-library/Communicator.cpp \
    remote-control-library/ConnectionProvider.cpp \
    remote-control-library/DataReader.cpp \
    remote-control-library/RingBuffer.cpp \
    remote-control-library/SimpleCommunicator.cpp \
    remote-control-library/UARTConnectionProviderWindows.cpp

HEADERS  += mainwindow.h \
    messages.h \
    startwindow.h \
    robocamera.h \
    calcwindow.h \
    graphicsscene.h \
    basetool.h \
    figure.h \
    communicator.h \
    joystick.h \
    remote-control-library/Communicator.h \
    remote-control-library/ConnectionProvider.h \
    remote-control-library/DataReader.h \
    remote-control-library/Exception.h \
    remote-control-library/RingBuffer.h \
    remote-control-library/SimpleCommunicator.h \
    remote-control-library/UARTConnectionProviderWindows.h \
    remote-control-library/Utils.h

FORMS    += mainwindow.ui \
    startwindow.ui \
    calcwindow.ui
