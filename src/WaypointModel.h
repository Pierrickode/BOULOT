#ifndef WAYPOINTMODEL_H
#define WAYPOINTMODEL_H

#include <QAbstractListModel>
#include <QList>
#include "Waypoint.h"

class WaypointModel : public QAbstractListModel {
    Q_OBJECT

public:
    enum WaypointRoles {
        IdRole = Qt::UserRole + 1,
        NameRole,
        LatitudeRole,
        LongitudeRole,
        AltitudeRole,
        DescriptionRole,
        CoordinateRole
    };

    explicit WaypointModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void addWaypoint(const QString &name, double lat, double lon, double alt = 0.0, const QString &desc = "");
    Q_INVOKABLE void insertWaypoint(int index, const QString &name, double lat, double lon, double alt = 0.0, const QString &desc = "");
    Q_INVOKABLE bool removeWaypoint(int index);
    Q_INVOKABLE void clear();
    Q_INVOKABLE void moveWaypoint(int fromIndex, int toIndex);
    Q_INVOKABLE Waypoint getWaypoint(int index) const;

    const QList<Waypoint>& waypoints() const { return m_waypoints; }
    void setWaypoints(const QList<Waypoint> &waypoints);

signals:
    void waypointsChanged();

private:
    QList<Waypoint> m_waypoints;
};

#endif // WAYPOINTMODEL_H
