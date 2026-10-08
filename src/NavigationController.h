#ifndef NAVIGATIONCONTROLLER_H
#define NAVIGATIONCONTROLLER_H

#include <QObject>
#include <QGeoCoordinate>
#include "WaypointModel.h"
#include "GpsProvider.h"

class NavigationController : public QObject {
    Q_OBJECT
    Q_PROPERTY(WaypointModel* waypointModel READ waypointModel CONSTANT)
    Q_PROPERTY(GpsProvider* gpsProvider READ gpsProvider CONSTANT)
    Q_PROPERTY(double totalDistance READ totalDistance NOTIFY routeUpdated)
    Q_PROPERTY(QString formattedTotalDistance READ formattedTotalDistance NOTIFY routeUpdated)
    Q_PROPERTY(QString formattedETA READ formattedETA NOTIFY routeUpdated)
    Q_PROPERTY(QString nextWaypointName READ nextWaypointName NOTIFY navigationStateUpdated)
    Q_PROPERTY(QString formattedDistanceToNext READ formattedDistanceToNext NOTIFY navigationStateUpdated)
    Q_PROPERTY(double distanceToNextMeters READ distanceToNextMeters NOTIFY navigationStateUpdated)
    Q_PROPERTY(double bearingToNext READ bearingToNext NOTIFY navigationStateUpdated)
    Q_PROPERTY(QString compassDirection READ compassDirection NOTIFY navigationStateUpdated)

public:
    explicit NavigationController(QObject *parent = nullptr);

    WaypointModel* waypointModel() const { return m_waypointModel; }
    GpsProvider* gpsProvider() const { return m_gpsProvider; }

    double totalDistance() const { return m_totalDistanceMeters; }
    QString formattedTotalDistance() const;
    QString formattedETA() const;

    QString nextWaypointName() const { return m_nextWaypointName; }
    QString formattedDistanceToNext() const;
    double distanceToNextMeters() const { return m_distanceToNextMeters; }
    double bearingToNext() const { return m_bearingToNext; }
    QString compassDirection() const;

    Q_INVOKABLE bool loadGpxFile(const QString &filePath);
    Q_INVOKABLE bool saveGpxFile(const QString &filePath);
    Q_INVOKABLE void addWaypointByCoordinate(double lat, double lon, const QString &name = "");
    Q_INVOKABLE void startSimulation(double speedKmh = 60.0);
    Q_INVOKABLE void loadDefaultDemoRoute();

signals:
    void routeUpdated();
    void navigationStateUpdated();
    void statusMessage(const QString &message, bool isError = false);

private slots:
    void updateRouteCalculations();
    void updateNavigationState();

private:
    WaypointModel *m_waypointModel{nullptr};
    GpsProvider *m_gpsProvider{nullptr};

    double m_totalDistanceMeters{0.0};
    int m_nextWaypointIndex{-1};
    QString m_nextWaypointName{"--"};
    double m_distanceToNextMeters{0.0};
    double m_bearingToNext{0.0};
};

#endif // NAVIGATIONCONTROLLER_H
