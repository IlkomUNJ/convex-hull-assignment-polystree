#include "drawingwindow.h"
#include "drawingcanvas.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMessageBox>

DrawingWindow::DrawingWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Computer Graphic Simulation");
    resize(600, 400);

    QWidget *mainContainer = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(mainContainer);

    canvas = new DrawingCanvas(this);

    QPushButton *pushButtonSlowConvex = new QPushButton("Run Slow Convex Hull", this);
    QPushButton *pushButtonClear = new QPushButton("Clear Canvas", this);
    QPushButton *pushButtonDraw = new QPushButton("Run Convex Hull", this);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(pushButtonDraw);
    buttonLayout->addWidget(pushButtonClear);
    buttonLayout->addWidget(pushButtonSlowConvex);

    mainLayout->addWidget(canvas);
    mainLayout->addLayout(buttonLayout);

    setCentralWidget(mainContainer);

    // clicking the hull button toggles hull-auto-mode and runs the hull once
    connect(pushButtonDraw, &QPushButton::clicked, [this](){
        canvas->setAutoModeHull();
        canvas->CalculateHullConvex();
    });
    connect(pushButtonClear, &QPushButton::clicked, canvas, &DrawingCanvas::ClearCanvas);
    // clicking the slow button toggles slow-auto-mode and runs the slow convex once
    connect(pushButtonSlowConvex, &QPushButton::clicked, [this](){
        canvas->setAutoModeSlow();
        canvas->SlowConvex();
    });

}
