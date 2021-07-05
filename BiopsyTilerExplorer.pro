QT += core widgets gui
CONFIG += c++1z
requires(qtConfig(combobox))

#DEFINES += NDEBUG
DEFINES += DEBUG_REPEATING_OCCUPATION_INDEXES

HEADERS       = BiopsyRenderer.h                              \
                BiopsyTilerExplorer.h                                  \
                ..\mathpath-core\src\BiopsyTiler.h        \
                ..\mathpath-core\src\BiopsyTilerMaps.h    \
                ..\mathpath-core\src\clipper.hpp          \
                ..\mathpath-core\src\Cell2D.h             \
                ..\mathpath-core\src\CellOccupationGrid.h \
                ..\mathpath-core\src\CellSystem.h         \
                ..\mathpath-core\src\CellSystemParams.h   \
                ..\mathpath-core\src\Chronograph.h        \
                ..\mathpath-core\src\Random.h

SOURCES       = main.cpp                                    \
                BiopsyRenderer.cpp                              \
                BiopsyTilerExplorer.cpp                                  \
                ..\mathpath-core\src\BiopsyTiler.cpp        \
                ..\mathpath-core\src\Cell2D.cpp             \
                ..\mathpath-core\src\CellOccupationGrid.cpp \
                ..\mathpath-core\src\CellSystem.cpp         \
                ..\mathpath-core\src\CellSystemParams.cpp   \
                ..\mathpath-core\src\clipper.cpp            \
                ..\mathpath-core\src\Chronograph.cpp        \
                ..\mathpath-core\src\Random.cpp

INCLUDEPATH = ..\mathpath-core\src     \
              ..\glm                   \
              ..\boost_1_72_0          \
              ..\clipper_ver6.4.2\cpp  \
              ..\json\include

#INSTALLS += target
