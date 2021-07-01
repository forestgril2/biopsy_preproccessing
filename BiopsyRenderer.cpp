#include "BiopsyRenderer.h"

#include <iostream>
#include <type_traits>

#include <QBrush>
#include <QPainter>
#include <QPaintEvent>
#include <QPainterPath>

#include <BiopsyTiler.h>

static QRectF getPointsBoundingRect(const QVector<QPointF>& points)
{
    BBox<qreal> bbox;
    const uint32_t limits = (uint32_t)points.size() -1;
    for(uint32_t i=0; i<limits; i+=2)
    {
        bbox.resize(BBox<qreal>({points[i   ].x(),
                                 points[i   ].y(),
                                 points[i +1].x(),
                                 points[i +1].y()}));
    }

    if (0 != points.size() % 2)
    {//Get last box for already included point and a last one, which is odd.
        bbox.resize(BBox<qreal>({points[limits -2].x(),
                                 points[limits -2].y(),
                                 points[limits -1].x(),
                                 points[limits -1].y()}));
    }
    return QRectF{QPointF{bbox[0], bbox[1]}, QPointF{bbox[2], bbox[3]}};
};

static const std::map<PolygonFlags, QColor> kQPolygonPathColorsMap =
{
    {PolygonFlags::Tumor             , Qt::red},
    {PolygonFlags::Control           , Qt::green},
    {PolygonFlags::Tissue            , Qt::blue},
    {PolygonFlags::Necrosis          , Qt::black},
    {PolygonFlags::Exclude           , Qt::darkYellow},
    {PolygonFlags::TissueAndTumor    , Qt::transparent},
    {PolygonFlags::ExcludeOrNecrosis , Qt::transparent},
    {PolygonFlags::Final             , Qt::darkGreen},
    {PolygonFlags::ConflictingBg     , Qt::transparent},
    {PolygonFlags::ConflictingCl     , Qt::transparent},
    {PolygonFlags::ConflictingSh     , Qt::transparent},
    {PolygonFlags::ConflictingTiles  , Qt::transparent},
};

static const Qt::PenStyle kBasePenStyle = Qt::SolidLine;
static const Qt::PenCapStyle kBasePenCapStyle = Qt::RoundCap;

static const std::map<PolygonFlags, QPen> kQPolygonPathPensMap =
{
    {PolygonFlags::Tumor             , QPen(Qt::black,   1,   kBasePenStyle, kBasePenCapStyle)},
    {PolygonFlags::Control           , QPen(Qt::black,   1,   kBasePenStyle, kBasePenCapStyle)},
    {PolygonFlags::Tissue            , QPen(Qt::black,   1,   kBasePenStyle, kBasePenCapStyle)},
    {PolygonFlags::Necrosis          , QPen(Qt::black,   1,   kBasePenStyle, kBasePenCapStyle)},
    {PolygonFlags::Exclude           , QPen(Qt::black,   1,   kBasePenStyle, kBasePenCapStyle)},
    {PolygonFlags::TissueAndTumor    , QPen(Qt::black,   1,   kBasePenStyle, kBasePenCapStyle)},
    {PolygonFlags::ExcludeOrNecrosis , QPen(Qt::magenta, 1,   kBasePenStyle, kBasePenCapStyle)},
    {PolygonFlags::Final             , QPen(Qt::yellow,  1,   kBasePenStyle, kBasePenCapStyle)},
    {PolygonFlags::ConflictingBg     , QPen(Qt::red,     1,   kBasePenStyle, kBasePenCapStyle)},
    {PolygonFlags::ConflictingCl     , QPen(Qt::blue,    1,   kBasePenStyle, kBasePenCapStyle)},
    {PolygonFlags::ConflictingSh     , QPen(Qt::red,     1,   kBasePenStyle, kBasePenCapStyle)},
    {PolygonFlags::ConflictingTiles  , QPen(Qt::black,   1,   kBasePenStyle, kBasePenCapStyle)}
};

static const std::map<PointFlags, QColor> kQPointColorsMap =
{
    {PointFlags::NonProliferatingCD8  , Qt::blue},
    {PointFlags::ProliferatingCD8     , Qt::cyan},
    {PointFlags::ProliferatingTumor   , Qt::red},
    {PointFlags::TumorProlifFinal     , Qt::yellow},
    {PointFlags::ImmuneProlifFinal    , Qt::cyan},
    {PointFlags::ImmuneNonProlifFinal , Qt::blue},
};

static const QPen kBasePointPen = QPen(Qt::black, 7, Qt::SolidLine, Qt::RoundCap);

static const std::map<PointFlags, QPen> kQPointPenMap =
{
    {PointFlags::NonProliferatingCD8  , kBasePointPen},
    {PointFlags::ProliferatingCD8     , kBasePointPen},
    {PointFlags::ProliferatingTumor   , kBasePointPen},
    {PointFlags::TumorProlifFinal     , kBasePointPen},
    {PointFlags::ImmuneProlifFinal    , kBasePointPen},
    {PointFlags::ImmuneNonProlifFinal , kBasePointPen}
};

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

static QRectF getQRectFromBBox(const BBox<double>& limits)
{
    QRectF converted;
    converted.setLeft(limits[0]);
    converted.setTop(limits[3]);
    converted.setRight(limits[2]);
    converted.setBottom(limits[1]);
    return converted;
}

BiopsyRenderer::BiopsyRenderer(const BiopsyTiler& biopsyTilerData, QWidget *parent)
    : QWidget(parent)
{
    _polygonFlags = std::underlying_type_t<PolygonFlags>(PolygonFlags::Tumor);
    _antialiased = false;
    _fitToTotalLimits = false;
    _pixmap.load(":/images/qt-logo.png");

    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);

    _totalLimits = getQRectFromBBox(biopsyTilerData.getTotalLimits());

    const std::map<PointFlags, std::vector<bgPoint>>& pointsBgVectorMap = biopsyTilerData.getBgPoints();
    const std::map<PolygonFlags, std::vector<bgPolygon>>& polygonVectorMap = biopsyTilerData.getBgPolygons();

    if (polygonVectorMap.end() != polygonVectorMap.find(PolygonFlags::ConflictingTiles))
    {
        const std::vector<bgPolygon>& conflictingTiles = polygonVectorMap.at(PolygonFlags::ConflictingTiles);
        for (const bgPolygon& tile : conflictingTiles)
        {
            _conflictingTileBoundaries.push_back(getQRectFromBBox(BiopsyTiler::calculateBgPolygonLimits(tile)));
        }
    }

    for (const auto& [flag, points] : pointsBgVectorMap)
    {
        qPointVectorMap[flag] = getQPointsF(points);
        std::cout << __FUNCTION__ << "  added markers: " << kPointTypeNamesToFlags.at(flag) << ", with size: " << points.size() << std::endl;
    }

    for (const auto& [flag, polygonsBg] : polygonVectorMap)
    {
        //TODO: This check should not be necessary:
        enumFlagCheck(kPolygonTypeNamesToFlags, flag);
        std::cout << __FUNCTION__ << "  adding polygon vector annotation: " << kPolygonTypeNamesToFlags.at(flag) << ", vector size: " << polygonsBg.size() << std::endl;

        QPainterPath paths;
        for(const bgPolygon& polygonBg : polygonsBg)
        {
            QVector<QPointF> points = getQPointsF(polygonBg.outer());
            paths.addPolygon(QPolygonF(points));

            //TODO: there may be more inceptions here!
            for (const auto& innerPolygon : polygonBg.inners())
            {
                bgPolygon polygon;
                bg::convert(innerPolygon, polygon);
                QPainterPath innerPath;
                QVector<QPointF> points = getQPointsF(polygon.outer());
                innerPath.addPolygon(QPolygonF(points));
                innerPath = innerPath.toReversed();
                paths.addPath(innerPath);
            }
        }
        qPathsMap[flag].swap(paths);
    }

    for (const auto& [box, cellSystem] : biopsyTilerData.getTiles())
    {
        _tiles.emplace_back(getQRectFromBBox(box), cellSystem);
    }

}
//! [0]

//! [1]
QSize BiopsyRenderer::minimumSizeHint() const
{
    return QSize(100, 100);
}
//! [1]

//! [2]
QSize BiopsyRenderer::sizeHint() const
{
    return QSize(1000, 500);
}

void BiopsyRenderer::setMarkers(uint32_t markers)
{
    this->_pointFlags = markers;
    update();
}
//! [2]

//! [3]
void BiopsyRenderer::setAnnotation(uint32_t annotations)
{
    this->_polygonFlags = annotations;
    update();
}
//! [3]

//! [4]
void BiopsyRenderer::setPen(const QPen &pen)
{
    this->_pen = pen;
    update();
}
//! [4]

//! [5]
void BiopsyRenderer::setBrush(const QBrush &brush)
{
    this->_brush = brush;
    update();
}
//! [5]

//! [6]
void BiopsyRenderer::setAntialiased(bool antialiased)
{
    this->_antialiased = antialiased;
    update();
}
//! [6]

//! [7]
void BiopsyRenderer::setFittedToTotalLmits(bool fitted)
{
    this->_fitToTotalLimits = fitted;
    update();
}

void BiopsyRenderer::setTumorGridCellsVisibility(bool areVisible)
{
    if (_areTumorGridCellsVisible == areVisible)
        return;

    _areTumorGridCellsVisible = areVisible;
    update();
}

void BiopsyRenderer::setImmuneGridCellsVisibility(bool areVisible)
{
    if (_areImmuneGridCellsVisible == areVisible)
        return;

    _areImmuneGridCellsVisible = areVisible;
    update();
}

uint32_t BiopsyRenderer::getTilesNumber() const
{
    return _tiles.size();
}

void BiopsyRenderer::setTile(int32_t number)
{
    _currentTile = number;
    update();
}

uint32_t BiopsyRenderer::getConflictingTilesNumber() const
{
    return _conflictingTileBoundaries.size();
}

void BiopsyRenderer::setConflictingTile(int32_t number)
{
    _currentConflictingTile = number;
    update();
}
//! [7]

//! [8]
void BiopsyRenderer::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setPen(_pen);
    painter.setBrush(_brush);
    if (_antialiased)
        painter.setRenderHint(QPainter::Antialiasing, true);

    const QSizeF paintAreaSize = event->rect().size();
    QSizeF scale(1, 1);
    QPointF translate(0, 0);

    const CellSystem* gridCells = nullptr;

    if (_fitToTotalLimits) {
        scale = {paintAreaSize.width()  / _totalLimits.size().width(),
                 paintAreaSize.height() / _totalLimits.size().height()};
        translate = {-_totalLimits.left(), -_totalLimits.top()};
    }
    else
    {
        QRectF limits = getChosenObjectsLimits();
        if (_currentConflictingTile >= 0)
        {
            limits = _conflictingTileBoundaries[_currentConflictingTile];
        }
        else if (_currentTile >= 0)
        {
            limits = _tiles[_currentTile].first;
            gridCells = &_tiles[_currentTile].second;
        }
        else
        {
            limits = getChosenObjectsLimits();
        }

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

    painter.setRenderHint(QPainter::Antialiasing, false);
//    painter.setPen(palette().dark().color());
//    painter.drawRect(QRect(0, 0, width() - 1, height() - 1));

    const auto drawPolygon = [this, &painter](PolygonFlags flag) {
        QColor controlColor(kQPolygonPathColorsMap.at(flag));
        painter.setPen(kQPolygonPathPensMap.at(flag));
        controlColor.setAlpha(128);
        painter.setBrush(controlColor);
        painter.drawPath(qPathsMap[flag]);
    };

    for(const auto& [polygonKeyFlag, path] : qPathsMap)
    {
        if (!(_polygonFlags & std::underlying_type_t<PolygonFlags>(polygonKeyFlag)))
            continue;

        drawPolygon(polygonKeyFlag);
    }

    const auto drawPointVector = [this, &painter](PointFlags flag) {
        const QColor color(kQPointColorsMap.at(flag));
        painter.setBrush(color);
        QPen pen(kQPointPenMap.at(flag));
        pen.setColor(color);
        painter.setPen(pen);
        painter.drawPoints(qPointVectorMap[flag]);
    };

    for(const auto& [pointKeyFlag, path] : qPointVectorMap)
    {
        if (!(_pointFlags & std::underlying_type_t<PointFlags>(pointKeyFlag)))
            continue;

        drawPointVector(pointKeyFlag);
    }

    const auto drawCellSystem = [this, &painter](const CellSystem& cellSystem, const QRectF& tile) {

        painter.setPen(QPen(Qt::black, 0.5, Qt::SolidLine, Qt::SquareCap));

        const QRectF cellRect(tile.x(), tile.y(), tile.width()*0.01, tile.height()*0.01);

        if (_areImmuneGridCellsVisible)
        {
            for (const Cell2D& cell : cellSystem._immuneCells)
            {
                QColor color;
                if (cell.getProliferationCapacity() == (int32_t)BiopsyTiler::getImmuneProliferatingCellCapacity())
                {
                    color = Qt::cyan;
                }
                else
                {
                    color.setRgbF(cell.getColor().x, cell.getColor().y, cell.getColor().z);
                }
                painter.setBrush(color);

                QRectF cellRectTranslated(cellRect);
                cellRectTranslated.translate(cell.getPosition().x*cellRect.width(), (99-cell.getPosition().y)*cellRect.height());
                painter.drawRect(cellRectTranslated);
            }
        }

        if (_areTumorGridCellsVisible)
        {
            for (const Cell2D& cell : cellSystem._tumorCells)
            {
                QColor color;
                if (cell.getProliferationCapacity() == (int32_t)BiopsyTiler::getTumorProliferatingCellCapacity())
                {
                    color = Qt::yellow;
                }
                else
                {
                    color.setRgbF(cell.getColor().x, cell.getColor().y, cell.getColor().z);
                }
                painter.setBrush(color);

                QRectF cellRectTranslated(cellRect);
                cellRectTranslated.translate(cell.getPosition().x*cellRect.width(), (99-cell.getPosition().y)*cellRect.height());
                painter.drawRect(cellRectTranslated);
            }
        }
    };

    painter.setPen(kBasePointPen);
    painter.setBrush(Qt::transparent);
    for(const auto& [tile, cellSystem]: _tiles)
    {
        painter.drawRect(tile);
        if (&cellSystem == gridCells)
        {
            drawCellSystem(cellSystem, tile);
        }
    }
}

QRectF BiopsyRenderer::getChosenObjectsLimits() const
{
    QPainterPath chosenAnnotationPaths;

    for(const auto& [polygonKeyFlag, path] : qPathsMap)
    {
        if (!(_polygonFlags & std::underlying_type_t<PolygonFlags>(polygonKeyFlag)))
            continue;

        chosenAnnotationPaths.addPath(qPathsMap.at(polygonKeyFlag));
    }

    QRectF bounding = chosenAnnotationPaths.boundingRect();

    for(const auto& [pointKeyFlag, path] : qPointVectorMap)
    {
        if (!(_pointFlags & std::underlying_type_t<PointFlags>(pointKeyFlag)))
            continue;

        bounding |= getPointsBoundingRect(qPointVectorMap.at(pointKeyFlag));
    }

    return bounding;
}
