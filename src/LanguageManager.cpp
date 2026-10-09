#include "LanguageManager.h"
#include <QDebug>

LanguageManager::LanguageManager(QObject *parent) : QObject(parent) {
    initTranslations();
}

void LanguageManager::setCurrentLanguage(const QString &langCode) {
    if (m_currentLanguage != langCode && m_translations.contains(langCode)) {
        m_currentLanguage = langCode;
        emit currentLanguageChanged();
    }
}

int LanguageManager::currentLanguageIndex() const {
    if (m_currentLanguage == "fr") return 0;
    if (m_currentLanguage == "en") return 1;
    if (m_currentLanguage == "ar") return 2;
    if (m_currentLanguage == "nl") return 3;
    return 0;
}

QString LanguageManager::tr(const QString &key) const {
    if (m_translations.contains(m_currentLanguage) && m_translations[m_currentLanguage].contains(key)) {
        return m_translations[m_currentLanguage][key];
    }
    // Fallback to French or Key
    if (m_translations["fr"].contains(key)) {
        return m_translations["fr"][key];
    }
    return key;
}

void LanguageManager::initTranslations() {
    // French (fr)
    auto &fr = m_translations["fr"];
    fr["app_title"] = "AppliGPS - Navigation i.MX8 Yocto";
    fr["waypoints_title"] = "Points de Navigation";
    fr["next_point"] = "PROCHAIN POINT:";
    fr["speed"] = "KM/H";
    fr["total_distance"] = "Total:";
    fr["eta"] = "ETA:";
    fr["distance"] = "Distance:";
    fr["bearing"] = "Cap:";
    fr["simulation_gps"] = "Simulation GPS:";
    fr["start"] = "Démarrer";
    fr["pause_play"] = "Pause / Play";
    fr["stop"] = "Stopper";
    fr["speed_label"] = "Vitesse:";
    fr["demo_route"] = "Démo Route";
    fr["clear_all"] = "Effacer Tout";
    fr["import_gpx"] = "Importer GPX";
    fr["export_gpx"] = "Exporter GPX";
    fr["language"] = "Langue:";
    fr["destination_reached"] = "Destination atteinte";
    fr["long_press_add"] = "Appuyez longuement sur la carte pour ajouter un point";
    fr["shortest_road_route"] = "Mode: Court par Route";

    // English (en)
    auto &en = m_translations["en"];
    en["app_title"] = "AppliGPS - i.MX8 Yocto Navigation";
    en["waypoints_title"] = "Navigation Waypoints";
    en["next_point"] = "NEXT WAYPOINT:";
    en["speed"] = "KM/H";
    en["total_distance"] = "Total:";
    en["eta"] = "ETA:";
    en["distance"] = "Distance:";
    en["bearing"] = "Bearing:";
    en["simulation_gps"] = "GPS Simulation:";
    en["start"] = "Start";
    en["pause_play"] = "Pause / Play";
    en["stop"] = "Stop";
    en["speed_label"] = "Speed:";
    en["demo_route"] = "Demo Route";
    en["clear_all"] = "Clear All";
    en["import_gpx"] = "Import GPX";
    en["export_gpx"] = "Export GPX";
    en["language"] = "Language:";
    en["destination_reached"] = "Destination reached";
    en["long_press_add"] = "Long press on map to add waypoint";
    en["shortest_road_route"] = "Mode: Shortest Road Route";

    // Arabic (ar)
    auto &ar = m_translations["ar"];
    ar["app_title"] = "AppliGPS - نظام الملاحة i.MX8";
    ar["waypoints_title"] = "نقاط الملاحة";
    ar["next_point"] = "النقطة التالية:";
    ar["speed"] = "كم/س";
    ar["total_distance"] = "الإجمالي:";
    ar["eta"] = "الوصول:";
    ar["distance"] = "المسافة:";
    ar["bearing"] = "الاتجاه:";
    ar["simulation_gps"] = "محاكاة GPS:";
    ar["start"] = "بدء";
    ar["pause_play"] = "إيقاف / تشغيل";
    ar["stop"] = "إيقاف";
    ar["speed_label"] = "السرعة:";
    ar["demo_route"] = "مسار تجريبي";
    ar["clear_all"] = "مسح الكل";
    ar["import_gpx"] = "استيراد GPX";
    ar["export_gpx"] = "تصدير GPX";
    ar["language"] = "اللغة:";
    ar["destination_reached"] = "تم الوصول إلى الوجهة";
    ar["long_press_add"] = "اضغط مطولاً على الخريطة لإضافة نقطة";
    ar["shortest_road_route"] = "المسار الأقصر عبر الطرق";

    // Dutch (nl)
    auto &nl = m_translations["nl"];
    nl["app_title"] = "AppliGPS - i.MX8 Yocto Navigatie";
    nl["waypoints_title"] = "Navigatiepunten";
    nl["next_point"] = "VOLGEND PUNT:";
    nl["speed"] = "KM/U";
    nl["total_distance"] = "Totaal:";
    nl["eta"] = "ETA:";
    nl["distance"] = "Afstand:";
    nl["bearing"] = "Koers:";
    nl["simulation_gps"] = "GPS Simulatie:";
    nl["start"] = "Starten";
    nl["pause_play"] = "Pauze / Play";
    nl["stop"] = "Stoppen";
    nl["speed_label"] = "Snelheid:";
    nl["demo_route"] = "Demo Route";
    nl["clear_all"] = "Alles Wissen";
    nl["import_gpx"] = "Importeren GPX";
    nl["export_gpx"] = "Exporteren GPX";
    nl["language"] = "Taal:";
    nl["destination_reached"] = "Bestemming bereikt";
    nl["long_press_add"] = "Druk lang op de kaart om een punt toe te voegen";
    nl["shortest_road_route"] = "Route: Kortste Weg";
}
