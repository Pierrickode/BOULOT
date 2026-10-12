#include "LocationSearchManager.h"
#include <qmath.h>

LocationSearchManager::LocationSearchManager(QObject *parent) : QObject(parent) {
    initLocalitiesDatabase();
}

void LocationSearchManager::initLocalitiesDatabase() {
    m_database = {
        // Paris & Île-de-France
        {"Paris Notre-Dame", "Paris, Île-de-France", 48.8530, 2.3499},
        {"Paris Tour Eiffel", "Paris, Île-de-France", 48.8584, 2.2945},
        {"Paris Gare du Nord", "Paris, Île-de-France", 48.8809, 2.3553},
        {"Château de Versailles", "Versailles, Yvelines", 48.8049, 2.1204},
        {"Fontainebleau", "Seine-et-Marne", 48.4022, 2.7006},
        {"Aéroport Orly", "Val-de-Marne", 48.7262, 2.3652},
        {"Aéroport Roissy CDG", "Val-d'Oise", 49.0097, 2.5479},
        {"Boulogne-Billancourt", "Hauts-de-Seine", 48.8333, 2.2500},
        {"Saint-Denis", "Seine-Saint-Denis", 48.9356, 2.3539},
        {"Rambouillet", "Yvelines", 48.6440, 1.8320},
        {"Massy-Palaiseau", "Essonne", 48.7250, 2.2720},
        {"Melun", "Seine-et-Marne", 48.5393, 2.6611},
        {"Evry-Courcouronnes", "Essonne", 48.6238, 2.4439},
        {"Creteil", "Val-de-Marne", 48.7833, 2.4667},
        {"Nanterre", "Hauts-de-Seine", 48.8924, 2.2071},

        // Major French Cities
        {"Lyon Centre", "Rhône, Auvergne-Rhône-Alpes", 45.7640, 4.8357},
        {"Marseille Vieux-Port", "Bouches-du-Rhône, PACA", 43.2965, 5.3698},
        {"Toulouse Capitole", "Haute-Garonne, Occitanie", 43.6047, 1.4442},
        {"Nice Promenade", "Alpes-Maritimes, PACA", 43.7102, 7.2620},
        {"Nantes Centre", "Loire-Atlantique, Pays de la Loire", 47.2184, -1.5536},
        {"Strasbourg Cathédrale", "Bas-Rhin, Grand Est", 48.5734, 7.7521},
        {"Bordeaux Centre", "Gironde, Nouvelle-Aquitaine", 44.8378, -0.5792},
        {"Lille Grand Place", "Nord, Hauts-de-France", 50.6292, 3.0573},
        {"Rennes Centre", "Ille-et-Vilaine, Bretagne", 48.1173, -1.6778}
    };
}

QVariantList LocationSearchManager::searchLocalities(const QString &query, int maxResults) const {
    QVariantList results;
    QString q = query.trimmed().toLower();
    if (q.isEmpty()) {
        return results;
    }

    int count = 0;
    for (const auto &item : m_database) {
        if (item.name.toLower().contains(q) || item.region.toLower().contains(q)) {
            QVariantMap map;
            map["name"] = item.name;
            map["region"] = item.region;
            map["latitude"] = item.latitude;
            map["longitude"] = item.longitude;
            results.append(map);

            count++;
            if (count >= maxResults) break;
        }
    }

    return results;
}

QVariantMap LocationSearchManager::getLocality(int index) const {
    QVariantMap map;
    if (index >= 0 && index < m_database.count()) {
        const auto &item = m_database[index];
        map["name"] = item.name;
        map["region"] = item.region;
        map["latitude"] = item.latitude;
        map["longitude"] = item.longitude;
    }
    return map;
}
