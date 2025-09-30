#ifndef DRAWINGWINDOW_H
#define DRAWINGWINDOW_H

#include <QMainWindow>
#include "drawingcanvas.h"

class DrawingWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit DrawingWindow(QWidget *parent = nullptr);

private:
    DrawingCanvas *canvas;
    bool value = true;

signals:
};

#endif // DRAWINGWINDOW_H
