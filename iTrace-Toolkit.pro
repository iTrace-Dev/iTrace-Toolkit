QT += quick
QT += widgets
QT += sql
QT += core
QT += xmlpatterns xml

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
        basicalgorithm.cpp \
        controller.cpp \
        database.cpp \
        fixation.cpp \
        fixationalgorithm.cpp \
        gaze.cpp \
        idtalgorithm.cpp \
        ivtalgorithm.cpp \
        logger.cpp \
        main.cpp \
        participantsmodel.cpp \
        srcmlhandler.cpp \
        srcmlmapper.cpp \
        xmlhandler.cpp

RESOURCES += qml.qrc \
    images.qrc

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
    basicalgorithm.h \
    controller.h \
    database.h \
    fixation.h \
    fixationalgorithm.h \
    gaze.h \
    idtalgorithm.h \
    ivtalgorithm.h \
    logger.h \
    participantsmodel.h \
    srcmlhandler.h \
    srcmlmapper.h \
    xmlhandler.h

CONFIG += qmltypes
QML_IMPORT_NAME = io.qt.examples.backend
QML_IMPORT_MAJOR_VERSION = 1
