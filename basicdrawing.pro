QT += widgets gui
CONFIG += c++1z
requires(qtConfig(combobox))

HEADERS       = renderarea.h                              \
                window.h                                  \
                ..\mathpath-core\src\BiopsyTiler.h        \
                ..\mathpath-core\src\clipper.hpp          \
                ..\mathpath-core\src\Cell2D.h             \
                ..\mathpath-core\src\CellOccupationGrid.h \
                ..\mathpath-core\src\CellSystem.h         \
                ..\mathpath-core\src\Chronograph.h        \
                ..\mathpath-core\src\BiopsyTilerMaps.h

SOURCES       = main.cpp                                    \
                renderarea.cpp                              \
                ..\mathpath-core\src\BiopsyTiler.cpp        \
                ..\mathpath-core\src\Cell2D.cpp             \
                ..\mathpath-core\src\CellOccupationGrid.cpp \
                ..\mathpath-core\src\CellSystem.cpp         \
                ..\mathpath-core\src\clipper.cpp            \
                ..\mathpath-core\src\Chronograph.cpp        \
                window.cpp
RESOURCES     = basicdrawing.qrc

INCLUDEPATH = ..\mathpath-core\src     \
              ..\glm                   \
              ..\boost_1_72_0          \
              ..\clipper_ver6.4.2\cpp  \
              ..\json\include

#INSTALLS += target
