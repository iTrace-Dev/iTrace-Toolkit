QT += quick
QT += widgets
QT += sql
QT += core

CONFIG += c++11
CONFIG += sql

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Refer to the documentation for the
# deprecated API to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    fixation.cpp \
    gaze.cpp \
        main.cpp \
        participantsmodel.cpp \
        database.cpp \


RESOURCES += qml.qrc

TRANSLATIONS += \
    iTrace-Toolkit_en_US.ts

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH = $$PWD

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# ICON
RC_ICONS = iTrace.ico

HEADERS += \
    fixation.h \
    gaze.h \
    participantsmodel.h \
    database.h

CONFIG += qmltypes
QML_IMPORT_NAME = io.qt.examples.backend
QML_IMPORT_MAJOR_VERSION = 1
