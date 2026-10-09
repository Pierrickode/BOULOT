#include "NavigationController.h"
#include "GpxHandler.h"
#include "RouteEngine.h"
#include <QUrl>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>

NavigationController::NavigationController(QObject *parent)
    : QObject(parent),
      m_waypointModel(new WaypointModel(this)),
      m_gpsProvider(new GpsProvider(this)),
      m_langMgr(new LanguageManager(this)) {

    connect(m_waypointModel, &WaypointModel::waypointsChanged, this, &NavigationController::updateRouteCalculations);
    connect(m_gpsProvider, &GpsProvider::positionChanged, this, &NavigationController::updateNavigationState);
    connect(m_gpsProvider, &GpsProvider::speedChanged, this, &NavigationController::updateNavigationState);

    // Initial demo route load
    loadDefaultDemoRoute();
}

QString NavigationController::formattedTotalDistance() const {
    return RouteEngine::formatDistance(m_totalDistanceMeters);
}

QString NavigationController::formattedETA() const {
    double speedKmh = m_gpsProvider->currentSpeedKmh();
    if (speedKmh < 5.0) speedKmh = 50.0; // Assume 50 km/h average if stationary
    double etaSeconds = RouteEngine::calculateETASeconds(m_totalDistanceMeters, speedKmh);
    return RouteEngine::formatDuration(etaSeconds);
}

QString NavigationController::formattedDistanceToNext() const {
    return RouteEngine::formatDistance(m_distanceToNextMeters);
}

QString NavigationController::compassDirection() const {
    return RouteEngine::compassDirection(m_bearingToNext);
}

QString NavigationController::tileCacheDirectory() const {
    QString cacheDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/tiles";
    QDir().mkpath(cacheDir);
    return cacheDir;
}

QString NavigationController::offlineMapDirectory() const {
    QString offlineDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/maps";
    QDir().mkpath(offlineDir);
    return offlineDir;
}

QVariantList NavigationController::roadPathCoordinates() const {
    QVariantList list;
    for (const auto &coord : m_roadPath) {
        list.append(QVariant::fromValue(coord));
    }
    return list;
}

bool NavigationController::loadGpxFile(const QString &filePath) {
    QString cleanPath = QUrl(filePath).isLocalFile() ? QUrl(filePath).toLocalFile() : filePath;
    QString errorMsg;
    QList<Waypoint> wpts = GpxHandler::loadFromGpx(cleanPath, &errorMsg);

    if (!errorMsg.isEmpty()) {
        emit statusMessage(errorMsg, true);
        return false;
    }

    if (wpts.isEmpty()) {
        emit statusMessage("Aucun point de passage trouvé dans le fichier GPX", true);
        return false;
    }

    m_waypointModel->setWaypoints(wpts);
    emit statusMessage(QString("%1 waypoints chargés avec succès").arg(wpts.count()), false);
    return true;
}

bool NavigationController::saveGpxFile(const QString &filePath) {
    QString cleanPath = QUrl(filePath).isLocalFile() ? QUrl(filePath).toLocalFile() : filePath;
    QString errorMsg;
    bool success = GpxHandler::saveToGpx(cleanPath, m_waypointModel->waypoints(), "Parcours AppliGPS", &errorMsg);

    if (!success) {
        emit statusMessage(errorMsg, true);
        return false;
    }

    emit statusMessage("Parcours exporté en GPX avec succès", false);
    return true;
}

void NavigationController::addWaypointByCoordinate(double lat, double lon, const QString &name) {
    QString wName = name.isEmpty() ? QString("Point %1").arg(m_waypointModel->rowCount() + 1) : name;
    m_waypointModel->addWaypoint(wName, lat, lon);
    emit statusMessage(QString("Point '%1' ajouté").arg(wName), false);
}

void NavigationController::startSimulation(double speedKmh) {
    if (m_waypointModel->rowCount() < 2) {
        emit statusMessage("Il faut au moins 2 points pour démarrer la navigation", true);
        return;
    }

    if (m_roadPath.count() >= 2) {
        m_gpsProvider->startSimulationCoordinates(m_roadPath, speedKmh);
    } else {
        m_gpsProvider->startSimulation(m_waypointModel->waypoints(), speedKmh);
    }
    emit statusMessage("Simulation GPS démarrée", false);
}

void NavigationController::loadDefaultDemoRoute() {
    m_waypointModel->clear();
    // Demo route in Paris / Île-de-France region
    m_waypointModel->addWaypoint("Paris Notre-Dame", 48.8530, 2.3499, 35.0, "Départ");
    m_waypointModel->addWaypoint("Château de Versailles", 48.8049, 2.1204, 130.0, "Étape 1");
    m_waypointModel->addWaypoint("Fontainebleau", 48.4022, 2.7006, 78.0, "Étape 2");
    m_waypointModel->addWaypoint("Aéroport Orly", 48.7262, 2.3652, 89.0, "Arrivée");
}

void NavigationController::updateRouteCalculations() {
    m_roadPath = RouteEngine::calculateShortestRoadRoute(m_waypointModel->waypoints());
    if (m_roadPath.count() >= 2) {
        m_totalDistanceMeters = RouteEngine::calculatePathDistanceMeters(m_roadPath);
    } else {
        m_totalDistanceMeters = RouteEngine::calculateTotalDistanceMeters(m_waypointModel->waypoints());
    }
    emit routeUpdated();
    updateNavigationState();
}

void NavigationController::updateNavigationState() {
    const auto &wpts = m_waypointModel->waypoints();
    if (wpts.isEmpty()) {
        m_nextWaypointName = "--";
        m_distanceToNextMeters = 0.0;
        m_bearingToNext = 0.0;
        emit navigationStateUpdated();
        return;
    }

    QGeoCoordinate currentPos = m_gpsProvider->currentPosition();
    m_nextWaypointIndex = RouteEngine::findNextWaypointIndex(currentPos, wpts);

    if (m_nextWaypointIndex >= 0 && m_nextWaypointIndex < wpts.count()) {
        const Waypoint &nextWpt = wpts[m_nextWaypointIndex];
        m_nextWaypointName = nextWpt.name;
        m_distanceToNextMeters = RouteEngine::calculateDistanceMeters(currentPos, nextWpt.coordinate());
        m_bearingToNext = RouteEngine::calculateBearingDegrees(currentPos, nextWpt.coordinate());
    } else {
        m_nextWaypointName = "Destination atteinte";
        m_distanceToNextMeters = 0.0;
        m_bearingToNext = 0.0;
    }

    emit navigationStateUpdated();
}
