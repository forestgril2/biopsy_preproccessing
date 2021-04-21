QT += widgets gui
requires(qtConfig(combobox))

HEADERS       = renderarea.h                         \
                window.h                             \
                ..\mathpath-core\src\BiopsyTiler.h   \
                ..\mathpath-core\src\clipper.hpp     \
                ..\mathpath-core\src\Chronograph.h

SOURCES       = main.cpp                             \
                renderarea.cpp                       \
                ..\mathpath-core\src\BiopsyTiler.cpp \
                ..\mathpath-core\src\clipper.cpp     \
                ..\mathpath-core\src\Chronograph.cpp \
                window.cpp
RESOURCES     = basicdrawing.qrc

INCLUDEPATH = ..\mathpath-core\src     \
              ..\boost_1_72_0          \
              ..\clipper_ver6.4.2\cpp  \
              ..\json\include

#INSTALLS += target
