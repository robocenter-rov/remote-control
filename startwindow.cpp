#include "startwindow.h"
#include "ui_startwindow.h"

StartWindow::StartWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::StartWindow)
{
    ui->setupUi(this);
    connect(ui->mainPilotButton, SIGNAL(clicked(bool)), this, SLOT(onMainPilotButtonClicked()));
    connect(ui->calcButton, SIGNAL(clicked(bool)), this, SLOT(onCalcButtonClicked()));
    ui->mainPilotButton->setIcon(QIcon(QPixmap::fromImage(QImage("../remote-control/img/main_pilot.png"))));
    ui->mainPilotButton->setIconSize(QSize(60, 60));

    ui->calcButton->setIcon(QIcon(QPixmap::fromImage(QImage("../remote-control/img/math_pilot.png"))));
    ui->calcButton->setIconSize(QSize(60, 60));
    loadQSS();
}

StartWindow::~StartWindow()
{
    delete ui;
}

void StartWindow::loadQSS()
{
    QFile styleF("../remote-control/qss/appstyles.css");
    if (!styleF.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "Warning: application styles not found";
    }
    this->setStyleSheet(styleF.readAll());
}

void StartWindow::onMainPilotButtonClicked()
{
    _mainWindow = new MainWindow();
    _mainWindow->showMaximized();
    this->close();
}

void StartWindow::onCalcButtonClicked()
{
    _calcWindow = new CalcWindow();
    _calcWindow->show();
    this->close();
}
