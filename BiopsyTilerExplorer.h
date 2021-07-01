#pragma once

#include <QWidget>

QT_BEGIN_NAMESPACE
class QCheckBox;
class QComboBox;
class QGridLayout;
class QLabel;
class QSpinBox;
QT_END_NAMESPACE
class BiopsyRenderer;

//! [0]
class BiopsyTilerExplorer : public QWidget
{
    Q_OBJECT

public:
    BiopsyTilerExplorer(const std::string& cellPositionsJsonPath,
                        const std::string& annotationsJsonPath);

private slots:
    void onTumorGridCellsVisibilityChanged();
    void onImmuneGridCellsVisibilityChanged();
    void onAnnotationsChanged();
    void onMarkersChanged();
    void conflictingTileChanged();
    void tileChanged();
    void penChanged();
    void brushChanged();

private:
    BiopsyRenderer *renderArea;

    QLabel *tileLabel;
    QLabel *conflictTileLabel;
    QSpinBox *tileNumberBox;
    QSpinBox *conflictTileNumberBox;

    QLabel *markersLabel;
    QLabel *annotationsLabel;
    QWidget *annotationCheckboxes;
    QWidget *markerCheckboxes;

    QCheckBox* tumorGridCellsCheckBox;
    QCheckBox* immuneGridCellsCheckBox;

    QLabel *penStyleLabel;
    QLabel *penCapLabel;
    QLabel *penJoinLabel;
    QLabel *brushStyleLabel;
    QLabel *otherOptionsLabel;
    QComboBox *penStyleComboBox;
    QComboBox *penCapComboBox;
    QComboBox *penJoinComboBox;
    QComboBox *brushStyleComboBox;
    QCheckBox *antialiasingCheckBox;
    QCheckBox *fitToAllCheckBox;
};
//! [0]
