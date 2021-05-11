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

#include <type_traits>
#include <set>

#include <QtWidgets>

#include <BiopsyTilerMaps.h>

#include "renderarea.h"
#include "window.h"



//! [0]
const int IdRole = Qt::UserRole;
//! [0]


static const std::set<PolygonFlags> kFlagsWitchCheckboxesChecked =
{
    PolygonFlags::ConflictingBg,
    PolygonFlags::ConflictingCl,
    PolygonFlags::ConflictingTiles
};

Window::Window()
{
    renderArea = new RenderArea;

    {
        annotationCheckboxes = new QWidget;
        annotationCheckboxes->setLayout(new QGridLayout);
        for(const auto& [key, typeString] : kPolygonTypeNamesToFlags)
        {
            QCheckBox* box = new QCheckBox(tr(typeString.c_str()));
            connect(box, &QCheckBox::stateChanged, this, &Window::onAnnotationsChanged);
            annotationCheckboxes->layout()->addWidget(box);
            box->setChecked(kFlagsWitchCheckboxesChecked.end() != kFlagsWitchCheckboxesChecked.find(key));
        }

        annotationsLabel = new QLabel(tr("&Annotations:"));
        annotationsLabel->setBuddy(annotationCheckboxes);
    }

    {
        markerCheckboxes = new QWidget;
        markerCheckboxes->setLayout(new QGridLayout);
        for(const auto& [key, typeString] : kPointTypeNamesToFlags)
        {
            QCheckBox* box = new QCheckBox(tr(typeString.c_str()));
            connect(box, &QCheckBox::stateChanged, this, &Window::onMarkersChanged);
            markerCheckboxes->layout()->addWidget(box);
            box->setChecked(false);
        }

        markersLabel = new QLabel(tr("&Markers:"));
        markersLabel->setBuddy(markerCheckboxes);
    }

//! [1]

//! [2]
    conflictTileNumberBox = new QSpinBox;
    conflictTileNumberBox->setRange(-1, renderArea->getConflictingTilesNumber() -1);
    conflictTileNumberBox->setSpecialValueText(tr("Conflict tile No."));
    conflictTileLabel = new QLabel(tr("Conflict &Label:"));
    conflictTileLabel->setBuddy(conflictTileNumberBox);

    tileNumberBox = new QSpinBox;
    tileNumberBox->setRange(-1, renderArea->getTilesNumber() -1);
    tileNumberBox->setSpecialValueText(tr("Tile No."));
    tileLabel = new QLabel(tr("Tile &Label:"));
    tileLabel->setBuddy(tileNumberBox);
//! [2]

//! [3]
    penStyleComboBox = new QComboBox;
    penStyleComboBox->addItem(tr("Solid"), static_cast<int>(Qt::SolidLine));
    penStyleComboBox->addItem(tr("Dash"), static_cast<int>(Qt::DashLine));
    penStyleComboBox->addItem(tr("Dot"), static_cast<int>(Qt::DotLine));
    penStyleComboBox->addItem(tr("Dash Dot"), static_cast<int>(Qt::DashDotLine));
    penStyleComboBox->addItem(tr("Dash Dot Dot"), static_cast<int>(Qt::DashDotDotLine));
    penStyleComboBox->addItem(tr("None"), static_cast<int>(Qt::NoPen));

    penStyleLabel = new QLabel(tr("&Pen Style:"));
    penStyleLabel->setBuddy(penStyleComboBox);

    penCapComboBox = new QComboBox;
    penCapComboBox->addItem(tr("Flat"), Qt::FlatCap);
    penCapComboBox->addItem(tr("Square"), Qt::SquareCap);
    penCapComboBox->addItem(tr("Round"), Qt::RoundCap);

    penCapLabel = new QLabel(tr("Pen &Cap:"));
    penCapLabel->setBuddy(penCapComboBox);

    penJoinComboBox = new QComboBox;
    penJoinComboBox->addItem(tr("Miter"), Qt::MiterJoin);
    penJoinComboBox->addItem(tr("Bevel"), Qt::BevelJoin);
    penJoinComboBox->addItem(tr("Round"), Qt::RoundJoin);

    penJoinLabel = new QLabel(tr("Pen &Join:"));
    penJoinLabel->setBuddy(penJoinComboBox);
//! [3]

//! [4]
    brushStyleComboBox = new QComboBox;
    brushStyleComboBox->addItem(tr("Solid"), static_cast<int>(Qt::SolidPattern));
    brushStyleComboBox->addItem(tr("Linear Gradient"),
            static_cast<int>(Qt::LinearGradientPattern));
    brushStyleComboBox->addItem(tr("Radial Gradient"),
            static_cast<int>(Qt::RadialGradientPattern));
    brushStyleComboBox->addItem(tr("Conical Gradient"),
            static_cast<int>(Qt::ConicalGradientPattern));
    brushStyleComboBox->addItem(tr("Texture"), static_cast<int>(Qt::TexturePattern));
    brushStyleComboBox->addItem(tr("Horizontal"), static_cast<int>(Qt::HorPattern));
    brushStyleComboBox->addItem(tr("Vertical"), static_cast<int>(Qt::VerPattern));
    brushStyleComboBox->addItem(tr("Cross"), static_cast<int>(Qt::CrossPattern));
    brushStyleComboBox->addItem(tr("Backward Diagonal"), static_cast<int>(Qt::BDiagPattern));
    brushStyleComboBox->addItem(tr("Forward Diagonal"), static_cast<int>(Qt::FDiagPattern));
    brushStyleComboBox->addItem(tr("Diagonal Cross"), static_cast<int>(Qt::DiagCrossPattern));
    brushStyleComboBox->addItem(tr("Dense 1"), static_cast<int>(Qt::Dense1Pattern));
    brushStyleComboBox->addItem(tr("Dense 2"), static_cast<int>(Qt::Dense2Pattern));
    brushStyleComboBox->addItem(tr("Dense 3"), static_cast<int>(Qt::Dense3Pattern));
    brushStyleComboBox->addItem(tr("Dense 4"), static_cast<int>(Qt::Dense4Pattern));
    brushStyleComboBox->addItem(tr("Dense 5"), static_cast<int>(Qt::Dense5Pattern));
    brushStyleComboBox->addItem(tr("Dense 6"), static_cast<int>(Qt::Dense6Pattern));
    brushStyleComboBox->addItem(tr("Dense 7"), static_cast<int>(Qt::Dense7Pattern));
    brushStyleComboBox->addItem(tr("None"), static_cast<int>(Qt::NoBrush));

    brushStyleLabel = new QLabel(tr("&Brush:"));
    brushStyleLabel->setBuddy(brushStyleComboBox);
//! [4]

//! [5]
    otherOptionsLabel = new QLabel(tr("Options:"));
//! [5] //! [6]
    antialiasingCheckBox = new QCheckBox(tr("&Antialiasing"));
//! [6] //! [7]
    fitToAllCheckBox = new QCheckBox(tr("&Fit to all data"));
//! [7]

//! [8]
//    connect(annotationCheckboxes, QOverload<int>::of(&QComboBox::activated),
//            this, &Window::onAnnotationsChanged);
    connect(conflictTileNumberBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &Window::conflictingTileChanged);
    connect(tileNumberBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &Window::tileChanged);
    connect(penStyleComboBox, QOverload<int>::of(&QComboBox::activated),
            this, &Window::penChanged);
    connect(penCapComboBox, QOverload<int>::of(&QComboBox::activated),
            this, &Window::penChanged);
    connect(penJoinComboBox, QOverload<int>::of(&QComboBox::activated),
            this, &Window::penChanged);
    connect(brushStyleComboBox, QOverload<int>::of(&QComboBox::activated),
            this, &Window::brushChanged);
    connect(antialiasingCheckBox, &QAbstractButton::toggled,
            renderArea, &RenderArea::setAntialiased);
    connect(fitToAllCheckBox, &QAbstractButton::toggled,
            renderArea, &RenderArea::setFittedToTotalLmits);
//! [8]

//! [9]
    QGridLayout *mainLayout = new QGridLayout;
//! [9] //! [10]
    mainLayout->setRowStretch(0, 1);

    mainLayout->addWidget(renderArea,            0, 0, 1, 3);

    mainLayout->addWidget(annotationsLabel,      2, 0, 1, 1, Qt::AlignBottom | Qt::AlignCenter);
    mainLayout->addWidget(annotationCheckboxes,  3, 0, 4, 1, Qt::AlignTop    | Qt::AlignCenter);

    mainLayout->addWidget(markerCheckboxes,      2, 1, 5, 1, Qt::AlignCenter);

    mainLayout->addWidget(conflictTileLabel,     2, 2, 1, 1, Qt::AlignBottom | Qt::AlignCenter);
    mainLayout->addWidget(conflictTileNumberBox, 3, 2, 1, 1, Qt::AlignTop    | Qt::AlignCenter);

    mainLayout->addWidget(tileLabel,             4, 2, 1, 1, Qt::AlignBottom | Qt::AlignCenter);
    mainLayout->addWidget(tileNumberBox,         5, 2, 1, 1, Qt::AlignTop    | Qt::AlignCenter);

    mainLayout->addWidget(fitToAllCheckBox,      6, 2, 1, 1, Qt::AlignCenter);

//    mainLayout->addWidget(penStyleLabel,         4, 0, Qt::AlignRight);
//    mainLayout->addWidget(penStyleComboBox,      4, 1);
//    mainLayout->addWidget(penCapLabel,           3, 2, Qt::AlignRight);
//    mainLayout->addWidget(penCapComboBox,        3, 3);
//    mainLayout->addWidget(penJoinComboBox,       2, 3);
//    mainLayout->addWidget(brushStyleLabel,       4, 2, Qt::AlignRight);
//    mainLayout->addWidget(brushStyleComboBox,    4, 3);
//    mainLayout->addWidget(otherOptionsLabel,     5, 0, Qt::AlignRight);
//    mainLayout->addWidget(antialiasingCheckBox,  5, 1, 1, 1, Qt::AlignRight);

    setLayout(mainLayout);

    onAnnotationsChanged();
    penChanged();
    brushChanged();
    antialiasingCheckBox->setChecked(true);

    setWindowTitle(tr("Pre-processing Tiling Debug Plotting"));
}
//! [10]

//! [11]
void Window::onAnnotationsChanged()
{
    std::underlying_type_t<PolygonFlags> annotationFlags = 0;
    for(uint32_t itemIndex=0; itemIndex<uint32_t(annotationCheckboxes->layout()->count()); ++itemIndex)
    {
        QCheckBox* box = dynamic_cast<QCheckBox*>(annotationCheckboxes->layout()->itemAt(itemIndex)->widget());
        if (!box || box->checkState() != Qt::Checked)
            continue;

        const auto pair = findStringValue(kPolygonTypeNamesToFlags, box->text().toStdString());
        assert(pair.first != PolygonFlags::None);

        annotationFlags |= std::underlying_type_t<PolygonFlags>(pair.first);
    }
    renderArea->setAnnotation(annotationFlags);
}

void Window::onMarkersChanged()
{
    uint32_t pointTypeFlags = 0;
    for(uint32_t itemIndex=0; itemIndex<uint32_t(markerCheckboxes->layout()->count()); ++itemIndex)
    {
        QCheckBox* box = dynamic_cast<QCheckBox*>(markerCheckboxes->layout()->itemAt(itemIndex)->widget());
        if (!box || box->checkState() != Qt::Checked)
            continue;

        const auto pair = findStringValue(kPointTypeNamesToFlags, box->text().toStdString());
        assert(pair.first != PointFlags::None);

        pointTypeFlags |= std::underlying_type_t<PointFlags>(pair.first);
    }
    renderArea->setMarkers(pointTypeFlags);
}

void Window::conflictingTileChanged()
{
    renderArea->setConflictingTile(conflictTileNumberBox->value());
}

void Window::tileChanged()
{
    renderArea->setTile(tileNumberBox->value());
}
//! [11]

//! [12]
void Window::penChanged()
{
    int width = 10;
    Qt::PenStyle style = Qt::PenStyle(penStyleComboBox->itemData(
            penStyleComboBox->currentIndex(), IdRole).toInt());
    Qt::PenCapStyle cap = Qt::PenCapStyle(penCapComboBox->itemData(
            penCapComboBox->currentIndex(), IdRole).toInt());
    Qt::PenJoinStyle join = Qt::PenJoinStyle(penJoinComboBox->itemData(
            penJoinComboBox->currentIndex(), IdRole).toInt());

    renderArea->setPen(QPen(Qt::blue, width, style, cap, join));
}
//! [12]

//! [13]
void Window::brushChanged()
{
    Qt::BrushStyle style = Qt::BrushStyle(brushStyleComboBox->itemData(
//! [13]
            brushStyleComboBox->currentIndex(), IdRole).toInt());

//! [14]
    if (style == Qt::LinearGradientPattern) {
        QLinearGradient linearGradient(0, 0, 100, 100);
        linearGradient.setColorAt(0.0, Qt::white);
        linearGradient.setColorAt(0.2, Qt::green);
        linearGradient.setColorAt(1.0, Qt::black);
        renderArea->setBrush(linearGradient);
//! [14] //! [15]
    } else if (style == Qt::RadialGradientPattern) {
        QRadialGradient radialGradient(50, 50, 50, 70, 70);
        radialGradient.setColorAt(0.0, Qt::white);
        radialGradient.setColorAt(0.2, Qt::green);
        radialGradient.setColorAt(1.0, Qt::black);
        renderArea->setBrush(radialGradient);
    } else if (style == Qt::ConicalGradientPattern) {
        QConicalGradient conicalGradient(50, 50, 150);
        conicalGradient.setColorAt(0.0, Qt::white);
        conicalGradient.setColorAt(0.2, Qt::green);
        conicalGradient.setColorAt(1.0, Qt::black);
        renderArea->setBrush(conicalGradient);
//! [15] //! [16]
    } else if (style == Qt::TexturePattern) {
        renderArea->setBrush(QBrush(QPixmap(":/images/brick.png")));
//! [16] //! [17]
    } else {
        renderArea->setBrush(QBrush(Qt::green, style));
    }
}
//! [17]
