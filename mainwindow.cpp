#include "mainwindow.h"
#include <iostream>
#include <fstream>
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
    qDebug() << COMportName;
    _connectionProvider = new UARTConnectionProvider_t(COMportName.toStdString().c_str(), 115200, 200, 200);
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

    showMessage("Connection...", CL_YELLOW);
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
    _mainCamera = new RoboCamera(0, _ui->mainView, this, new QGraphicsScene(), true);
    _extraCamera = new RoboCamera(1, _ui->extraView, this);
}

bool MainWindow::eventFilter(QObject *, QEvent *event)
{
    _joy->handleEvent();
    if (event->type() == QEvent::Resize) {
        _ui->mainView->fitInView(_mainCamera->getScene()->sceneRect(), Qt::IgnoreAspectRatio);
        _ui->extraView->fitInView(_extraCamera->getScene()->sceneRect(), Qt::KeepAspectRatio);
        return true;
    }
    return false;
}

void MainWindow::updateDepth(float depth)
{
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
        _connectionProvider->Begin();

        _communicator->OnRobotRestart([]()
        {
            //qDebug() << "Arduino was restart\n";
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

        //_communicator->SetReceiveRawSensorData(true);

        _communicator->Begin();
    } catch (CantOpenPortException_t &e) {
        qDebug() << e.error_message.c_str() << "Port name: " << e.port_name.c_str() << " Error code: " << e.error_code;
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
        printf(e.error_message.c_str());
    }
}

void MainWindow::onDisconnectButtonClick(bool)
{
    try {
        _connectionProvider->Stop();
    } catch (ControllerException_t &e) {
        printf(e.error_message.c_str());
    }
}

void MainWindow::updateStatus(SimpleCommunicator_t::State_t state)
{
    _ui->flashLightRadioButton->setCheckable(true);
    _ui->flashLightRadioButton->setChecked(state.FlashlightState);
    _ui->flashLightRadioButton->setCheckable(false);
}

void MainWindow::updatePosInfo(SimpleCommunicator_t::RawSensorData_t rawSensorData)
{
    updateDepth(rawSensorData.Depth);
}

#define ABS(x) ((x < 0) ? (-x) : (x))

void MainWindow::readAndSendJoySensors()
{
    _joy->update();
    float thrust[6];
    for (int i = 0; i < 6; i++) {
        thrust[i] = _joy->axesAt(i);
    }
    float eps = 0.12;
    float x = (ABS(thrust[1]) < eps) ? 0 : thrust[1];
    float y = (ABS(thrust[0]) < eps) ? 0 : thrust[0];
    float z = (ABS(thrust[4]) < eps) ? 0 : thrust[4];
    float ty = 0;
    float tz = (ABS(thrust[3]) < eps) ? 0 : thrust[3];
    float dist = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));

    if (dist > 1) {
        x /= dist;
        y /= dist;
    }
    try {
        _communicator->SetMovementForce(x * 2, y * 2);
        _communicator->SetSinkingForce(z * 2);
        _communicator->SetPitchForce(ty);
        _communicator->SetYawForce(tz);
      /*  if (ABS(thrust[4]) < eps) {
            _communicator->SetDepth(_depth);
            _ui->stabDepthValue->setText(std::to_string(_depth).c_str());
            _ui->autoDepthCheckBox->setCheckable(true);
            _ui->autoDepthCheckBox->setChecked(true);
            _ui->autoDepthCheckBox->setCheckable(false);
        } else {
            _depth = z*2;
        }

        if (ABS(thrust[3]) < eps) {
            _communicator->SetYaw(_yaw);
            _ui->stabYawValue->setText(std::to_string(_yaw).c_str());
            _ui->autoYawCheckBox->setCheckable(true);
            _ui->autoYawCheckBox->setChecked(true);
            _ui->autoYawCheckBox->setCheckable(false);
        } else {
            _yaw = tz;
        }*/
    } catch (ControllerException_t &e) {
        printf(e.error_message.c_str());
    }
}

void MainWindow::joyButtonHandle()
{
    try {
        joyManipulatorButtonHandle();
    } catch (ControllerException_t &e) {
        printf(e.error_message.c_str());
    }
}

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

void MainWindow::joyManipulatorButtonHandle()
{
    _curManipulator._handPos = 0;
    _curManipulator._armPos = 0;
    if (_joy->atBtn(0)){
        _curManipulator._handPos = -0.1f;
    }
    if (_joy->atBtn(1)) {
        _curManipulator._handPos = 0.1f;
    }
    if (_joy->atBtn(9)) {
         _curManipulator._armPos = 0.1f;
    }
    if (_joy->atBtn(10)) {
        _curManipulator._armPos = -0.1f;
    }
    if (_joy->atBtn(2)) {
        cameraPos1 = MIN(6.28f, cameraPos1 + 0.1);
        _communicator->SetCamera1Pos(cameraPos1);
        qDebug() << cameraPos1;
    }
    if (_joy->atBtn(4)) {
        cameraPos1 = MAX(0.f, cameraPos1 - 0.1);
        _communicator->SetCamera1Pos(cameraPos1);
        qDebug() << cameraPos1;
    }
    if (_joy->atBtn(5)) {
        cameraPos2 = MIN(6.28f, cameraPos2 + 0.1);
        _communicator->SetCamera2Pos(cameraPos2);
        qDebug() << cameraPos2;
    }
    if (_joy->atBtn(6)) {
        cameraPos2 = MAX(0.f, cameraPos2 - 0.1);
        _communicator->SetCamera2Pos(cameraPos2);
        qDebug() << cameraPos2;
    }
    if (_joy->atBtn(13)) {
        if (_joy->btnStateChanged(13)) {
            _communicator->SetFlashlightState(_flashLightState = !_flashLightState);
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
    updateHeading(angles[2]*180/3.1416);
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
    _ui->radioPCA1->setCheckable(true);
    _ui->radioPCA2->setCheckable(true);
    _ui->radioADXL345->setCheckable(true);
    _ui->radioHMC58X3->setCheckable(true);
    _ui->radioITG3200->setCheckable(true);
    _ui->radioBMP085->setCheckable(true);
    _ui->radioMS5803->setCheckable(true);

    _ui->radioPCA1->setChecked(PCA1);
    _ui->radioPCA2->setChecked(PCA2);
    _ui->radioADXL345->setChecked(ADXL345);
    _ui->radioHMC58X3->setChecked(HMC58X3);
    _ui->radioITG3200->setChecked(ITG3200);
    _ui->radioBMP085->setChecked(BMP085);
    _ui->radioMS5803->setChecked(MS5803);

    _ui->radioPCA1->setCheckable(false);
    _ui->radioPCA2->setCheckable(false);
    _ui->radioADXL345->setCheckable(false);
    _ui->radioHMC58X3->setCheckable(false);
    _ui->radioITG3200->setCheckable(false);
    _ui->radioBMP085->setCheckable(false);
    _ui->radioMS5803->setCheckable(false);
}

void MainWindow::onScaneI2CdevicesButtonClick(bool)
{
    try {
        _communicator->SetRescanI2CDevices();
    } catch (ControllerException_t &e) {
        printf(e.error_message.c_str());
    }
}

void MainWindow::onBluetoothMsgRecieve(std::string msg)
{
    _ui->bluetoothLabel->setText(msg.c_str());
}

void MainWindow::onBluetoothButtonClick(bool value)
{
    try {
        _communicator->SetReadBluetoothState(true);
    } catch (ControllerException_t &e) {
        printf(e.error_message.c_str());
    }
}

void MainWindow::onMotor1SliderChanged(int value)
{
    float val = value/100.0;
    _ui->motor1valueLabel->setText(QString(std::to_string(value).c_str()) + "%");
    try {
        _communicator->SetMotorState(0, val);
    } catch (ControllerException_t &e) {
        printf(e.error_message.c_str());
    }
}

void MainWindow::onMotor2SliderChanged(int value)
{
    float val = value/100.0;
    _ui->motor2valueLabel->setText(QString(std::to_string(value).c_str()) + "%");
    try {
        _communicator->SetMotorState(1, val);
    } catch (ControllerException_t &e) {
        printf(e.error_message.c_str());
    }
}

void MainWindow::onMotor3SliderChanged(int value)
{
    float val = value/100.0;
    _ui->motor3valueLabel->setText(QString(std::to_string(value).c_str()) + "%");
    try {
        _communicator->SetMotorState(2, val);
    } catch (ControllerException_t &e) {
        printf(e.error_message.c_str());
    }
}

void MainWindow::onMotor4SliderChanged(int value)
{
    float val = value/100.0;
    _ui->motor4valueLabel->setText(QString(std::to_string(value).c_str()) + "%");
    try {
        _communicator->SetMotorState(3, val);
    } catch (ControllerException_t &e) {
        printf(e.error_message.c_str());
    }
}

void MainWindow::onMotor5SliderChanged(int value)
{
    float val = value/100.0;
    _ui->motor5valueLabel->setText(QString(std::to_string(value).c_str()) + "%");
    try {
        _communicator->SetMotorState(4, val);
    } catch (ControllerException_t &e) {
        printf(e.error_message.c_str());
    }
}

void MainWindow::onMotor6SliderChanged(int value)
{
    float val = value/100.0f;
    _ui->motor6valueLabel->setText(QString(std::to_string(value).c_str()) + "%");
    try {
        _communicator->SetMotorState(5, val);
    } catch (ControllerException_t &e) {
        printf(e.error_message.c_str());
    }
}

void MainWindow::onStopMotorsButtonClicked(bool value)
{
    _ui->motor1Slider->setValue(0);
    _ui->motor2Slider->setValue(0);
    _ui->motor3Slider->setValue(0);
    _ui->motor4Slider->setValue(0);
    _ui->motor5Slider->setValue(0);
    _ui->motor6Slider->setValue(0);
}

void MainWindow::onCamera1PosChanged(int value)
{
    _ui->camera1valueLabel->setText(QString(std::to_string(value).c_str()));
    try {
        _communicator->SetCamera1Pos(value*3.1415/180.0+3.1415);
    } catch (ControllerException_t &e) {
        printf(e.error_message.c_str());
    }
}

void MainWindow::onCamera2PosChanged(int value)
{
    _ui->camera2valueLabel->setText(QString(std::to_string(value).c_str()));
    try {
        _communicator->SetCamera2Pos(value*3.1415/180.0+3.1415);
    } catch (ControllerException_t &e) {
        printf(e.error_message.c_str());
    }
}

void MainWindow::onSetMotorsClicked(bool value)
{
    int m[6];
    m[0] = _ui->motorIdxSpinBox_1->value();
    m[1] = _ui->motorIdxSpinBox_2->value();
    m[2] = _ui->motorIdxSpinBox_3->value();
    m[3] = _ui->motorIdxSpinBox_4->value();
    m[4] = _ui->motorIdxSpinBox_5->value();
    m[5] = _ui->motorIdxSpinBox_6->value();

    for (int i = 0; i < 6; i++) {
        for (int j = i + 1; j < 6; j++) {
            if (m[i] == m[j]) {
                _ui->setMotorsMsg->setText("Motors should not be the same");
                return;
            }
        }
    }
    _ui->setMotorsMsg->setText("");
    try {
        _communicator->SetMotorsPositions(m[0], m[1], m[2], m[3], m[4], m[5]);
    } catch (ControllerException_t &e) {
        printf(e.error_message.c_str());
    }
}

void MainWindow::onDepthPIDSpinBoxChanged(bool value)
{
    double p = _ui->depthPSpinBox->value();
    double i = _ui->depthISpinBox->value();
    double d = _ui->depthDSpinBox->value();

    try {
        std::ofstream fout;
        fout.open("depth.txt");
        fout << p << " " << i << " "<< d;
        _communicator->SetDepthPid(p, i, d);
        fout.close();
    } catch (ControllerException_t &e) {
        printf(e.error_message.c_str());
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
    try {
        _communicator->SetMotorsMultiplier(m1, m2, m3, m4, m5, m6);
    } catch (ControllerException_t &e) {
        printf(e.error_message.c_str());
    }
}

void MainWindow::onPitchPIDSpinBoxChanged(bool value)
{
    double p = _ui->pitchPSpinBox->value();
    double i = _ui->pitchISpinBox->value();
    double d = _ui->pitchDSpinBox->value();
    try {
        std::ofstream fout;
        fout.open("pitch.txt");
        fout << p << " " << i << " "<< d;
        _communicator->SetPitcPid(p, i, d);
        fout.close();
    } catch (ControllerException_t &e) {
        printf(e.error_message.c_str());
    }
}

void MainWindow::onYawPIDSpinBoxChanged(bool value)
{
    double p = _ui->yawPSpinBox->value();
    double i = _ui->yawISpinBox->value();
    double d = _ui->yawDSpinBox->value();
    try {
        std::ofstream fout;
        fout.open("yaw.txt");
        fout << p << " " << i << " "<< d;
        _communicator->SetYawPid(p, i, d);
        fout.close();
    } catch (ControllerException_t &e) {
        printf(e.error_message.c_str());
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
}
