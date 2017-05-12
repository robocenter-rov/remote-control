#ifndef BASETOOLS_H
#define BASETOOLS_H

#include <QObject>
#include <QPushButton>
#include <QtCore>
#include <QDoubleSpinBox>
#include "figure.h"

class GraphicsScene;

class BaseTool : public QObject
{
    Q_OBJECT
public:
    BaseTool(QWidget *parent = nullptr);
    virtual ~BaseTool();
    void virtual drawOnMouseDoubleClick(GraphicsScene *scene, QPointF point) {}
    void virtual drawOnMousePress(GraphicsScene *scene, QPointF point) {}
    void virtual drawOnMouseMove(GraphicsScene *scene, QPointF point) {}
    void virtual drawOnMouseRelease(GraphicsScene *scene, QPointF point) {}
    void virtual destroyProperties() {}
    QString virtual getInfo();
private slots:
    void selectButtonTool(bool);
protected:
    QString _iconsPath;
    QWidget *_parent;
    bool _isDraw;
    QPushButton *_button;
    int _id;
    static int _nextId;
};

class LineTool: public BaseTool
{
    Q_OBJECT
public:
    LineTool(QWidget *parent = nullptr);
    ~LineTool();
    void drawOnMouseDoubleClick(GraphicsScene *scene, QPointF point) override;
    void drawOnMousePress(GraphicsScene *scene, QPointF point) override;
    void drawOnMouseMove(GraphicsScene *scene, QPointF point) override;
    void drawOnMouseRelease(GraphicsScene *scene, QPointF point) override;
    void destroyProperties() override;
    QString getInfo() override;
private slots:
    void calcScaleCoef();
protected:
    QPointF _startPos;
    QPointF _endPos;
    LineFigure *_line;
    QDoubleSpinBox *_spinBox;
    QPen _pen;
};

extern QList <BaseTool *> tools;
extern BaseTool *currentTool;
extern double scaleCoef;

class SelectTool : public BaseTool
{
public:
    SelectTool(QWidget *parent = nullptr);
    ~SelectTool(){}
    void drawOnMouseDoubleClick(GraphicsScene *scene, QPointF point) override;
    void drawOnMousePress(GraphicsScene *scene, QPointF point) override;
    void drawOnMouseMove(GraphicsScene *scene, QPointF point) override;
    void drawOnMouseRelease(GraphicsScene *scene, QPointF point) override;
    QString getInfo() override;
private:
    QPointF _startPoint;
    QList <Figure *> _selectedFigures;
    bool _isResize;
    int _resizePointIdx;
};

class PoolLineTool : public LineTool
{
    Q_OBJECT
public:
    PoolLineTool(QWidget *parent = nullptr);
    void drawOnMouseRelease(GraphicsScene *scene, QPointF point) override;
    ~PoolLineTool() {}
    QString getInfo() override;
private slots:
    void calcAngleOffset(double);
private:
    double _relAngle;
    double _absAngle;
};

class ReplaceAxisTool : public BaseTool
{
public:
    ReplaceAxisTool(QWidget *parent = nullptr);
    void drawOnMouseDoubleClick(GraphicsScene *scene, QPointF point) override;
    void drawOnMousePress(GraphicsScene *scene, QPointF point) override;
    void drawOnMouseMove(GraphicsScene *scene, QPointF point) override;
    void drawOnMouseRelease(GraphicsScene *scene, QPointF point) override;
    QString getInfo() override;
private:
    bool _replace;
    QPointF _startPos;
};

class ContainersCenterTool : public BaseTool
{
    Q_OBJECT
public:
    ContainersCenterTool(QWidget *parent = nullptr);
    void drawOnMouseDoubleClick(GraphicsScene *scene, QPointF point) override;
    void drawOnMousePress(GraphicsScene *scene, QPointF point) override;
    void drawOnMouseMove(GraphicsScene *scene, QPointF point) override;
    void drawOnMouseRelease(GraphicsScene *scene, QPointF point) override;
    QString getInfo() override;
private slots:
    void selectButtonTool(bool);
private:
    double _offset;
    GraphicsScene *_scene;
};
#endif // BASETOOLS_H
