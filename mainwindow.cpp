#include "mainwindow.h"
#include <iostream>

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
      _depthTimer(new QTimer(this)), // Temp timer. Look header
      _connectionProvider(new UARTConnectionProvider_t("COM3", 115200, 200, 200)),
      _communicator(new SimpleCommunicator_t(_connectionProvider))
{
    _ui->setupUi(this);
    cameraInit();
    loadQSS();
    connect(_ui->startButton, SIGNAL(clicked(bool)), this, SLOT(onStartButtonClick(bool)));
    connect(_taskTimer, SIGNAL(timeout()), this, SLOT(onTaskTimeout()));

    // Temp code begin
    _currentDepth = 50.1;
    // Temp code end

    connect(_depthTimer, SIGNAL(timeout()), this, SLOT(updateDepth()));
    _depthTimer->setInterval(100);
    _depthTimer->start();
    connect(_ui->horizontalSlider, SIGNAL(valueChanged(int)), this, SLOT(updateManipulator(int)));
    connect(_ui->flashLightButton, SIGNAL(clicked(bool)), this, SLOT(updateFlashLight(bool)));
    connectionProviderInit();
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
        _ui->mainView->fitInView(_mainCamera->getScene()->sceneRect(), Qt::KeepAspectRatioByExpanding);
        _ui->extraView->fitInView(_extraCamera->getScene()->sceneRect(), Qt::KeepAspectRatio);
        return true;
    }
    return false;
}

void MainWindow::updateDepth()
{
    _mainCamera->getVideoWidget()->setCurrentDepth(_currentDepth);
    _currentDepth += 0.1;
}

void MainWindow::showMessage(QString msg, QColor msgColor)
{
    QGraphicsScene *scene = _ui->mainView->scene();
    scene->addRect(0, 0, scene->width(), 24, QPen(msgColor), QBrush(msgColor));

    QGraphicsTextItem *text = scene->addText(msg, QFont("Times", 10));
    text->setPos(QPointF(scene->width()/2 - 24, 0));
    text->setDefaultTextColor(QColor(255, 255, 255));
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
    try {
        _connectionProvider->Begin();

        _communicator->OnRobotRestart([]()
        {
            qDebug() << "Arduino was restart\n";
        });

        /*_communicator->OnPacketsLeak([](int send, int receive)
        {
            std::string s = "Attention! Leak! Send : ";
            s += send; s += ", recieved : "; s += recieve;
            showMessage(s.c_str(), QColor(255, 102, 102));
        });*/

        _communicator->OnConnectionStateChange([this](bool connected)
        {
            if (connected) {
                qDebug() << "Connected";
                this->showMessage("Connected");
            } else {
                qDebug() << "Disconnect";
                this->showMessage("Disconnect", QColor(255, 204, 102));
            }
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

        _communicator->Begin();
    } catch (ControllerException_t &e) {
        qDebug() << e.error_message.c_str();
    }
}

void MainWindow::updateFlashLight(bool)
{
    try {
        _communicator->SetFlashlightState(_flashLightState = !_flashLightState);
        _ui->flashLightLabel->setText((_flashLightState) ? "true" : "false");
    } catch (ControllerException_t &e) {
        qDebug() << e.error_message.c_str();
    }
}
