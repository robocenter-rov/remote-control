#ifndef BASETOOL_H
#define BASETOOL_H

#include <QObject>
#include <QWidget>
#include <QPushButton>
#include <QCoreApplication>

struct Point
{
    double x, y;
};

class BaseTool : public QObject
{    
    Q_OBJECT
public:
    BaseTool(QWidget *parent = nullptr);
    ~BaseTool();
    void virtual draw() = 0;
protected:
    QPushButton *_button;
    QString _iconsPath;
private slots:
    void onToolButtonClick(bool checked = false);
};

class LineTool: public BaseTool
{
public:
    LineTool(QWidget *parent = nullptr);
    ~LineTool() {}
    void draw();
private:
    Point _startPos;
};

#endif // BASETOOL_H
