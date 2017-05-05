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
    connect(_ui->flashLightButton, SIGNAL(clicked(bool)), this, SLOT(updateFlashLight(bool)));
    connect(_ui->heading, SIGNAL(valueChanged(int)), this, SLOT(updateHeading(int)));

    connectionProviderInit();

    connect(_messageTimer, SIGNAL(timeout()), this, SLOT(hideMessage()));
    _messageTimer->setInterval(2000);

    connect(_joyTimer, SIGNAL(timeout()), this, SLOT(readAndSendJoySensors()));
    _joyTimer->setInterval(100);

    connect(_joy, SIGNAL(joyButtonEvent(int,uint8_t)), this, SLOT(joyButtonHandle(int,uint8_t)));
    connect(this, SIGNAL(connectionChangedEvent(bool)), this, SLOT(updateConnectionStatus(bool)));
    connect(this, SIGNAL(stateChangedEvent(SimpleCommunicator_t::State_t)), this, SLOT(updateStatus(SimpleCommunicator_t::State_t)));
    connect(this, SIGNAL(rawSensorDataRecievedEvent(SimpleCommunicator_t::RawSensorData_t)), this, SLOT(updatePosInfo(SimpleCommunicator_t::RawSensorData_t)));
    connect(this, SIGNAL(leakEvent(int, int)), this, SLOT(onLeak(int, int)));
    connect(this, SIGNAL(orientationReceivedEvent(SimpleCommunicator_t::Orientation_t)), this, SLOT(updateOrient(SimpleCommunicator_t::Orientation_t)));

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

        _communicator->OnI2CDevicesReceive([](SimpleCommunicator_t::I2CDevices_t devices)
        {
            printf("PCA1: ");
            printf(devices.PCA1 ? "connected\n" : "disconnected\n");

            printf("PCA2: ");
            printf(devices.PCA2 ? "connected\n" : "disconnected\n");

            printf("ADXL345: ");
            printf(devices.ADXL345 ? "connected\n" : "disconnected\n");

            printf("HMC58X3: ");
            printf(devices.HMC58X3 ? "connected\n" : "disconnected\n");

            printf("ITG3200: ");
            printf(devices.ITG3200 ? "connected\n" : "disconnected\n");

            printf("BMP085: ");
            printf(devices.BMP085 ? "connected\n" : "disconnected\n");

            printf("MS5803: ");
            printf(devices.MS5803 ? "connected\n" : "disconnected\n");
        });

        _communicator->OnStateChange([&](SimpleCommunicator_t::State_t state){
            emit stateChangedEvent(state);
        });

        _communicator->OnRawSensorDataReceive([&](SimpleCommunicator_t::RawSensorData_t rawSensorData){
            emit rawSensorDataRecievedEvent(rawSensorData);
        });

        _communicator->OnOrientationReceive([&](SimpleCommunicator_t::Orientation_t o){
            emit orientationReceivedEvent(o);
        });

        _communicator->Begin();
    } catch (ControllerException_t &e) {
        qDebug() << e.error_message.c_str();
    }
}

void MainWindow::updateFlashLight(bool)
{
    try {
        _communicator->SetFlashlightState(_flashLightState = !_flashLightState);
        //_ui->flashLightLabel->setText((_flashLightState) ? "true" : "false");
    } catch (ControllerException_t &e) {
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
    _ui->flashLightLabel->setText(state.FlashlightState ? "true" : "false");
}

void MainWindow::updatePosInfo(SimpleCommunicator_t::RawSensorData_t rawSensorData)
{
    updateDepth(rawSensorData.Depth);
}

void MainWindow::readAndSendJoySensors()
{
    try {
        _joy->update();
        float thrust[6];
        for (int i = 0; i < 6; i++) {
            thrust[i] = _joy->axesAt(i);
        }

        int eps = 4000;
        uint8_t axes1 = (abs(thrust[1]) < eps) ? 0 : thrust[1];
        uint8_t axes0 = (abs(thrust[0]) < eps) ? 0 : thrust[0];
        uint8_t axes4 = (abs(thrust[4]) < eps) ? 0 : thrust[4];
        double dist = sqrt(pow(axes1, 2) + pow(axes0, 2) + pow(axes4, 2));
        double x, y, z;
        if (dist > INT16_MAX) {    
            x = axes1/dist * INT16_MAX;
            y = axes0/dist * INT16_MAX;
            z = axes4/dist * INT16_MAX;
         } else {
            x = axes1;
            y = axes0;
            z = axes4;
         }
         /*_pos.ty = 0; // pitch
          _pos.tz = axes3; // heading*/

         double t = INT16_MAX/100.0;
         x = x/t;
         y = y/t;
         z = z/t;
         _communicator->SetMovementForce(x, y);
         _communicator->SetSinkingForce(z);
    } catch (ControllerException_t &e) {
        printf(e.error_message.c_str());
    }
}

void MainWindow::joyButtonHandle(int idx, uint8_t value)
{
    try {
        joyManipulatorButtonHandle(idx, value);
    } catch (ControllerException_t &e) {
        printf(e.error_message.c_str());
    }
}

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

void MainWindow::joyManipulatorButtonHandle(int idx, uint8_t value)
{
    if (idx == 9 && value) {
        _curManipulator._cntChanged += 1;
        _curManipulator._handPos = MAX(-1, _curManipulator._handPos - 0.01f);
    }
    if (idx == 10 && value) {
        _curManipulator._handPos = MIN(1, _curManipulator._handPos + 0.01f);
        _curManipulator._cntChanged += 1;
    }
    if (idx == 11 && value) {
        _curManipulator._armPos = MIN(1, _curManipulator._armPos + 0.01f);
        _curManipulator._cntChanged += 1;
    }
    if (idx == 12 && value) {
        _curManipulator._armPos = MAX(-1, _curManipulator._armPos - 0.01f);
        _curManipulator._cntChanged += 1;
    }
    if (_curManipulator._cntChanged) {
        _curManipulator._cntChanged = 0;
        _communicator->SetManipulatorState(
                    _curManipulator._armPos,
                    _curManipulator._handPos,
                    _curManipulator._m1,
                    _curManipulator._m2
                    );
    }
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
    updateHeading(angles[2]*180/3.1416);
}

void MainWindow::updateHeading(int value)
{
    std::string s = std::to_string(value);
    _ui->headingLabel->setText(s.c_str());
    _ui->heading->setValue(value);
}
