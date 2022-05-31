##########################################################################################################################################################################################################################################################
# @file iTrace-Toolkit.pro
#
# @Copyright (C) 2022 i-trace.org
#
# This file is part of iTrace Infrastructure http://www.i-trace.org/.
# iTrace Infrastructure is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
# iTrace Infrastructure is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
# You should have received a copy of the GNU General Public License along with iTrace Infrastructure. If not, see <https://www.gnu.org/licenses/>.
##########################################################################################################################################################################################################################################################

QT += quick

CONFIG += c++11

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
<<<<<<< HEAD
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
=======
        main.cpp

RESOURCES += qml.qrc
>>>>>>> 73b168bc23d7cf768d99a7692d5c70b04ddc1b27

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
<<<<<<< HEAD

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
=======
>>>>>>> 73b168bc23d7cf768d99a7692d5c70b04ddc1b27
