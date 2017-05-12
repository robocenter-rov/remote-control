#include "mainwindow.h"
#include <iostream>
#include <fstream>
#include "calc-tools/basetool.h"

QString COMportName;

static QColor msgColor[3] = {
    QColor(0, 204, 102),
    QColor(255, 102, 102),
    QColor(255, 204, 102),
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
      _joy(new Joystick())
{
    qRegisterMetaType<SimpleCommunicator_t::PidState_t>("SimpleCommunicator_t::PidState_t");
    qRegisterMetaType<SimpleCommunicator_t::State_t>("SimpleCommunicator_t::State_t");
    qRegisterMetaType<SimpleCommunicator_t::CalibratedSensorData_t>("SimpleCommunicator_t::CalibratedSensorData_t");
    _connectionProvider = new UARTConnectionProvider_t(COMportName.toStdString().c_str(), 19200, 1 << 20, 1 << 20);
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
    connect(_ui->stopMotorsButton, SIGNAL(clicked(bool)), this, SLOT(onStopMotorsButtonClicked(bool)));
    connect(_ui->SetMotorsIdx, SIGNAL(clicked(bool)), this, SLOT(onSetMotorsClicked(bool)));
    connect(_ui->camera1Slider, SIGNAL(valueChanged(int)), this, SLOT(onCamera1PosChanged(int)));
    connect(_ui->camera2Slider, SIGNAL(valueChanged(int)), this, SLOT(onCamera2PosChanged(int)));
    connect(_ui->depthPIDButtton, SIGNAL(clicked(bool)), this, SLOT(onDepthPIDSpinBoxChanged(bool)));
    connect(_ui->pitchPIDButtton, SIGNAL(clicked(bool)), this, SLOT(onPitchPIDSpinBoxChanged(bool)));
    connect(_ui->yawPIDButtton, SIGNAL(clicked(bool)), this, SLOT(onYawPIDSpinBoxChanged(bool)));
    connect(_ui->setMotorsMulButton, SIGNAL(clicked(bool)), this, SLOT(onSetMotorsMultiplier(bool)));
    connect(_ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(onTabChanged(int)));
    connect(_ui->autoDepthCB, SIGNAL(clicked(bool)), this, SLOT(onAutoDepthClicked(bool)));
    connect(_ui->autoPitchCB, SIGNAL(clicked(bool)), this, SLOT(onAutoPitchClicked(bool)));
    connect(_ui->autoYawCB, SIGNAL(clicked(bool)), this, SLOT(onAutoYawClicked(bool)));
    connect(_ui->servo1Slider, SIGNAL(valueChanged(int)), this, SLOT(onServo1SliderChanged(int)));
    connect(_ui->useJoyButton, SIGNAL(clicked(bool)), this, SLOT(onUseJoyCheckButtonClicked(bool)));
    connect(_ui->depthEdit, SIGNAL(textEdited(QString)), this, SLOT(onAutoDepthEdit(QString)));
    connect(_ui->pitchEdit, SIGNAL(textEdited(QString)), this, SLOT(onAutoPitchEdit(QString)));
    connect(_ui->yawEdit, SIGNAL(textEdited(QString)), this, SLOT(onAutoYawEdit(QString)));
    connect(_ui->autoDepthCurrentCB, SIGNAL(clicked(bool)), this, SLOT(onAutoCurrentDepthClicked(bool)));
    connect(_ui->autoPitchCurrentCB, SIGNAL(clicked(bool)), this, SLOT(onAutoCurrentPitchClicked(bool)));
    connect(_ui->autoYawCurrentCB, SIGNAL(clicked(bool)), this, SLOT(onAutoCurrentYawClicked(bool)));
    connectionProviderInit();

    connect(_messageTimer, SIGNAL(timeout()), this, SLOT(hideMessage()));
    _messageTimer->setInterval(2000);

    connect(_joyTimer, SIGNAL(timeout()), this, SLOT(readAndSendJoySensors()));
    _joyTimer->setInterval(100);

    connect(_joy, SIGNAL(joyButtonEvent()), this, SLOT(joyButtonHandle()));
    connect(this, SIGNAL(connectionChangedEvent(bool)), this, SLOT(updateConnectionStatus(bool)));
    connect(this, SIGNAL(stateChangedEvent(SimpleCommunicator_t::State_t)), this, SLOT(updateStatus(SimpleCommunicator_t::State_t)));
    connect(this, SIGNAL(rawSensorDataRecievedEvent(SimpleCommunicator_t::RawSensorData_t)), this, SLOT(updatePosInfo(SimpleCommunicator_t::RawSensorData_t)));
    connect(this, SIGNAL(leakEvent(int, int)), this, SLOT(onLeak(int, int)));
    connect(this, SIGNAL(orientationReceivedEvent(float,float,float,float)), this, SLOT(updateOrient(float,float,float,float)));
    connect(this, SIGNAL(I2CDevicesRecieveEvent(bool,bool,bool,bool,bool,bool,bool)), this, SLOT(updateI2CDevicesState(bool,bool,bool,bool,bool,bool,bool)));
    connect(this, SIGNAL(bluetoothMsgRecieveEvent(std::string)), this, SLOT(onBluetoothMsgRecieve(std::string)));
    connect(this, SIGNAL(depthRecieveEvent(float)), this, SLOT(updateDepth(float)));
    connect(this, SIGNAL(motorStateReceiveEvent(float,float,float,float,float,float)), this, SLOT(onMotorStateRecieved(float,float,float,float,float,float)));
    connect(this, SIGNAL(pidStateReceiveEvent(SimpleCommunicator_t::PidState_t,SimpleCommunicator_t::PidState_t,SimpleCommunicator_t::PidState_t)), this, SLOT(onPidStateReceived(SimpleCommunicator_t::PidState_t,SimpleCommunicator_t::PidState_t,SimpleCommunicator_t::PidState_t)));

    showMessage("Connection...", CL_YELLOW);
    graphInit();
    replotData();
    generateMapTools();
    connect(_ui->axesCheckBox, SIGNAL(stateChanged(int)), this, SLOT(showAxis(int)));
    initPIDcoeffs();
    initMotorsMultipliers();
    initCameraMinMax();
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
    _mainCamera = new RoboCamera(0, _ui->mainView, this, _videoScene, true);
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

    _extraCamera = new RoboCamera(1, _ui->extraView, this);
}

bool MainWindow::eventFilter(QObject *, QEvent *event)
{
    _joy->handleEvent();
    if (event->type() == QEvent::Resize) {
        _ui->mainView->fitInView(_mainCamera->getScene()->sceneRect(), Qt::IgnoreAspectRatio);
        _ui->extraView->fitInView(_extraCamera->getScene()->sceneRect(), Qt::KeepAspectRatio);
        _ui->screensView->fitInView(_mainCamera->getScene()->sceneRect(), Qt::KeepAspectRatio);
        _ui->mapView->fitInView(_mainCamera->getScene()->sceneRect(), Qt::KeepAspectRatio);
        return true;
    }
    return false;
}

void MainWindow::updateDepth(float depth)
{
    _currentDepth = depth;
    _ui->depthValueLabel->setText(std::to_string(depth).c_str());
    _mainCamera->getVideoWidget()->setCurrentDepth(depth);
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

    _ui->minutesLCDNumber->display(min);
    _ui->secondsLCDNumber->display(sec);
}

#include <string>

void MainWindow::connectionProviderInit()
{
    try {
        _communicator->SetSendMessageFrequency(50);

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
                        devices.ADXL345,
                        devices.HMC58X3,
                        devices.ITG3200,
                        devices.BMP085,
                        devices.MS5803);
        });

        _communicator->OnStateChange([&](SimpleCommunicator_t::State_t state){
            emit stateChangedEvent(state);
        });

        /*_communicator->OnRawSensorDataReceive([&](SimpleCommunicator_t::RawSensorData_t rawSensorData){
            emit rawSensorDataRecievedEvent(rawSensorData);
        });*/

        _communicator->OnOrientationReceive([&](SimpleCommunicator_t::Orientation_t o){
            emit orientationReceivedEvent(o.q1, o.q2, o.q3, o.q4);
        });

        _communicator->OnBluetoothMsgReceive([&](std::string msg){
            emit bluetoothMsgRecieveEvent(msg);
        });

        _communicator->OnDepthReceive([&](float depth){
            emit depthRecieveEvent(depth);
        });

        _communicator->OnMotorsStateReceive([&](SimpleCommunicator_t::MotorsState_t motorState){

            emit motorStateReceiveEvent(motorState.M1Force, motorState.M2Force, motorState.M3Force,
                                        motorState.M4Force, motorState.M5Force, motorState.M6Force);
        });

        _communicator->OnPidStateReceive([&](SimpleCommunicator_t::PidState_t depth,
                                         SimpleCommunicator_t::PidState_t yaw,
                                         SimpleCommunicator_t::PidState_t pitch){
            emit pidStateReceiveEvent(depth, yaw, pitch);
        });
        //_communicator->SetReceiveRawSensorData(true);

        _communicator->Begin();
    } catch (CantOpenPortException_t &e) {
        qDebug() << e.error_message.c_str() << "Port name: " << e.port_name.c_str();
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
    float z = (ABS(thrust[4]) < 0.10) ? 0 : thrust[4];
    float ty = (ABS(thrust[2]) < eps) ? 0 : thrust[2];;
    float tz = (ABS(thrust[3]) < eps) ? 0 : thrust[3];
    float dist = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));

    if (dist > 1) {
        x /= dist;
        y /= dist;
    }
    if (z == 0 && _isAutoDepth) {
        if (!_communicator->IsAutoDepthEnabled()) {
            _communicator->SetDepth(_currentDepth);
            _ui->autoDepthMainInfoCB->setChecked(true);
            _ui->autoDepthMainInfoCB->setText(QString("AutoDepth: ") + std::to_string(_currentDepth).c_str());
        }
    } else {
        _ui->autoDepthMainInfoCB->setChecked(false);
        _ui->autoDepthMainInfoCB->setText(QString("AutoDepth"));
        _communicator->SetSinkingForce(z * 2);
    }
    if (tz == 0 && _isAutoYaw) {
        if (!_communicator->IsAutoYawEnabled()) {
            _communicator->SetYaw(_currentYaw);
            _ui->autoYawMainInfoCB->setChecked(true);
            _ui->autoYawMainInfoCB->setText(QString("AutoYaw: ") + std::to_string(_currentYaw).c_str());
        }
    } else {
        _ui->autoYawMainInfoCB->setChecked(false);
        _ui->autoYawMainInfoCB->setText(QString("AutoYaw"));
        _communicator->SetYawForce(tz*0.4);
    }
    if (ty == 0 && _isAutoPitch) {
        if (!_communicator->IsAutoPitchEnabled()) {
            _communicator->SetPitch(_currentPitch);
            _ui->autoPitchMainInfoCB->setChecked(true);
            _ui->autoPitchMainInfoCB->setText(QString("AutoPitch: ") + std::to_string(_currentPitch).c_str());
        }
    } else {
        _ui->autoPitchMainInfoCB->setChecked(false);
        _ui->autoPitchMainInfoCB->setText(QString("AutoPitch"));
        _communicator->SetPitchForce(_currentPitch);
    }
    _communicator->SetMovementForce(-x * 1.5, y * 1.5);
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
    if (_joy->atBtn(0)){
        _curManipulator._handPos = -0.3f;
    }
    if (_joy->atBtn(1)) {
        _curManipulator._handPos = 0.3f;
    }
    if (_joy->atBtn(9)) {
         _curManipulator._armPos = 0.3f;
    }
    if (_joy->atBtn(10)) {
        _curManipulator._armPos = -0.3f;
    }
    if (_joy->atBtn(2)) {
        if (_joy->btnStateChanged(2)) {
            cameraPos1 = MIN(3.14f/2.0, cameraPos1 + 0.1);
            _communicator->SetCamera1GlobalPos(cameraPos1);
            qDebug() << cameraPos1;
        }
    }
    if (_joy->atBtn(4)) {
        if (_joy->btnStateChanged(4)) {
            cameraPos1 = MAX(-3.14f/2.0, cameraPos1 - 0.1);
            _communicator->SetCamera1GlobalPos(cameraPos1);
            qDebug() << cameraPos1;
        }
    }
    if (_joy->atBtn(5)) {
        if (_joy->btnStateChanged(5)) {
            cameraPos2 = MIN(3.14f/2.0, cameraPos2 + 0.1);
            _communicator->SetCamera2GlobalPos(cameraPos2);
            qDebug() << cameraPos2;
        }
    }
    if (_joy->atBtn(6)) {
        if (_joy->btnStateChanged(6)) {
            cameraPos2 = MAX(-3.14f/2.0, cameraPos2 - 0.1);
            _communicator->SetCamera2GlobalPos(cameraPos2);
            qDebug() << cameraPos2;
        }
    }
    if (_joy->atBtn(13)) {
        if (_joy->btnStateChanged(13)) {
            _communicator->SetFlashlightState(_flashLightState = !_flashLightState);
        }
    }
    if (_joy->atBtn(7)) {
        if (_joy->btnStateChanged(7)) {
            _isAutoDepth = !_isAutoDepth;
        }
    }
    if (_joy->atBtn(8)) {
        if (_joy->btnStateChanged(8)) {
            _isAutoYaw = !_isAutoYaw;
        }
    }
    if (_joy->atBtn(3)) {
        if (_joy->btnStateChanged(3)) {
            _isAutoPitch = !_isAutoPitch;
        }
    }
    _communicator->SetManipulatorState(
        _curManipulator._armPos,
        _curManipulator._handPos,
        _curManipulator._m1,
        _curManipulator._m2
    );
}

void MainWindow::onLeak(int send, int receive)
{
    std::string s = "ATTENTION: Leak: send ";
    s += std::to_string(send); s += ", recieved "; s += std::to_string(receive);
    showMessageByTimer(s.c_str(), CL_RED);
}

void MainWindow::updateOrient(float q1, float q2, float q3, float q4)
{
    float angles[3];
    angles[0] = atan2(2 * q2 * q3 - 2 * q1 * q4, 2 * q1 * q1 + 2 * q2 * q2 - 1); // psi
    angles[1] = -asin(2 * q2 * q4 + 2 * q1 * q3); // theta
    angles[2] = atan2(2 * q3 * q4 - 2 * q1 * q2, 2 * q1 * q1 + 2 * q4 * q4 - 1); // phi
    _ui->psiLabel->setText(std::to_string(angles[0]).c_str());
    _ui->thetaLabel->setText(std::to_string(angles[1]).c_str());
    _ui->phiLabel->setText(std::to_string(angles[2]).c_str());
    updateHeading(angles[0]*180/3.1416);
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
        bool PCA1, bool PCA2, bool ADXL345, bool HMC58X3, bool ITG3200, bool BMP085, bool MS5803)
{
    _ui->radioPCA1->setChecked(PCA1);
    _ui->radioPCA2->setChecked(PCA2);
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

void MainWindow::onBluetoothMsgRecieve(std::string msg)
{
    _ui->bluetoothLabel->setText(msg.c_str());
}

void MainWindow::onBluetoothButtonClick(bool value)
{
    _communicator->SetReadBluetoothState(value);
}

void MainWindow::onMotor1SliderChanged(int value)
{
    float val = value/100.0;
    _ui->motor1valueLabel->setText(QString(std::to_string(value).c_str()) + "%");
    _communicator->SetMotorState(0, val);
}

void MainWindow::onMotor2SliderChanged(int value)
{
    float val = value/100.0;
    _ui->motor2valueLabel->setText(QString(std::to_string(value).c_str()) + "%");
    _communicator->SetMotorState(1, val);
}

void MainWindow::onMotor3SliderChanged(int value)
{
    float val = value/100.0;
    _ui->motor3valueLabel->setText(QString(std::to_string(value).c_str()) + "%");
    _communicator->SetMotorState(2, val);
}

void MainWindow::onMotor4SliderChanged(int value)
{
    float val = value/100.0;
    _ui->motor4valueLabel->setText(QString(std::to_string(value).c_str()) + "%");
    _communicator->SetMotorState(3, val);
}

void MainWindow::onMotor5SliderChanged(int value)
{
    float val = value/100.0;
    _ui->motor5valueLabel->setText(QString(std::to_string(value).c_str()) + "%");
    _communicator->SetMotorState(4, val);
}

void MainWindow::onMotor6SliderChanged(int value)
{
    float val = value/100.0f;
    _ui->motor6valueLabel->setText(QString(std::to_string(value).c_str()) + "%");
    _communicator->SetMotorState(5, val);
}

void MainWindow::onStopMotorsButtonClicked(bool value)
{
    _ui->motor1Slider->setValue(0);
    _ui->motor2Slider->setValue(0);
    _ui->motor3Slider->setValue(0);
    _ui->motor4Slider->setValue(0);
    _ui->motor5Slider->setValue(0);
    _ui->motor6Slider->setValue(0);
    _communicator->SetMotorsState(0, 0, 0, 0, 0, 0);
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
    int m[6];
    m[0] = _ui->motorIdxSpinBox_1->value() - 1;
    m[1] = _ui->motorIdxSpinBox_2->value() - 1;
    m[2] = _ui->motorIdxSpinBox_3->value() - 1;
    m[3] = _ui->motorIdxSpinBox_4->value() - 1;
    m[4] = _ui->motorIdxSpinBox_5->value() - 1;
    m[5] = _ui->motorIdxSpinBox_6->value() - 1;

    for (int i = 0; i < 6; i++) {
        for (int j = i + 1; j < 6; j++) {
            if (m[i] == m[j]) {
                _ui->setMotorsMsg->setText("Motors should not be the same");
                return;
            }
        }
    }
    _ui->setMotorsMsg->setText("");
    _communicator->SetMotorsPositions(m[0], m[1], m[2], m[3], m[4], m[5]);
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
    std::ofstream fout;
    fout.open("multipliers.txt");
    if (fout.is_open()) {
        fout << m1 << " " << m2 << " " << m3 << " " << m4 << " " << m5 << " " << m6;
        _communicator->SetMotorsMultiplier(m1, m2, m3, m4, m5, m6);
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
    std::ofstream fout;
    fout.open("pitch.txt");
    if (fout.is_open()) {
        fout << p << " " << i << " "<< d;
        _communicator->SetPitcPid(p, i, d);
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
    std::ofstream fout;
    fout.open("yaw.txt");
    if (fout.is_open()) {
        fout << p << " " << i << " "<< d;
        _communicator->SetYawPid(p, i, d);
        fout.close();
    } else {
        qDebug() << "Cant't open file: yaw.txt";
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
        _communicator->SetMotorsState(0, 0, 0, 0, 0, 0);
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
        _communicator->SetMotorsState(0, 0, 0, 0, 0, 0);
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
        _communicator->SetMotorsState(0, 0, 0, 0, 0, 0);
    }
}

void MainWindow::onServo1SliderChanged(int value)
{
    try { /* DO: check values */
        _curManipulator._m1 = value*3.1415/180.0+3.1415;
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

void MainWindow::onMotorStateRecieved(float m1, float m2, float m3, float m4, float m5, float m6)
{
    _ui->m1curLabel->setText(QString(std::to_string(m1*100).c_str()) + "%");
    _ui->m2curLabel->setText(QString(std::to_string(m2*100).c_str()) + "%");
    _ui->m3curLabel->setText(QString(std::to_string(m3*100).c_str()) + "%");
    _ui->m4curLabel->setText(QString(std::to_string(m4*100).c_str()) + "%");
    _ui->m5curLabel->setText(QString(std::to_string(m5*100).c_str()) + "%");
    _ui->m6curLabel->setText(QString(std::to_string(m6*100).c_str()) + "%");

    _ui->m0LoadPositiveProgressBar->setValue(std::max(0.f, m1*100));
    _ui->m0LoadNegativeProgressBar->setValue(std::min(0.f, m1*100)*-1);

    _ui->m1LoadPositiveProgressBar->setValue(std::max(0.f, m2*100));
    _ui->m1LoadNegativeProgressBar->setValue(std::min(0.f, m2*100)*-1);

    _ui->m2LoadPositiveProgressBar->setValue(std::max(0.f, m3*100));
    _ui->m2LoadNegativeProgressBar->setValue(std::min(0.f, m3*100)*-1);

    _ui->m3LoadPositiveProgressBar->setValue(std::max(0.f, m4*100));
    _ui->m3LoadNegativeProgressBar->setValue(std::min(0.f, m4*100)*-1);

    _ui->m4LoadPositiveProgressBar->setValue(std::max(0.f, m5*100));
    _ui->m4LoadNegativeProgressBar->setValue(std::min(0.f, m5*100)*-1);

    _ui->m5LoadPositiveProgressBar->setValue(std::max(0.f, m6*100));
    _ui->m5LoadNegativeProgressBar->setValue(std::min(0.f, m6*100)*-1);
}

void MainWindow::onPidStateReceived(SimpleCommunicator_t::PidState_t depth, SimpleCommunicator_t::PidState_t yaw, SimpleCommunicator_t::PidState_t pitch)
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

    _depthData.pop_front();
    _depthData.push_back(depth);
    _pitchData.pop_front();
    _pitchData.push_back(pitch);
    _yawData.pop_front();
    _yawData.push_back(yaw);
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

        _ui->autoDepthGraph->graph(i)->setData(x, y);
        _ui->autoPitchGraph->graph(i)->setData(x, y);
        _ui->autoYawGraph->graph(i)->setData(x, y);

        _ui->autoDepthGraph->graph(i)->setPen(colors[i]);
        _ui->autoPitchGraph->graph(i)->setPen(colors[i]);
        _ui->autoYawGraph->graph(i)->setPen(colors[i]);
    }
    _ui->autoDepthGraph->xAxis->setRange(-DEPTH_DATA_SIZE, 0);
    _ui->autoPitchGraph->xAxis->setRange(-PITCH_DATA_SIZE, 0);
    _ui->autoYawGraph->xAxis->setRange(-YAW_DATA_SIZE, 0);

    _ui->autoDepthGraph->replot();
    _ui->autoPitchGraph->replot();
    _ui->autoYawGraph->replot();

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
}

void MainWindow::replotData()
{
    replotDataDepth();
    replotDataPitch();
    replotDataYaw();
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
    if (minY > 0) minY = 25;
    if (maxY < 50) maxY = 35;
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
    _communicator->SetReceiveMotorsState(checked);
}

void MainWindow::on_resetPositionsPushButton_clicked()
{
    _ui->xPositionVerticalSlider->setValue(0);
    _ui->yPositionVerticalSlider->setValue(0);
    _communicator->SetMovementForce(_x_pos = 0, _y_pos = 0);
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
        _communicator->SetMotorsState(0, 0, 0, 0, 0, 0);
    }
}

void MainWindow::onAutoPitchEdit(QString value)
{
    float pitch = value.toFloat();
    if (_ui->autoPitchCB->isChecked()) {
        _ui->stabPitchValue->setText(_ui->pitchEdit->text());
        _communicator->SetPitch(pitch);
    } else {
        _communicator->SetMotorsState(0, 0, 0, 0, 0, 0);
    }
}

void MainWindow::onAutoYawEdit(QString value)
{
    float yaw = value.toFloat();
    if (_ui->autoYawCB->isChecked()) {
        _ui->stabYawValue->setText(_ui->yawEdit->text());
        _communicator->SetYaw(yaw);
    } else {
        _communicator->SetMotorsState(0, 0, 0, 0, 0, 0);
    }
}

void MainWindow::onAutoCurrentDepthClicked(bool value)
{
    if (value) {
        _ui->autoDepthCB->setChecked(false);
        _ui->stabDepthValue->setText(std::to_string(_currentDepth).c_str());
        _communicator->SetDepth(_currentDepth);
    } else {
        _communicator->SetMotorsState(0, 0, 0, 0, 0, 0);
    }
}

void MainWindow::onAutoCurrentPitchClicked(bool value)
{
    if (value) {
        _ui->autoPitchCB->setChecked(false);
        _ui->stabPitchValue->setText(std::to_string(_currentPitch).c_str());
        _communicator->SetPitch(_currentPitch);
    } else {
        _communicator->SetMotorsState(0, 0, 0, 0, 0, 0);
    }
}

void MainWindow::onAutoCurrentYawClicked(bool value)
{
    if (value) {
        _ui->autoYawCB->setChecked(false);
        _ui->stabYawValue->setText(std::to_string(_currentYaw).c_str());
        _communicator->SetYaw(_currentYaw);
    } else {
        _communicator->SetMotorsState(0, 0, 0, 0, 0, 0);
    }
}

void MainWindow::generateMapTools()
{
    currentTool = new LineTool(_ui->toolsWidget);
    _tools.append(currentTool);
    _tools.append(new SelectTool(_ui->toolsWidget));
    _tools.append(new PoolLineTool(_ui->toolsWidget));
    _tools.append(new ReplaceAxisTool(_ui->toolsWidget));
    _tools.append(new ContainersCenterTool(_ui->toolsWidget));
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
    std::ifstream fDepth, fPitch, fYaw;
    double p, i, d;
    qDebug() << "init pid coeffs";
    try {
        fDepth.open("depth.txt", fstream::in);
        p = 0.0, i = 0.0, d = 0.0;
        if (fDepth.is_open()) {
            fDepth >> p >> i >> d;
            qDebug() << "depth: " << p << i << d;
            setDepthPID(p, i, d);
        } else {
            qDebug() << "Can't open file: " << "depth.txt";
        }
        fDepth.close();

        fPitch.open("pitch.txt", fstream::in);
        p = 0.0; i = 0.0; d = 0.0;
        if (fPitch.is_open()) {
            fPitch >> p >> i >> d;
            qDebug() << "pitch: " << p << i << d;
            setPitchPID(p, i, d);
        } else {
            qDebug() << "Can't open file: " << "pitch.txt";
        }
        fPitch.close();

        fYaw.open("yaw.txt", fstream::in);
        p = 0.0; i = 0.0; d = 0.0;
        if (fYaw.is_open()) {
            fYaw >> p >> i >> d;
            qDebug() << "yaw: " << p << i << d;
            setYawPID(p, i, d);
        } else {
            qDebug() << "Can't open file: " << "yaw.txt";
        }
        fYaw.close();
    } catch (std::ifstream::failure e) {
        qDebug() << "Exception opening file: " << std::strerror(errno);
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
    _communicator->SetPitcPid(p, i, d);
}

void MainWindow::setYawPID(double p, double i, double d)
{
    _ui->yawPSpinBox->setValue(p);
    _ui->yawISpinBox->setValue(i);
    _ui->yawDSpinBox->setValue(d);
    _communicator->SetYawPid(p, i, d);
}

void MainWindow::initMotorsMultipliers()
{
    std::ifstream fmult;
    double m1 = 0.0, m2 = 0.0, m3 = 0.0, m4 = 0.0, m5 = 0.0, m6 = 0.0;
    qDebug() << "init motors multipliers";
    try {
        fmult.open("multipliers.txt", fstream::in);
        if (fmult.is_open()) {
            fmult >> m1  >> m2 >> m3 >> m4 >> m5 >> m6;
            qDebug() << "muplipliers: " << m1 << m2 << m3 << m4 << m5 << m6;
            _ui->m1MultSpinBox->setValue(m1);
            _ui->m2MultSpinBox->setValue(m2);
            _ui->m3MultSpinBox->setValue(m3);
            _ui->m4MultSpinBox->setValue(m4);
            _ui->m5MultSpinBox->setValue(m5);
            _ui->m6MultSpinBox->setValue(m6);
            _communicator->SetMotorsMultiplier(m1, m2, m3, m4, m5, m6);
        } else {
            qDebug() << "Can't open file: " << "multipliers.txt";
        }
        fmult.close();
    } catch (std::ifstream::failure e) {
        qDebug() << "Exception opening file: " << std::strerror(errno);
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
        qDebug() << "Exception opening file: " << std::strerror(errno);
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
