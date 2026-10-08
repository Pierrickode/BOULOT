#include "GpsProvider.h"
#include "RouteEngine.h"
#include <QDebug>

GpsProvider::GpsProvider(QObject *parent) : QObject(parent) {
    m_simTimer = new QTimer(this);
    m_simTimer->setInterval(200); // 5 Hz update rate for fluid animation
    connect(m_simTimer, &QTimer::timeout, this, &GpsProvider::onSimulationStep);
}

GpsProvider::~GpsProvider() {
    if (m_posSource) {
        m_posSource->stopUpdates();
    }
}

void GpsProvider::startLivePositioning() {
    stopSimulation();

    if (!m_posSource) {
        m_posSource = QGeoPositionInfoSource::createDefaultSource(this);
        if (m_posSource) {
            connect(m_posSource, &QGeoPositionInfoSource::positionUpdated, this, &GpsProvider::onPositionUpdated);
            m_posSource->setUpdateInterval(1000);
        }
    }

    if (m_posSource) {
        m_posSource->startUpdates();
    }
}

void GpsProvider::startSimulation(const QList<Waypoint> &route, double speedKmh) {
    if (route.count() < 2) {
        qWarning() << "Cannot start simulation with fewer than 2 waypoints";
        return;
    }

    if (m_posSource) {
        m_posSource->stopUpdates();
    }

    m_simRoute = route;
    m_simBaseSpeedKmh = speedKmh;
    m_simCurrentSegment = 0;
    m_simProgressInSegment = 0.0;

    m_currentPosition = route.first().coordinate();
    m_currentSpeedKmh = speedKmh * m_simMultiplier;
    m_currentHeading = RouteEngine::calculateBearingDegrees(route[0].coordinate(), route[1].coordinate());

    emit positionChanged(m_currentPosition);
    emit speedChanged(m_currentSpeedKmh);
    emit headingChanged(m_currentHeading);

    m_isSimulating = true;
    emit simulationStateChanged(true);

    m_simTimer->start();
}

void GpsProvider::pauseSimulation() {
    if (m_simTimer->isActive()) {
        m_simTimer->stop();
        m_currentSpeedKmh = 0.0;
        emit speedChanged(0.0);
    } else if (m_isSimulating) {
        m_simTimer->start();
        m_currentSpeedKmh = m_simBaseSpeedKmh * m_simMultiplier;
        emit speedChanged(m_currentSpeedKmh);
    }
}

void GpsProvider::stopSimulation() {
    if (m_simTimer->isActive()) {
        m_simTimer->stop();
    }
    if (m_isSimulating) {
        m_isSimulating = false;
        m_currentSpeedKmh = 0.0;
        emit speedChanged(0.0);
        emit simulationStateChanged(false);
    }
}

void GpsProvider::setSimulationSpeed(double speedMultiplier) {
    if (speedMultiplier > 0.1) {
        m_simMultiplier = speedMultiplier;
        if (m_isSimulating && m_simTimer->isActive()) {
            m_currentSpeedKmh = m_simBaseSpeedKmh * m_simMultiplier;
            emit speedChanged(m_currentSpeedKmh);
        }
    }
}

void GpsProvider::setManualPosition(double lat, double lon, double heading, double speedKmh) {
    m_currentPosition = QGeoCoordinate(lat, lon);
    m_currentHeading = heading;
    m_currentSpeedKmh = speedKmh;

    emit positionChanged(m_currentPosition);
    emit headingChanged(m_currentHeading);
    emit speedChanged(m_currentSpeedKmh);
}

void GpsProvider::onPositionUpdated(const QGeoPositionInfo &info) {
    if (!m_isSimulating && info.isValid()) {
        m_currentPosition = info.coordinate();
        emit positionChanged(m_currentPosition);

        if (info.hasAttribute(QGeoPositionInfo::GroundSpeed)) {
            m_currentSpeedKmh = info.attribute(QGeoPositionInfo::GroundSpeed) * 3.6;
            emit speedChanged(m_currentSpeedKmh);
        }

        if (info.hasAttribute(QGeoPositionInfo::Direction)) {
            m_currentHeading = info.attribute(QGeoPositionInfo::Direction);
            emit headingChanged(m_currentHeading);
        }
    }
}

void GpsProvider::onSimulationStep() {
    if (!m_isSimulating || m_simRoute.count() < 2) return;

    if (m_simCurrentSegment >= m_simRoute.count() - 1) {
        stopSimulation();
        emit simulationFinished();
        return;
    }

    const QGeoCoordinate &p1 = m_simRoute[m_simCurrentSegment].coordinate();
    const QGeoCoordinate &p2 = m_simRoute[m_simCurrentSegment + 1].coordinate();

    double segmentDistMeters = RouteEngine::calculateDistanceMeters(p1, p2);
    if (segmentDistMeters <= 1.0) {
        m_simCurrentSegment++;
        m_simProgressInSegment = 0.0;
        onSimulationStep();
        return;
    }

    double currentSpeedMps = (m_simBaseSpeedKmh * m_simMultiplier) / 3.6;
    double deltaSeconds = m_simTimer->interval() / 1000.0;
    double stepMeters = currentSpeedMps * deltaSeconds;

    m_simProgressInSegment += (stepMeters / segmentDistMeters);

    if (m_simProgressInSegment >= 1.0) {
        m_simCurrentSegment++;
        m_simProgressInSegment = 0.0;
        if (m_simCurrentSegment >= m_simRoute.count() - 1) {
            m_currentPosition = m_simRoute.last().coordinate();
            emit positionChanged(m_currentPosition);
            stopSimulation();
            emit simulationFinished();
            return;
        }
    }

    const QGeoCoordinate &start = m_simRoute[m_simCurrentSegment].coordinate();
    const QGeoCoordinate &end = m_simRoute[m_simCurrentSegment + 1].coordinate();

    double curLat = start.latitude() + (end.latitude() - start.latitude()) * m_simProgressInSegment;
    double curLon = start.longitude() + (end.longitude() - start.longitude()) * m_simProgressInSegment;

    m_currentPosition = QGeoCoordinate(curLat, curLon);
    m_currentHeading = RouteEngine::calculateBearingDegrees(start, end);
    m_currentSpeedKmh = m_simBaseSpeedKmh * m_simMultiplier;

    emit positionChanged(m_currentPosition);
    emit headingChanged(m_currentHeading);
    emit speedChanged(m_currentSpeedKmh);
}
