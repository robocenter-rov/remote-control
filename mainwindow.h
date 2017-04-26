#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ui_mainwindow.h"
#include <QCamera>
#include <QCameraInfo>
#include <QWidget>
#include <QtMultimediaWidgets>
#include <QtGui/QtGui>
#include "robocamera.h"

#include "remote-control-library/ConnectionProvider.h"
#include "remote-control-library/UARTConnectionProviderWindows.h"
#include "remote-control-library/SimpleCommunicator.h"
#include <iostream>
#include "remote-control-library/Utils.h"

namespace Ui {
    class MainWindow;
}

enum msg_color_t {
    CL_GREEN,
    CL_RED,
    CL_YELLOW
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();    
private slots:
    void updateDepth();    
    void onStartButtonClick(bool);
    void onTaskTimeout();
    void updateManipulator(int);
    void updateFlashLight(bool);
    void hideMessage();
    void updateConnectionStatus(bool);
    void onConnectButtonClick(bool);
    void onDisconnectButtonClick(bool);
signals:
    void connectionChangedEvent(bool connectedStatus);
private:   
    void cameraInit();
    void loadQSS();
    bool eventFilter(QObject *, QEvent *event);
    void showMessage(QString msg, msg_color_t color);
    void showMessageByTimer(QString msg, msg_color_t color);
    void connectionProviderInit();
    Ui::MainWindow *_ui;
    RoboCamera *_mainCamera;
    RoboCamera *_extraCamera;
    QTimer *_depthTimer;
    QTimer *_taskTimer;
    double _currentDepth; // Temp var. DO: why _ui->mainView->scene->height() return different values after redrawing depth

    ConnectionProvider_t *_connectionProvider;
    SimpleCommunicator_t *_communicator;

    bool _flashLightState;
    bool _showMessage = false;
    bool _showMessageByTimer = false;
    QTimer *_messageTimer;
};

#endif // MAINWINDOW_H
