QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

HEADERS += \
        ../../Src/LogUI/LogFilter.h \
        ../../Src/LogUI/LogTheme.h

SOURCES += \
        main.cpp \
        ../../Src/LogUI/LogFilter.cpp \
        ../../Src/LogUI/LogTheme.cpp

target.path = ../../Temp/$${TARGET}
INSTALLS += target
