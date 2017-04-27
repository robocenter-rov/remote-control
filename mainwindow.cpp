#include "mainwindow.h"
#include <iostream>

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
    _ui->setupUi(this);

    cameraInit();
    loadQSS();
    connect(_ui->startButton, SIGNAL(clicked(bool)), this, SLOT(onStartButtonClick(bool)));
    connect(_taskTimer, SIGNAL(timeout()), this, SLOT(onTaskTimeout()));

    connect(_ui->connectButton, SIGNAL(clicked(bool)), this, SLOT(onConnectButtonClick(bool)));
    connect(_ui->disconnectButton, SIGNAL(clicked(bool)), this, SLOT(onDisconnectButtonClick(bool)));

    connect(_ui->horizontalSlider, SIGNAL(valueChanged(int)), this, SLOT(updateManipulator(int)));
    connect(_ui->flashLightButton, SIGNAL(clicked(bool)), this, SLOT(updateFlashLight(bool)));

    connectionProviderInit();

    connect(_messageTimer, SIGNAL(timeout()), this, SLOT(hideMessage()));
    _messageTimer->setInterval(2000);

    connect(_joyTimer, SIGNAL(timeout()), this, SLOT(readAndSendJoySensors()));
    _joyTimer->setInterval(100);

    connect(this, SIGNAL(connectionChangedEvent(bool)), this, SLOT(updateConnectionStatus(bool)));
    connect(this, SIGNAL(stateChangedEvent(SimpleCommunicator_t::State_t)), this, SLOT(updateStatus(SimpleCommunicator_t::State_t)));
    connect(this, SIGNAL(rawSensorDataRecievedEvent(SimpleCommunicator_t::RawSensorData_t)), this, SLOT(updatePosInfo(SimpleCommunicator_t::RawSensorData_t)));

    showMessage("Connection...", CL_YELLOW);
}

MainWindow::~MainWindow()
{
    delete _ui;
    delete _communicator;
    delete _connectionProvider;
}

void MainWindow::cameraInit()
{
    _mainCamera = new RoboCamera(0, _ui->mainView, this, new QGraphicsScene(), true);
    _extraCamera = new RoboCamera(1, _ui->extraView, this);
}

bool MainWindow::eventFilter(QObject *, QEvent *event)
{
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
    _showMessageByTimer = true;
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

void MainWindow::updateManipulator(int val)
{
    QString t("Manipulator ");
    QString s;
    s.setNum(val); s += "%";
    _ui->Manipulator->setText(t+s);
}

#include <string>

void MainWindow::connectionProviderInit()
{
    _connectionProvider = new UARTConnectionProvider_t("COM3", 115200, 200, 200);
    _communicator = new SimpleCommunicator_t(_connectionProvider);
    try {
        _connectionProvider->Begin();

        _communicator->OnRobotRestart([]()
        {
            //qDebug() << "Arduino was restart\n";
        });

        /*_communicator->OnPacketsLeak([](int send, int receive)
        {
            std::string s = "Attention! Leak! Send : ";
            s += send; s += ", recieved : "; s += recieve;
            showMessage(s.c_str(), QColor(255, 102, 102));
        });*/
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
    if (_showMessageByTimer) {
        _messageTimer->stop();
        _showMessageByTimer = false;
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
        qDebug() << thrust;
        _communicator->SetMotorsState(thrust[0], thrust[1], thrust[2], thrust[3], thrust[4], thrust[5]);
    } catch (ControllerException_t &e) {
        printf(e.error_message.c_str());
    }
}
