#include "mainwindow.h"
#include <iostream>

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
    connect(this, SIGNAL(orientationReceivedEvent(SimpleCommunicator_t::Orientation_t)), this, SLOT(updateOrient(SimpleCommunicator_t::Orientation_t)));
    connect(this, SIGNAL(I2CDevicesRecieveEvent(SimpleCommunicator_t::I2CDevices_t)), this, SLOT(updateI2CDevicesState(SimpleCommunicator_t::I2CDevices_t)));
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
            emit I2CDevicesRecieveEvent(devices);
        });

        _communicator->OnStateChange([&](SimpleCommunicator_t::State_t state){
            emit stateChangedEvent(state);
        });

        /*_communicator->OnRawSensorDataReceive([&](SimpleCommunicator_t::RawSensorData_t rawSensorData){
            emit rawSensorDataRecievedEvent(rawSensorData);
        });*/

        _communicator->OnOrientationReceive([&](SimpleCommunicator_t::Orientation_t o){
            emit orientationReceivedEvent(o);
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
        qDebug() << i << " "<< thrust[i];
    }
    float eps = 0.12;
    float x = (ABS(thrust[1]) < eps) ? 0 : thrust[1];
    float y = (ABS(thrust[0]) < eps) ? 0 : thrust[0];
    float z = (ABS(thrust[4]) < eps) ? 0 : thrust[4];
    float ty = 0;
    float tz = thrust[3];
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
        _communicator->SetFlashlightState(_flashLightState = !_flashLightState);
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

void MainWindow::updateOrient(SimpleCommunicator_t::Orientation_t o)
{
    float angles[3];
    angles[0] = atan2(2 * o.q2 * o.q3 - 2 * o.q1 * o.q4, 2 * o.q1 * o.q1 + 2 * o.q2 * o.q2 - 1); // psi
    angles[1] = -asin(2 * o.q2 * o.q4 + 2 * o.q1 * o.q3); // theta
    angles[2] = atan2(2 * o.q3 * o.q4 - 2 * o.q1 * o.q2, 2 * o.q1 * o.q1 + 2 * o.q4 * o.q4 - 1); // phi
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

void MainWindow::updateI2CDevicesState(SimpleCommunicator_t::I2CDevices_t devices)
{
    _ui->radioPCA1->setCheckable(true);
    _ui->radioPCA2->setCheckable(true);
    _ui->radioADXL345->setCheckable(true);
    _ui->radioHMC58X3->setCheckable(true);
    _ui->radioITG3200->setCheckable(true);
    _ui->radioBMP085->setCheckable(true);
    _ui->radioMS5803->setCheckable(true);

    _ui->radioPCA1->setChecked(devices.PCA1);
    _ui->radioPCA2->setChecked(devices.PCA2);
    _ui->radioADXL345->setChecked(devices.ADXL345);
    _ui->radioHMC58X3->setChecked(devices.HMC58X3);
    _ui->radioITG3200->setChecked(devices.ITG3200);
    _ui->radioBMP085->setChecked(devices.BMP085);
    _ui->radioMS5803->setChecked(devices.MS5803);

    _ui->radioPCA1->setCheckable(false);
    _ui->radioPCA2->setCheckable(false);
    _ui->radioADXL345->setCheckable(false);
    _ui->radioHMC58X3->setCheckable(false);
    _ui->radioITG3200->setCheckable(false);
    _ui->radioBMP085->setCheckable(false);
    _ui->radioMS5803->setCheckable(false);
}

void MainWindow::onScaneI2CdevicesButtonClick(bool value)
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
