import QtQuick 2.0
import QtQuick.Window 2.3
import QtQuick.Controls 1.4

Window {
    id: settings
    //flags: Qt.FramelessWindowHint | Qt.Window
    //color: "transparent"
    opacity: 1
    title: "Settings"
    width: 1024
    height: 750
    visible: true

    Label {
        id: label
        text: qsTr("Default web site:")
        font.pointSize: 14
        anchors.left: parent.left
        anchors.leftMargin: 10
    }

    TextField {
        id: textField
        height: 30
        anchors.right: parent.right
        anchors.rightMargin: 10
        anchors.left: label.right
        anchors.leftMargin: 10
        placeholderText: qsTr("google.com")
    }
}
