#include <QtTest>
#include "../src/WaypointModel.h"
#include "../src/GpxHandler.h"
#include "../src/RouteEngine.h"
#include "../src/NavigationController.h"
#include <QTemporaryFile>

class TestNavigation : public QObject {
    Q_OBJECT

private slots:
    void testWaypointModelCRUD();
    void testGpxExportAndImport();
    void testRouteEngineGeodesic();
    void testNavigationControllerLogic();
};

void TestNavigation::testWaypointModelCRUD() {
    WaypointModel model;
    QCOMPARE(model.rowCount(), 0);

    model.addWaypoint("Paris", 48.8566, 2.3522);
    model.addWaypoint("Lyon", 45.7640, 4.8357);
    model.addWaypoint("Marseille", 43.2965, 5.3698);

    QCOMPARE(model.rowCount(), 3);
    QCOMPARE(model.getWaypoint(0).name, QString("Paris"));
    QCOMPARE(model.getWaypoint(1).name, QString("Lyon"));

    // Test Move
    model.moveWaypoint(0, 2);
    QCOMPARE(model.getWaypoint(2).name, QString("Paris"));

    // Test Remove
    QVERIFY(model.removeWaypoint(0));
    QCOMPARE(model.rowCount(), 2);
}

void TestNavigation::testGpxExportAndImport() {
    QList<Waypoint> original;
    original.append(Waypoint("1", "Point A", 48.8566, 2.3522, 100.0, "Start"));
    original.append(Waypoint("2", "Point B", 45.7640, 4.8357, 150.0, "End"));

    QTemporaryFile tempFile;
    QVERIFY(tempFile.open());
    QString filePath = tempFile.fileName();
    tempFile.close();

    QString errorMsg;
    bool saved = GpxHandler::saveToGpx(filePath, original, "Test Route", &errorMsg);
    QVERIFY2(saved, qPrintable(errorMsg));

    QList<Waypoint> loaded = GpxHandler::loadFromGpx(filePath, &errorMsg);
    QCOMPARE(loaded.count(), 2);
    QCOMPARE(loaded[0].name, QString("Point A"));
    QCOMPARE(loaded[1].name, QString("Point B"));
    QCOMPARE(loaded[0].latitude, 48.8566);
    QCOMPARE(loaded[1].longitude, 4.8357);
}

void TestNavigation::testRouteEngineGeodesic() {
    QGeoCoordinate paris(48.8566, 2.3522);
    QGeoCoordinate lyon(45.7640, 4.8357);

    // Distance Paris -> Lyon is ~392 km (392,000 meters)
    double distMeters = RouteEngine::calculateDistanceMeters(paris, lyon);
    QVERIFY(distMeters > 380000 && distMeters < 410000);

    // Bearing Paris -> Lyon is roughly SSE (~156 degrees)
    double bearing = RouteEngine::calculateBearingDegrees(paris, lyon);
    QVERIFY(bearing > 140.0 && bearing < 170.0);

    QString compass = RouteEngine::compassDirection(bearing);
    QCOMPARE(compass, QString("SE"));

    // Formatters
    QCOMPARE(RouteEngine::formatDistance(1500), QString("1.5 km"));
    QCOMPARE(RouteEngine::formatDistance(450), QString("450 m"));
}

void TestNavigation::testNavigationControllerLogic() {
    NavigationController controller;
    QVERIFY(controller.waypointModel()->rowCount() > 0);

    double totalDist = controller.totalDistance();
    QVERIFY(totalDist > 0.0);

    controller.startSimulation(100.0);
    QVERIFY(controller.gpsProvider()->isSimulating());
    QCOMPARE(controller.gpsProvider()->currentSpeedKmh(), 100.0);

    controller.gpsProvider()->stopSimulation();
    QVERIFY(!controller.gpsProvider()->isSimulating());
}

QTEST_MAIN(TestNavigation)
#include "test_navigation.moc"
