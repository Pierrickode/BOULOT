#ifndef GPXHANDLER_H
#define GPXHANDLER_H

#include <QString>
#include <QList>
#include "Waypoint.h"

class GpxHandler {
public:
    GpxHandler() = default;

    static QList<Waypoint> loadFromGpx(const QString &filePath, QString *errorMsg = nullptr);
    static bool saveToGpx(const QString &filePath, const QList<Waypoint> &waypoints, const QString &routeName = "AppliGPS Route", QString *errorMsg = nullptr);
};

#endif // GPXHANDLER_H
