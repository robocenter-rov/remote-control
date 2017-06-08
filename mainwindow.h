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
#include "joystick.h"
#include "remote-control-library/ConnectionProvider.h"
#include "remote-control-library/UARTConnectionProviderWindows.h"
#include "remote-control-library/SimpleCommunicator.h"
#include <iostream>
#include "remote-control-library/Utils.h"
#include <QString>
#include "graphicsscene.h"

extern QString COMportName;

Q_DECLARE_METATYPE(SimpleCommunicator_t::PidState_t)
Q_DECLARE_METATYPE(SimpleCommunicator_t::State_t)
Q_DECLARE_METATYPE(SimpleCommunicator_t::CalibratedSensorData_t)

namespace Ui {
    class MainWindow;
}

enum msg_color_t {
    CL_GREEN,
    CL_RED,
    CL_YELLOW
};

class BaseTool;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();    
private slots:
    void updateDepth(float depth);
    void onStartButtonClick(bool);
    void onTaskTimeout();
    void hideMessage();
    void updateConnectionStatus(bool);
    void onConnectButtonClick(bool);
    void onDisconnectButtonClick(bool);
    void onScaneI2CdevicesButtonClick(bool);
    void updateStatus(SimpleCommunicator_t::State_t);
    void updatePosInfo(SimpleCommunicator_t::RawSensorData_t);
    void updateRawSensorData(SimpleCommunicator_t::RawSensorData_t);
    void updateCalibratedSensorData(SimpleCommunicator_t::CalibratedSensorData_t calibratedSensorData);
    void readAndSendJoySensors();
    void joyButtonHandle();
    void onLeak(int send, int receive);
    void updateOrient(float q1, float q2, float q3, float q4);
    void updateHeading(int value);
    void updateI2CDevicesState(bool PCA1, bool PCA2, bool PCA3, bool ADXL345, bool HMC58X3, bool ITG3200, bool BMP085, bool MS5803);
    void onBluetoothMsgRecieve(std::string msg);
    void onBluetoothButtonClick(bool);
    void onMotor1SliderChanged(int);
    void onMotor2SliderChanged(int);
    void onMotor3SliderChanged(int);
    void onMotor4SliderChanged(int);
    void onMotor5SliderChanged(int);
    void onMotor6SliderChanged(int);
    void onMotor7SliderChanged(int);
    void onMotor8SliderChanged(int);
    void onStopMotorsButtonClicked(bool);
    void onCamera1PosChanged(int);
    void onCamera2PosChanged(int);
    void onSetMotorsClicked(bool);
    void onDepthPIDSpinBoxChanged(bool);
    void onPitchPIDSpinBoxChanged(bool);
    void onYawPIDSpinBoxChanged(bool);
    void onRollPIDSpinBoxChanged(bool);
    void onSetMotorsMultiplier(bool);
    void onTabChanged(int idx);
    void onAutoDepthClicked(bool);
    void onAutoPitchClicked(bool);
    void onAutoYawClicked(bool);
    void onAutoRollClicked(bool);
    void onServo1SliderChanged(int value);
    void onUseJoyCheckButtonClicked(bool);
    void onMotorStateRecieved(float m1, float m2, float m3, float m4, float m5, float m6, float m7, float m8);
    void onPidStateReceived(SimpleCommunicator_t::PidState_t depth, SimpleCommunicator_t::PidState_t yaw, SimpleCommunicator_t::PidState_t pitch, SimpleCommunicator_t::PidState_t roll);
    void on_receivePidStatesCheckbox_toggled(bool checked);
    void on_startAutoPitchButton_clicked();
    void on_checkBox_toggled(bool checked);
    void on_resetPositionsPushButton_clicked();
    void on_xPositionVerticalSlider_sliderMoved(int position);
    void on_yPositionVerticalSlider_sliderMoved(int position);
    void on_rotationSlider_valueChanged(int value);
    void on_verticalSlider_valueChanged(int value);
    void onAutoDepthEdit(QString value);
    void onAutoPitchEdit(QString value);
    void onAutoYawEdit(QString value);
    void onAutoRollEdit(QString value);
    void onAutoCurrentDepthClicked(bool value);
    void onAutoCurrentPitchClicked(bool value);
    void onAutoCurrentYawClicked(bool value);
    void onAutoCurrentRollClicked(bool value);
    void clearScreenView();
    void showAxis(int value);    
    void on_cam1MinValSpinBox_valueChanged(double arg1);
    void on_cam1MaxValSpinBox_valueChanged(double arg1);
    void on_cam2MinValSpinBox_valueChanged(double arg1);
    void on_cam2MaxValSpinBox_valueChanged(double arg1);
    void on_nextStepButton_clicked(bool checked);
    void on_previousStepButton_clicked(bool checked);
    void on_zPositionVerticalSlider_valueChanged(int value);
    void on_ReceiveRawIMUValues_CheckBox_toggled(bool checked);
    void on_CalibrateGyro_PushButton_pressed();
    void on_ReceiveCalibratedIMUValues_CheckBox_toggled(bool checked);
    void on_CalibrateGyro_PushButton_released();
    void on_setCalibrationValues_clicked();
    void on_setCalibrationValues_PushButton_clicked();

    void on_CalibrateGyro_PushButton_toggled(bool checked);

signals:
    void connectionChangedEvent(bool connectedStatus);
    void stateChangedEvent(SimpleCommunicator_t::State_t state);
    void rawSensorDataRecievedEvent(SimpleCommunicator_t::RawSensorData_t rawSensorData);
    void calibratedSensorDataRecievedEvent(SimpleCommunicator_t::CalibratedSensorData_t calibratedSensorData);
    void leakEvent(int send, int receive);
    void orientationReceivedEvent(float q1, float q2, float q3, float q4);
    void I2CDevicesRecieveEvent(bool PCA1, bool PCA2, bool PCA3, bool ADXL345, bool HMC58X3, bool ITG3200, bool BMP085, bool MS5803);
    void bluetoothMsgRecieveEvent(std::string msg);
    void depthRecieveEvent(float depth);
    void motorStateReceiveEvent(float m1, float m2, float m3, float m4, float m5, float m6, float m7, float m8);
    void pidStateReceiveEvent(SimpleCommunicator_t::PidState_t, SimpleCommunicator_t::PidState_t, SimpleCommunicator_t::PidState_t, SimpleCommunicator_t::PidState_t);
private:
    void replotData();
    void replotDataDepth();
    void replotDataPitch();
    void replotDataYaw();
    void replotDataRoll();
    void graphInit();
    void cameraInit();
    void loadQSS();
    bool eventFilter(QObject *, QEvent *event);
    void showMessage(QString msg, msg_color_t color);
    void showMessageByTimer(QString msg, msg_color_t color);
    void connectionProviderInit();
    void joyManipulatorButtonHandle();
    void generateMapTools();
    void initClearButton();
    void initPIDcoeffs();
    void initMotorsMultipliers();
    void initCameraMinMax();
    void initIMUCalibration();
    void saveIMUCalibration();
    void saveCamMinMax();
    void setDepthPID(double p, double i, double d);
    void setPitchPID(double p, double i, double d);
    void setYawPID(double p, double i, double d);
    void setRollPID(double p, double i, double d);
    void setCurrentTool();
    void setMotorsPos();
    Ui::MainWindow *_ui;
    RoboCamera *_mainCamera;
    RoboCamera *_extraCamera;
    QTimer *_taskTimer;

    ConnectionProvider_t *_connectionProvider;
    SimpleCommunicator_t *_communicator;

    bool _flashLightState;
    bool _showMessage = false;
    QTimer *_messageTimer;

    Joystick *_joy;
    QTimer *_joyTimer;

    struct Manipulator {
        float _armPos = 0.0f;
        float _handPos = 0.0f;
        float _m1 = 0.0f;
        float _m2 = 0.0f;
        int _cntChanged = 0;
    } _curManipulator;

    float cameraPos1 = 0.0f;
    float cameraPos2 = 0.0f;

    float _x_pos;
    float _y_pos;

    float _currentDepth = 0.0;
    float _currentYaw = 0.0;
    float _currentPitch = 0.0;
    float _currentRoll = 0.0;

    float _xGyroOffset = 0.0;
    float _yGyroOffset = 0.0;
    float _zGyroOffset = 0.0;

    float _calibrateIteration = 0;

    int _count_of_recieved_pid = 0;

    int _control_sensitivity_level = 1;
    static const int _control_sensitivity_count = 3;
    float _control_sensitivities[_control_sensitivity_count] = {0.3f, 1.f, 3.f};
    float _x_move_force = 0;
    float _y_move_force = 0;
    float _z_rotate_force = 0;

    bool _isAutoDepth = true;
    bool _isAutoYaw = true;
    bool _isAutoPitch = true;
    bool _isAutoRoll = true;
#define DEPTH_DATA_SIZE 200
#define PITCH_DATA_SIZE 200
#define YAW_DATA_SIZE 200
#define ROLL_DATA_SIZE 200
    QVector<SimpleCommunicator_t::PidState_t> _depthData;
    QVector<SimpleCommunicator_t::PidState_t> _pitchData;
    QVector<SimpleCommunicator_t::PidState_t> _yawData;
    QVector<SimpleCommunicator_t::PidState_t> _rollData;
//    QTimer *_depthDataTimer;
    QGraphicsPixmapItem _screen;
    VideoGraphicsScene *_videoScene;
    QList<BaseTool *> _tools;
    QPushButton *_clearButton;

    GraphicsScene *_screenScene;
    MapGraphicsScene *_mapScene;
    int _curToolIdx;
};

#endif // MAINWINDOW_H
