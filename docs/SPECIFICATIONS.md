# Spécifications Techniques et Architecture - AppliGPS

## 1. Présentation du Projet
**AppliGPS** est une application de navigation GPS embarquée, conçue spécifiquement pour les systèmes sur puce (SoC) de la famille **NXP i.MX8** fonctionnant sous **Yocto Linux**.
L'application s'appuie sur le framework **Qt 5 / Qt Quick (QML)** et utilise des données cartographiques Open Source issues de **OpenStreetMap (OSM)** via l'API `Qt Location`.

AppliGPS permet la gestion complète de points de navigation (waypoints), le calcul de parcours géodésiques, l'import/export de fichiers GPX 1.1, la simulation de parcours GPS, ainsi que l'intégration fluide dans des environnements graphiques embarqués tels que Wayland / Weston.

---

## 2. Architecture Système & Environnement Cible

### 2.1 Cible Matérielle & Système
* **SoC Cible** : NXP i.MX8M / i.MX8M Mini / i.MX8QuadMax (Architecture 64-bit ARMv8-A - `aarch64`).
* **Système d'Exploitation** : Yocto Linux (distributions type NXP Linux BSP, Poky / Kirkstone / Scarthgap).
* **Serveur d'Affichage** : Wayland avec le compositeur Weston (Qt Wayland platform backend: `-platform wayland` ou `-platform eglfs`).
* **Ecran Cible** : Résolutions tactiles embarquées (ex. 800x480, 1024x600, 1280x800, 1920x1080).

### 2.2 Chaîne de Compilation & Outils
* **Langage C++** : C++17
* **Framework Qt** : Qt 5.15+ (Qt Core, Qt GUI, Qt Quick, Qt Positioning, Qt Location, Qt Network, Qt Test).
* **Système de Build** : CMake (>= 3.16) avec fichier toolchain dédié (`cmake/aarch64-toolchain.cmake`)
* **Toolchain de Cross-Compilation** : `aarch64-linux-gnu-g++` / `aarch64-poky-linux-g++`

### 2.3 Méthodes de Cross-Compilation
1. **Via CMake et Toolchain File** :
   ```bash
   # Si Qt5Config.cmake n'est pas dans le chemin système standard, spécifiez CMAKE_PREFIX_PATH ou Qt5_DIR :
   cmake -B build_aarch64 \
         -DCMAKE_TOOLCHAIN_FILE=cmake/aarch64-toolchain.cmake \
         -DCMAKE_PREFIX_PATH=/usr/lib/aarch64-linux-gnu/cmake
   cmake --build build_aarch64 -j$(nproc)
   ```
2. **Via Yocto eSDK** :
   ```bash
   source /opt/poky/3.1/environment-setup-aarch64-poky-linux
   cmake -B build_yocto -DCMAKE_TOOLCHAIN_FILE=$OECORE_NATIVE_SYSROOT/usr/share/cmake/OECMAKE_TARGET_TOOLCHAIN_FILE
   cmake --build build_yocto -j$(nproc)
   ```
3. **Via Recette BitBake (`meta-appligps`)** :
   ```bash
   bitbake appligps
   ```

---

## 3. Architecture Logicielle & Composants C++

L'application suit le pattern architectural Model-View-Controller (MVC) adaptés à Qt Quick / QML.

```
+-----------------------------------------------------------------+
|                        IHM Qt Quick / QML                       |
|   (MapView.qml, WaypointPanel.qml, NavigationHUD.qml, main.qml) |
+-----------------------------------------------------------------+
                                |
                                v (Q_PROPERTY / Signals / Slots)
+-----------------------------------------------------------------+
|                    NavigationController (C++)                   |
+-----------------------------------------------------------------+
      |                        |                         |
      v                        v                         v
+--------------+    +--------------------+    +--------------------+
| WaypointModel|    |    RouteEngine     |    |    GpsProvider     |
| (QAbstract-  |    | (Calculs Haversine,|    | (NMEA / Simulation |
|  ListModel)  |    |  Cap, Distance)    |    |  de déplacement)   |
+--------------+    +--------------------+    +--------------------+
      |                          ^
      v                          |
+----------------------------------------+
|          GpxHandler (Parser)           |
+----------------------------------------+
```

### 3.1 Description des Classes C++ Principales
1. **`Waypoint`** : Structure représentant un point de passage (`id`, `name`, `latitude`, `longitude`, `elevation`, `description`).
2. **`WaypointModel`** : Hérite de `QAbstractListModel`. Gère la liste ordonnée des waypoints, l'ajout, la suppression, la modification et le réordonnancement drag-and-drop.
3. **`GpxHandler`** : Module de lecture et écriture au format standard XML GPX 1.1 (`<wpt>`, `<rte>`, `<trk>`).
4. **`RouteEngine`** : Moteur mathématique calculant :
   * La distance géodésique totale (Formule de Haversine).
   * Les caps (bearings) entre waypoints consécutifs.
   * L'estimation du temps de trajet (ETA) basée sur la vitesse moyenne configurée.
   * La détection d'arrivée au waypoint le plus proche.
5. **`GpsProvider`** : Gestionnaire de positionnement.
   * Mode Réel : Reçoit les coordonnées du matériel via `QGeoPositionInfoSource`.
   * Mode Simulation : Rejoue une trace GPX ou génère un déplacement fluide entre les waypoints définis.
6. **`NavigationController`** : Contrôleur principal exposé au QML regroupant les modèles, le statut de navigation, la vitesse actuelle et la simulation.

---

## 4. Spécifications de l'Interface Utilisateur (IHM QML)

L'interface est optimisée pour une utilisation sur écran tactile d'automobile ou de véhicule industriel :

1. **Carte Interactive (`MapView.qml`)** :
   * Cartographie vectorielle/raster fournie par **OpenStreetMap (OSM)** via le plugin `osm`.
   * Contrôles tactiles : Zoom, déplacement (pan), centrage automatique sur le véhicule.
   * Affichage dynamique des repères (markers) pour chaque waypoint.
   * Affichage de la ligne du parcours (Polyline) reliant les waypoints dans l'ordre de navigation.

2. **Panneau de Gestion des Waypoints (`WaypointPanel.qml`)** :
   * Liste déroulante des waypoints avec leurs coordonnées et distances relatives.
   * Boutons d'action : Ajouter un point (manuellement ou par clic sur la carte), Supprimer, Supprimer tout.
   * Fonctionnalité Import GPX et Export GPX.

3. **HUD de Navigation / Tableau de Bord (`NavigationHUD.qml`)** :
   * Indicateur de vitesse instantanée (km/h).
   * Boussole / Cap de navigation (degrés et orientation N/E/S/O).
   * Distance restante jusqu'au prochain waypoint et jusqu'à la destination finale.
   * Temps estimé d'arrivée (ETA).

4. **Barre de Simulation (`SimulationBar.qml`)** :
   * Boutons Lecture / Pause / Stop pour simuler le trajet.
   * Sélecteur de vitesse de simulation (1x, 2x, 5x, 10x).

---

## 5. Intégration Yocto (`meta-appligps`)

Pour une intégration native dans une image Yocto Linux pour i.MX8, une couche BitBake dédiée est fournie :

### 5.1 Arborescence du Layer Yocto
```
meta-appligps/
├── conf/
│   └── layer.conf
├── recipes-apps/
│   └── appligps/
│       ├── appligps_1.0.bb
│       └── files/
│           └── appligps.service
```

### 5.2 Dépendances du Recipe BitBake (`appligps_1.0.bb`)
* `qtbase` (avec support EGL / OpenGL ES2)
* `qtdeclarative`
* `qtlocation`
* `qtpositioning`
* `qtquickcontrols2`

---

## 6. Spécifications des Fichiers Compilés et Artifacts

| Fichier / Artifact | Cible | Description |
|---|---|---|
| `bin/AppliGPS-imx8-aarch64` | Executable ARM64 (i.MX8) | Binaire compilé avec la toolchain cross `aarch64-linux-gnu-g++` |
| `build/AppliGPS` | Executable Host (x86_64) | Binaire compilé pour la vérification et les tests locaux |
| `build/AppliGPS_tests` | Executable Tests (x86_64) | Suite de tests unitaires automatisés (QtTest) |
| `meta-appligps/` | Layer Yocto BitBake | Recette et configuration pour l'intégration d'image OS i.MX8 |

---

## 7. Directives de Validation et Tests Unitaires

La suite de tests unitaires valide :
1. **Model Tests** : Insertion, modification et réordonnancement de waypoints.
2. **GPX Tests** : Parsing exact de fichiers GPX de test et génération valide conforme au schéma XML GPX 1.1.
3. **GeoMath Tests** : Exactitude de la formule de Haversine et des calculs de cap par rapport à des paires de coordonnées GPS connues (ex: Paris -> Lyon, Londres -> Paris).
