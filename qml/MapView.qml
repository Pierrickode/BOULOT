import QtQuick 2.15
import QtQuick.Controls 2.15
import QtLocation 5.15
import QtPositioning 5.15

Item {
    id: mapViewRoot

    property alias map: map

    Map {
        id: map
        anchors.fill: parent

        plugin: Plugin {
            name: "osm"
            PluginParameter { name: "osm.mapping.providerserial"; value: "1" }
            PluginParameter { name: "osm.useragent"; value: "AppliGPS i.MX8 Automotive Navigation" }
            PluginParameter { name: "osm.mapping.cache.directory"; value: navCtrl.tileCacheDirectory }
            PluginParameter { name: "osm.mapping.cache.disk.size"; value: "100000000" }
            PluginParameter { name: "osm.mapping.offline.directory"; value: navCtrl.offlineMapDirectory }
        }

        center: navCtrl.gpsProvider.currentPosition
        zoomLevel: 12

        // Map Polyline route following roads
        MapPolyline {
            id: routePolyline
            line.width: 6
            line.color: "#00B0FF"

            function updatePath() {
                var roadCoords = navCtrl.roadPathCoordinates;
                if (roadCoords && roadCoords.length > 0) {
                    routePolyline.path = roadCoords;
                } else {
                    routePolyline.path = [];
                    for (var i = 0; i < navCtrl.waypointModel.rowCount(); i++) {
                        var wpt = navCtrl.waypointModel.getWaypoint(i);
                        routePolyline.addCoordinate(QtPositioning.coordinate(wpt.latitude, wpt.longitude));
                    }
                }
            }

            Component.onCompleted: updatePath()

            Connections {
                target: navCtrl
                function onRouteUpdated() {
                    routePolyline.updatePath();
                }
            }
        }

        // Topographic Contour Line Overlay (rendered when topographicMode is active)
        MapPolyline {
            id: contourPolyline1
            visible: topoBtn.topographicMode
            line.width: 2
            line.color: "#8B5A2B"
            path: [
                QtPositioning.coordinate(45.515, -73.590),
                QtPositioning.coordinate(45.510, -73.585),
                QtPositioning.coordinate(45.505, -73.580),
                QtPositioning.coordinate(45.500, -73.585),
                QtPositioning.coordinate(45.505, -73.595)
            ]
        }

        MapPolyline {
            id: contourPolyline2
            visible: topoBtn.topographicMode
            line.width: 2
            line.color: "#CD853F"
            path: [
                QtPositioning.coordinate(48.810, 2.110),
                QtPositioning.coordinate(48.805, 2.120),
                QtPositioning.coordinate(48.800, 2.130),
                QtPositioning.coordinate(48.795, 2.120),
                QtPositioning.coordinate(48.800, 2.110)
            ]
        }

        // Waypoint Markers
        MapItemView {
            model: navCtrl.waypointModel
            delegate: MapQuickItem {
                coordinate: QtPositioning.coordinate(latitude, longitude)
                anchorPoint.x: markerBubble.width / 2
                anchorPoint.y: markerBubble.height

                sourceItem: Rectangle {
                    id: markerBubble
                    width: Math.max(32, markerText.implicitWidth + 16)
                    height: 32
                    radius: 16
                    color: index === 0 ? "#27AE60" : (index === (navCtrl.waypointModel.rowCount() - 1) ? "#C0392B" : "#2980B9")
                    border.color: "#FFFFFF"
                    border.width: 2

                    Text {
                        id: markerText
                        anchors.centerIn: parent
                        text: (index + 1) + ". " + name
                        color: "#FFFFFF"
                        font.pixelSize: 12
                        font.bold: true
                    }
                }
            }
        }

        // Vehicle / Current Position Marker
        MapQuickItem {
            id: vehicleMarker
            coordinate: navCtrl.gpsProvider.currentPosition
            anchorPoint.x: vehicleIcon.width / 2
            anchorPoint.y: vehicleIcon.height / 2

            sourceItem: Item {
                id: vehicleIcon
                width: 40
                height: 40

                Rectangle {
                    anchors.fill: parent
                    radius: 20
                    color: "#E74C3C"
                    border.color: "#FFFFFF"
                    border.width: 3

                    transform: Rotation {
                        origin.x: 20
                        origin.y: 20
                        angle: navCtrl.gpsProvider.currentHeading
                    }

                    Rectangle {
                        width: 4
                        height: 12
                        color: "#FFFFFF"
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.top: parent.top
                        anchors.topMargin: 4
                    }
                }
            }
        }

        // Long press on map to add waypoint
        MouseArea {
            anchors.fill: parent
            preventStealing: false
            pressAndHoldInterval: 600

            onPressAndHold: function(mouse) {
                var coord = map.toCoordinate(Qt.point(mouse.x, mouse.y));
                if (coord.isValid) {
                    navCtrl.addWaypointByCoordinate(coord.latitude, coord.longitude, "");
                }
            }
        }
    }

    // Map controls overlay (Zoom In / Out / Recenter / Topographic Mode)
    Column {
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 20
        spacing: 10

        Button {
            id: topoBtn
            width: 48
            height: 48
            text: "⛰️"
            font.pixelSize: 18
            highlighted: topographicMode
            property bool topographicMode: false
            onClicked: {
                topographicMode = !topographicMode;
                if (topographicMode) {
                    navCtrl.statusMessage("Mode Topographique & Lignes de Niveau activé", false);
                } else {
                    navCtrl.statusMessage("Mode Carte standard activé", false);
                }
            }
        }

        Button {
            width: 48
            height: 48
            text: "+"
            font.pixelSize: 24
            font.bold: true
            onClicked: map.zoomLevel = Math.min(map.maximumZoomLevel, map.zoomLevel + 1)
        }

        Button {
            width: 48
            height: 48
            text: "-"
            font.pixelSize: 24
            font.bold: true
            onClicked: map.zoomLevel = Math.max(map.minimumZoomLevel, map.zoomLevel - 1)
        }

        Button {
            width: 48
            height: 48
            text: "🎯"
            font.pixelSize: 20
            onClicked: map.center = navCtrl.gpsProvider.currentPosition
        }
    }
}
