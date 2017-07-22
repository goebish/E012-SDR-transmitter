TEMPLATE = app
TARGET = e012_transmitter
DESTDIR = ./build
QT += core network gamepad
CONFIG += debug console
DEFINES += QT_DLL QT_NETWORK_LIB
INCLUDEPATH += . \
    ./GeneratedFiles/debug
DEPENDPATH += .
MOC_DIR += ./GeneratedFiles/debug
OBJECTS_DIR += debug
UI_DIR += ./GeneratedFiles
RCC_DIR += ./GeneratedFiles
include(e012_transmitter.pri)
