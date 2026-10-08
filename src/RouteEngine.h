#ifndef ROUTEENGINE_H
#define ROUTEENGINE_H

#include <QObject>
#include <QGeoCoordinate>
#include <QList>
#include "Waypoint.h"

class RouteEngine : public QObject {
    Q_OBJECT

public:
    explicit RouteEngine(QObject *parent = nullptr);

    // Geodesic calculations (Haversine formula)
    static double calculateDistanceMeters(const QGeoCoordinate &coord1, const QGeoCoordinate &coord2);
    static double calculateBearingDegrees(const QGeoCoordinate &coord1, const QGeoCoordinate &coord2);
    static double calculateTotalDistanceMeters(const QList<Waypoint> &waypoints);
    static double calculateETASeconds(double distanceMeters, double speedKmh);
    static QString formatDistance(double meters);
    static QString formatDuration(double totalSeconds);
    static QString compassDirection(double bearingDegrees);

    // Navigation state helpers
    static int findNextWaypointIndex(const QGeoCoordinate &currentPos, const QList<Waypoint> &waypoints, double thresholdMeters = 30.0);
};

#endif // ROUTEENGINE_H
