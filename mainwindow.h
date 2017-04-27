#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ui_mainwindow.h"
#include <QCamera>
#include <QCameraInfo>
#include <QWidget>
#include <QtMultimediaWidgets>
#include <QtGui/QtGui>
#include "robocamera.h"
#include "joystick.h"
#include "remote-control-library/ConnectionProvider.h"
#include "remote-control-library/UARTConnectionProviderWindows.h"
#include "remote-control-library/SimpleCommunicator.h"
#include <iostream>
#include "remote-control-library/Utils.h"

namespace Ui {
    class MainWindow;
}

enum msg_color_t {
    CL_GREEN,
    CL_RED,
    CL_YELLOW
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();    
private slots:
    void updateDepth(float depth);
    void onStartButtonClick(bool);
    void onTaskTimeout();
    void updateManipulator(int);
    void updateFlashLight(bool);
    void hideMessage();
    void updateConnectionStatus(bool);
    void onConnectButtonClick(bool);
    void onDisconnectButtonClick(bool);
    void updateStatus(SimpleCommunicator_t::State_t);
    void updatePosInfo(SimpleCommunicator_t::RawSensorData_t);
    void readAndSendJoySensors();
    void joyButtonHandle(int idx, uint8_t value);
signals:
    void connectionChangedEvent(bool connectedStatus);
    void stateChangedEvent(SimpleCommunicator_t::State_t state);
    void rawSensorDataRecievedEvent(SimpleCommunicator_t::RawSensorData_t rawSensorData);
private:
    void cameraInit();
    void loadQSS();
    bool eventFilter(QObject *, QEvent *event);
    void showMessage(QString msg, msg_color_t color);
    void showMessageByTimer(QString msg, msg_color_t color);
    void connectionProviderInit();
    void joyManipulatorButtonHandle(int idx, uint8_t value);
    Ui::MainWindow *_ui;
    RoboCamera *_mainCamera;
    RoboCamera *_extraCamera;
    QTimer *_taskTimer;

    ConnectionProvider_t *_connectionProvider;
    SimpleCommunicator_t *_communicator;

    bool _flashLightState;
    bool _showMessage = false;
    QTimer *_messageTimer;

    Joystick *_joy;
    QTimer *_joyTimer;

    struct Manipulator {
        float _armPos = 0.0f;
        float _handPos = 0.0f;
        float _m1 = 0.0f;
        float _m2 = 0.0f;
        int _cntChanged = 0;
    } _curManipulator;
};

#endif // MAINWINDOW_H
