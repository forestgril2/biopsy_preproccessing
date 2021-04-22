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

#include <QBrush>
#include <QPainter>
#include <QPaintEvent>
#include <QPainterPath>

#include <BiopsyTiler.h>

static QVector<QPointF> getQPointsF(const std::vector<bgPoint>& bgPoints)
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
    _annotationFlags = Tumor;
    _antialiased = false;
    _fitToTotalLimits = false;
    _pixmap.load(":/images/qt-logo.png");

    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);

    const BiopsyTiler biopsyData;

    BBox<double> limits = biopsyData.getTotalLimits();

    _totalLimits.setLeft(limits[0]);
    _totalLimits.setTop(limits[3]);
    _totalLimits.setRight(limits[2]);
    _totalLimits.setBottom(limits[1]);

    const std::map<std::string, std::vector<bgPoint>*> pointsBgVectorMap = biopsyData.getBgPoints();
    const std::map<std::string, std::vector<bgPolygon>*> polygonVectorMap = biopsyData.getBgPolygons();

    for (const auto& pair : pointsBgVectorMap)
    {
        qPointVectorMap[pair.first] = getQPointsF(*pair.second);
    }

    for (const auto& pair : polygonVectorMap)
    {
        const std::string annotation = pair.first;
        const std::vector<bgPolygon>& bgPolygons = *pair.second;
        std::cout << __FUNCTION__ << "  polygon vector annotation: " << annotation  << ", vector size: " << bgPolygons.size() << std::endl;

        QPainterPath paths;
        for(const bgPolygon& polygonBg : bgPolygons)
        {
            QVector<QPointF> points = getQPointsF(polygonBg.outer());
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
    return QSize(1000, 500);
}
//! [2]

//! [3]
void RenderArea::setAnnotation(uint32_t annotations)
{
    this->_annotationFlags = annotations;
    update();
}
//! [3]

//! [4]
void RenderArea::setPen(const QPen &pen)
{
    this->_pen = pen;
    update();
}
//! [4]

//! [5]
void RenderArea::setBrush(const QBrush &brush)
{
    this->_brush = brush;
    update();
}
//! [5]

//! [6]
void RenderArea::setAntialiased(bool antialiased)
{
    this->_antialiased = antialiased;
    update();
}
//! [6]

//! [7]
void RenderArea::setFittedToTotalLmits(bool fitted)
{
    this->_fitToTotalLimits = fitted;
    update();
}
//! [7]

//! [8]
void RenderArea::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setPen(_pen);
    painter.setBrush(_brush);
    if (_antialiased)
        painter.setRenderHint(QPainter::Antialiasing, true);

    const QSizeF paintAreaSize = event->rect().size();
    QSizeF scale(1, 1);
    QPointF translate(0, 0);

    if (_fitToTotalLimits) {
        scale = {paintAreaSize.width()  / _totalLimits.size().width(),
                 paintAreaSize.height() / _totalLimits.size().height()};
        translate = {-_totalLimits.left(), -_totalLimits.top()};
    }
    else
    {
        const QRectF limits = getChosenAnnotationLimits();

        QRectF flippedLimits;
        flippedLimits.setLeft(limits.left());
        flippedLimits.setRight(limits.right());
        flippedLimits.setTop(limits.bottom());
        flippedLimits.setBottom(limits.top());

        scale = {paintAreaSize.width()  / flippedLimits.size().width(),
                 paintAreaSize.height() / flippedLimits.size().height()};
        translate = {-flippedLimits.left(), -flippedLimits.top()};
    }

    painter.scale(scale.width(), scale.height());
    painter.translate(translate);

    if (_annotationFlags & Control)
    {
        QColor controlColor(Qt::green);
        controlColor.setAlpha(128);
        painter.setBrush(controlColor);
        painter.drawPath(qPathsMap["control"]);
    }
    if (_annotationFlags & Tumor)
    {
        QColor tumorColor(Qt::red);
        tumorColor.setAlpha(128);
        painter.setBrush(tumorColor);
        painter.drawPath(qPathsMap["tumor"]);
    }
    if (_annotationFlags & Tissue)
    {
        QColor tissueColor(Qt::blue);
        tissueColor.setAlpha(128);
        painter.setBrush(tissueColor);
        painter.drawPath(qPathsMap["tissue"]);
    }
    if (_annotationFlags & Necrosis)
    {
        QColor necrosisColor(Qt::black);
        necrosisColor.setAlpha(128);
        painter.setBrush(necrosisColor);
        painter.drawPath(qPathsMap["necrosis"]);
    }
    if (_annotationFlags & Exclude)
    {
        QColor excludeColor(Qt::darkYellow);
        excludeColor.setAlpha(128);
        painter.setBrush(excludeColor);
        painter.drawPath(qPathsMap["exclude"]);
    }

    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setPen(palette().dark().color());
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(QRect(0, 0, width() - 1, height() - 1));
}

QRectF RenderArea::getChosenAnnotationLimits() const
{
    QPainterPath chosenAnnotationPaths;
    if (_annotationFlags & Tumor)
    {
        chosenAnnotationPaths.addPath(qPathsMap.at("tumor"));
    }
    if (_annotationFlags & Control)
    {
        chosenAnnotationPaths.addPath(qPathsMap.at("control"));
    }
    if (_annotationFlags & Tissue)
    {
        chosenAnnotationPaths.addPath(qPathsMap.at("tissue"));
    }
    if (_annotationFlags & Necrosis)
    {
        chosenAnnotationPaths.addPath(qPathsMap.at("necrosis"));
    }
    if (_annotationFlags & Exclude)
    {
        chosenAnnotationPaths.addPath(qPathsMap.at("exclude"));
    }

    return chosenAnnotationPaths.boundingRect();
}
//! [13]
