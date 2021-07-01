#include <BiopsyTilerExplorer.h>

#include <type_traits>
#include <set>

#include <QtWidgets>

#include <BiopsyRenderer.h>
#include <BiopsyTilerMaps.h>

//! [0]
const int IdRole = Qt::UserRole;
//! [0]


static const std::set<PolygonFlags> kFlagsWitchCheckboxesChecked =
{
    PolygonFlags::ConflictingBg,
    PolygonFlags::ConflictingCl,
    PolygonFlags::ConflictingTiles
};

BiopsyTilerExplorer::BiopsyTilerExplorer()
{
    renderArea = new BiopsyRenderer;

    {
        annotationCheckboxes = new QWidget;
        annotationCheckboxes->setLayout(new QGridLayout);
        for(const auto& [key, typeString] : kPolygonTypeNamesToFlags)
        {
            QCheckBox* box = new QCheckBox(tr(typeString.c_str()));
            connect(box, &QCheckBox::stateChanged, this, &BiopsyTilerExplorer::onAnnotationsChanged);
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
            connect(box, &QCheckBox::stateChanged, this, &BiopsyTilerExplorer::onMarkersChanged);
            markerCheckboxes->layout()->addWidget(box);
            box->setChecked(false);
        }

        markersLabel = new QLabel(tr("&Markers:"));
        markersLabel->setBuddy(markerCheckboxes);
    }

    tumorGridCellsCheckBox = new QCheckBox("Tumor grid cells");
    tumorGridCellsCheckBox->setChecked(false);
    connect(tumorGridCellsCheckBox, &QCheckBox::stateChanged, this, &BiopsyTilerExplorer::onTumorGridCellsVisibilityChanged);

    immuneGridCellsCheckBox = new QCheckBox("Immune  grid cells");
    immuneGridCellsCheckBox->setChecked(false);
    connect(immuneGridCellsCheckBox, &QCheckBox::stateChanged, this, &BiopsyTilerExplorer::onImmuneGridCellsVisibilityChanged);

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
//            this, &BiopsyTilerExplorer::onAnnotationsChanged);
    connect(conflictTileNumberBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &BiopsyTilerExplorer::conflictingTileChanged);
    connect(tileNumberBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &BiopsyTilerExplorer::tileChanged);
    connect(penStyleComboBox, QOverload<int>::of(&QComboBox::activated),
            this, &BiopsyTilerExplorer::penChanged);
    connect(penCapComboBox, QOverload<int>::of(&QComboBox::activated),
            this, &BiopsyTilerExplorer::penChanged);
    connect(penJoinComboBox, QOverload<int>::of(&QComboBox::activated),
            this, &BiopsyTilerExplorer::penChanged);
    connect(brushStyleComboBox, QOverload<int>::of(&QComboBox::activated),
            this, &BiopsyTilerExplorer::brushChanged);
    connect(antialiasingCheckBox, &QAbstractButton::toggled,
            renderArea, &BiopsyRenderer::setAntialiased);
    connect(fitToAllCheckBox, &QAbstractButton::toggled,
            renderArea, &BiopsyRenderer::setFittedToTotalLmits);
//! [8]

//! [9]
    QGridLayout *mainLayout = new QGridLayout;
//! [9] //! [10]
    mainLayout->setColumnStretch(0, 1);

    mainLayout->addWidget(renderArea,               0, 0, 10, 1);

    mainLayout->addWidget(annotationsLabel,         0, 1, 1, 1, Qt::AlignBottom | Qt::AlignCenter);
    mainLayout->addWidget(annotationCheckboxes,     1, 1, 1, 1, Qt::AlignTop    | Qt::AlignCenter);

    mainLayout->addWidget(markerCheckboxes,         2, 1, 1, 1, Qt::AlignBottom | Qt::AlignCenter);
    mainLayout->addWidget(tumorGridCellsCheckBox,   3, 1, 1, 1, Qt::AlignBottom | Qt::AlignCenter);
    mainLayout->addWidget(immuneGridCellsCheckBox,  4, 1, 1, 1, Qt::AlignTop    | Qt::AlignCenter);

    mainLayout->addWidget(conflictTileLabel,        5, 1, 1, 1, Qt::AlignBottom | Qt::AlignCenter);
    mainLayout->addWidget(conflictTileNumberBox,    6, 1, 1, 1, Qt::AlignTop    | Qt::AlignCenter);

    mainLayout->addWidget(tileLabel,                7, 1, 1, 1, Qt::AlignBottom | Qt::AlignCenter);
    mainLayout->addWidget(tileNumberBox,            8, 1, 1, 1, Qt::AlignTop    | Qt::AlignCenter);

    mainLayout->addWidget(fitToAllCheckBox,         9, 1, 1, 1, Qt::AlignCenter);

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

void BiopsyTilerExplorer::onTumorGridCellsVisibilityChanged()
{
    renderArea->setTumorGridCellsVisibility(tumorGridCellsCheckBox->checkState() == Qt::Checked);
}

void BiopsyTilerExplorer::onImmuneGridCellsVisibilityChanged()
{
    renderArea->setImmuneGridCellsVisibility(immuneGridCellsCheckBox->checkState() == Qt::Checked);
}
//! [10]

//! [11]
void BiopsyTilerExplorer::onAnnotationsChanged()
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

void BiopsyTilerExplorer::onMarkersChanged()
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

void BiopsyTilerExplorer::conflictingTileChanged()
{
    renderArea->setConflictingTile(conflictTileNumberBox->value());
}

void BiopsyTilerExplorer::tileChanged()
{
    renderArea->setTile(tileNumberBox->value());
}
//! [11]

//! [12]
void BiopsyTilerExplorer::penChanged()
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
void BiopsyTilerExplorer::brushChanged()
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
