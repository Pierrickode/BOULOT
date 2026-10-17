import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: hudRoot
    height: 100

    Rectangle {
        anchors.fill: parent
        color: "#1E1E1E"
        opacity: 0.9
        radius: 8

        RowLayout {
            anchors.fill: parent
            anchors.margins: 15
            spacing: 20

            // Speedometer, Heading & Altitude Widget
            ColumnLayout {
                spacing: 2
                Text {
                    text: Math.round(navCtrl.gpsProvider.currentSpeedKmh)
                    color: "#00E676"
                    font.pixelSize: 28
                    font.bold: true
                    Layout.alignment: Qt.AlignHCenter
                }
                Text {
                    text: "KM/H"
                    color: "#AAAAAAAA"
                    font.pixelSize: 10
                    font.bold: true
                    Layout.alignment: Qt.AlignHCenter
                }
                Text {
                    text: "Alt: " + Math.round(navCtrl.gpsProvider.currentAltitude) + " m"
                    color: "#00E5FF"
                    font.pixelSize: 11
                    font.bold: true
                    Layout.alignment: Qt.AlignHCenter
                }
                Text {
                    text: "Cap: " + Math.round(navCtrl.gpsProvider.currentHeading) + "°"
                    color: "#FFD600"
                    font.pixelSize: 11
                    font.bold: true
                    Layout.alignment: Qt.AlignHCenter
                }
            }

            Rectangle {
                width: 1
                Layout.fillHeight: true
                color: "#333333"
            }

            // Next Waypoint / Direction Widget
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 4

                RowLayout {
                    spacing: 8
                    Text {
                        text: navCtrl.langMgr.t("next_point")
                        color: "#888888"
                        font.pixelSize: 11
                        font.bold: true
                    }
                    Text {
                        text: navCtrl.nextWaypointName
                        color: "#FFFFFF"
                        font.pixelSize: 16
                        font.bold: true
                        elide: Text.ElideRight
                    }
                }

                RowLayout {
                    spacing: 15
                    Text {
                        text: navCtrl.langMgr.t("distance") + " " + navCtrl.formattedDistanceToNext
                        color: "#00B0FF"
                        font.pixelSize: 14
                        font.bold: true
                    }
                    Text {
                        text: navCtrl.langMgr.t("bearing") + " " + Math.round(navCtrl.bearingToNext) + "° (" + navCtrl.compassDirection + ")"
                        color: "#FFAB00"
                        font.pixelSize: 14
                        font.bold: true
                    }
                }
            }

            Rectangle {
                width: 1
                Layout.fillHeight: true
                color: "#333333"
            }

            // Total Distance & ETA
            ColumnLayout {
                spacing: 4

                Text {
                    text: navCtrl.langMgr.t("total_distance") + " " + navCtrl.formattedTotalDistance
                    color: "#FFFFFF"
                    font.pixelSize: 14
                }

                Text {
                    text: navCtrl.langMgr.t("eta") + " " + navCtrl.formattedETA
                    color: "#E0E0E0"
                    font.pixelSize: 14
                }
            }
        }
    }
}
