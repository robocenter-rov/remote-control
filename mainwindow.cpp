#include "mainwindow.h"
#include <iostream>

QDataStream& operator<<(QDataStream &ds, const Cmd::AxesValue &msg)
{
    ds << msg.axis0 << msg.axis1 << msg.axis2 << msg.axis3 << msg.axis4;
    return ds;
}

void MainWindow::loadQSS()
{
    QFile styleF("../remote-control/qss/appstyles.css");
    if (!styleF.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "Warning: application styles not found";
    }
    this->setStyleSheet(styleF.readAll());
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      _ui(new Ui::MainWindow),
      _joy(0),
      _joyTimer(new QTimer(this))
{
    _ui->setupUi(this);
    _socket = new QUdpSocket(this);
    _socket->bind(QHostAddress(IP_ADDR), PORT);

    connect(_socket, SIGNAL(readyRead()), this, SLOT(readMessage()));
   // connect(_ui->sendButton, SIGNAL(clicked(bool)), this, SLOT(onSendButtonClicked()));

    joyInit();
    cameraInit();
    loadQSS();
}

MainWindow::~MainWindow()
{
    delete _ui;
    SDL_JoystickClose(_joy);
}

void MainWindow::readMessage()
{
    QByteArray buffer;
    buffer.resize(_socket->pendingDatagramSize());

    QHostAddress sender;
    quint16 senderPort;

    _socket->readDatagram(buffer.data(), buffer.size(), &sender, &senderPort);
    qDebug() << "Message from: " << sender.toString();
    qDebug() << "Message port: " << senderPort;
    qDebug() << "Message: " << buffer;
}

void MainWindow::onSendButtonClicked()
{
    sendMessage("Hello, robot!");
}


void MainWindow::joyInit()
{
    if (SDL_Init(SDL_INIT_JOYSTICK) == 0) {
        qDebug() << "SDL_INIT_JOYSTICK initialization was successful";
    } else {
        qDebug() << "SDL_INIT_JOYSTICK initialization failed";
        qDebug() << "Error: " << SDL_GetError();
    }

    qDebug() << "NumJoysticks = " << SDL_NumJoysticks();
    if (SDL_NumJoysticks() > 0) {
        _joy = SDL_JoystickOpen(0);
    }

    if(_joy) {
        qDebug() << "Opened Joystick 0";
        qDebug() << "Name: " <<  SDL_JoystickName(0);
        qDebug() << "Number of Axes: " << SDL_JoystickNumAxes(_joy);
        qDebug() << "Number of Buttons: " << SDL_JoystickNumButtons(_joy);
        qDebug() << "Number of Balls: " << SDL_JoystickNumBalls(_joy);
    } else {
        qDebug() << "Couldn't open Joystick 0\n";
    }

    connect(_joyTimer, SIGNAL(timeout()), this, SLOT(readAndSendJoySensors()));
    _joyTimer->start(100);
}

void MainWindow::readAndSendJoySensors()
{
    SDL_JoystickUpdate();
    Cmd::AxesValue axesValue(SDL_JoystickGetAxis(_joy, 0), SDL_JoystickGetAxis(_joy, 1),
                             SDL_JoystickGetAxis(_joy, 2), SDL_JoystickGetAxis(_joy, 3),
                             SDL_JoystickGetAxis(_joy, 4));
    sendMessage(axesValue);
}

void MainWindow::cameraInit()
{
    _mainScene = new QGraphicsScene();
    _mainVideoWidget = new QVideoWidget();
    _ui->mainView->setScene(_mainScene);
    _ui->mainView->viewport()->installEventFilter(this);
    _mainScene->addWidget(_mainVideoWidget);
    _ui->mainView->show();

    if (QCamera::availableDevices().count() > 0){
        _mainCamera = new QCamera();
        _mainCamera->setCaptureMode(QCamera::CaptureVideo);
        _mainCamera->setViewfinder(_mainVideoWidget);
        _mainCamera->start();
    }
}

bool MainWindow::eventFilter(QObject *, QEvent *event)
{
    if(event->type() == QEvent::Resize ) {
        _ui->mainView->fitInView(_mainScene->sceneRect(), Qt::KeepAspectRatio);
        return true;
    }
    return false;
}
