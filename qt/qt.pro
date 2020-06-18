QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    picture.cpp \
    pictureLabel.cpp \
    window.cpp \
    ../core/kdTree.cpp \
    ../core/parser.cpp \
    ../core/areaLight.cpp \
    ../core/shader.cpp \
    ../core/camera.cpp \
    ../core/threadPool.cpp \
    ../core/util.cpp \
    ../core/const.cpp

HEADERS += \
    picture.h \
    pictureLabel.h \
    window.h \
    ../core/kdTree.h \
    ../core/parser.h \
    ../core/areaLight.h \
    ../core/shader.h \
    ../core/camera.h \
    ../core/threadPool.h \
    ../core/util.h \
    ../core/const.h

FORMS += \
    window.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
