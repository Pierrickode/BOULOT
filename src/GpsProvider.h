#ifndef GPSPROVIDER_H
#define GPSPROVIDER_H

#include <QObject>
#include <QGeoCoordinate>
#include <QGeoPositionInfoSource>
#include <QTimer>
#include "Waypoint.h"

class GpsProvider : public QObject {
    Q_OBJECT
    Q_PROPERTY(QGeoCoordinate currentPosition READ currentPosition NOTIFY positionChanged)
    Q_PROPERTY(double currentSpeedKmh READ currentSpeedKmh NOTIFY speedChanged)
    Q_PROPERTY(double currentHeading READ currentHeading NOTIFY headingChanged)
    Q_PROPERTY(double currentAltitude READ currentAltitude NOTIFY altitudeChanged)
    Q_PROPERTY(bool isSimulating READ isSimulating NOTIFY simulationStateChanged)

public:
    explicit GpsProvider(QObject *parent = nullptr);
    ~GpsProvider();

    QGeoCoordinate currentPosition() const { return m_currentPosition; }
    double currentSpeedKmh() const { return m_currentSpeedKmh; }
    double currentHeading() const { return m_currentHeading; }
    double currentAltitude() const { return m_currentAltitude; }
    bool isSimulating() const { return m_isSimulating; }

    Q_INVOKABLE void startLivePositioning();
    Q_INVOKABLE void startSimulation(const QList<Waypoint> &route, double speedKmh = 60.0);
    Q_INVOKABLE void startSimulationCoordinates(const QList<QGeoCoordinate> &pathCoords, double speedKmh = 60.0);
    Q_INVOKABLE void pauseSimulation();
    Q_INVOKABLE void stopSimulation();
    Q_INVOKABLE void setSimulationSpeed(double speedMultiplier);
    Q_INVOKABLE void setManualPosition(double lat, double lon, double heading = 0.0, double speedKmh = 0.0);

signals:
    void positionChanged(const QGeoCoordinate &position);
    void speedChanged(double speedKmh);
    void headingChanged(double heading);
    void altitudeChanged(double altitude);
    void simulationStateChanged(bool isSimulating);
    void simulationFinished();

private slots:
    void onPositionUpdated(const QGeoPositionInfo &info);
    void onSimulationStep();

private:
    QGeoCoordinate m_currentPosition{48.8566, 2.3522, 35.0}; // Paris default with 35m altitude
    double m_currentSpeedKmh{0.0};
    double m_currentHeading{0.0};
    double m_currentAltitude{35.0};
    bool m_isSimulating{false};

    QGeoPositionInfoSource *m_posSource{nullptr};

    // Simulation variables
    QTimer *m_simTimer{nullptr};
    QList<Waypoint> m_simRoute;
    QList<QGeoCoordinate> m_simCoords;
    int m_simCurrentSegment{0};
    double m_simProgressInSegment{0.0}; // 0.0 to 1.0
    double m_simBaseSpeedKmh{60.0};
    double m_simMultiplier{1.0};
};

#endif // GPSPROVIDER_H
