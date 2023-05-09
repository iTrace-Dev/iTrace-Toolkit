/********************************************************************************************************************************************************
* @file main.cpp
*
* @Copyright (C) 2022 i-trace.org
*
* This file is part of iTrace Infrastructure http://www.i-trace.org/.
* iTrace Infrastructure is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
* iTrace Infrastructure is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
* You should have received a copy of the GNU General Public License along with iTrace Infrastructure. If not, see <https://www.gnu.org/licenses/>.
********************************************************************************************************************************************************/

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "participantsmodel.h"
//#include "control.h"

int main(int argc, char *argv[])
{
    //QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_DisableHighDpiScaling);

    QApplication app(argc, argv);

    app.setOrganizationName("i-Trace.org");
    app.setOrganizationDomain("Eye Tracking Framework");

    qmlRegisterType<ParticipantsModel>("Participants", 1, 0, "ParticipantsModel");
    qmlRegisterUncreatableType<ParticipantsList>("Participants", 1, 0, "ParticipantsList", QStringLiteral("Participants should not be created in QML"));

    ParticipantsList participants;

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.rootContext()->setContextProperty("participants", &participants);
    engine.load(url);

    return app.exec();
}
