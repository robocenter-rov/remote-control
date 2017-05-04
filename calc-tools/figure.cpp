#include "figure.h"
#include <QtCore>
#include <string>
#include "basetool.h"

#define min(x, y) (((x) < (y)) ? (x) : (y))
#define max(x, y) (((x) > (y)) ? (x) : (y))

static QPointF rotate(QPointF p, double angle)
{
    return QPointF(p.x()*cos(angle) - p.y()*sin(angle), p.x()*sin(angle) + p.y()*cos(angle));
}

Figure::Figure()
{

}

LineFigure::LineFigure(QPointF p1, QPointF p2, QPen pen) :
    Figure()
{
    _p1 = (p1.x() < p2.x()) ? p1 : p2;
    _p2 = (p1.x() < p2.x()) ? p2 : p1;
    double deltay = _p2.y() - _p1.y();
    double deltax = _p2.x() - _p1.x();
    _angle = atan(deltay/deltax);
    _pen = pen;
    calcArea();
}

void LineFigure::draw(QGraphicsScene *scene)
{
    scene->addLine(_p1.x(), _p1.y(), _p2.x(), _p2.y(), _pen);
}

void LineFigure::drawArea(QGraphicsScene *scene)
{
    calcArea();
    if (!_area.isEmpty()) {
        scene->addPolygon(_area, QPen(QColor(127, 0, 127, 127), 1, Qt::DotLine));
        scene->addText(getInfo());
    }
    drawResizePoints(scene);
}

void LineFigure::drawResizePoints(QGraphicsScene *scene)
{
    if (!_resizePoints.empty()) {
        scene->addPolygon(_resizePoints[0], QPen(QColor(0, 0, 0)), QBrush(QColor(255, 255, 102)));
        scene->addPolygon(_resizePoints[1], QPen(QColor(0, 0, 0)), QBrush(QColor(255, 255, 102)));
    }
}

#include <math.h>

bool LineFigure::inArea(QPointF p)
{
    QPointF t = rotate(QPointF(p.x() - _p1.x(), p.y() - _p1.y()), -_angle);
    if ((t.x() > 0) &&
        (t.x() < rotatedEndPoint2().x()) &&
        (t.y() > - _offset) &&
        (t.y() < _offset))
        return true;
    return false;
}

QPointF LineFigure::rotatedEndPoint2()
{
    return rotate(QPointF(_p2.x() - _p1.x(), _p2.y() - _p1.y()), -_angle);
}

void LineFigure::calcArea()
{
    QPointF np2 = rotatedEndPoint2();

    QPointF p1t = rotate(QPointF(0, -_offset), _angle); // p1t point1top
    QPointF p1b = rotate(QPointF(0, _offset), _angle);  // p1b point1bottom
    QPointF p2t = rotate(QPointF(np2.x(), -_offset), _angle);
    QPointF p2b = rotate(QPointF(np2.x(), _offset), _angle);

    QPointF r1t(_p1.x() + p1t.x(), _p1.y() + p1t.y()); // result1top
    QPointF r1b(_p1.x() + p1b.x(), _p1.y() + p1b.y()); // result1bottom
    QPointF r2t(_p1.x() + p2t.x(), _p1.y() + p2t.y()); // ...
    QPointF r2b(_p1.x() + p2b.x(), _p1.y() + p2b.y()); // ...

    QVector <QPointF> points;
    points << r1t << r2t << r2b << r1b;

    _area = QPolygonF(points);
    calcResizePoints();
}

void LineFigure::resetPoints(QPointF deltaPoint)
{
    _p1.setX(_p1.x() + deltaPoint.x());
    _p1.setY(_p1.y() + deltaPoint.y());
    _p2.setX(_p2.x() + deltaPoint.x());
    _p2.setY(_p2.y() + deltaPoint.y());
    calcArea();
}

int LineFigure::length()
{
    return sqrt(pow(_p2.x() - _p1.x(), 2) + pow(_p2.y() - _p1.y(), 2));
}

QString LineFigure::getInfo()
{
    QString s, len;
    len.setNum(length());
    s.append(len);
    s.append(" spt");
    if (scaleCoef) {
        s.append("\n");
        s.append(len.setNum(length()/scaleCoef));
        s.append(" cm");
    }
    return s;
}

void LineFigure::calcResizePoints()
{
    _resizePoints.clear();
    QPointF np2 = rotatedEndPoint2();

    QList<QPointF> tps;
    tps << rotate(QPointF(0, -_offset), _angle) <<
           rotate(QPointF(0, _offset), _angle) <<
           rotate(QPointF(_offset*2, _offset), _angle) <<
           rotate(QPointF(_offset*2, -_offset), _angle) <<

           rotate(QPointF(np2.x() - _offset*2, -_offset), _angle) <<
           rotate(QPointF(np2.x() - _offset*2, _offset), _angle) <<
           rotate(QPointF(np2.x(), _offset), _angle) <<
           rotate(QPointF(np2.x(), -_offset), _angle);

    QVector <QPointF> pts1, pts2;
    for (int i = 0; i < 4; i++) {
        pts1 << QPointF(_p1.x() + tps[i].x(), _p1.y() + tps[i].y());
        pts2 << QPointF(_p1.x() + tps[i + 4].x(), _p1.y() + tps[i + 4].y());
    }
    _resizePoints << QPolygonF(pts1) << QPolygonF(pts2);
}

void LineFigure::resizePoint(int idx, QPointF point)
{
    if (idx == 1) {
        _p1 = point;
    }
    if (idx == 2) {
        _p2 = point;
    }
    double deltay = _p2.y() - _p1.y();
    double deltax = _p2.x() - _p1.x();
    _angle = atan2(deltay, deltax);
}

int LineFigure::inResizePointArea(QPointF p)
{
    QPointF t = rotate(QPointF(p.x() - _p1.x(), p.y() - _p1.y()), -_angle);
    if ((t.x() > 0) && (t.x() < _offset*2) && (t.y() > -_offset) && (t.y() < _offset)) {
        return 1;
    }

    if ((t.x() > rotatedEndPoint2().x() - _offset*2) && (t.x() < rotatedEndPoint2().x()) && (t.y() > - _offset) && (t.y() < _offset)) {
        return 2;
    }
    return 0;
}

void LineFigure::sortPoints()
{
    QPointF p1 = _p1, p2 = _p2;
    _p1 = (p1.x() <= p2.x()) ? p1 : p2;
    _p2 = (p1.x() <= p2.x()) ? p2 : p1;
    double deltay = _p2.y() - _p1.y();
    double deltax = _p2.x() - _p1.x();
    _angle = atan2(deltay, deltax);
}
