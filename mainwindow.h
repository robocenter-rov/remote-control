#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ui_mainwindow.h"
#ifdef Q_OS_WIN32
    #include <SDL.h>
#endif
#ifdef Q_OS_WIN64
    #include <SDL.h>
#endif
#ifdef Q_OS_LINUX
    #include <SDL2/SDL.h>
#endif

#undef main
#include <QTimer>
#include "messages.h"
#include <QByteArray>
#include <QDataStream>
#include <QCamera>
#include <QCameraInfo>
#include <QWidget>
#include <QtMultimediaWidgets>
#include <QtGui/QtGui>
#include "robocamera.h"
#include "communicator.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();    
private slots:
    void readAndSendJoySensors();
private:
    void joyInit();
    void cameraInit();
    void loadQSS();
    bool eventFilter(QObject *, QEvent *event);

    Ui::MainWindow *_ui;
    SDL_Joystick *_joy;
    QTimer *_joyTimer;
    RoboCamera *_mainCamera;
    Communicator *_communicator;
};

#endif // MAINWINDOW_H
