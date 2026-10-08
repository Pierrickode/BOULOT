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
            PluginParameter { name: "osm.mapping.providerserial", value: "1" }
            PluginParameter { name: "osm.useragent", value: "AppliGPS i.MX8 Automotive Navigation" }
        }

        center: navCtrl.gpsProvider.currentPosition
        zoomLevel: 12

        // Map Polyline route connecting all waypoints
        MapPolyline {
            id: routePolyline
            line.width: 5
            line.color: "#007ACC"
            path: {
                var coords = [];
                for (var i = 0; i < navCtrl.waypointModel.rowCount(); i++) {
                    var wpt = navCtrl.waypointModel.getWaypoint(i);
                    coords.push(QtPositioning.coordinate(wpt.latitude, wpt.longitude));
                }
                return coords;
            }
        }

        // Waypoint Markers
        MapItemView {
            model: navCtrl.waypointModel
            delegate: MapQuickItem {
                coordinate: QtPositioning.coordinate(model.latitude, model.longitude)
                anchorPoint.x: markerBubble.width / 2
                anchorPoint.y: markerBubble.height

                sourceItem: Rectangle {
                    id: markerBubble
                    width: Math.max(32, markerText.implicitWidth + 16)
                    height: 32
                    radius: 16
                    color: index === 0 ? "#27AE60" : (index === navCtrl.waypointModel.rowCount() - 1 ? "#C0392B" : "#2980B9")
                    border.color: "#FFFFFF"
                    border.width: 2

                    Text {
                        id: markerText
                        anchors.centerIn: parent
                        text: (index + 1) + ". " + model.name
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

        // Tap on map to add waypoint
        MouseArea {
            anchors.fill: parent
            onDoubleClicked: (mouse) => {
                var coord = map.toCoordinate(Qt.point(mouse.x, mouse.y));
                navCtrl.addWaypointByCoordinate(coord.latitude, coord.longitude, "");
            }
        }
    }

    // Map controls overlay (Zoom In / Out / Recenter)
    Column {
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 20
        spacing: 10

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
