#ifndef WAYPOINT_H
#define WAYPOINT_H

#include <QString>
#include <QGeoCoordinate>
#include <QJsonObject>
#include <QMetaType>

struct Waypoint {
    QString id;
    QString name;
    double latitude{0.0};
    double longitude{0.0};
    double altitude{0.0};
    QString description;

    Waypoint() = default;
    Waypoint(const QString &id, const QString &name, double lat, double lon, double alt = 0.0, const QString &desc = "")
        : id(id), name(name), latitude(lat), longitude(lon), altitude(alt), description(desc) {}

    QGeoCoordinate coordinate() const {
        return QGeoCoordinate(latitude, longitude, altitude);
    }

    QJsonObject toJson() const {
        QJsonObject json;
        json["id"] = id;
        json["name"] = name;
        json["latitude"] = latitude;
        json["longitude"] = longitude;
        json["altitude"] = altitude;
        json["description"] = description;
        return json;
    }

    static Waypoint fromJson(const QJsonObject &json) {
        return Waypoint(
            json["id"].toString(),
            json["name"].toString(),
            json["latitude"].toDouble(),
            json["longitude"].toDouble(),
            json["altitude"].toDouble(),
            json["description"].toString()
        );
    }
};

Q_DECLARE_METATYPE(Waypoint)

#endif // WAYPOINT_H
