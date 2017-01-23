#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ui_mainwindow.h"
#include <QUdpSocket>
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
#define IP_ADDR "192.168.1.177"
#define PORT 8888

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();    

public slots:
    void readMessage();

private slots:
    void onSendButtonClicked();
    void readAndSendJoySensors();

private:
    void joyInit();
    void cameraInit();
    bool eventFilter(QObject *, QEvent *event);
    void loadQSS();
    template<typename T>
    void sendMessage(T msg)
    {
        QByteArray buff;
        QDataStream ds(&buff, QIODevice::WriteOnly);
        ds.setByteOrder(QDataStream::LittleEndian);
        ds << msg;
        _socket->writeDatagram(buff, QHostAddress(IP_ADDR), PORT);
    }

    Ui::MainWindow *_ui;
    QUdpSocket *_socket;
    SDL_Joystick *_joy;
    QTimer *_joyTimer;

    QCamera *_mainCamera;
    QGraphicsScene *_mainScene;
    QVideoWidget *_mainVideoWidget;
};

#endif // MAINWINDOW_H
