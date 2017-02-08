#include "communicator.h"

Communicator::Communicator() :
    _joy(new Joystick()),
    _joyTimer(new QTimer(this))
{
    connect(_joyTimer, SIGNAL(timeout()), this, SLOT(readAndSendJoySensors()));
    _joyTimer->start(100);
    udpSocketInit();
}

Communicator::~Communicator()
{
    delete _joy;
    delete _joyTimer;
    delete _socket;
}

void Communicator::udpSocketInit()
{
    _socket = new QUdpSocket(this);
    _socket->bind(QHostAddress(IP_ADDR), PORT);
    connect(_socket, SIGNAL(readyRead()), this, SLOT(readMessage()));
}

void Communicator::readMessage()
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

void Communicator::readAndSendJoySensors()
{
    sendMessage(_joy->getMotorsThrust());
}
