import QtQuick 2.9
import Qt.labs.platform 1.0

SystemTrayIcon {
    id: trayMenu
    objectName: "trayMenu"
    visible: true
    tooltip: "4GRunner"
    iconSource: "tray-icon.png"

    signal showHide
    signal startGame
    signal update

    menu: Menu {
        MenuItem {
            text: qsTr("Start game")
            onTriggered: trayMenu.startGame()
        }

        MenuItem {
            text: qsTr("Show/Hide")
            onTriggered: trayMenu.showHide()
        }

        MenuItem {
            text: qsTr("Update")
            onTriggered: trayMenu.update()
        }

        MenuItem {
            text: qsTr("Quit")
            onTriggered: Qt.quit()
        }
    }
}
