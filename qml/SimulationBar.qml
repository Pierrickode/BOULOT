import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: simulationBarRoot
    height: 60
    color: "#2D2D30"
    radius: 6

    RowLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 15

        Text {
            text: navCtrl.langMgr.t("simulation_gps")
            color: "#FFFFFF"
            font.pixelSize: 14
            font.bold: true
        }

        Button {
            text: navCtrl.gpsProvider.isSimulating ? navCtrl.langMgr.t("pause_play") : navCtrl.langMgr.t("start")
            onClicked: {
                if (navCtrl.gpsProvider.isSimulating) {
                    navCtrl.gpsProvider.pauseSimulation();
                } else {
                    navCtrl.startSimulation(60.0);
                }
            }
        }

        Button {
            text: navCtrl.langMgr.t("stop")
            enabled: navCtrl.gpsProvider.isSimulating
            onClicked: navCtrl.gpsProvider.stopSimulation()
        }

        Text {
            text: navCtrl.langMgr.t("speed_label")
            color: "#CCCCCC"
            font.pixelSize: 12
        }

        ComboBox {
            model: ["1x", "2x", "5x", "10x", "20x"]
            currentIndex: 0
            onCurrentIndexChanged: {
                var mults = [1.0, 2.0, 5.0, 10.0, 20.0];
                navCtrl.gpsProvider.setSimulationSpeed(mults[currentIndex]);
            }
        }

        Item { Layout.fillWidth: true }
    }
}
