import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs 1.3

Rectangle {
    id: waypointPanelRoot
    color: "#252526"

    FileDialog {
        id: importGpxDialog
        title: navCtrl.langMgr.t("import_gpx")
        nameFilters: ["Fichiers GPX (*.gpx)", "Tous les fichiers (*)"]
        selectExisting: true
        onAccepted: {
            navCtrl.loadGpxFile(importGpxDialog.fileUrl)
        }
    }

    FileDialog {
        id: exportGpxDialog
        title: navCtrl.langMgr.t("export_gpx")
        nameFilters: ["Fichiers GPX (*.gpx)"]
        selectExisting: false
        onAccepted: {
            navCtrl.saveGpxFile(exportGpxDialog.fileUrl)
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 10

        // Language Selector Row
        RowLayout {
            Layout.fillWidth: true
            spacing: 5

            Text {
                text: navCtrl.langMgr.t("language")
                color: "#CCCCCC"
                font.pixelSize: 13
                font.bold: true
            }

            ComboBox {
                Layout.fillWidth: true
                model: navCtrl.langMgr.languageNames
                currentIndex: navCtrl.langMgr.currentLanguageIndex
                onActivated: {
                    var codes = ["fr", "en", "ar", "nl"]
                    navCtrl.langMgr.setLanguage(codes[index])
                }
            }
        }

        Text {
            text: navCtrl.langMgr.t("waypoints_title")
            color: "#FFFFFF"
            font.pixelSize: 18
            font.bold: true
            Layout.alignment: Qt.AlignHCenter
        }

        // Locality Search Input
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 2

            TextField {
                id: searchInput
                Layout.fillWidth: true
                placeholderText: navCtrl.langMgr.t("search_placeholder")
                color: "#FFFFFF"
                background: Rectangle {
                    color: "#333337"
                    radius: 4
                    border.color: searchInput.activeFocus ? "#3498DB" : "#555555"
                }
                onTextChanged: {
                    searchResultsList.model = navCtrl.searchMgr.searchLocalities(text, 6)
                }
            }

            // Search Results Popup Overlay
            ListView {
                id: searchResultsList
                Layout.fillWidth: true
                implicitHeight: Math.min(contentHeight, 150)
                visible: searchInput.text.length > 0 && count > 0
                clip: true
                spacing: 2

                delegate: Rectangle {
                    width: searchResultsList.width
                    height: 36
                    color: searchMouseArea.containsMouse ? "#34495E" : "#2C3E50"
                    radius: 4

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 6

                        Text {
                            text: modelData.name + " (" + modelData.region + ")"
                            color: "#FFFFFF"
                            font.pixelSize: 12
                            font.bold: true
                            Layout.fillWidth: true
                            elide: Text.ElideRight
                        }

                        Text {
                            text: "+ Ajouter"
                            color: "#2ECC71"
                            font.pixelSize: 11
                            font.bold: true
                        }
                    }

                    MouseArea {
                        id: searchMouseArea
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: {
                            navCtrl.addWaypointByCoordinate(modelData.latitude, modelData.longitude, modelData.name)
                            searchInput.text = ""
                        }
                    }
                }
            }
        }

        ListView {
            id: listView
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: navCtrl.waypointModel
            spacing: 6

            delegate: Rectangle {
                width: listView.width
                height: 54
                color: "#333337"
                radius: 6

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 8
                    spacing: 10

                    Rectangle {
                        width: 28
                        height: 28
                        radius: 14
                        color: index === 0 ? "#27AE60" : (index === navCtrl.waypointModel.rowCount() - 1 ? "#C0392B" : "#2980B9")

                        Text {
                            anchors.centerIn: parent
                            text: index + 1
                            color: "#FFFFFF"
                            font.bold: true
                        }
                    }

                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 2

                        Text {
                            text: model.name
                            color: "#FFFFFF"
                            font.pixelSize: 14
                            font.bold: true
                            elide: Text.ElideRight
                        }

                        Text {
                            text: model.latitude.toFixed(4) + "°, " + model.longitude.toFixed(4) + "°"
                            color: "#AAAAAA"
                            font.pixelSize: 11
                        }
                    }

                    Button {
                        text: "▲"
                        implicitWidth: 30
                        implicitHeight: 30
                        enabled: index > 0
                        onClicked: navCtrl.waypointModel.moveWaypoint(index, index - 1)
                    }

                    Button {
                        text: "▼"
                        implicitWidth: 30
                        implicitHeight: 30
                        enabled: index < navCtrl.waypointModel.rowCount() - 1
                        onClicked: navCtrl.waypointModel.moveWaypoint(index, index + 1)
                    }

                    Button {
                        text: "✕"
                        implicitWidth: 30
                        implicitHeight: 30
                        onClicked: navCtrl.waypointModel.removeWaypoint(index)
                    }
                }
            }
        }

        // Action Toolbar
        GridLayout {
            columns: 2
            Layout.fillWidth: true
            rowSpacing: 8
            columnSpacing: 8

            Button {
                text: navCtrl.langMgr.t("demo_route")
                Layout.fillWidth: true
                onClicked: navCtrl.loadDefaultDemoRoute()
            }

            Button {
                text: navCtrl.langMgr.t("clear_all")
                Layout.fillWidth: true
                onClicked: navCtrl.waypointModel.clear()
            }

            Button {
                text: navCtrl.langMgr.t("import_gpx")
                Layout.fillWidth: true
                onClicked: importGpxDialog.open()
            }

            Button {
                text: navCtrl.langMgr.t("export_gpx")
                Layout.fillWidth: true
                onClicked: exportGpxDialog.open()
            }
        }
    }
}
