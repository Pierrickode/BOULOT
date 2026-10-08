#include "WaypointModel.h"
#include <QUuid>

WaypointModel::WaypointModel(QObject *parent)
    : QAbstractListModel(parent) {
}

int WaypointModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;
    return m_waypoints.count();
}

QVariant WaypointModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() < 0 || index.row() >= m_waypoints.count())
        return QVariant();

    const Waypoint &wpt = m_waypoints[index.row()];

    switch (role) {
    case IdRole:
        return wpt.id;
    case NameRole:
        return wpt.name;
    case LatitudeRole:
        return wpt.latitude;
    case LongitudeRole:
        return wpt.longitude;
    case AltitudeRole:
        return wpt.altitude;
    case DescriptionRole:
        return wpt.description;
    case CoordinateRole:
        return QVariant::fromValue(wpt.coordinate());
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> WaypointModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[IdRole] = "id";
    roles[NameRole] = "name";
    roles[LatitudeRole] = "latitude";
    roles[LongitudeRole] = "longitude";
    roles[AltitudeRole] = "altitude";
    roles[DescriptionRole] = "description";
    roles[CoordinateRole] = "coordinate";
    return roles;
}

void WaypointModel::addWaypoint(const QString &name, double lat, double lon, double alt, const QString &desc) {
    insertWaypoint(m_waypoints.count(), name, lat, lon, alt, desc);
}

void WaypointModel::insertWaypoint(int index, const QString &name, double lat, double lon, double alt, const QString &desc) {
    if (index < 0 || index > m_waypoints.count())
        index = m_waypoints.count();

    beginInsertRows(QModelIndex(), index, index);
    QString id = QUuid::createUuid().toString(QUuid::WithoutBraces);
    m_waypoints.insert(index, Waypoint(id, name, lat, lon, alt, desc));
    endInsertRows();

    emit waypointsChanged();
}

bool WaypointModel::removeWaypoint(int index) {
    if (index < 0 || index >= m_waypoints.count())
        return false;

    beginRemoveRows(QModelIndex(), index, index);
    m_waypoints.removeAt(index);
    endRemoveRows();

    emit waypointsChanged();
    return true;
}

void WaypointModel::clear() {
    if (m_waypoints.isEmpty())
        return;

    beginResetModel();
    m_waypoints.clear();
    endResetModel();

    emit waypointsChanged();
}

void WaypointModel::moveWaypoint(int fromIndex, int toIndex) {
    if (fromIndex < 0 || fromIndex >= m_waypoints.count() ||
        toIndex < 0 || toIndex >= m_waypoints.count() || fromIndex == toIndex) {
        return;
    }

    int destination = (toIndex > fromIndex) ? toIndex + 1 : toIndex;
    if (!beginMoveRows(QModelIndex(), fromIndex, fromIndex, QModelIndex(), destination)) {
        return;
    }

    m_waypoints.move(fromIndex, toIndex);
    endMoveRows();

    emit waypointsChanged();
}

Waypoint WaypointModel::getWaypoint(int index) const {
    if (index < 0 || index >= m_waypoints.count())
        return Waypoint();
    return m_waypoints[index];
}

void WaypointModel::setWaypoints(const QList<Waypoint> &waypoints) {
    beginResetModel();
    m_waypoints = waypoints;
    endResetModel();

    emit waypointsChanged();
}
