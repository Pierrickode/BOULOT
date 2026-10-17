#ifndef VALHALLAENGINE_H
#define VALHALLAENGINE_H

#include <QObject>
#include <QGeoCoordinate>
#include <QList>
#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include "Waypoint.h"

class ValhallaEngine : public QObject {
    Q_OBJECT

public:
    explicit ValhallaEngine(QObject *parent = nullptr);

    // Checks if local Valhalla tile tilesets exist for regions
    bool isMontrealAvailable() const;
    bool isVersaillesAvailable() const;
    QString valhallaTilesDirectory() const;

    // Local / Offline Valhalla Route Calculation
    QList<QGeoCoordinate> calculateValhallaRoute(const QList<Waypoint> &waypoints, const QString &costing = "auto");

    // Valhalla offline geocoding and elevation lookup helpers
    double getElevationMeters(const QGeoCoordinate &coord) const;
    QJsonObject createValhallaRouteRequestJson(const QList<Waypoint> &waypoints, const QString &costing = "auto") const;

private:
    void initializeValhallaTiles();
    QList<QGeoCoordinate> parseValhallaShape(const QString &encodedShape) const;

    QString m_tilesDir;
    bool m_montrealTilesPresent{true};
    bool m_versaillesTilesPresent{true};
};

#endif // VALHALLAENGINE_H
