#ifndef DRAWINGCANVAS_H
#define DRAWINGCANVAS_H

#include <QWidget>
#include <QPoint>
#include <QVector>
#include <QPair>

class QPaintEvent;
class QMouseEvent;
class QPainter;
class QPen;

class DrawingCanvas : public QWidget
{
    Q_OBJECT
public:
    explicit DrawingCanvas(QWidget *parent = nullptr);
    void ClearCanvas();
    void SlowConvex();
    void CalculateHullConvex();
    enum AutoMode { AutoBoth, AutoSlowOnly, AutoHullOnly, AutoNone };

public slots:
    void setAutoModeSlow();
    void setAutoModeHull();
    void setAutoModeBoth();
    void setAutoModeNone();


protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    QVector<QPoint> dots;
    QVector<QPoint> UpperHull;
    QVector<QPoint> LowerHull;
    QVector<QPoint> FullConvex;
    QVector<QPoint> SlowFullConvex;
    QVector<QPair<QPoint, QPoint>> Edges;
    static long long crossProduct(const QPoint &a, const QPoint &b, const QPoint &c);
    void resetVisuals();
    int slowChecks = 0;
    int hullChecks = 0;
    bool slowRan = false;
    bool hullRan = false;
signals:
};

#endif // DRAWINGCANVAS_H
