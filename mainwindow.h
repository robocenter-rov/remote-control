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
private:   
    void cameraInit();
    void loadQSS();
    bool eventFilter(QObject *, QEvent *event);
    void showMessage(QString msg, QColor msgColor = QColor(0, 204, 102));
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
};

#endif // MAINWINDOW_H
