#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include "NavigationController.h"

int main(int argc, char *argv[]) {
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);
    app.setOrganizationName("AppliGPS");
    app.setOrganizationDomain("appligps.org");

    qRegisterMetaType<Waypoint>("Waypoint");

    QQuickStyle::setStyle("Basic");

    NavigationController navCtrl;

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("navCtrl", &navCtrl);

    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    engine.load(url);

    return app.exec();
}
