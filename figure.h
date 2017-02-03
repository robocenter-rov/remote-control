#ifndef FIGURE_H
#define FIGURE_H

#include "graphicsscene.h"

class Figure
{
public:
    Figure();
    ~Figure(){}
    void virtual draw(GraphicsScene *scene){}
    bool virtual inArea(QPointF p) { return true; }
protected:
    QPen _pen;
};

class LineFigure : public Figure
{
public:
    LineFigure(QPointF p1, QPointF p2);
    ~LineFigure(){}
    void draw(GraphicsScene *scene) override;
    bool inArea(QPointF p) override;
    void calcArea();
    QPointF rotatedEndPoint2();
private:
    QPointF _p1;
    QPointF _p2;
    QPolygonF _area;
    const double _offset = 3;
    double _angle;

};

class RectFigure : public Figure
{
public:
    RectFigure(QPointF p1, QPointF p2);
    ~RectFigure(){}
    void draw(GraphicsScene *scene) override;
    bool inArea(QPointF p) override;
private:
    QPointF _p1;
    QPointF _p2;
};

extern void calcPoints(QPointF &p1, QPointF &p2);
#endif // FIGURE_H
