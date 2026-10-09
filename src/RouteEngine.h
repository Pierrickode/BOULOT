#ifndef ROUTEENGINE_H
#define ROUTEENGINE_H

#include <QObject>
#include <QGeoCoordinate>
#include <QList>
#include "Waypoint.h"

struct RoadNode {
    int id;
    QGeoCoordinate coord;
    QString name;
};

struct RoadEdge {
    int fromNode;
    int toNode;
    double distanceMeters;
};

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

    // Offline Shortest Road Routing (Dijkstra algorithm on road network)
    static QList<QGeoCoordinate> calculateShortestRoadRoute(const QList<Waypoint> &waypoints);
    static double calculatePathDistanceMeters(const QList<QGeoCoordinate> &path);

    // Navigation state helpers
    static int findNextWaypointIndex(const QGeoCoordinate &currentPos, const QList<Waypoint> &waypoints, double thresholdMeters = 30.0);

private:
    static void initRoadNetwork();
    static int findNearestNode(const QGeoCoordinate &coord);
    static QList<QGeoCoordinate> dijkstraShortestPath(int startNodeId, int endNodeId);
    static QList<QGeoCoordinate> interpolateSegment(const QGeoCoordinate &c1, const QGeoCoordinate &c2, double stepMeters = 100.0);

    static QList<RoadNode> s_nodes;
    static QList<RoadEdge> s_edges;
    static bool s_initialized;
};

#endif // ROUTEENGINE_H
