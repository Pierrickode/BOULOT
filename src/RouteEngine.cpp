#include "RouteEngine.h"
#include <QtMath>

static constexpr double EARTH_RADIUS_METERS = 6371000.0;

RouteEngine::RouteEngine(QObject *parent) : QObject(parent) {
}

double RouteEngine::calculateDistanceMeters(const QGeoCoordinate &coord1, const QGeoCoordinate &coord2) {
    if (!coord1.isValid() || !coord2.isValid()) {
        return 0.0;
    }

    double lat1Rad = qDegreesToRadians(coord1.latitude());
    double lat2Rad = qDegreesToRadians(coord2.latitude());
    double dLat = qDegreesToRadians(coord2.latitude() - coord1.latitude());
    double dLon = qDegreesToRadians(coord2.longitude() - coord1.longitude());

    double a = qSin(dLat / 2.0) * qSin(dLat / 2.0) +
               qCos(lat1Rad) * qCos(lat2Rad) *
               qSin(dLon / 2.0) * qSin(dLon / 2.0);

    double c = 2.0 * qAtan2(qSqrt(a), qSqrt(1.0 - a));

    return EARTH_RADIUS_METERS * c;
}

double RouteEngine::calculateBearingDegrees(const QGeoCoordinate &coord1, const QGeoCoordinate &coord2) {
    if (!coord1.isValid() || !coord2.isValid()) {
        return 0.0;
    }

    double lat1Rad = qDegreesToRadians(coord1.latitude());
    double lat2Rad = qDegreesToRadians(coord2.latitude());
    double dLon = qDegreesToRadians(coord2.longitude() - coord1.longitude());

    double y = qSin(dLon) * qCos(lat2Rad);
    double x = qCos(lat1Rad) * qSin(lat2Rad) -
               qSin(lat1Rad) * qCos(lat2Rad) * qCos(dLon);

    double bearingRad = qAtan2(y, x);
    double bearingDeg = qRadiansToDegrees(bearingRad);

    return std::fmod((bearingDeg + 360.0), 360.0);
}

double RouteEngine::calculateTotalDistanceMeters(const QList<Waypoint> &waypoints) {
    if (waypoints.count() < 2) {
        return 0.0;
    }

    double total = 0.0;
    for (int i = 0; i < waypoints.count() - 1; ++i) {
        total += calculateDistanceMeters(waypoints[i].coordinate(), waypoints[i+1].coordinate());
    }
    return total;
}

double RouteEngine::calculateETASeconds(double distanceMeters, double speedKmh) {
    if (speedKmh <= 0.1) {
        return 0.0;
    }
    double speedMps = speedKmh / 3.6;
    return distanceMeters / speedMps;
}

QString RouteEngine::formatDistance(double meters) {
    if (meters >= 1000.0) {
        return QString::number(meters / 1000.0, 'f', 1) + " km";
    }
    return QString::number(qRound(meters)) + " m";
}

QString RouteEngine::formatDuration(double totalSeconds) {
    int totalSecs = qRound(totalSeconds);
    int hours = totalSecs / 3600;
    int minutes = (totalSecs % 3600) / 60;

    if (hours > 0) {
        return QString("%1 h %2 min").arg(hours).arg(minutes, 2, 10, QChar('0'));
    }
    return QString("%1 min").arg(minutes);
}

QString RouteEngine::compassDirection(double bearingDegrees) {
    static const QString directions[] = { "N", "NE", "E", "SE", "S", "SW", "W", "NW", "N" };
    int index = qRound(bearingDegrees / 45.0) % 8;
    if (index < 0) index += 8;
    return directions[index];
}

int RouteEngine::findNextWaypointIndex(const QGeoCoordinate &currentPos, const QList<Waypoint> &waypoints, double thresholdMeters) {
    if (waypoints.isEmpty()) return -1;

    for (int i = 0; i < waypoints.count(); ++i) {
        double dist = calculateDistanceMeters(currentPos, waypoints[i].coordinate());
        if (dist > thresholdMeters) {
            return i;
        }
    }
    return waypoints.count() - 1;
}
