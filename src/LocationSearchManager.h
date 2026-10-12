#ifndef LOCATIONSEARCHMANAGER_H
#define LOCATIONSEARCHMANAGER_H

#include <QObject>
#include <QVariantList>
#include <QVariantMap>
#include <QGeoCoordinate>
#include <QList>
#include <QString>

struct LocalityItem {
    QString name;
    QString region;
    double latitude;
    double longitude;
};

class LocationSearchManager : public QObject {
    Q_OBJECT

public:
    explicit LocationSearchManager(QObject *parent = nullptr);

    Q_INVOKABLE QVariantList searchLocalities(const QString &query, int maxResults = 8) const;
    Q_INVOKABLE QVariantMap getLocality(int index) const;

private:
    void initLocalitiesDatabase();
    QList<LocalityItem> m_database;
};

#endif // LOCATIONSEARCHMANAGER_H
