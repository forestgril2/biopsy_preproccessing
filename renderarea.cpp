/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "renderarea.h"

#include <iostream>

#include <QPainter>
#include <QPainterPath>

#include <BiopsyTiler.h>

QVector<QPointF> getQPointsF(const std::vector<bgPoint>& bgPoints)
{
    QVector<QPointF> newPoints;
    newPoints.reserve(bgPoints.size());
    for(const bgPoint& pointBg : bgPoints)
    {
        newPoints.append(QPointF(pointBg.x(), pointBg.y()));
    }
    return newPoints;
}

RenderArea::RenderArea(QWidget *parent)
    : QWidget(parent)
{
    shape = Polygon;
    antialiased = false;
    transformed = false;
    pixmap.load(":/images/qt-logo.png");

    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);

    BiopsyTiler biopsyData;

    const std::map<std::string, std::vector<bgPoint>*> pointsBgVectorMap = biopsyData.getBgPoints();
    const std::map<std::string, std::vector<bgPolygon>*> polygonVectorMap = biopsyData.getBgPolygons();

    for (const auto& pair : pointsBgVectorMap)
    {
        qPointVectorMap[pair.first] = getQPointsF(*pair.second);
    }

    std::cout << __FUNCTION__ << "  polygon vector map loop:" << std::endl;
    for (const auto& pair : polygonVectorMap)
    {
        const std::string annotation = pair.first;
        std::cout << __FUNCTION__ << "  polygon vector annotation: " << annotation  << std::endl;
        const std::vector<bgPolygon>& bgPolygons = *pair.second;
        std::cout << __FUNCTION__ << "  polygon vector size: " << bgPolygons.size() << std::endl;

        QPainterPath paths;
        for(const bgPolygon& polygonBg : bgPolygons)
        {
            QVector<QPointF> points = getQPointsF(polygonBg.outer());
            std::cout << __FUNCTION__ << "  adding polygon path with points.size():" << points.size() << "," << "" << std::endl;
            paths.addPolygon(QPolygonF(points));
        }
        qPathsMap[annotation].swap(paths);
    }
}
//! [0]

//! [1]
QSize RenderArea::minimumSizeHint() const
{
    return QSize(100, 100);
}
//! [1]

//! [2]
QSize RenderArea::sizeHint() const
{
    return QSize(400, 200);
}
//! [2]

//! [3]
void RenderArea::setShape(Shape shape)
{
    this->shape = shape;
    update();
}
//! [3]

//! [4]
void RenderArea::setPen(const QPen &pen)
{
    this->pen = pen;
    update();
}
//! [4]

//! [5]
void RenderArea::setBrush(const QBrush &brush)
{
    this->brush = brush;
    update();
}
//! [5]

//! [6]
void RenderArea::setAntialiased(bool antialiased)
{
    this->antialiased = antialiased;
    update();
}
//! [6]

//! [7]
void RenderArea::setTransformed(bool transformed)
{
    this->transformed = transformed;
    update();
}
//! [7]

//! [8]
void RenderArea::paintEvent(QPaintEvent * /* event */)
{
    static const std::vector<QPoint> points = {
        QPoint(10, 80),
        QPoint(20, 10),
        QPoint(80, 30),
        QPoint(90, 70)
    };

    QRect rect(10, 20, 80, 60);

    QPainterPath path(qPathsMap["tumor"]);

//    std::cout << __FUNCTION__ << "  :" << path.toFillPolygon() << "," << "" << std::endl;

    QPainter painter(this);
    painter.setPen(pen);
    painter.setBrush(brush);
    if (antialiased)
        painter.setRenderHint(QPainter::Antialiasing, true);

    if (transformed) {
//                painter.translate(50, 50);
//                painter.rotate(60.0);
        painter.scale(0.01, 0.01);
//                painter.translate(-50, -50);
    }

    switch (shape) {
    case Line:
        painter.drawLine(rect.bottomLeft(), rect.topRight());
        break;
    case Points:
        painter.drawPoints(points.data(), (int)points.size());
        break;
    case Polyline:
        painter.drawPolyline(points.data(), (int)points.size());
        break;
    case Polygon:
        painter.drawPolygon(points.data(), (int)points.size());
        break;
    case Text:
        painter.drawText(rect,
                         Qt::AlignCenter,
                         tr("Qt by\nThe Qt Company"));
        break;
    case Path:
        painter.drawPath(path);
    }

    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setPen(palette().dark().color());
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(QRect(0, 0, width() - 1, height() - 1));
}
//! [13]
