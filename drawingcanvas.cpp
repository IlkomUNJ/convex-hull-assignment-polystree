#include "drawingcanvas.h"
#include <algorithm>
#include <QMouseEvent>
#include <QPainter>

DrawingCanvas::DrawingCanvas(QWidget *parent) : QWidget{parent} {}

static DrawingCanvas::AutoMode g_defaultMode = DrawingCanvas::AutoNone;

void DrawingCanvas::setAutoModeSlow()
{
    g_defaultMode = g_defaultMode == AutoSlowOnly ? AutoNone : AutoSlowOnly;
    if (g_defaultMode == AutoSlowOnly) {
        FullConvex.clear();
        hullChecks = 0;
        hullRan = false;
    }
    update();
}

void DrawingCanvas::setAutoModeHull()
{
    g_defaultMode = g_defaultMode == AutoHullOnly ? AutoNone : AutoHullOnly;
    if (g_defaultMode == AutoHullOnly) {
        SlowFullConvex.clear();
        slowChecks = 0;
        slowRan = false;
    }
    update();
}

void DrawingCanvas::setAutoModeBoth() { g_defaultMode = AutoBoth; update(); }
void DrawingCanvas::setAutoModeNone() { g_defaultMode = AutoNone; update(); }

void DrawingCanvas::ClearCanvas()
{
    dots.clear();
    UpperHull.clear();
    LowerHull.clear();
    FullConvex.clear();
    SlowFullConvex.clear();
    Edges.clear();
    slowChecks = hullChecks = 0;
    slowRan = hullRan = false;
    update();
}

void DrawingCanvas::resetVisuals()
{
    FullConvex.clear();
    SlowFullConvex.clear();
    Edges.clear();
    UpperHull.clear();
    LowerHull.clear();
    slowChecks = hullChecks = 0;
    slowRan = hullRan = false;
}

void DrawingCanvas::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) return;
    dots.append(event->pos());
    resetVisuals();

    if (g_defaultMode == AutoSlowOnly || g_defaultMode == AutoBoth) SlowConvex();
    if (g_defaultMode == AutoHullOnly || g_defaultMode == AutoBoth) CalculateHullConvex();
    if (g_defaultMode == AutoNone) update();
}

void DrawingCanvas::SlowConvex()
{
    FullConvex.clear();
    UpperHull.clear();
    LowerHull.clear();
    Edges.clear();
    slowChecks = 0;
    slowRan = true;
    const int n = dots.size();
    if (!n) { update(); return; }

    for (int i = 0; i < n; ++i) {
        const QPoint &p = dots[i];
        for (int j = 0; j < n; ++j) {
            if (i == j) continue;
            const QPoint &q = dots[j];
            bool valid = true;
            for (int k = 0; k < n; ++k) {
                if (k == i || k == j) continue;
                if (crossProduct(p, q, dots[k]) > 0) { valid = false; break; }
            }
            ++slowChecks;
            if (valid) Edges.append(QPair<QPoint, QPoint>(p, q));
        }
    }

    SlowFullConvex.clear();
    if (!Edges.isEmpty()) {
        const auto startIt = std::min_element(Edges.begin(), Edges.end(), [](const auto &a, const auto &b){
            return a.first.x() == b.first.x() ? a.first.y() < b.first.y() : a.first.x() < b.first.x();
        });
        QPoint start = startIt->first;
        QPoint current = start;
        SlowFullConvex.append(start);
        while (true) {
            auto edgeIt = std::find_if(Edges.begin(), Edges.end(), [&](const auto &e){ return e.first == current; });
            if (edgeIt == Edges.end() || edgeIt->second == start) break;
            SlowFullConvex.append(edgeIt->second);
            current = edgeIt->second;
        }
    }
    update();
}

void DrawingCanvas::CalculateHullConvex()
{
    SlowFullConvex.clear();
    Edges.clear();
    UpperHull.clear();
    LowerHull.clear();
    FullConvex.clear();
    hullChecks = 0;
    hullRan = true;
    const int n = dots.size();
    if (!n) { update(); return; }

    std::sort(dots.begin(), dots.end(), [](const QPoint &a, const QPoint &b){
        return a.x() == b.x() ? a.y() < b.y() : a.x() < b.x();
    });

    if (n <= 2) { FullConvex = dots; update(); return; }

    UpperHull.clear();
    LowerHull.clear();
    UpperHull.reserve(n);
    LowerHull.reserve(n);
    for (const QPoint &p : dots) {
        UpperHull.append(p);
        while (UpperHull.size() > 2) {
            long long cross = crossProduct(UpperHull[UpperHull.size()-3], UpperHull[UpperHull.size()-2], UpperHull.back());
            ++hullChecks;
            if (cross >= 0) UpperHull.remove(UpperHull.size()-2);
            else break;
        }
    }
    for (int i = n - 1; i >= 0; --i) {
        LowerHull.append(dots[i]);
        while (LowerHull.size() > 2) {
            long long cross = crossProduct(LowerHull[LowerHull.size()-3], LowerHull[LowerHull.size()-2], LowerHull.back());
            ++hullChecks;
            if (cross >= 0) LowerHull.remove(LowerHull.size()-2);
            else break;
        }
    }

    if (!LowerHull.isEmpty()) LowerHull.removeFirst();
    if (!LowerHull.isEmpty()) LowerHull.removeLast();

    FullConvex = UpperHull;
    for (const QPoint &p : LowerHull) FullConvex.append(p);
    update();
}

void DrawingCanvas::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QFont font = painter.font();
    font.setPointSize(12);
    font.setBold(true);
    painter.setFont(font);
    painter.drawText(10, 20, QString("Number of points: %1").arg(dots.size()));
    const QString slowText = slowRan ? QString::number(slowChecks) : QStringLiteral("-");
    const QString hullText = hullRan ? QString::number(hullChecks) : QStringLiteral("-");
    painter.drawText(10, 40, QString("Iterations (hull / slow): %1 / %2").arg(hullText, slowText));

    painter.setPen(QPen(Qt::blue, 2));
    for (const auto &e : Edges) painter.drawLine(e.first, e.second);

    painter.setPen(QPen(Qt::darkGreen, 2));
    for (int i = 0; i < SlowFullConvex.size(); ++i)
        painter.drawLine(SlowFullConvex[i], SlowFullConvex[(i + 1) % SlowFullConvex.size()]);

    painter.setPen(QPen(Qt::blue, 2));
    for (int i = 0; i < FullConvex.size(); ++i)
        painter.drawLine(FullConvex[i], FullConvex[(i + 1) % FullConvex.size()]);

    painter.setPen(QPen(Qt::red, 8, Qt::SolidLine, Qt::RoundCap));
    for (const QPoint &p : dots) painter.drawPoint(p);
}

long long DrawingCanvas::crossProduct(const QPoint &a, const QPoint &b, const QPoint &c)
{
    return 1LL * (b.x() - a.x()) * (c.y() - a.y()) - 1LL * (b.y() - a.y()) * (c.x() - a.x());
}

