#include "mainwindow.h"
#include <iostream>

#define IP_ADDR "192.168.1.177"
#define PORT 8888

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      _ui(new Ui::MainWindow)
{
    _ui->setupUi(this);
    _socket = new QUdpSocket(this);
    _socket->bind(QHostAddress(IP_ADDR), PORT);
    connect(_socket, SIGNAL(readyRead()), this, SLOT(readMessage()));
    connect(_ui->sendButton, SIGNAL(clicked(bool)), this, SLOT(onSendButtonClicked()));
}

MainWindow::~MainWindow()
{
    delete _ui;
}

void MainWindow::sendMessage()
{
    QByteArray data;
    data.append("Hello, robot!");
    _socket->writeDatagram(data, QHostAddress(IP_ADDR), PORT);
}

void MainWindow::readMessage()
{
    QByteArray buffer;
    buffer.resize(_socket->pendingDatagramSize());

    QHostAddress sender;
    quint16 senderPort;

    _socket->readDatagram(buffer.data(), buffer.size(), &sender, &senderPort);
    qDebug() << "Message from: " << sender.toString();
    qDebug() << "Message port: " << senderPort;
    qDebug() << "Message: " << buffer;
}

void MainWindow::onSendButtonClicked()
{
    sendMessage();
}
