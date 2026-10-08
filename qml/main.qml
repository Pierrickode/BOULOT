import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Window {
    id: mainWindow
    visible: true
    width: 1024
    height: 600
    title: "AppliGPS - Navigation i.MX8 Yocto"

    Rectangle {
        anchors.fill: parent
        color: "#121212"

        RowLayout {
            anchors.fill: parent
            spacing: 0

            // Left side: Interactive Map + HUD + Simulation Bar
            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true

                MapView {
                    id: mapView
                    anchors.fill: parent
                }

                NavigationHUD {
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.margins: 15
                }

                SimulationBar {
                    anchors.bottom: parent.bottom
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.margins: 15
                }
            }

            // Right side: Waypoint Management Drawer / Panel
            WaypointPanel {
                Layout.preferredWidth: 320
                Layout.fillHeight: true
            }
        }

        // Status Toast Notification
        Rectangle {
            id: statusToast
            width: statusText.implicitWidth + 30
            height: 36
            radius: 18
            color: "#323232"
            opacity: 0.0
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 80

            Behavior on opacity { NumberAnimation { duration: 250 } }

            Text {
                id: statusText
                anchors.centerIn: parent
                color: "#FFFFFF"
                font.pixelSize: 13
            }

            Timer {
                id: statusTimer
                interval: 3000
                onTriggered: statusToast.opacity = 0.0
            }
        }

        Connections {
            target: navCtrl
            function onStatusMessage(msg, isError) {
                statusText.text = msg;
                statusToast.color = isError ? "#D32F2F" : "#388E3C";
                statusToast.opacity = 0.9;
                statusTimer.restart();
            }
        }
    }
}
