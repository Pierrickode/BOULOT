#include "RouteEngine.h"
#include <QtMath>
#include <queue>
#include <limits>
#include <QVector>
#include <QPair>

static constexpr double EARTH_RADIUS_METERS = 6371000.0;

QList<RoadNode> RouteEngine::s_nodes;
QList<RoadEdge> RouteEngine::s_edges;
bool RouteEngine::s_initialized = false;

RouteEngine::RouteEngine(QObject *parent) : QObject(parent) {
    if (!s_initialized) {
        initRoadNetwork();
    }
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

void RouteEngine::initRoadNetwork() {
    if (s_initialized) return;

    // Detailed road graph network covering Île-de-France highways (A1, A6, A10, A13, A86, N118)
    s_nodes = {
        {0, QGeoCoordinate(48.8530, 2.3499), "Paris Centre (Notre-Dame)"},
        {1, QGeoCoordinate(48.8410, 2.2810), "Paris Porte de St-Cloud (A13)"},
        {2, QGeoCoordinate(48.8280, 2.3020), "Paris Porte d'Orléans (N20/A6)"},
        {3, QGeoCoordinate(48.8190, 2.3610), "Paris Porte d'Italie (A6b)"},
        {4, QGeoCoordinate(48.8250, 2.3880), "Paris Porte de Bercy (A4)"},
        {5, QGeoCoordinate(48.8990, 2.3560), "Paris Porte de la Chapelle (A1)"},
        {6, QGeoCoordinate(48.8333, 2.2500), "Boulogne-Billancourt Pont de Sèvres"},
        {7, QGeoCoordinate(48.8150, 2.2150), "Sèvres N118 Interchange"},
        {8, QGeoCoordinate(48.8050, 2.1900), "Meudon N118"},
        {9, QGeoCoordinate(48.7750, 2.2050), "Vélizy-Villacoublay A86/N118"},
        {10, QGeoCoordinate(48.8080, 2.1500), "Versailles East (A13/D185)"},
        {11, QGeoCoordinate(48.8049, 2.1204), "Château de Versailles"},
        {12, QGeoCoordinate(48.8010, 2.0650), "Saint-Cyr-l'École N12"},
        {13, QGeoCoordinate(48.6440, 1.8320), "Rambouillet N10"},
        {14, QGeoCoordinate(48.7300, 2.2220), "Bièvres N118"},
        {15, QGeoCoordinate(48.7250, 2.2720), "Massy-Palaiseau A10/A86"},
        {16, QGeoCoordinate(48.6880, 2.2520), "Longjumeau A10"},
        {17, QGeoCoordinate(48.7262, 2.3652), "Aéroport Orly A6/A106"},
        {18, QGeoCoordinate(48.7833, 2.4667), "Créteil A86/TVM"},
        {19, QGeoCoordinate(48.6238, 2.4439), "Évry-Courcouronnes A6"},
        {20, QGeoCoordinate(48.5393, 2.6611), "Melun A6/N105"},
        {21, QGeoCoordinate(48.4022, 2.7006), "Fontainebleau A6/N7"},
        {22, QGeoCoordinate(48.9356, 2.3539), "Saint-Denis A1/A86"},
        {23, QGeoCoordinate(49.0097, 2.5479), "Aéroport Roissy CDG A1"},

        // Montreal & Quebec Region Nodes
        {24, QGeoCoordinate(45.5017, -73.5532), "Montréal Vieux-Port"},
        {25, QGeoCoordinate(45.5017, -73.5673), "Montréal Centre-Ville"},
        {26, QGeoCoordinate(45.5088, -73.5878), "Montréal Mont-Royal"},
        {27, QGeoCoordinate(45.4657, -73.7455), "Montréal Aéroport YUL"},
        {28, QGeoCoordinate(45.5758, -73.7531), "Laval Centre"},
        {29, QGeoCoordinate(45.5250, -73.5180), "Longueuil Metro"}
    };

    QList<QPair<int, int>> connections = {
        // Paris ring connectors
        {0, 1}, {0, 2}, {0, 3}, {0, 4}, {0, 5},
        // Boulevard Périphérique / Boulogne
        {1, 6}, {6, 7}, {7, 8}, {8, 9}, {9, 10}, {10, 11}, {11, 12}, {12, 13},
        // N118 / Massy / A10
        {9, 14}, {14, 15}, {15, 16}, {15, 17},
        // A6 / Orly / Évry / Fontainebleau
        {2, 17}, {3, 17}, {17, 18}, {17, 19}, {19, 20}, {20, 21}, {16, 19},
        // North A1 / Roissy
        {5, 22}, {22, 23},

        // Montreal Highway Network Connections
        {24, 25}, {25, 26}, {25, 27}, {25, 29}, {26, 28}, {27, 28}
    };

    for (const auto &conn : connections) {
        double dist = calculateDistanceMeters(s_nodes[conn.first].coord, s_nodes[conn.second].coord);
        s_edges.append({conn.first, conn.second, dist});
    }

    s_initialized = true;
}

int RouteEngine::findNearestNode(const QGeoCoordinate &coord) {
    if (!s_initialized) initRoadNetwork();
    if (s_nodes.isEmpty()) return -1;

    int bestIndex = 0;
    double bestDist = calculateDistanceMeters(coord, s_nodes[0].coord);

    for (int i = 1; i < s_nodes.count(); ++i) {
        double dist = calculateDistanceMeters(coord, s_nodes[i].coord);
        if (dist < bestDist) {
            bestDist = dist;
            bestIndex = i;
        }
    }
    return bestIndex;
}

QList<QGeoCoordinate> RouteEngine::dijkstraShortestPath(int startNodeId, int endNodeId) {
    if (!s_initialized) initRoadNetwork();

    if (startNodeId == endNodeId || startNodeId < 0 || endNodeId < 0) {
        if (startNodeId >= 0 && startNodeId < s_nodes.count()) {
            return { s_nodes[startNodeId].coord };
        }
        return {};
    }

    int n = s_nodes.count();
    QVector<double> dist(n, std::numeric_limits<double>::infinity());
    QVector<int> parent(n, -1);

    using Pair = std::pair<double, int>;
    std::priority_queue<Pair, std::vector<Pair>, std::greater<Pair>> pq;

    dist[startNodeId] = 0.0;
    pq.push({0.0, startNodeId});

    QVector<QVector<QPair<int, double>>> adj(n);
    for (const auto &e : s_edges) {
        adj[e.fromNode].append({e.toNode, e.distanceMeters});
        adj[e.toNode].append({e.fromNode, e.distanceMeters});
    }

    while (!pq.empty()) {
        auto top = pq.top();
        pq.pop();
        double d = top.first;
        int u = top.second;

        if (d > dist[u]) continue;
        if (u == endNodeId) break;

        for (const auto &edge : adj[u]) {
            int v = edge.first;
            double w = edge.second;

            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                parent[v] = u;
                pq.push({dist[v], v});
            }
        }
    }

    if (parent[endNodeId] == -1) {
        return { s_nodes[startNodeId].coord, s_nodes[endNodeId].coord };
    }

    QList<QGeoCoordinate> pathCoords;
    int curr = endNodeId;
    while (curr != -1) {
        pathCoords.prepend(s_nodes[curr].coord);
        curr = parent[curr];
    }

    return pathCoords;
}

QList<QGeoCoordinate> RouteEngine::interpolateSegment(const QGeoCoordinate &c1, const QGeoCoordinate &c2, double stepMeters) {
    QList<QGeoCoordinate> points;
    double dist = calculateDistanceMeters(c1, c2);
    if (dist <= stepMeters) {
        points.append(c1);
        points.append(c2);
        return points;
    }

    int steps = std::max(2, qCeil(dist / stepMeters));
    for (int i = 0; i <= steps; ++i) {
        double f = static_cast<double>(i) / steps;
        double lat = c1.latitude() + f * (c2.latitude() - c1.latitude());
        double lon = c1.longitude() + f * (c2.longitude() - c1.longitude());
        points.append(QGeoCoordinate(lat, lon));
    }
    return points;
}

QList<QGeoCoordinate> RouteEngine::calculateShortestRoadRoute(const QList<Waypoint> &waypoints) {
    if (!s_initialized) initRoadNetwork();

    if (waypoints.isEmpty()) return {};
    if (waypoints.count() == 1) return { waypoints[0].coordinate() };

    QList<QGeoCoordinate> fullPath;

    for (int i = 0; i < waypoints.count() - 1; ++i) {
        QGeoCoordinate startCoord = waypoints[i].coordinate();
        QGeoCoordinate endCoord = waypoints[i+1].coordinate();

        int nStart = findNearestNode(startCoord);
        int nEnd = findNearestNode(endCoord);

        QList<QGeoCoordinate> roadNodesPath = dijkstraShortestPath(nStart, nEnd);

        QList<QGeoCoordinate> legWaypoints;
        legWaypoints.append(startCoord);

        for (const auto &c : roadNodesPath) {
            if (legWaypoints.isEmpty() || calculateDistanceMeters(legWaypoints.last(), c) > 50.0) {
                legWaypoints.append(c);
            }
        }

        if (calculateDistanceMeters(legWaypoints.last(), endCoord) > 10.0) {
            legWaypoints.append(endCoord);
        }

        for (int j = 0; j < legWaypoints.count() - 1; ++j) {
            QList<QGeoCoordinate> interpolated = interpolateSegment(legWaypoints[j], legWaypoints[j+1], 80.0);
            for (const auto &p : interpolated) {
                if (fullPath.isEmpty() || calculateDistanceMeters(fullPath.last(), p) > 10.0) {
                    fullPath.append(p);
                }
            }
        }
    }

    return fullPath;
}

double RouteEngine::calculatePathDistanceMeters(const QList<QGeoCoordinate> &path) {
    if (path.count() < 2) return 0.0;
    double total = 0.0;
    for (int i = 0; i < path.count() - 1; ++i) {
        total += calculateDistanceMeters(path[i], path[i+1]);
    }
    return total;
}
