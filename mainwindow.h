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
#include <QString>

extern QString COMportName;

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
    void hideMessage();
    void updateConnectionStatus(bool);
    void onConnectButtonClick(bool);
    void onDisconnectButtonClick(bool);
    void onScaneI2CdevicesButtonClick(bool);
    void updateStatus(SimpleCommunicator_t::State_t);
    void updatePosInfo(SimpleCommunicator_t::RawSensorData_t);
    void readAndSendJoySensors();
    void joyButtonHandle();
    void onLeak(int send, int receive);
    void updateOrient(float q1, float q2, float q3, float q4);
    void updateHeading(int value);
    void updateI2CDevicesState(bool PCA1, bool PCA2, bool ADXL345, bool HMC58X3, bool ITG3200, bool BMP085, bool MS5803);
    void onBluetoothMsgRecieve(std::string msg);
    void onBluetoothButtonClick(bool);
    void onMotor1SliderChanged(int);
    void onMotor2SliderChanged(int);
    void onMotor3SliderChanged(int);
    void onMotor4SliderChanged(int);
    void onMotor5SliderChanged(int);
    void onMotor6SliderChanged(int);
    void onStopMotorsButtonClicked(bool);
    void onSetMotorsClicked(bool);
signals:
    void connectionChangedEvent(bool connectedStatus);
    void stateChangedEvent(SimpleCommunicator_t::State_t state);
    void rawSensorDataRecievedEvent(SimpleCommunicator_t::RawSensorData_t rawSensorData);
    void leakEvent(int send, int receive);
    void orientationReceivedEvent(float q1, float q2, float q3, float q4);
    void I2CDevicesRecieveEvent(bool PCA1, bool PCA2, bool ADXL345, bool HMC58X3, bool ITG3200, bool BMP085, bool MS5803);
    void bluetoothMsgRecieveEvent(std::string msg);
    void depthRecieveEvent(float depth);
private:
    void cameraInit();
    void loadQSS();
    bool eventFilter(QObject *, QEvent *event);
    void showMessage(QString msg, msg_color_t color);
    void showMessageByTimer(QString msg, msg_color_t color);
    void connectionProviderInit();
    void joyManipulatorButtonHandle();
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

    float cameraPos1 = 0.0f;
    float cameraPos2 = 0.0f;
};

#endif // MAINWINDOW_H
