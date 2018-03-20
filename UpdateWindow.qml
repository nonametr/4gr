import QtQuick 2.0
import QtQuick.Window 2.3
import QtQuick.Controls 1.4

Window {
    id: updateWindow
    opacity: 1
    title: "Update"
    maximumWidth: 400
    maximumHeight: 50
    minimumWidth: 400
    minimumHeight: 50
    visible: false

    Rectangle {
        anchors.fill: parent
        anchors.bottomMargin: 10
        anchors.leftMargin: 10
        anchors.rightMargin: 10
        anchors.topMargin: 10

        Row {
            Text {
                id: text
                text: qsTr("Progress: ")
                font.pixelSize: 18
            }
            ProgressBar {
                id: progressBar
                minimumValue: 0
                maximumValue: 100
                height: 25
                width: 300
                value: updaterBackend.download_progress
            }
        }
    }
}
