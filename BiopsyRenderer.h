#pragma once

#include <QBrush>
#include <QPen>
#include <QPixmap>
#include <QWidget>
#include <QPainterPath>

#include <BiopsyTilerMaps.h>

class CellSystem;

class BiopsyRenderer : public QWidget
{
    Q_OBJECT

public:
    explicit BiopsyRenderer(QWidget *parent = nullptr);

    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;

public slots:
    void setMarkers(uint32_t markers);
    void setAnnotation(uint32_t annotations);
    void setPen(const QPen &pen);
    void setBrush(const QBrush &brush);
    void setAntialiased(bool antialiased);
    void setFittedToTotalLmits(bool fitted);


    void setTumorGridCellsVisibility(bool areVisible);
    void setImmuneGridCellsVisibility(bool areVisible);

    uint32_t getTilesNumber() const;
    void setTile(int32_t number);

    uint32_t getConflictingTilesNumber() const;
    void setConflictingTile(int32_t number);

protected:
    void paintEvent(QPaintEvent *event) override;

private:    
    QRectF getChosenObjectsLimits() const;

    uint32_t _polygonFlags;
    uint32_t _pointFlags;
    QPen _pen;
    QBrush _brush;
    bool _antialiased;
    bool _fitToTotalLimits;
    QPixmap _pixmap;

    bool _areTumorGridCellsVisible = false;
    bool _areImmuneGridCellsVisible = false;

    std::map<PointFlags, QVector<QPointF>> qPointVectorMap;
    std::map<PolygonFlags, QPainterPath> qPathsMap;
    QRectF _totalLimits;
    std::vector<std::pair<QRectF, CellSystem>> _tiles;
    std::vector<QRectF> _conflictingTileBoundaries;

    int32_t _currentTile = -1;
    int32_t _currentConflictingTile = -1;

};
//! [0]
