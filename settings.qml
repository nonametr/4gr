import QtQuick 2.0
import QtQuick.Window 2.3
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.4

Window {
    id: settings
    //flags: Qt.FramelessWindowHint | Qt.Window
    //color: "transparent"
    opacity: 1
    title: "Settings"
    maximumWidth: 470
    minimumWidth: 470
    maximumHeight: 220
    minimumHeight: 220
    property alias columnLayout: columnLayout
    visible: false

    ColumnLayout {
        id: columnLayout
        height: 100
        anchors.top: parent.top
        anchors.topMargin: 5
        anchors.left: parent.left
        anchors.leftMargin: 5
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        anchors.rightMargin: 5

        RowLayout {
            id: rowLayout
            width: 100
            height: 100
            Layout.fillHeight: false
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter | Qt.AlignTop

            Label {
                id: label
                text: qsTr("Default page:")
                font.pointSize: 14
            }

            TextField {
                id: textField
                width: 160
                height: 30
                clip: false
                Layout.fillWidth: true
                font.pointSize: 12
                placeholderText: qsTr("google.com")
                onTextChanged: {
                    config.default_url = text
                }
                Component.onCompleted: {
                    text = config.default_url
                }
            }
        }

        RowLayout {
            id: rowLayout1
            x: 0
            y: 0
            width: 100
            height: 100
            Layout.fillHeight: false
            visible: true
            transformOrigin: Item.Center
            spacing: 5
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter | Qt.AlignTop

            Label {
                id: label1
                text: qsTr("Game name:")
                font.pointSize: 14
            }

            TextField {
                id: textField1
                height: 30
                font.pointSize: 12
                Layout.fillWidth: true
                placeholderText: qsTr("Lineage II")
                onTextChanged: {
                    config.window_name = text
                }
                Component.onCompleted: {
                    text = config.window_name
                }
            }
        }

        RowLayout {
            id: rowLayout5
            x: 0
            y: 0
            width: 100
            height: 100
            Layout.fillHeight: false
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
            Label {
                id: label3
                text: qsTr("Start after:")
                font.pointSize: 14
            }

            TextField {
                id: textField2
                width: 136
                height: 30
                placeholderText: qsTr("Frost")
                font.pointSize: 12
                Layout.fillWidth: true
                onTextChanged: {
                    config.start_after_process = text
                }
                Component.onCompleted: {
                    text = config.start_after_process
                }
            }
            spacing: 5
            visible: true
            transformOrigin: Item.Center
        }

        RowLayout {
            id: rowLayout2
            x: 0
            y: 0
            width: 100
            height: 100
            Layout.fillHeight: false
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            Label {
                id: label2
                text: qsTr("Start delay:")
                font.pointSize: 14
            }

            SpinBox {
                id: spinBox
                decimals: 2
                maximumValue: 50
                stepSize: 0.1
                Layout.fillWidth: false
                onValueChanged: {
                    config.delay = value
                }
                Component.onCompleted: {
                    value = config.delay
                }
            }

            Label {
                id: label4
                text: qsTr("seconds")
            }
            spacing: 5
            visible: true
            transformOrigin: Item.Center
        }

        RowLayout {
            id: rowLayout3
            x: 0
            y: 0
            width: 100
            height: 100
            Layout.fillHeight: false
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop

            CheckBox {
                id: checkBox
                height: 30
                Layout.fillHeight: false
                Layout.fillWidth: false

                style: CheckBoxStyle {
                    label: Text {
                        font.pixelSize: 16
                        text: qsTr("Start on load")
                    }
                }

                onCheckedChanged: {
                    config.autostart = checked
                }
                Component.onCompleted: {
                    checked = config.autostart
                }
            }
            spacing: 5
            visible: true
            transformOrigin: Item.Center
        }

        RowLayout {
            id: rowLayout4
            x: 0
            y: 0
            width: 100
            height: 100
            Layout.fillHeight: false
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            CheckBox {
                id: checkBox1
                height: 30
                Layout.fillHeight: false
                Layout.fillWidth: false
                style: CheckBoxStyle {
                    label: Text {
                        font.pixelSize: 16
                        text: qsTr("Auto restart")
                    }
                }
                onCheckedChanged: {
                    config.keep_alive = checked
                }
                Component.onCompleted: {
                    checked = config.keep_alive
                }
            }
            spacing: 5
            visible: true
            transformOrigin: Item.Center
        }
    }
}
