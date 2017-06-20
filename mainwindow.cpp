#include "mainwindow.h"
#include <iostream>
#include <fstream>
#include "calc-tools/basetool.h"
#include "remote-control-library/UDPConnectionProvider.h"

QString COMportName;

static QColor msgColor[4] = {
    QColor(0, 204, 102),
    QColor(255, 102, 102),
    QColor(255, 204, 102),
    QColor(0, 102, 204)
};

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
      _taskTimer(new QTimer(this)),
      _messageTimer(new QTimer(this)),
      _joyTimer(new QTimer(this)),
      _joy(new Joystick()),
      _bluetoothTimer(new QTimer())
{
    qRegisterMetaType<SimpleCommunicator_t::PidState_t>("SimpleCommunicator_t::PidState_t");
    qRegisterMetaType<SimpleCommunicator_t::State_t>("SimpleCommunicator_t::State_t");
    qRegisterMetaType<SimpleCommunicator_t::CalibratedSensorData_t>("SimpleCommunicator_t::CalibratedSensorData_t");
    qRegisterMetaType<SimpleCommunicator_t::CalibratedSensorData_t>("SimpleCommunicator_t::RawSensorData_t");
    _connectionProvider = new UDPConnectionProvider_t(QHostAddress("192.168.0.50"), 3000, 1 << 20, 1 << 20);
    _communicator = new SimpleCommunicator_t(_connectionProvider);
    _ui->setupUi(this);

    cameraInit();
    loadQSS();

    connect(_ui->startButton, SIGNAL(clicked(bool)), this, SLOT(onStartButtonClick(bool)));
    connect(_taskTimer, SIGNAL(timeout()), this, SLOT(onTaskTimeout()));
    connect(_ui->connectButton, SIGNAL(clicked(bool)), this, SLOT(onConnectButtonClick(bool)));
    connect(_ui->disconnectButton, SIGNAL(clicked(bool)), this, SLOT(onDisconnectButtonClick(bool)));
    connect(_ui->heading, SIGNAL(valueChanged(int)), this, SLOT(updateHeading(int)));
    connect(_ui->scanI2Cbutton, SIGNAL(clicked(bool)), this, SLOT(onScaneI2CdevicesButtonClick(bool)));
    connect(_ui->bluetoothButton, SIGNAL(clicked(bool)), this, SLOT(onBluetoothButtonClick(bool)));
    connect(_ui->motor1Slider, SIGNAL(valueChanged(int)), this, SLOT(onMotor1SliderChanged(int)));
    connect(_ui->motor2Slider, SIGNAL(valueChanged(int)), this, SLOT(onMotor2SliderChanged(int)));
    connect(_ui->motor3Slider, SIGNAL(valueChanged(int)), this, SLOT(onMotor3SliderChanged(int)));
    connect(_ui->motor4Slider, SIGNAL(valueChanged(int)), this, SLOT(onMotor4SliderChanged(int)));
    connect(_ui->motor5Slider, SIGNAL(valueChanged(int)), this, SLOT(onMotor5SliderChanged(int)));
    connect(_ui->motor6Slider, SIGNAL(valueChanged(int)), this, SLOT(onMotor6SliderChanged(int)));
    connect(_ui->motor7Slider, SIGNAL(valueChanged(int)), this, SLOT(onMotor7SliderChanged(int)));
    connect(_ui->motor8Slider, SIGNAL(valueChanged(int)), this, SLOT(onMotor8SliderChanged(int)));
    connect(_ui->stopMotorsButton, SIGNAL(clicked(bool)), this, SLOT(onStopMotorsButtonClicked(bool)));
    connect(_ui->SetMotorsIdx, SIGNAL(clicked(bool)), this, SLOT(onSetMotorsClicked(bool)));
    connect(_ui->camera1Slider, SIGNAL(valueChanged(int)), this, SLOT(onCamera1PosChanged(int)));
    connect(_ui->camera2Slider, SIGNAL(valueChanged(int)), this, SLOT(onCamera2PosChanged(int)));
    connect(_ui->depthPIDButtton, SIGNAL(clicked(bool)), this, SLOT(onDepthPIDSpinBoxChanged(bool)));
    connect(_ui->pitchPIDButtton, SIGNAL(clicked(bool)), this, SLOT(onPitchPIDSpinBoxChanged(bool)));
    connect(_ui->yawPIDButtton, SIGNAL(clicked(bool)), this, SLOT(onYawPIDSpinBoxChanged(bool)));
    connect(_ui->rollPIDButtton, SIGNAL(clicked(bool)), this, SLOT(onRollPIDSpinBoxChanged(bool)));
    connect(_ui->setMotorsMulButton, SIGNAL(clicked(bool)), this, SLOT(onSetMotorsMultiplier(bool)));
    connect(_ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(onTabChanged(int)));
    connect(_ui->autoDepthCB, SIGNAL(clicked(bool)), this, SLOT(onAutoDepthClicked(bool)));
    connect(_ui->autoPitchCB, SIGNAL(clicked(bool)), this, SLOT(onAutoPitchClicked(bool)));
    connect(_ui->autoYawCB, SIGNAL(clicked(bool)), this, SLOT(onAutoYawClicked(bool)));
    connect(_ui->autoRollCB, SIGNAL(clicked(bool)), this, SLOT(onAutoRollClicked(bool)));
    connect(_ui->servo1Slider, SIGNAL(valueChanged(int)), this, SLOT(onServo1SliderChanged(int)));
    connect(_ui->useJoyButton, SIGNAL(clicked(bool)), this, SLOT(onUseJoyCheckButtonClicked(bool)));
    connect(_ui->depthEdit, SIGNAL(textEdited(QString)), this, SLOT(onAutoDepthEdit(QString)));
    connect(_ui->pitchEdit, SIGNAL(textEdited(QString)), this, SLOT(onAutoPitchEdit(QString)));
    connect(_ui->yawEdit, SIGNAL(textEdited(QString)), this, SLOT(onAutoYawEdit(QString)));
    connect(_ui->rollEdit, SIGNAL(textEdited(QString)), this, SLOT(onAutoRollEdit(QString)));
    connect(_ui->autoDepthCurrentCB, SIGNAL(clicked(bool)), this, SLOT(onAutoCurrentDepthClicked(bool)));
    connect(_ui->autoPitchCurrentCB, SIGNAL(clicked(bool)), this, SLOT(onAutoCurrentPitchClicked(bool)));
    connect(_ui->autoYawCurrentCB, SIGNAL(clicked(bool)), this, SLOT(onAutoCurrentYawClicked(bool)));
    connect(_ui->autoRollCurrentCB, SIGNAL(clicked(bool)), this, SLOT(onAutoCurrentRollClicked(bool)));
    connectionProviderInit();

    connect(_messageTimer, SIGNAL(timeout()), this, SLOT(hideMessage()));
    _messageTimer->setInterval(2000);

    connect(_joyTimer, SIGNAL(timeout()), this, SLOT(readAndSendJoySensors()));
    _joyTimer->setInterval(100);

    connect(_bluetoothTimer, SIGNAL(timeout()), this, SLOT(hideBTInfo()));
    _bluetoothTimer->setInterval(2000);
    connect(_joy, SIGNAL(joyButtonEvent()), this, SLOT(joyButtonHandle()));
    connect(this, SIGNAL(connectionChangedEvent(bool)), this, SLOT(updateConnectionStatus(bool)));
    connect(this, SIGNAL(stateChangedEvent(SimpleCommunicator_t::State_t)), this, SLOT(updateStatus(SimpleCommunicator_t::State_t)));
    connect(this, SIGNAL(rawSensorDataRecievedEvent(SimpleCommunicator_t::RawSensorData_t)), this, SLOT(updatePosInfo(SimpleCommunicator_t::RawSensorData_t)));
    connect(this, SIGNAL(rawSensorDataRecievedEvent(SimpleCommunicator_t::RawSensorData_t)), this, SLOT(updateRawSensorData(SimpleCommunicator_t::RawSensorData_t)));
    connect(this, SIGNAL(calibratedSensorDataRecievedEvent(SimpleCommunicator_t::RawSensorData_t)), this, SLOT(updateCalibratedSensorData(SimpleCommunicator_t::RawSensorData_t)));
    connect(this, SIGNAL(leakEvent(int, int)), this, SLOT(onLeak(int, int)));
    connect(this, SIGNAL(orientationReceivedEvent(float,float,float,float)), this, SLOT(updateOrient(float,float,float,float)));
    connect(this, SIGNAL(I2CDevicesRecieveEvent(bool,bool,bool,bool,bool,bool,bool,bool)), this, SLOT(updateI2CDevicesState(bool,bool,bool,bool,bool,bool,bool,bool)));
    connect(this, SIGNAL(bluetoothMsgRecieveEvent(QString)), this, SLOT(onBluetoothMsgRecieve(QString)));
    connect(this, SIGNAL(depthRecieveEvent(float)), this, SLOT(updateDepth(float)));
    connect(this, SIGNAL(motorStateReceiveEvent(float,float,float,float,float,float,float,float)), this, SLOT(onMotorStateRecieved(float,float,float,float,float,float,float,float)));
    connect(this, SIGNAL(pidStateReceiveEvent(SimpleCommunicator_t::PidState_t,SimpleCommunicator_t::PidState_t,SimpleCommunicator_t::PidState_t, SimpleCommunicator_t::PidState_t)), this, SLOT(onPidStateReceived(SimpleCommunicator_t::PidState_t,SimpleCommunicator_t::PidState_t,SimpleCommunicator_t::PidState_t, SimpleCommunicator_t::PidState_t)));

    showMessage("Connection...", CL_YELLOW);
    graphInit();
    replotData();
    generateMapTools();
    connect(_ui->axesCheckBox, SIGNAL(stateChanged(int)), this, SLOT(showAxis(int)));
    initPIDcoeffs();
    initMotorsMultipliers();
    initCameraMinMax();
    initIMUCalibration();
    initStabilizationUpdateFrequency();
    setMotorsPos();
}

MainWindow::~MainWindow()
{
    delete _ui;
    delete _communicator;
    delete _connectionProvider;
    delete _joy;
    delete _joyTimer;
    delete _messageTimer;
    delete _taskTimer;
}

void MainWindow::cameraInit()
{
    _videoScene = new VideoGraphicsScene();
    _videoScene->addScreen(&_screen);
    _mainCamera = new RoboCamera(1, _ui->mainView, this, _videoScene, true);
    _videoScene->setCamera(_mainCamera);

    _screenScene = new GraphicsScene();
    _screenScene->addScreen(&_screen);
    _ui->screensView->setScene(_screenScene);
    _ui->screensView->viewport()->installEventFilter(this);
    _ui->screensView->show();

    _videoScene->addScreenScene(_screenScene);

    _mapScene = new MapGraphicsScene();
    _ui->mapView->setScene(_mapScene);
    _ui->mapView->viewport()->installEventFilter(this);
    _ui->mapView->show();

    _videoScene->addMapScene(_mapScene);
}

bool MainWindow::eventFilter(QObject *, QEvent *event)
{
    _joy->handleEvent();
    if (event->type() == QEvent::Resize) {
        _ui->mainView->fitInView(_mainCamera->getScene()->sceneRect(), Qt::IgnoreAspectRatio);
        _ui->screensView->fitInView(_mainCamera->getScene()->sceneRect(), Qt::KeepAspectRatio);
        _ui->mapView->fitInView(_mainCamera->getScene()->sceneRect(), Qt::KeepAspectRatio);
        return true;
    }
    return false;
}

void MainWindow::updateDepth(float depth)
{
    _currentDepth = depth;
    _ui->depthValueLabel->setText(std::to_string(int(depth - 1000)).c_str());
    _mainCamera->getVideoWidget()->setCurrentDepth(int(depth - 1000));
}

void MainWindow::showMessage(QString msg, msg_color_t color)
{
    if (_showMessage) {
        hideMessage();
    }
    _showMessage = true;
    QGraphicsScene *scene = _ui->mainView->scene();
    scene->addRect(0, 0, scene->width() - 1, scene->height()*0.04, QPen(msgColor[color]), QBrush(msgColor[color]));

    QGraphicsTextItem *text = scene->addText(msg, QFont("Times", 8));
    text->setPos(QPointF(scene->width()/2 - 24, 0));
    text->setDefaultTextColor(QColor(255, 255, 255));
}

void MainWindow::showMessageByTimer(QString msg, msg_color_t color)
{
    showMessage(msg, color);
    _messageTimer->start();
}

void MainWindow::onStartButtonClick(bool)
{
    _taskTimer->stop();
    _ui->minutesLCDNumber->display(15);
    _ui->secondsLCDNumber->display(0);
    _taskTimer->start(1000);
}

void MainWindow::onTaskTimeout()
{
    double min = _ui->minutesLCDNumber->value(), sec = _ui->secondsLCDNumber->value();

    if (sec == 0) {
        sec = 59;
        min -= 1;
    } else {
        sec -= 1;
    }

    if (min == 0.0 && sec == 0.0) {
        _taskTimer->stop();
    }

    if (min > 10) {
        _ui->minutesLCDNumber->setStyleSheet("QLCDNumber#minutesLCDNumber{color:green;}");
        _ui->secondsLCDNumber->setStyleSheet("QLCDNumber#secondsLCDNumber{color:green;}");
    } else if (min > 5) {
        _ui->minutesLCDNumber->setStyleSheet("QLCDNumber#minutesLCDNumber{color:orange;}");
        _ui->secondsLCDNumber->setStyleSheet("QLCDNumber#secondsLCDNumber{color:green;}");
    } else if (min > 0) {
        _ui->secondsLCDNumber->setStyleSheet("QLCDNumber#minutesLCDNumber{color:red;}");
        _ui->secondsLCDNumber->setStyleSheet("QLCDNumber#secondsLCDNumber{color:green;}");
    }
    _ui->minutesLCDNumber->display(min);
    _ui->secondsLCDNumber->display(sec);
}

#include <string>

void MainWindow::connectionProviderInit()
{
    try {
        _communicator->SetSendMessageFrequency(100);
        _communicator->SetRemoteSendMessageFrequency(100);

        _connectionProvider->Begin();

        _communicator->OnRobotRestart([]()
        {
            qDebug() << "Arduino was restarted\n";
        });

        _communicator->OnPacketsLeak([&](int send, int receive)
        {
            emit leakEvent(send, receive);
        });
        _communicator->OnConnectionStateChange([&](bool connectedStatus)
        {
            emit connectionChangedEvent(connectedStatus);
        });

        _communicator->OnI2CDevicesReceive([&](SimpleCommunicator_t::I2CDevices_t devices)
        {
            emit I2CDevicesRecieveEvent(
                        devices.PCA1,
                        devices.PCA2,
                        devices.PCA3,
                        devices.ADXL345,
                        devices.HMC58X3,
                        devices.ITG3200,
                        devices.BMP085,
                        devices.MS5803);
        });

        _communicator->OnStateChange([&](SimpleCommunicator_t::State_t state){
            emit stateChangedEvent(state);
        });

        _communicator->OnRawSensorDataReceive([&](SimpleCommunicator_t::RawSensorData_t rawSensorData){
            emit rawSensorDataRecievedEvent(rawSensorData);
        });

        _communicator->OnCalibratedSensorDataReceive([&](SimpleCommunicator_t::CalibratedSensorData_t calibratedSensorData) {
            emit calibratedSensorDataRecievedEvent(calibratedSensorData);
        });

        _communicator->OnOrientationReceive([&](SimpleCommunicator_t::Orientation_t o){
            emit orientationReceivedEvent(o.q1, o.q2, o.q3, o.q4);
        });

        _communicator->OnBluetoothMsgReceive([&](std::string msg){
            emit bluetoothMsgRecieveEvent(msg.c_str());
        });

        _communicator->OnDepthReceive([&](float depth){
            emit depthRecieveEvent(depth);
        });

        _communicator->OnMotorsStateReceive([&](SimpleCommunicator_t::MotorsState_t motorState){

            emit motorStateReceiveEvent(motorState.M1Force, motorState.M2Force, motorState.M3Force,
                                        motorState.M4Force, motorState.M5Force, motorState.M6Force,
                                        motorState.M7Force, motorState.M8Force);
        });

        _communicator->OnPidStateReceive([&](SimpleCommunicator_t::PidState_t depth,
                                         SimpleCommunicator_t::PidState_t yaw,
                                         SimpleCommunicator_t::PidState_t pitch,
                                         SimpleCommunicator_t::PidState_t roll){
            emit pidStateReceiveEvent(depth, yaw, pitch, roll);
        });

        _communicator->OnRemoteProcessorLoad([&](unsigned long freq) {
            _ui->LoopFrequencyLabel->setText(std::to_string(freq).c_str());
        });
        _communicator->Begin();
    } catch (ConnectionProviderException_t &e) {
        qDebug() << e.error_message.c_str();
    }
}

void MainWindow::updateConnectionStatus(bool connectedStatus)
{
    qDebug() << "connection is " << connectedStatus;
    if (connectedStatus) {
        showMessageByTimer("Connected", CL_GREEN);
        _joyTimer->start();
    } else {
        showMessage("Connection...", CL_YELLOW);
        if (_joyTimer->isActive()) {
            _joyTimer->stop();
        }
    }
}

#include <QList>
#include <QGraphicsItem>
#include <QGraphicsScene>

void MainWindow::hideMessage()
{
    if (!_showMessage) return;
    if (_messageTimer->isActive()) {
        _messageTimer->stop();
    }
    _showMessage = false;
    QList<QGraphicsItem *> t = _ui->mainView->items();

    if (t.size() == 1) return;
    int i = 1;
    for (auto it = t.begin(); it != t.end(); it++, i++) {
        if (i <= 2) { // i = 1 - msg, 2 - msg_rect. remove only msg
            _ui->mainView->scene()->removeItem(dynamic_cast<QGraphicsItem *>(*it));
        }
    }
}

void MainWindow::onConnectButtonClick(bool)
{
    try {
        _connectionProvider->Begin();
    } catch (ControllerException_t &e) {
        qDebug() << e.error_message.c_str();
    }
}

void MainWindow::onDisconnectButtonClick(bool)
{
    try {
        _connectionProvider->Stop();
    } catch (ControllerException_t &e) {
        qDebug() << e.error_message.c_str();
    }
}

void MainWindow::updateStatus(SimpleCommunicator_t::State_t state)
{
    _ui->flashLightRadioButton->setChecked(state.FlashlightState);
}

void MainWindow::updatePosInfo(SimpleCommunicator_t::RawSensorData_t rawSensorData)
{
    updateDepth(rawSensorData.Depth);
}

void MainWindow::updateRawSensorData(SimpleCommunicator_t::RawSensorData_t rawSensorData)
{
    _ui->xGyroRawValue_Label->setText(std::to_string(rawSensorData.Gx).c_str());
    _ui->yGyroRawValue_Label->setText(std::to_string(rawSensorData.Gy).c_str());
    _ui->zGyroRawValue_Label->setText(std::to_string(rawSensorData.Gz).c_str());

    _ui->xAccRawValue_Label->setText(std::to_string(rawSensorData.Ax).c_str());
    _ui->yAccRawValue_Label->setText(std::to_string(rawSensorData.Ay).c_str());
    _ui->zAccRawValue_Label->setText(std::to_string(rawSensorData.Az).c_str());

    if (_ui->CalibrateGyro_PushButton->isChecked()) {
        _xGyroOffset += rawSensorData.Gx;
        _yGyroOffset += rawSensorData.Gy;
        _zGyroOffset += rawSensorData.Gz;
        _calibrateIteration++;
        _ui->xGyroOffset_LineEdit->setText(std::to_string(_xGyroOffset/_calibrateIteration).c_str());
        _ui->yGyroOffset_LineEdit->setText(std::to_string(_yGyroOffset/_calibrateIteration).c_str());
        _ui->zGyroOffset_LineEdit->setText(std::to_string(_zGyroOffset/_calibrateIteration).c_str());
    }
}

void MainWindow::updateCalibratedSensorData(SimpleCommunicator_t::CalibratedSensorData_t calibratedSensorData)
{
    _ui->xGyroCalibratedValue_Label->setText(std::to_string(calibratedSensorData.Gx).c_str());
    _ui->yGyroCalibratedValue_Label->setText(std::to_string(calibratedSensorData.Gy).c_str());
    _ui->zGyroCalibratedValue_Label->setText(std::to_string(calibratedSensorData.Gz).c_str());

    _ui->xAccCalibratedValue_Label->setText(std::to_string(calibratedSensorData.Ax).c_str());
    _ui->yAccCalibratedValue_Label->setText(std::to_string(calibratedSensorData.Ay).c_str());
    _ui->zAccCalibratedValue_Label->setText(std::to_string(calibratedSensorData.Az).c_str());
}

void MainWindow::setAutoModeStates(QLabel *am_label, auto_mode_state_t state, QString currentVal)
{
    QString color_style[3] = {"color: rgb(170, 170, 0);", "color: rgb(0, 170, 0);", "color: rgb(170, 0, 0);"};

    QString style = QString("QLabel#") + am_label->objectName() +
            QString("{") +
            QString(color_style[state]) +
            QString("font-size: 14px; }");
    am_label->setText(currentVal);
    am_label->setStyleSheet(style);
}

#define ABS(x) ((x < 0) ? (-x) : (x))

void MainWindow::readAndSendJoySensors()
{
    _joy->update();
    float thrust[5];
    for (int i = 0; i < 5; i++) {
        thrust[i] = _joy->axesAt(i);
    }
    float eps = 0.03;
    float y = (ABS(thrust[1]) < eps) ? 0 : thrust[1];
    float x = (ABS(thrust[0]) < eps) ? 0 : thrust[0];
    float z = (ABS(thrust[4]) < 0.3) ? 0 : thrust[4];
    float ty = 0;
    float tz = (ABS(thrust[3]) < eps) ? 0 : thrust[3];
    float dist = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));

    x *= 2.83;
    y *= 2.83;
    float _sensitivity = _control_sensitivity;
\
    const float start_val = 0.2f;
    const float zero_val = 0.05f;
    _turbo_control = true;
    if (z == 0) {
        _turbo_control = false;
        showMessage("Z dead zone", CL_BLUE);
        if (_isAutoDepth) {
            if (!_communicator->IsAutoDepthEnabled()) {
                _communicator->SetDepth(_currentDepth);
            }
            setAutoModeStates(_ui->autoDepthStateLabel, AM_USED, std::to_string(_communicator->GetAutoDepthValue()).c_str());
        } else {
            setAutoModeStates(_ui->autoDepthStateLabel, AM_OFF, "OFF");
            _communicator->DisableAutoDepth();
        }
    } else {
        setAutoModeStates(_ui->autoDepthStateLabel, AM_OFF, "OFF");
        _communicator->DisableAutoDepth();
    }
    _communicator->SetLocalZForce(z *_control_sensitivity * 4);

    if (_isAutoYaw) {
        if (tz == 0) {
            _turbo_control = _turbo_control || false;
            if (!_communicator->IsAutoYawEnabled()) {
                _communicator->SetYaw(_communicator->GetAutoYawValue());
            }
            setAutoModeStates(_ui->autoYawStateLabel, AM_USED, std::to_string(_currentYaw).c_str());
        } else {
            setAutoModeStates(_ui->autoYawStateLabel, AM_ON, "ON");
        }
    } else {
        setAutoModeStates(_ui->autoYawStateLabel, AM_OFF, "OFF");
        if (abs(tz) < zero_val) {
            _z_rotate_force = 0;
        } else {
            _z_rotate_force += (tz*0.4 * _sensitivity - _z_rotate_force) * 0.5f;
        }
    }
    _communicator->SetYawForce(tz * _sensitivity);

    _turbo_control = (x && y) ? (_turbo_control || false): false;
    _x_move_force = -x * _sensitivity;
/*
    if (abs(-x) < zero_val) {
        _x_move_force = 0;
    } else if (abs(-x) < start_val) {
        _x_move_force = start_val * ((-x) < 0 ? -1 : 1);
    } else {
        _x_move_force += ((-x * _sensitivity) - _x_move_force) * 0.3f;
    }*/
/*
    if (abs(y) < zero_val) {
        _y_move_force = 0;
    } else if (abs(y) < start_val) {
        _y_move_force = start_val * (y < 0 ? -1 : 1);
    } else {
        _y_move_force += ((y * _sensitivity) - _y_move_force) * 0.3f;
    }*/

    _y_move_force = y * _sensitivity;
    if (!_turbo_control) _control_sensitivity = _last_control_sensitivity;
    _communicator->SetMovementForce(_signDirection*_x_move_force, _signDirection*_y_move_force);
}

void MainWindow::joyButtonHandle()
{
    try {
        joyManipulatorButtonHandle();
    } catch (ControllerException_t &e) {
        qDebug() << e.error_message.c_str();
    }
}

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

void MainWindow::joyManipulatorButtonHandle()
{
    _curManipulator._handPos = 0;
    _curManipulator._armPos = 0;
    if (_joy->atBtn(3)){
        _curManipulator._handPos = -0.45f;
    }
    if (_joy->atBtn(4)) {
        _curManipulator._handPos = 0.45f;
    }
    if (_joy->atBtn(1)) {
         _curManipulator._armPos = 0.45f;
    }
    if (_joy->atBtn(0)) {
        _curManipulator._armPos = -0.45f;
    }
    if (_joy->atBtn(12)) {
        if (_joy->btnStateChanged(12)) {
            cameraPos1 = MIN(3.14f/2.0, cameraPos1 + 0.35);
            _communicator->SetCamera1LocalPos(cameraPos1);
            qDebug() << cameraPos1;
        }
    }
    if (_joy->atBtn(11)) {
        if (_joy->btnStateChanged(11)) {
            cameraPos1 = MAX(-3.14f/2.0, cameraPos1 - 0.35);
            _communicator->SetCamera1LocalPos(cameraPos1);
            qDebug() << cameraPos1;
        }
    }
    if (_joy->atHat(SDL_HAT_DOWN)) {
        cameraPos2 = MIN(3.14f/2.0, cameraPos2 + 0.05);
        _communicator->SetCamera2LocalPos(cameraPos2);
        qDebug() << cameraPos2;
    }
    if (_joy->atHat(SDL_HAT_UP)) {
        cameraPos2 = MAX(-3.14f/2.0, cameraPos2 - 0.05);
        _communicator->SetCamera2LocalPos(cameraPos2);
        qDebug() << cameraPos2;
    }
    if (_joy->atBtn(13)) {
        if (_joy->btnStateChanged(13)) {
            _communicator->SetFlashlightState(_flashLightState = !_flashLightState);
        }
    }
    if (_joy->btnDoubleClicked(13)) {
        _ui->invertCB->setChecked(_signDirection == 1);
        _signDirection = (_signDirection == 1) ? -1 : 1;

    }
    if (_joy->atBtn(5)) {
        if (_joy->btnStateChanged(5)) {
            _control_sensitivity = 0.25f;
            updateSensitivity();
        }
    }
    if (_joy->atBtn(6)) {
        if (_joy->btnStateChanged(6)) {
            _control_sensitivity = 0.5f;
            updateSensitivity();
        }
    }
    if (_joy->atBtn(7)) {
        if (_joy->btnStateChanged(7)) {
            _control_sensitivity = 0.75f;
            updateSensitivity();
        }
    }
    if (_joy->atBtn(8)) {
        if (_joy->btnStateChanged(8)) {
            _communicator->SetMotorsState(0, 0, 0, 0, 0, 0, 0, 0);
        }
    }
    if (_joy->atBtn(2)) {
        if (_turbo_control) {
            _last_control_sensitivity = _control_sensitivity;
            _control_sensitivity = 1;
            updateSensitivity();
        }
    }
    if (_joy->atBtn(9)) {
        _curManipulator._m1 = MAX(-3.14f/2.0f, _curManipulator._m1 - 0.45f);
    }
    if (_joy->atBtn(10)) {
        _curManipulator._m1 = MIN(3.14f/2.0f, _curManipulator._m1 + 0.45f);
    }
    _communicator->SetManipulatorState(
        _curManipulator._armPos,
        _curManipulator._handPos,
        _curManipulator._m1,
        _curManipulator._m2
    );
}

void MainWindow::updateSensitivity()
{
    _ui->sensitivityPB->setValue(int(_control_sensitivity*10)*10);
}

void MainWindow::onLeak(int send, int receive)
{
    return;
    std::string s = "ATTENTION: Leak: send ";
    s += std::to_string(send); s += ", recieved "; s += std::to_string(receive);
    showMessageByTimer(s.c_str(), CL_RED);
}

void MainWindow::updateOrient(float q0, float q1, float q2, float q3)
{
    float angles[3];/*
    angles[0] = atan2(2 * q2 * q3 - 2 * q1 * q4, 2 * q1 * q1 + 2 * q2 * q2 - 1); // psi
    angles[1] = -asin(2 * q2 * q4 + 2 * q1 * q3); // theta
    angles[2] = atan2(2 * q3 * q4 - 2 * q1 * q2, 2 * q1 * q1 + 2 * q4 * q4 - 1); // phi*/

    float gx, gy, gz; // estimated gravity direction

    gx = 2 * (q1*q3 - q0*q2);
    gy = 2 * (q0*q1 + q2*q3);
    gz = q0*q0 - q1*q1 - q2*q2 + q3*q3;

    angles[0] = atan2(2 * q1 * q2 - 2 * q0 * q3, 2 * q0*q0 + 2 * q1 * q1 - 1);
    angles[1] = atan(gx / sqrt(gy*gy + gz*gz));
    angles[2] = atan(gy / sqrt(gx*gx + gz*gz));

    _ui->psiLabel->setText(std::to_string(int(angles[0] / 3.14159 * 180)).c_str());
    _ui->thetaLabel->setText(std::to_string(int(angles[1] / 3.14159 * 180)).c_str());
    _ui->phiLabel->setText(std::to_string(int(angles[2] / 3.14159 * 180)).c_str());
    updateHeading(angles[0] / 3.14159 * 180);
    _currentYaw = angles[0];
    _currentPitch = angles[2];
}

void MainWindow::updateHeading(int value)
{
    std::string s = std::to_string(value);
    _ui->headingLabel->setText(s.c_str());
    _ui->heading->setValue(value);
}

void MainWindow::updateI2CDevicesState(
        bool PCA1, bool PCA2, bool PCA3, bool ADXL345, bool HMC58X3, bool ITG3200, bool BMP085, bool MS5803)
{
    _ui->radioPCA1->setChecked(PCA1);
    _ui->radioPCA2->setChecked(PCA2);
    _ui->radioPCA3->setChecked(PCA3);
    _ui->radioADXL345->setChecked(ADXL345);
    _ui->radioHMC58X3->setChecked(HMC58X3);
    _ui->radioITG3200->setChecked(ITG3200);
    _ui->radioBMP085->setChecked(BMP085);
    _ui->radioMS5803->setChecked(MS5803);
}

void MainWindow::onScaneI2CdevicesButtonClick(bool)
{
    _communicator->SetRescanI2CDevices();
}

void MainWindow::onBluetoothMsgRecieve(QString msg)
{
    _ui->bluetoothLabel->setText(msg);
    if (_bluetoothTimer != nullptr)
        _bluetoothTimer->start();
}

void MainWindow::hideBTInfo() {
    if (_bluetoothTimer->isActive()) {
        _bluetoothTimer->stop();
    }
    _ui->bluetoothLabel->setTest("");
}

void MainWindow::onBluetoothButtonClick(bool value)
{
    _communicator->SetReadBluetoothState(value);
}

void MainWindow::onMotor1SliderChanged(int value)
{
    float val = value/127.0;
    _ui->motor1valueLabel->setText(QString(std::to_string(value*100/127).c_str()) + "%");
    _communicator->SetMotorState(0, val);
}

void MainWindow::onMotor2SliderChanged(int value)
{
    float val = value/127.0;
    _ui->motor2valueLabel->setText(QString(std::to_string(value*100/127).c_str()) + "%");
    _communicator->SetMotorState(1, val);
}

void MainWindow::onMotor3SliderChanged(int value)
{
    float val = value/127.0;
    _ui->motor3valueLabel->setText(QString(std::to_string(value*100/127).c_str()) + "%");
    _communicator->SetMotorState(2, val);
}

void MainWindow::onMotor4SliderChanged(int value)
{
    float val = value/127.0;
    _ui->motor4valueLabel->setText(QString(std::to_string(value*100/127).c_str()) + "%");
    _communicator->SetMotorState(3, val);
}

void MainWindow::onMotor5SliderChanged(int value)
{
    float val = value/127.0;
    _ui->motor5valueLabel->setText(QString(std::to_string(value*100/127).c_str()) + "%");
    _communicator->SetMotorState(4, val);
}

void MainWindow::onMotor6SliderChanged(int value)
{
    float val = value/127.0f;
    _ui->motor6valueLabel->setText(QString(std::to_string(value*100/127).c_str()) + "%");
    _communicator->SetMotorState(5, val);
}

void MainWindow::onMotor7SliderChanged(int value)
{
    float val = value/127.0f;
    _ui->motor7valueLabel->setText(QString(std::to_string(value*100/127).c_str()) + "%");
    _communicator->SetMotorState(6, val);
}

void MainWindow::onMotor8SliderChanged(int value)
{
    float val = value/127.0f;
    _ui->motor8valueLabel->setText(QString(std::to_string(value*100/127).c_str()) + "%");
    _communicator->SetMotorState(7, val);
}

void MainWindow::onStopMotorsButtonClicked(bool value)
{
    _ui->motor1Slider->setValue(0);
    _ui->motor2Slider->setValue(0);
    _ui->motor3Slider->setValue(0);
    _ui->motor4Slider->setValue(0);
    _ui->motor5Slider->setValue(0);
    _ui->motor6Slider->setValue(0);
    _ui->motor7Slider->setValue(0);
    _ui->motor8Slider->setValue(0);
    _communicator->SetMotorsState(0, 0, 0, 0, 0, 0, 0, 0);
}

void MainWindow::onCamera1PosChanged(int value)
{
    _ui->camera1valueLabel->setText(QString(std::to_string(value).c_str()));
    _communicator->SetCamera1LocalPos(value*3.1415/180.0);
}

void MainWindow::onCamera2PosChanged(int value)
{
    _ui->camera2valueLabel->setText(QString(std::to_string(value).c_str()));
    _communicator->SetCamera2LocalPos(value*3.1415/180.0);
}

void MainWindow::onSetMotorsClicked(bool value)
{
    int m[8];
    m[0] = _ui->motorIdxSpinBox_1->value() - 1;
    m[1] = _ui->motorIdxSpinBox_2->value() - 1;
    m[2] = _ui->motorIdxSpinBox_3->value() - 1;
    m[3] = _ui->motorIdxSpinBox_4->value() - 1;
    m[4] = _ui->motorIdxSpinBox_5->value() - 1;
    m[5] = _ui->motorIdxSpinBox_6->value() - 1;
    m[6] = _ui->motorIdxSpinBox_7->value() - 1;
    m[7] = _ui->motorIdxSpinBox_8->value() - 1;

    for (int i = 0; i < 8; i++) {
        for (int j = i + 1; j < 8; j++) {
            if (m[i] == m[j]) {
                _ui->setMotorsMsg->setText("Motors should not be the same");
                return;
            }
        }
    }
    _ui->setMotorsMsg->setText("");
    _communicator->SetMotorsPositions(m[0], m[1], m[2], m[3], m[4], m[5], m[6], m[7]);
}

void MainWindow::onDepthPIDSpinBoxChanged(bool value)
{
    double p = _ui->depthPSpinBox->value();
    double i = _ui->depthISpinBox->value();
    double d = _ui->depthDSpinBox->value();
    std::ofstream fout;
    fout.open("depth.txt");
    if (fout.is_open()) {
        fout << p << " " << i << " "<< d;
        _communicator->SetDepthPid(p, i, d);
        fout.close();
    } else {
        qDebug() << "Can't open file: depth.txt";
    }
}

void MainWindow::onSetMotorsMultiplier(bool value)
{
    float m1 = _ui->m1MultSpinBox->value();
    float m2 = _ui->m2MultSpinBox->value();
    float m3 = _ui->m3MultSpinBox->value();
    float m4 = _ui->m4MultSpinBox->value();
    float m5 = _ui->m5MultSpinBox->value();
    float m6 = _ui->m6MultSpinBox->value();
    float m7 = _ui->m7MultSpinBox->value();
    float m8 = _ui->m8MultSpinBox->value();
    std::ofstream fout;
    fout.open("multipliers.txt");
    if (fout.is_open()) {
        fout << m1 << " " << m2 << " " << m3 << " " << m4 << " " << m5 << " " << m6 << " " << m7 << " " << m8;
        _communicator->SetMotorsMultiplier(m1, m2, m3, m4, m5, m6, m7, m8);
        fout.close();
    } else {
        qDebug() << "Can't open file: multipliers.txt";
    }
}

void MainWindow::onPitchPIDSpinBoxChanged(bool value)
{
    double p = _ui->pitchPSpinBox->value();
    double i = _ui->pitchISpinBox->value();
    double d = _ui->pitchDSpinBox->value();
    _communicator->SetPitchPid(p, i, d);
    std::ofstream fout;
    fout.open("pitch.txt");
    if (fout.is_open()) {
        fout << p << " " << i << " "<< d;
        fout.close();
    } else {
        qDebug() << "Can't open file: pitch.txt";
    }
}

void MainWindow::onYawPIDSpinBoxChanged(bool value)
{
    double p = _ui->yawPSpinBox->value();
    double i = _ui->yawISpinBox->value();
    double d = _ui->yawDSpinBox->value();
    _communicator->SetYawPid(p, i, d);
    std::ofstream fout;
    fout.open("yaw.txt");
    if (fout.is_open()) {
        fout << p << " " << i << " "<< d;
        fout.close();
    } else {
        qDebug() << "Cant't open file: yaw.txt";
    }
}

void MainWindow::onRollPIDSpinBoxChanged(bool)
{
    double p = _ui->rollPSpinBox->value();
    double i = _ui->rollISpinBox->value();
    double d = _ui->rollDSpinBox->value();
    _communicator->SetRollPid(p, i, d);
    std::ofstream fout;
    fout.open("roll.txt");
    if (fout.is_open()) {
        fout << p << " " << i << " "<< d;
        fout.close();
    } else {
        qDebug() << "Cant't open file: roll.txt";
    }
}

void MainWindow::onTabChanged(int idx)
{
    if (idx == 0) {
        if (!_joyTimer->isActive()) {
            _joyTimer->start();
        }
    }
    if (idx == 1) {
        if (_joyTimer->isActive()) {
            _joyTimer->stop();
        }
    }
    if (idx == 4) {
        _mapScene->setSceneRect(_ui->mainView->sceneRect());
        _mapScene->updateScene();
    }
}

void MainWindow::onAutoDepthClicked(bool value)
{
    float depth = _ui->depthEdit->text().toFloat();
    if (value) {
        _ui->autoDepthCurrentCB->setChecked(false);
        _ui->stabDepthValue->setText(_ui->depthEdit->text());
        _communicator->SetDepth(depth);
    } else {
        _communicator->SetMotorsState(0, 0, 0, 0, 0, 0, 0, 0);
    }
}

void MainWindow::onAutoPitchClicked(bool value)
{
    float pitch = _ui->pitchEdit->text().toFloat();
    if (value) {
        _ui->autoPitchCurrentCB->setChecked(false);
        _ui->stabPitchValue->setText(_ui->pitchEdit->text());
        _communicator->SetPitch(pitch);
    } else {
        _communicator->SetMotorsState(0, 0, 0, 0, 0, 0, 0, 0);
    }
}

void MainWindow::onAutoRollClicked(bool value)
{
    float roll = _ui->rollEdit->text().toFloat();
    if (value) {
        _ui->autoRollCurrentCB->setChecked(false);
        _ui->stabRollValue->setText(_ui->rollEdit->text());
        _communicator->SetRoll(roll);
    } else {
        _communicator->SetMotorsState(0, 0, 0, 0, 0, 0, 0, 0);
    }
}

void MainWindow::onAutoYawClicked(bool value)
{
    float yaw = _ui->yawEdit->text().toFloat();
    if (value) {
        _ui->autoYawCurrentCB->setChecked(false);
        _ui->stabYawValue->setText(_ui->yawEdit->text());
        _communicator->SetYaw(yaw);
    } else {
        _communicator->SetMotorsState(0, 0, 0, 0, 0, 0, 0, 0);
    }
}

void MainWindow::onServo1SliderChanged(int value)
{
    try { /* DO: check values */
        _curManipulator._m1 = value*3.1415/180.0;
        _communicator->SetManipulatorState(
            _curManipulator._armPos,
            _curManipulator._handPos,
            _curManipulator._m1,
            _curManipulator._m2
        );
        _ui->servo1ValueLabel->setText(QString(std::to_string(value/9*10).c_str()) + "%");
    } catch (ControllerException_t &e) {
        qDebug() << e.error_message.c_str();
    }
}

void MainWindow::on_servo2Slider_valueChanged(int value)
{
    try { /* DO: check values */
        _curManipulator._m2 = value*3.1415/180.0+3.1415;
        _communicator->SetManipulatorState(
            _curManipulator._armPos,
            _curManipulator._handPos,
            _curManipulator._m1,
            _curManipulator._m2
        );
        _ui->servo2ValueLabel->setText(QString(std::to_string(value/9*10).c_str()) + "%");
    } catch (ControllerException_t &e) {
        qDebug() << e.error_message.c_str();
    }
}

void MainWindow::onUseJoyCheckButtonClicked(bool value)
{
    if (value) {
        if (!_joyTimer->isActive()) {
            _joyTimer->start();
        }
    } else {
        if (_joyTimer->isActive()) {
            _joyTimer->stop();
        }
    }
}

void MainWindow::onMotorStateRecieved(float m1, float m2, float m3, float m4, float m5, float m6, float m7, float m8)
{
    const int maxval = 100;

    _ui->m1curLabel->setText(QString(std::to_string(m1*100).c_str()) + "%");
    _ui->m2curLabel->setText(QString(std::to_string(m2*100).c_str()) + "%");
    _ui->m3curLabel->setText(QString(std::to_string(m3*100).c_str()) + "%");
    _ui->m4curLabel->setText(QString(std::to_string(m4*100).c_str()) + "%");
    _ui->m5curLabel->setText(QString(std::to_string(m5*100).c_str()) + "%");
    _ui->m6curLabel->setText(QString(std::to_string(m6*100).c_str()) + "%");
    _ui->m7curLabel->setText(QString(std::to_string(m7*100).c_str()) + "%");
    _ui->m8curLabel->setText(QString(std::to_string(m8*100).c_str()) + "%");

    _ui->m0LoadPositiveProgressBar->setValue(std::max(0.f, m1*maxval));
    _ui->m0LoadNegativeProgressBar->setValue(std::min(0.f, m1*maxval)*-1);

    _ui->m1LoadPositiveProgressBar->setValue(std::max(0.f, m2*maxval));
    _ui->m1LoadNegativeProgressBar->setValue(std::min(0.f, m2*maxval)*-1);

    _ui->m2LoadPositiveProgressBar->setValue(std::max(0.f, m3*maxval));
    _ui->m2LoadNegativeProgressBar->setValue(std::min(0.f, m3*maxval)*-1);

    _ui->m3LoadPositiveProgressBar->setValue(std::max(0.f, m4*maxval));
    _ui->m3LoadNegativeProgressBar->setValue(std::min(0.f, m4*maxval)*-1);

    _ui->m4LoadPositiveProgressBar->setValue(std::max(0.f, m5*maxval));
    _ui->m4LoadNegativeProgressBar->setValue(std::min(0.f, m5*maxval)*-1);

    _ui->m5LoadPositiveProgressBar->setValue(std::max(0.f, m6*maxval));
    _ui->m5LoadNegativeProgressBar->setValue(std::min(0.f, m6*maxval)*-1);

    _ui->m6LoadPositiveProgressBar->setValue(std::max(0.f, m7*maxval));
    _ui->m6LoadNegativeProgressBar->setValue(std::min(0.f, m7*maxval)*-1);

    _ui->m7LoadPositiveProgressBar->setValue(std::max(0.f, m8*maxval));
    _ui->m7LoadNegativeProgressBar->setValue(std::min(0.f, m8*maxval)*-1);
}

void MainWindow::onPidStateReceived(SimpleCommunicator_t::PidState_t depth, SimpleCommunicator_t::PidState_t yaw, SimpleCommunicator_t::PidState_t pitch, SimpleCommunicator_t::PidState_t roll)
{
    _count_of_recieved_pid++;
    _ui->depthInValueLabel->setText(std::to_string(depth.In).c_str());
    _ui->depthTarValueLabel->setText(std::to_string(depth.Target).c_str());
    _ui->depthOutValueLabel->setText(std::to_string(depth.Out).c_str());

    _ui->yawInValueLabel->setText(std::to_string(yaw.In).c_str());
    _ui->yawTarValueLabel->setText(std::to_string(yaw.Target).c_str());
    _ui->yawOutValueLabel->setText(std::to_string(yaw.Out).c_str());

    _ui->pitchInValueLabel->setText(std::to_string(pitch.In).c_str());
    _ui->pitchTarValueLabel->setText(std::to_string(pitch.Target).c_str());
    _ui->pitchOutValueLabel->setText(std::to_string(pitch.Out).c_str());

    _ui->rollInValueLabel->setText(std::to_string(roll.In).c_str());
    _ui->rollTarValueLabel->setText(std::to_string(roll.Target).c_str());
    _ui->rollOutValueLabel->setText(std::to_string(roll.Out).c_str());

    _depthData.pop_front();
    _depthData.push_back(depth);
    _pitchData.pop_front();
    _pitchData.push_back(pitch);
    _yawData.pop_front();
    _yawData.push_back(yaw);
    _rollData.pop_front();
    _rollData.push_back(roll);
    replotData();
}

void MainWindow::graphInit()
{
    QVector<double> x(DEPTH_DATA_SIZE), y(DEPTH_DATA_SIZE);
    for (int i = 0; i < DEPTH_DATA_SIZE; i++){
        x[i] = i - DEPTH_DATA_SIZE;
        y[i] = 0;
    }
    QVector<QColor> colors = { QColor(40, 110, 255), QColor(255, 110, 40), QColor(110, 255, 40) };
    for (int i = 0; i < 3; i++) {
        _ui->autoDepthGraph->addGraph();
        _ui->autoPitchGraph->addGraph();
        _ui->autoYawGraph->addGraph();
        _ui->autoRollGraph->addGraph();

        _ui->autoDepthGraph->graph(i)->setData(x, y);
        _ui->autoPitchGraph->graph(i)->setData(x, y);
        _ui->autoYawGraph->graph(i)->setData(x, y);
        _ui->autoRollGraph->graph(i)->setData(x, y);

        _ui->autoDepthGraph->graph(i)->setPen(colors[i]);
        _ui->autoPitchGraph->graph(i)->setPen(colors[i]);
        _ui->autoYawGraph->graph(i)->setPen(colors[i]);
        _ui->autoRollGraph->graph(i)->setPen(colors[i]);
    }
    _ui->autoDepthGraph->xAxis->setRange(-DEPTH_DATA_SIZE, 0);
    _ui->autoPitchGraph->xAxis->setRange(-PITCH_DATA_SIZE, 0);
    _ui->autoYawGraph->xAxis->setRange(-YAW_DATA_SIZE, 0);
    _ui->autoRollGraph->xAxis->setRange(-ROLL_DATA_SIZE, 0);

    _ui->autoDepthGraph->replot();
    _ui->autoPitchGraph->replot();
    _ui->autoYawGraph->replot();
    _ui->autoRollGraph->replot();

    SimpleCommunicator_t::PidState_t t;
    t.In = 0.f;
    t.Out = 0.f;
    t.Target = 0.f;
    for (int i = 0; i < DEPTH_DATA_SIZE; i++) {
       _depthData.push_back(t);
    }
    for (int i = 0; i < PITCH_DATA_SIZE; i++) {
       _pitchData.push_back(t);
    }
    for (int i = 0; i < YAW_DATA_SIZE; i++) {
       _yawData.push_back(t);
    }
    for (int i = 0; i < ROLL_DATA_SIZE; i++) {
        _rollData.push_back(t);
    }
}

void MainWindow::replotData()
{
    replotDataDepth();
    replotDataPitch();
    replotDataYaw();
    replotDataRoll();
}

void MainWindow::replotDataDepth()
{
    QVector<double> x(DEPTH_DATA_SIZE), y1(DEPTH_DATA_SIZE), y2(DEPTH_DATA_SIZE), y3(DEPTH_DATA_SIZE);
    for (int i = 0; i < DEPTH_DATA_SIZE; i++){
        x[i] = i + _count_of_recieved_pid - DEPTH_DATA_SIZE;
        y1[i] = _depthData[i].In;
        y2[i] = _depthData[i].Out;
        y3[i] = _depthData[i].Target;
    }
    _ui->autoDepthGraph->graph(0)->setData(x, y1);
    _ui->autoDepthGraph->graph(1)->setData(x, y2);
    _ui->autoDepthGraph->graph(2)->setData(x, y3);

    double minY = y1[0], maxY = y1[0];
    for (int i = 0; i < DEPTH_DATA_SIZE; i++) {
        minY = MIN(minY, MIN(y1[i], MIN(y2[i], y3[i])));
        minY = MAX(maxY, MAX(y1[i], MAX(y2[i], y3[i])));
    }
    minY = -50;
    maxY = 500;
    _ui->autoDepthGraph->xAxis->setRange(x[0], x[0] + DEPTH_DATA_SIZE);
    _ui->autoDepthGraph->yAxis->setRange(minY, maxY);
    _ui->autoDepthGraph->replot();
}

void MainWindow::replotDataPitch() {
    QVector<double> x(PITCH_DATA_SIZE), y1(PITCH_DATA_SIZE), y2(PITCH_DATA_SIZE), y3(PITCH_DATA_SIZE);
    for (int i = 0; i < PITCH_DATA_SIZE; i++){
        x[i] = i + _count_of_recieved_pid - PITCH_DATA_SIZE;
        y1[i] = _pitchData[i].In;
        y2[i] = _pitchData[i].Out;
        y3[i] = _pitchData[i].Target;
    }
    _ui->autoPitchGraph->graph(0)->setData(x, y1);
    _ui->autoPitchGraph->graph(1)->setData(x, y2);
    _ui->autoPitchGraph->graph(2)->setData(x, y3);

    double minY = y1[0], maxY = y1[0];
    for (int i = 0; i < PITCH_DATA_SIZE; i++) {
        minY = MIN(minY, MIN(y1[i], MIN(y2[i], y3[i])));
        minY = MAX(maxY, MAX(y1[i], MAX(y2[i], y3[i])));
    }
    _ui->autoPitchGraph->xAxis->setRange(x[0], x[0] + PITCH_DATA_SIZE);
    _ui->autoPitchGraph->yAxis->setRange(-3.1416, 3.1416);
    _ui->autoPitchGraph->replot();
}

void MainWindow::replotDataYaw()
{
    QVector<double> x(YAW_DATA_SIZE), y1(YAW_DATA_SIZE), y2(YAW_DATA_SIZE), y3(YAW_DATA_SIZE);
    for (int i = 0; i < YAW_DATA_SIZE; i++){
        x[i] = i + _count_of_recieved_pid - YAW_DATA_SIZE;
        y1[i] = _yawData[i].In;
        y2[i] = _yawData[i].Out;
        y3[i] = _yawData[i].Target;
    }
    _ui->autoYawGraph->graph(0)->setData(x, y1);
    _ui->autoYawGraph->graph(1)->setData(x, y2);
    _ui->autoYawGraph->graph(2)->setData(x, y3);

    double minY = y1[0], maxY = y1[0];
    for (int i = 0; i < YAW_DATA_SIZE; i++) {
        minY = MIN(minY, MIN(y1[i], MIN(y2[i], y3[i])));
        minY = MAX(maxY, MAX(y1[i], MAX(y2[i], y3[i])));
    }
    _ui->autoYawGraph->xAxis->setRange(x[0], x[0] + YAW_DATA_SIZE);
    _ui->autoYawGraph->yAxis->setRange(-3.1416, 3.1416);
    _ui->autoYawGraph->replot();
}

void MainWindow::replotDataRoll() {
    QVector<double> x(ROLL_DATA_SIZE), y1(ROLL_DATA_SIZE), y2(ROLL_DATA_SIZE), y3(ROLL_DATA_SIZE);
    for (int i = 0; i < ROLL_DATA_SIZE; i++){
        x[i] = i + _count_of_recieved_pid - ROLL_DATA_SIZE;
        y1[i] = _rollData[i].In;
        y2[i] = _rollData[i].Out;
        y3[i] = _rollData[i].Target;
    }
    _ui->autoRollGraph->graph(0)->setData(x, y1);
    _ui->autoRollGraph->graph(1)->setData(x, y2);
    _ui->autoRollGraph->graph(2)->setData(x, y3);

    double minY = y1[0], maxY = y1[0];
    for (int i = 0; i < ROLL_DATA_SIZE; i++) {
        minY = MIN(minY, MIN(y1[i], MIN(y2[i], y3[i])));
        minY = MAX(maxY, MAX(y1[i], MAX(y2[i], y3[i])));
    }
    _ui->autoRollGraph->xAxis->setRange(x[0], x[0] + ROLL_DATA_SIZE);
    _ui->autoRollGraph->yAxis->setRange(-3.1416, 3.1416);
    _ui->autoRollGraph->replot();
}

void MainWindow::on_receivePidStatesCheckbox_toggled(bool checked)
{
    _communicator->SetReceivePidState(checked);
}

void MainWindow::on_startAutoPitchButton_clicked()
{

}

void MainWindow::on_checkBox_toggled(bool checked)
{
    _ui->m0LoadPositiveProgressBar->setEnabled(checked);
    _ui->m0LoadNegativeProgressBar->setEnabled(checked);
    _ui->m1LoadPositiveProgressBar->setEnabled(checked);
    _ui->m1LoadNegativeProgressBar->setEnabled(checked);
    _ui->m2LoadPositiveProgressBar->setEnabled(checked);
    _ui->m2LoadNegativeProgressBar->setEnabled(checked);
    _ui->m3LoadPositiveProgressBar->setEnabled(checked);
    _ui->m3LoadNegativeProgressBar->setEnabled(checked);
    _ui->m4LoadPositiveProgressBar->setEnabled(checked);
    _ui->m4LoadNegativeProgressBar->setEnabled(checked);
    _ui->m5LoadPositiveProgressBar->setEnabled(checked);
    _ui->m5LoadNegativeProgressBar->setEnabled(checked);
    _ui->m6LoadPositiveProgressBar->setEnabled(checked);
    _ui->m6LoadNegativeProgressBar->setEnabled(checked);
    _ui->m7LoadPositiveProgressBar->setEnabled(checked);
    _ui->m7LoadNegativeProgressBar->setEnabled(checked);
    _communicator->SetReceiveMotorsState(checked);
}

void MainWindow::on_resetPositionsPushButton_clicked()
{
    _ui->xPositionVerticalSlider->setValue(0);
    _ui->yPositionVerticalSlider->setValue(0);
    _ui->zPositionVerticalSlider->setValue(0);
    _communicator->SetMovementForce(_x_pos = 0, _y_pos = 0);
    _communicator->SetLocalZForce(0);
}

void MainWindow::on_xPositionVerticalSlider_sliderMoved(int position)
{
    _x_pos = position / 100.f;
    _communicator->SetMovementForce(_x_pos, _y_pos);
}

void MainWindow::on_yPositionVerticalSlider_sliderMoved(int position)
{
    _y_pos = position / 100.f;
    _communicator->SetMovementForce(_x_pos, _y_pos);
}

void MainWindow::on_zPositionVerticalSlider_valueChanged(int value)
{
    _communicator->SetLocalZForce(value/400.f);
}

void MainWindow::on_rotationSlider_valueChanged(int value)
{
    _communicator->SetYawForce(value / 100.f);
}

void MainWindow::on_verticalSlider_valueChanged(int value)
{
    _communicator->SetPitchForce(value / 100.f);
}

void MainWindow::onAutoDepthEdit(QString value)
{
    float depth = value.toFloat();
    if (_ui->autoDepthCB->isChecked()) {
        _ui->stabDepthValue->setText(_ui->depthEdit->text());
        _communicator->SetDepth(depth);
    } else {
        _communicator->SetMotorsState(0, 0, 0, 0, 0, 0, 0, 0);
    }
}

void MainWindow::onAutoPitchEdit(QString value)
{
    float pitch = value.toFloat();
    if (_ui->autoPitchCB->isChecked()) {
        _ui->stabPitchValue->setText(_ui->pitchEdit->text());
        _communicator->SetPitch(pitch);
    } else {
        _communicator->SetMotorsState(0, 0, 0, 0, 0, 0, 0, 0);
    }
}

void MainWindow::onAutoYawEdit(QString value)
{
    float yaw = value.toFloat();
    if (_ui->autoYawCB->isChecked()) {
        _ui->stabYawValue->setText(_ui->yawEdit->text());
        _communicator->SetYaw(yaw);
    } else {
        _communicator->SetMotorsState(0, 0, 0, 0, 0, 0, 0, 0);
    }
}

void MainWindow::onAutoRollEdit(QString value)
{
    float roll = value.toFloat();
    if (_ui->autoRollCB->isChecked()) {
        _ui->stabRollValue->setText(_ui->rollEdit->text());
        _communicator->SetRoll(roll);
    } else {
        _communicator->SetMotorsState(0, 0, 0, 0, 0, 0, 0, 0);
    }
}

void MainWindow::onAutoCurrentDepthClicked(bool value)
{
    _isAutoDepth = value;
    if (value) {
        _ui->autoDepthCB->setChecked(false);
        _ui->stabDepthValue->setText(std::to_string(_currentDepth).c_str());
        _communicator->SetDepth(_currentDepth);
    } else {
        _communicator->SetMotorsState(0, 0, 0, 0, 0, 0, 0, 0);
    }
}

void MainWindow::onAutoCurrentPitchClicked(bool value)
{
    _isAutoPitch = value;
    if (value) {
        _ui->autoPitchCB->setChecked(false);
        _ui->stabPitchValue->setText(std::to_string(_currentPitch).c_str());
        _communicator->SetPitch(_currentPitch);
    } else {
        _communicator->SetMotorsState(0, 0, 0, 0, 0, 0, 0, 0);
    }
}

void MainWindow::onAutoCurrentYawClicked(bool value)
{
    if (value) {
        _ui->autoYawCB->setChecked(false);
        _ui->stabYawValue->setText(std::to_string(_currentYaw).c_str());
        _communicator->SetYaw(_currentYaw);
    } else {
        _communicator->SetMotorsState(0, 0, 0, 0, 0, 0, 0, 0);
    }
}

void MainWindow::onAutoCurrentRollClicked(bool value)
{
    if (value) {
        _ui->autoRollCB->setChecked(false);
        _ui->stabRollValue->setText(std::to_string(_currentRoll).c_str());
        _communicator->SetRoll(_currentRoll);
    } else {
        _communicator->SetMotorsState(0, 0, 0, 0, 0, 0, 0, 0);
    }
}

void MainWindow::generateMapTools()
{
    _curToolIdx = 0;
    _tools.append(new LineTool(_ui->propWidget));
    //_tools.append(new SelectTool(_ui->toolsWidget));
    _tools.append(new ContainersCenterTool(_ui->propWidget));
    _tools.append(new PoolLineTool(_ui->propWidget));
    currentTool = _tools[_curToolIdx];
}

void MainWindow::initClearButton()
{
    _clearButton = new QPushButton(_ui->toolsWidget);
    _clearButton->setText("Clear");
    _clearButton->setIconSize(QSize(30, 30));
    connect(_clearButton, SIGNAL(clicked(bool)), this, SLOT(clearScreenView()));
}

void MainWindow::clearScreenView()
{
    _mapScene->clearScene();
    _screenScene->updateScene();
}

void MainWindow::showAxis(int value)
{
    _screenScene->setShowAxes(value);
}

void MainWindow::on_cam1MinValSpinBox_valueChanged(double arg1)
{
    saveCamMinMax();
    _communicator->SetCam1MinVal(arg1);
}

void MainWindow::on_cam1MaxValSpinBox_valueChanged(double arg1)
{
    saveCamMinMax();
    _communicator->SetCam1MaxVal(arg1);
}

void MainWindow::on_cam2MinValSpinBox_valueChanged(double arg1)
{
    saveCamMinMax();
    _communicator->SetCam2MinVal(arg1);
}

void MainWindow::on_cam2MaxValSpinBox_valueChanged(double arg1)
{
    saveCamMinMax();
    _communicator->SetCam2MaxVal(arg1);
}

void MainWindow::initPIDcoeffs()
{
    std::string fname[4] = {"depth.txt", "pitch.txt", "yaw.txt", "roll.txt" };
    std::ifstream fout;
    double p, i, d;
    qDebug() << "init pid coeffs";
    try {
        for (int k = 0; k < 4; k++) {
            fout.open(fname[k], fstream::in);
            p = 0.0, i = 0.0, d = 0.0;
            if (fout.is_open()) {
                fout >> p >> i >> d;
                qDebug() << p << i << d << ">" << fname[k].c_str();
                if (k == 0) setDepthPID(p, i, d);
                if (k == 1) setPitchPID(p, i, d);
                if (k == 2) setYawPID(p, i, d);
                if (k == 3) setRollPID(p, i, d);
            } else {
                qDebug() << "Can't open file: " << fname[k].c_str();
            }
            fout.close();
        }
    } catch (std::ifstream::failure e) {
        qDebug() << "Exception opening file: " << strerror(errno);
    }
}

void MainWindow::setDepthPID(double p, double i, double d)
{
    _ui->depthPSpinBox->setValue(p);
    _ui->depthISpinBox->setValue(i);
    _ui->depthDSpinBox->setValue(d);
    _communicator->SetDepthPid(p, i, d);
}

void MainWindow::setPitchPID(double p, double i, double d)
{
    _ui->pitchPSpinBox->setValue(p);
    _ui->pitchISpinBox->setValue(i);
    _ui->pitchDSpinBox->setValue(d);
    _communicator->SetPitchPid(p, i, d);
}

void MainWindow::setYawPID(double p, double i, double d)
{
    _ui->yawPSpinBox->setValue(p);
    _ui->yawISpinBox->setValue(i);
    _ui->yawDSpinBox->setValue(d);
    _communicator->SetYawPid(p, i, d);
}

void MainWindow::setRollPID(double p, double i, double d)
{
    _ui->rollPSpinBox->setValue(p);
    _ui->rollISpinBox->setValue(i);
    _ui->rollDSpinBox->setValue(d);
    _communicator->SetRollPid(p, i, d);
}

void MainWindow::initMotorsMultipliers()
{
    std::ifstream fmult;
    double m1 = 0.0, m2 = 0.0, m3 = 0.0, m4 = 0.0, m5 = 0.0, m6 = 0.0, m7 = 0.0, m8 = 0.0;
    qDebug() << "init motors multipliers";
    try {
        fmult.open("multipliers.txt", fstream::in);
        if (fmult.is_open()) {
            fmult >> m1  >> m2 >> m3 >> m4 >> m5 >> m6 >> m7 >> m8;
            qDebug() << "muplipliers: " << m1 << m2 << m3 << m4 << m5 << m6 << m7 << m8;
            _ui->m1MultSpinBox->setValue(m1);
            _ui->m2MultSpinBox->setValue(m2);
            _ui->m3MultSpinBox->setValue(m3);
            _ui->m4MultSpinBox->setValue(m4);
            _ui->m5MultSpinBox->setValue(m5);
            _ui->m6MultSpinBox->setValue(m6);
            _ui->m7MultSpinBox->setValue(m7);
            _ui->m8MultSpinBox->setValue(m8);
            _communicator->SetMotorsMultiplier(m1, m2, m3, m4, m5, m6, m7, m8);
        } else {
            qDebug() << "Can't open file: " << "multipliers.txt";
        }
        fmult.close();
    } catch (std::ifstream::failure e) {
        qDebug() << "Exception opening file: " << strerror(errno);
    }
}

void MainWindow::initCameraMinMax()
{
    std::ifstream fin;
    double cam1min = 150, cam1max = 600, cam2min = 150, cam2max = 600;
    try {
        fin.open("campos.txt", fstream::in);
        if (fin.is_open()) {
            fin >> cam1min >> cam1max >> cam2min >> cam2max;
            qDebug() << "camera positions: " << cam1min << cam1max << cam2min << cam2max;
            _ui->cam1MinValSpinBox->setValue(cam1min);
            _ui->cam1MaxValSpinBox->setValue(cam1max);
            _ui->cam2MinValSpinBox->setValue(cam2min);
            _ui->cam2MaxValSpinBox->setValue(cam2max);
            _communicator->SetCam1MinVal(cam1min);
            _communicator->SetCam1MaxVal(cam1max);
            _communicator->SetCam2MinVal(cam2min);
            _communicator->SetCam2MaxVal(cam2max);
        } else {
            qDebug() << "Can't open file: campos.txt";
        }
        fin.close();
    } catch (std::ifstream::failure e) {
        qDebug() << "Exception opening file: " << strerror(errno);
    }
}

void MainWindow::saveCamMinMax()
{
    std::ofstream fout;
    fout.open("campos.txt");
    double cam1min, cam1max, cam2min, cam2max;
    cam1min = _ui->cam1MinValSpinBox->value();
    cam1max = _ui->cam1MaxValSpinBox->value();
    cam2min = _ui->cam2MinValSpinBox->value();
    cam2max = _ui->cam2MaxValSpinBox->value();
    if (fout.is_open()) {
        fout << cam1min << " " << cam1max << " " << cam2min << " " << cam2max;
    } else {
        qDebug() << "Can't open file: campos.txt";
    }
}

void MainWindow::initIMUCalibration()
{
    std::ifstream fin;
    float xGyroOffset = 0, yGyroOffset = 0, zGyroOffset = 0, gyroScale = 1,
          xAccelOffset = 0, yAccelOffset = 0, zAccelOffset = 0,
          xAccelScale = 0, yAccelScale = 0, zAccelScale = 0;
    try {
        fin.open("IMUCalibration.txt", fstream::in);
        if (fin.is_open()) {
            fin >> xGyroOffset >> yGyroOffset >> zGyroOffset >> gyroScale
                >> xAccelOffset >> yAccelOffset >> zAccelOffset
                >> xAccelScale >> yAccelScale >> zAccelScale;
            _ui->xGyroOffset_LineEdit->setText(std::to_string(xGyroOffset).c_str());
            _ui->yGyroOffset_LineEdit->setText(std::to_string(yGyroOffset).c_str());
            _ui->zGyroOffset_LineEdit->setText(std::to_string(zGyroOffset).c_str());
            _ui->GyroScale_LineEdit->setText(std::to_string(gyroScale).c_str());

            _ui->xAccelOffset_LineEdit->setText(std::to_string(xAccelOffset).c_str());
            _ui->yAccelOffset_LineEdit->setText(std::to_string(yAccelOffset).c_str());
            _ui->zAccelOffset_LineEdit->setText(std::to_string(zAccelOffset).c_str());

            _ui->xAccelScale_LineEdit->setText(std::to_string(xAccelScale).c_str());
            _ui->yAccelScale_LineEdit->setText(std::to_string(yAccelScale).c_str());
            _ui->zAccelScale_LineEdit->setText(std::to_string(zAccelScale).c_str());

            _communicator->SetGyroConfig(xGyroOffset, yGyroOffset, zGyroOffset, gyroScale);
            _communicator->SetAccelConfig(xAccelOffset, yAccelOffset, zAccelOffset,
                                          xAccelScale, yAccelScale, zAccelScale);
        } else {
            qDebug() << "Can't open file: IMUCalibration.txt";
        }
        fin.close();
    } catch (std::ifstream::failure e) {
        qDebug() << "Exception opening file: " << strerror(errno);
    }
}

void MainWindow::saveIMUCalibration()
{
    std::ofstream fout;
    fout.open("IMUCalibration.txt");
    if (fout.is_open()) {
        fout
            << _ui->xGyroOffset_LineEdit->text().toFloat() << std::endl
            << _ui->yGyroOffset_LineEdit->text().toFloat() << std::endl
            << _ui->zGyroOffset_LineEdit->text().toFloat() << std::endl
            << _ui->GyroScale_LineEdit->text().toFloat() << std::endl
            << _ui->xAccelOffset_LineEdit->text().toFloat() << std::endl
            << _ui->yAccelOffset_LineEdit->text().toFloat() << std::endl
            << _ui->zAccelOffset_LineEdit->text().toFloat() << std::endl
            << _ui->xAccelScale_LineEdit->text().toFloat() << std::endl
            << _ui->yAccelScale_LineEdit->text().toFloat() << std::endl
            << _ui->zAccelScale_LineEdit->text().toFloat() << std::endl
        ;
    } else {
        qDebug() << "Can't open file: IMUCalibration.txt";
    }
}

void MainWindow::initStabilizationUpdateFrequency()
{
    std::ifstream fin("StabilizationUpdateFrequency.txt");
    int stabilization_update_frequency = 100;
    if (fin.is_open()) {
        fin >> stabilization_update_frequency;
    } else {
        qDebug() << "Can't open file: StabilizationUpdateFrequency.txt";
    }
    _communicator->SetStabilizationUpdateFrequency(stabilization_update_frequency);
    _ui->UpdateFrequencyEdit->setText(QString(std::to_string(stabilization_update_frequency).c_str()));
}

void MainWindow::on_nextStepButton_clicked(bool checked)
{
    if (!_ui->previousStepButton->isEnabled()) {
        _ui->previousStepButton->setEnabled(true);
    }
    _curToolIdx++;
    setCurrentTool();
    if (_curToolIdx == _tools.size() - 1) {
        _ui->nextStepButton->setEnabled(false);
        return;
    }
}

void MainWindow::on_previousStepButton_clicked(bool checked)
{
    if (!_ui->nextStepButton->isEnabled()) {
        _ui->nextStepButton->setEnabled(true);
    }
    _curToolIdx--;
    setCurrentTool();
    if (_curToolIdx == 0) {
        _ui->previousStepButton->setEnabled(false);
        return;
    }
}

void MainWindow::setCurrentTool()
{
    currentTool->destroyProperties();
    currentTool = _tools[_curToolIdx];
    _ui->currentToolLabel->setText(currentTool->getInfo());
}

void MainWindow::setMotorsPos()
{
    _communicator->SetMotorsPositions(0, 1, 2, 3, 4, 5, 6, 7);
}

void MainWindow::on_ReceiveRawIMUValues_CheckBox_toggled(bool checked)
{
    _communicator->SetReceiveRawSensorData(checked);
}

void MainWindow::on_CalibrateGyro_PushButton_pressed()
{
}

void MainWindow::on_ReceiveCalibratedIMUValues_CheckBox_toggled(bool checked)
{
    _communicator->SetReceiveCalibratedSensorData(checked);
}

void MainWindow::on_CalibrateGyro_PushButton_released()
{
}

void MainWindow::on_setCalibrationValues_clicked()
{

}

void MainWindow::on_setCalibrationValues_PushButton_clicked()
{
    saveIMUCalibration();
    _communicator->SetGyroConfig(
        _ui->xGyroOffset_LineEdit->text().toFloat(),
        _ui->yGyroOffset_LineEdit->text().toFloat(),
        _ui->zGyroOffset_LineEdit->text().toFloat(),
        _ui->GyroScale_LineEdit->text().toFloat()
    );
}

void MainWindow::on_CalibrateGyro_PushButton_toggled(bool checked)
{
    if (checked) {
        _xGyroOffset = _yGyroOffset = _zGyroOffset = 0;
        _calibrateIteration = 0;
        _ui->ReceiveRawIMUValues_CheckBox->setChecked(true);
    }
}

void MainWindow::on_invertCB_clicked(bool checked)
{
    qDebug() << checked;
    _signDirection = (checked) ? -1 : 1;
}

void MainWindow::on_SetUpdateFrequencyButton_clicked()
{
    int stabilization_update_frequency = _ui->UpdateFrequencyEdit->text().toInt();
    ofstream fout("StabilizationUpdateFrequency.txt");
    fout << stabilization_update_frequency;
    _communicator->SetStabilizationUpdateFrequency(stabilization_update_frequency);
}
