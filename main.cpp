#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "participantsmodel.h"
#include "control.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication app(argc, argv);

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
