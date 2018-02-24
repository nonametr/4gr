import QtQuick 2.9
import QtQuick.Window 2.3
import QtWebEngine 1.6
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.4
import QtQuick.Dialogs 1.2
import QtQuick.Controls.Styles.Flat 1.0
import QtQml 2.2
import Qt.labs.settings 1.0
import MMOMaskedMouseArea 1.0
import QtGraphicalEffects 1.0

ApplicationWindow {
    id: root
    objectName: "root"
    //flags: Qt.FramelessWindowHint | Qt.Window
    //color: "transparent"
    opacity: 1
    title: "4GRunner"
    width: 1024
    height: 750
    visible: true
    property Item currentWebView: tabView.getTab(tabView.currentIndex).item
    property int showTabPopup: -1

    Connections {
        target: backend
        onReadyToLaunch: {
            startButton.enabled = false
            backend.interceptGame(id)

            var targetX = config.profiles[id].target_pos.x
            var targetY = config.profiles[id].target_pos.y - (Math.round(
                                                                  40 * FlatStyle.scaleFactor) * 0.5)

            tabView.getTab(id).item.runJavaScript(
                        "var target = document.elementFromPoint(" + targetX + ", " + targetY + ");\n
var evt = new MouseEvent(\"click\", {
view: window,
bubbles: true,
cancelable: true
});
target.dispatchEvent(evt);")
        }

        onEndLaunch: {
            startButton.enabled = true
        }

        onGameIntercepted: {
            print("onGameIntercepted => #" + id)
            if (tabView.count - 2 > id) {
                backend.beginStartGame(id + 1)
            }
            startButton.enabled = true
        }
    }

    Component.onCompleted: {
        if (updaterBackend.checkForNewVersion()) {
            updateDialog.open()
        }
        if (config.autostart)
            autoStartDialog.show()
    }

    Loader {
        id: settingsLoader
        source: "settings.qml"
    }

    Loader {
        id: updateLoader
        source: "update.qml"
    }

    Loader {
        id: trayLoader
        source: "tray.qml"
        Connections {
            target: trayLoader.item
            onShowHide: root.visible = !root.visible
            onStartGame: startGame()
            onUpdate: {
                if (updaterBackend.checkForNewVersion()) {
                    updateDialog.open()
                } else {
                    noUpdateDialog.open()
                }
            }
        }
    }

    signal loadStatusChanged

    function startGame() {
        backend.beginStartGame(0)
    }

    menuBar: MenuBar {
        id: mainMenu
        Menu {
            id: fileMenu
            title: qsTr("File")

            MenuItem {
                text: qsTr("&Reset")
                onTriggered: {
                    resetDialog.open()
                }
            }
            MenuItem {
                text: qsTr("&Load...")
                onTriggered: loadFileDialog.open()
            }
            MenuItem {
                text: qsTr("&Save...")
                onTriggered: saveFileDialog.open()
            }
            MenuItem {
                text: qsTr("&Quit")
                onTriggered: Qt.quit()
            }
        }

        Menu {
            id: editMenu
            title: qsTr("&Edit")
            MenuItem {
                text: qsTr("&Settings")
                onTriggered: settingsLoader.item.show()
            }

            MenuItem {
                text: qsTr("&Default web page")
                onTriggered: defaultWebPageDialog.show()
            }
            MenuItem {
                text: qsTr("&Proccess name patern")
                onTriggered: processNameDialog.show()
            }
            MenuItem {
                checkable: true
                text: qsTr("&Start on load")
                checked: config.autostart
                onTriggered: {
                    if (checked) {
                        startOnLoadDialog.open()
                    }
                }
            }
            MenuItem {
                checkable: true
                checked: config.keep_alive
                text: qsTr("&Keep alive")
                onTriggered: {
                    if (checked)
                        keepAliveDialog.open()
                }
            }
        }

        Menu {
            id: viewMenu
            title: qsTr("&View")
            MenuItem {
                checkable: true
                onCheckedChanged: config.show_nav = checked
                text: qsTr("&Navigation bar")

                Component.onCompleted: checked = config.show_nav
            }
            MenuItem {
                checkable: true
                onCheckedChanged: config.show_ssl_errors = checked
                text: qsTr("&SSL errors")

                Component.onCompleted: checked = config.show_ssl_errors
            }
        }

        Menu {
            id: helpMenu
            title: qsTr("&Help")
            MenuItem {
                text: qsTr("&About")
                onTriggered: aboutDialog.show()
            }
            MenuItem {
                text: qsTr("&Donate")
                onTriggered: Qt.openUrlExternally(
                                 "http://magicmess.online/donate/")
            }
        }
    }

    toolBar: ToolBar {
        id: navigationBar
        visible: config.show_nav
        RowLayout {
            anchors.fill: parent
            ToolButton {
                enabled: currentWebView && (currentWebView.canGoBack
                                            || currentWebView.canGoForward)
                menu: Menu {
                    id: historyMenu

                    Instantiator {
                        model: currentWebView
                               && currentWebView.navigationHistory.items
                        MenuItem {
                            text: model.title
                            onTriggered: currentWebView.goBackOrForward(
                                             model.offset)
                            checkable: !enabled
                            checked: !enabled
                            enabled: model.offset
                        }

                        onObjectAdded: historyMenu.insertItem(index, object)
                        onObjectRemoved: historyMenu.removeItem(object)
                    }
                }
            }

            ToolButton {
                id: backButton
                iconSource: "icons/go-previous.png"
                onClicked: currentWebView.goBack()
                enabled: currentWebView && currentWebView.canGoBack
            }
            ToolButton {
                id: forwardButton
                iconSource: "icons/go-next.png"
                onClicked: currentWebView.goForward()
                enabled: currentWebView && currentWebView.canGoForward
            }
            ToolButton {
                id: reloadButton
                iconSource: currentWebView
                            && currentWebView.loading ? "icons/process-stop.png" : "icons/view-refresh.png"
                onClicked: currentWebView
                           && currentWebView.loading ? currentWebView.stop(
                                                           ) : currentWebView.reload()
            }
            TextField {
                id: addressBar
                Image {
                    anchors.verticalCenter: addressBar.verticalCenter
                    x: 5
                    z: 2
                    id: faviconImage
                    width: 16
                    height: 16
                    sourceSize: Qt.size(width, height)
                    source: currentWebView && currentWebView.icon
                }
                style: TextFieldStyle {
                    padding {
                        left: 26
                    }
                }
                focus: true
                Layout.fillWidth: true
                text: currentWebView && currentWebView.url
                onAccepted: currentWebView.url = backend.fromUserInput(text)
            }
            ToolButton {
                id: settingsMenuButton
                menu: Menu {
                    MenuItem {
                        id: loadImages
                        text: "Autoload images"
                        checkable: true
                        checked: WebEngine.settings.autoLoadImages
                        onToggled: WebEngine.settings.autoLoadImages = checked
                    }
                    MenuItem {
                        id: autoLoadIconsForPage
                        text: "Icons On"
                        checkable: true
                        checked: WebEngine.settings.autoLoadIconsForPage
                        onToggled: WebEngine.settings.autoLoadIconsForPage = checked
                    }
                }
            }
        }
        ProgressBar {
            id: progressBar
            height: 3
            anchors {
                left: parent.left
                top: parent.bottom
                right: parent.right
                leftMargin: -parent.leftMargin
                rightMargin: -parent.rightMargin
            }
            z: -2
            minimumValue: 0
            maximumValue: 100
            value: (currentWebView
                    && currentWebView.loadProgress < 100) ? currentWebView.loadProgress : 0
        }
    }
    Rectangle {
        id: rootRect
        anchors.fill: parent

        TabView {
            id: tabView
            anchors.fill: parent

            onCurrentIndexChanged: {
                targetButton.x = config.profiles[tabView.currentIndex].target_pos.x
                targetButton.y = config.profiles[tabView.currentIndex].target_pos.y
                targetButton.injectPromo()
            }

            Component.onCompleted: {
                for (var i = 0; i < config.profiles.length; ++i) {
                    var tab = tabView.insertTab(i, config.profiles[i].name,
                                                webPage)
                    tab.active = true //IMPORTANT TO DO THIS FIRST!!! Component will be instantieted only after this!
                    var newProfile = webPageProfile.createObject(root, {
                                                                     storageName: "mmo_" + i + "_profile"
                                                                 })
                    tab.item.profile = newProfile
                    tab.item.url = backend.fromUserInput(config.profiles[i].url)
                }
                tabView.addTab("+")
                tabView.currentIndex = 0

                targetButton.x = config.profiles[0].target_pos.x
                targetButton.y = config.profiles[0].target_pos.y
            }

            function createTab() {
                config.appendProfile()

                var tab_id = tabView.count - 1
                var tab = tabView.insertTab(tab_id,
                                            config.profiles[tab_id].name,
                                            webPage)
                tab.active = true //IMPORTANT TO DO THIS FIRST!!! Component will be instantieted only after this!

                var newProfile = webPageProfile.createObject(root, {
                                                                 storageName: "mmo_" + tab_id + "_profile"
                                                             })

                tab.item.profile = newProfile
                tab.item.url = backend.fromUserInput(
                            config.profiles[tab_id].url)
                tabView.currentIndex = tab_id
                return tab
            }

            Component {
                id: webPageProfile

                WebEngineProfile {
                    httpCacheType: WebEngineProfile.DiskHttpCache
                    persistentCookiesPolicy: WebEngineProfile.ForcePersistentCookies
                    offTheRecord: false
                }
            }

            Component {
                id: webPage

                WebEngineView {
                    id: webPageView
                    focus: true
                    anchors.fill: parent

                    onLoadingChanged: {
                        loadStatusChanged()
                    }

                    onLinkHovered: {
                        if (hoveredUrl == "")
                            resetStatusText.start()
                        else {
                            resetStatusText.stop()
                            statusText.text = hoveredUrl
                        }
                    }
                    onCertificateError: {
                        error.defer()
                        if (config.show_ssl_errors === true) {
                            sslDialog.enqueue(error)
                        }
                    }

                    onRenderProcessTerminated: {
                        var status = ""
                        switch (terminationStatus) {
                        case WebEngineView.NormalTerminationStatus:
                            status = "(normal exit)"
                            break
                        case WebEngineView.AbnormalTerminationStatus:
                            status = "(abnormal exit)"
                            break
                        case WebEngineView.CrashedTerminationStatus:
                            status = "(crashed)"
                            break
                        case WebEngineView.KilledTerminationStatus:
                            status = "(killed)"
                            break
                        }

                        print("Render process exited with code " + exitCode + " " + status)
                        reloadTimer.running = true
                    }

                    onWindowCloseRequested: {
                        if (tabView.count === 1)
                            root.close()
                        //else
                        //tabView.removeTab(tabView.currentIndex)
                    }
                    Component.onCompleted: {
                        reload()
                    }

                    Timer {
                        id: reloadTimer
                        interval: 0
                        running: false
                        repeat: false
                        onTriggered: currentWebView.reload()
                    }
                }
            }

            style: TabViewStyle {
                readonly property int frameWidth: Math.round(
                                                      FlatStyle.scaleFactor)

                tabsMovable: true
                tabOverlap: -frameWidth
                frameOverlap: frameWidth

                frame: Rectangle {
                    visible: control.frameVisible
                    color: FlatStyle.backgroundColor
                    border.color: FlatStyle.lightFrameColor

                    Text {
                        anchors.centerIn: parent
                        text: "Loading, please wait..."
                        font.family: FlatStyle.fontFamily
                        font.pixelSize: 18
                    }
                }

                tab: Item {
                    id: thisTab
                    readonly property int totalWidth: styleData.availableWidth
                                                      + tabOverlap * (control.count - 1)
                    readonly property int tabWidth: Math.max(
                                                        1,
                                                        totalWidth / Math.max(
                                                            1, control.count))
                    readonly property int remainder: (styleData.index === control.count - 1
                                                      && tabWidth > 0) ? totalWidth % tabWidth : 0

                    implicitWidth: tabWidth + remainder
                    implicitHeight: Math.round(40 * FlatStyle.scaleFactor)

                    Rectangle {
                        anchors.fill: parent
                        visible: true
                        color: styleData.selected ? (config.profiles[styleData.index].enabled ? "#00d857" : "#a0a0a0") : (config.profiles[styleData.index].enabled ? "#00ad45" : "#636363")
                        opacity: styleData.pressed ? 1.0 : 0.8
                        //color: styleData.activeFocus ? (styleData.pressed ? FlatStyle.checkedFocusedAndPressedColor : FlatStyle.focusedColor) : styleData.pressed ? FlatStyle.pressedColor : styleData.selected ? FlatStyle.backgroundColor : !styleData.enabled ? FlatStyle.disabledColor : FlatStyle.styleColor
                    }
                    Rectangle {
                        id: statusRect
                        x: 10
                        y: (thisTab.implicitHeight - height) * 0.5
                        width: 10
                        height: 10
                        radius: height * 0.5
                        color: config.profiles[styleData.index].is_running ? "green" : (tabView.getTab(styleData.index).item.loading ? "#ffba60" : "#b3f442")
                        visible: styleData.index !== tabView.count - 1

                        BusyIndicator {
                            id: busy
                            parent: thisTab
                            x: 5
                            y: (thisTab.implicitHeight - height) * 0.5
                            width: 20
                            height: 20
                            visible: styleData.index !== tabView.count - 1
                            running: tabView.getTab(
                                         styleData.index).item.loading

                            Component.onCompleted: {
                                loadStatusChanged.connect(statusChanged)
                            }
                        }
                    }

                    Text {
                        text: styleData.title
                        anchors.fill: parent
                        anchors.leftMargin: Math.round(
                                                5 * FlatStyle.scaleFactor)
                        anchors.rightMargin: anchors.leftMargin
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        font.family: FlatStyle.fontFamily
                        renderType: FlatStyle.__renderType
                        elide: Text.ElideRight
                        opacity: !styleData.enabled
                                 && styleData.selected ? FlatStyle.disabledOpacity : 1.0
                        color: !styleData.enabled
                               && styleData.selected ? FlatStyle.disabledColor : styleData.selected
                                                       && styleData.enabled
                                                       && !styleData.activeFocus
                                                       && !styleData.pressed ? FlatStyle.styleColor : FlatStyle.selectedTextColor
                    }

                    MouseArea {
                        anchors.fill: parent
                        acceptedButtons: Qt.RightButton | Qt.LeftButton
                        onPressed: {
                            switch (mouse.button) {
                            case Qt.LeftButton:
                                if (styleData.index === tabView.count - 1) {
                                    tabView.createTab()
                                } else {
                                    tabView.currentIndex = styleData.index
                                    mouse.accepted = false
                                }
                                break
                            case Qt.RightButton:
                                tabView.currentIndex = styleData.index
                                showTabPopup = styleData.index
                                break
                            }
                        }
                    }
                }

                tabBar: Rectangle {
                    color: FlatStyle.backgroundColor
                    border.color: control.frameVisible ? FlatStyle.lightFrameColor : "transparent"
                    anchors.fill: parent
                    Rectangle {
                        height: frameWidth
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.margins: frameWidth
                        y: control.tabPosition === Qt.TopEdge ? parent.height - height : 0
                        color: FlatStyle.backgroundColor
                        visible: control.frameVisible
                    }
                }
            }
        }

        Item {
            id: profileButton
            parent: Overlay.overlay
            width: 256
            height: 256
            anchors.right: parent.right
            anchors.bottom: parent.bottom

            Image {
                id: profileRect
                anchors.fill: parent
                source: "right_bot_corner.png"
                scale: profileMouseArea.containsMouse ? 1.1 : 1.0
                opacity: profileMouseArea.containsMouse ? 0.9 : 1

                Behavior on opacity {
                    NumberAnimation {
                        duration: 200
                    }
                }
                Behavior on scale {
                    ScaleAnimator {
                        duration: 200
                    }
                }
                MaskedMouseArea {
                    id: profileMouseArea
                    anchors.fill: parent
                    alphaThreshold: 0.3
                    maskSource: profileRect.source
                    onReleased: {
                        winPosDialog.show()
                    }
                }
            }

            Glow {
                anchors.fill: profileRect
                radius: profileMouseArea.containsMouse ? 10 : 1
                samples: 16
                color: "yellow"
                source: profileRect

                Behavior on radius {
                    NumberAnimation {
                        duration: 300
                    }
                }
            }

            ColumnLayout {
                x: 120
                y: 115
                RowLayout {
                    Text {
                        font.pointSize: 18
                        text: "x:"
                    }
                    Text {
                        font.pointSize: 18
                        text: config.profiles[tabView.currentIndex].win_pos.x
                    }
                }

                RowLayout {
                    Text {
                        font.pointSize: 18
                        text: "y:"
                    }
                    Text {
                        font.pointSize: 18
                        text: config.profiles[tabView.currentIndex].win_pos.y
                    }
                }
                RowLayout {
                    Text {
                        font.pointSize: 18
                        text: "height:"
                    }
                    Text {
                        font.pointSize: 18
                        text: config.profiles[tabView.currentIndex].win_pos.z
                    }
                }
                RowLayout {
                    Text {
                        font.pointSize: 18
                        text: "width:"
                    }
                    Text {
                        font.pointSize: 18
                        text: config.profiles[tabView.currentIndex].win_pos.w
                    }
                }
            }
        }

        Item {
            id: targetButton
            width: 40
            height: 40
            parent: tabView
            scale: targetMouseArea.containsMouse ? 1.0 : 0.75
            opacity: targetMouseArea.containsMouse ? 1.0 : 0.7

            function injectPromo() {
                var targetX = targetButton.x
                var targetY = targetButton.y - (Math.round(
                                                    40 * FlatStyle.scaleFactor) * 0.5)

                tabView.getTab(tabView.currentIndex).item.runJavaScript(
                            "var target = document.elementFromPoint(" + targetX
                            + ", " + targetY + ");\n
var old_elem = document.getElementById(\"mmo_inj_promo\");
if(old_elem != null)
old_elem.remove();
if(target != null)
target.insertAdjacentHTML('afterend', '<div id = \"mmo_inj_promo\"><font style=\"text-align: center\" size=\"2\" color=\"red\">Powered by <a href=\"http://magicmess.online\">magicmess.online</a></font></div>');")

                return
            }

            Timer {
                running: true
                repeat: true
                interval: 1000
                onTriggered: targetButton.injectPromo()
            }

            onXChanged: {
                config.profiles[tabView.currentIndex].target_pos.x = x
            }
            onYChanged: {
                config.profiles[tabView.currentIndex].target_pos.y = y
            }

            Image {
                id: targetImg
                source: "click_area.png"
                anchors.fill: parent
            }

            Behavior on scale {
                NumberAnimation {
                    duration: 250
                }
            }
            Behavior on opacity {
                NumberAnimation {
                    duration: 250
                }
            }
            RotationAnimation on rotation {
                from: 0
                to: 360
                duration: 6000
                loops: Animation.Infinite
            }

            MouseArea {
                id: targetMouseArea
                anchors.fill: parent
                drag.target: parent
                hoverEnabled: true
            }
        }

        Item {
            id: startButton
            width: 240
            height: 120
            parent: Overlay.overlay
            scale: startMouseArea.containsMouse ? 1.0 : 0.75
            opacity: startMouseArea.containsMouse ? 1.0 : 0.7

            Component.onCompleted: {
                x = config.start_pos.x
                y = config.start_pos.y
            }

            onXChanged: config.start_pos.x = x
            onYChanged: config.start_pos.y = y

            Image {
                id: startImg
                source: "button-start-game.png"
                anchors.fill: parent
            }

            Behavior on scale {
                NumberAnimation {
                    easing.type: Easing.OutBounce
                    duration: 600
                }
            }
            Behavior on opacity {
                NumberAnimation {
                    duration: 250
                }
            }

            MaskedMouseArea {
                id: startMouseArea
                anchors.fill: parent
                alphaThreshold: 0.4
                maskSource: startImg.source
                onPressed: {
                    startGame()
                }
            }
        }
        Rectangle {
            id: statusBubble
            color: "oldlace"
            property int padding: 8

            anchors.left: parent.left
            anchors.bottom: parent.bottom
            width: statusText.paintedWidth + padding
            height: statusText.paintedHeight + padding

            Text {
                id: statusText
                anchors.centerIn: statusBubble
                elide: Qt.ElideMiddle

                Timer {
                    id: resetStatusText
                    interval: 750
                    onTriggered: statusText.text = ""
                }
            }
        }
    }

    Menu {
        id: tabMenu
        property int tabId: 0

        MenuItem {
            text: "Rename"
            onTriggered: {
                renameDialog.show()
            }
        }

        MenuItem {
            text: "Enable\Disable"
            checkable: true
            checked: config.profiles[tabMenu.tabId].enabled
            onTriggered: {
                config.profiles[tabMenu.tabId].enabled = !config.profiles[tabMenu.tabId].enabled
            }
        }

        MenuItem {
            text: "Kill"
            onTriggered: {
                if (config.profiles[tabMenu.tabId].is_running) {
                    backend.killGame(tabMenu.tabId)
                } else {
                    infoDialog.text = "This profile currently not in game!"
                    infoDialog.open()
                }
            }
        }

        MenuItem {
            text: "Reload"
            onTriggered: {
                tabView.getTab(tabMenu.tabId).item.reload()
            }
        }
        MenuItem {
            text: "Delete"
            onTriggered: {
                config.removeProfile(tabMenu.tabId)
                tabView.removeTab(tabMenu.tabId)
            }
        }
    }

    Timer {
        id: tabMenuCheckTimer
        interval: 200
        running: true
        repeat: true
        onTriggered: {
            if (showTabPopup > -1) {
                tabMenu.tabId = showTabPopup
                tabMenu.popup()
            }

            showTabPopup = -1
        }
    }

    MouseArea {
        id: globalMouseArea
        anchors.fill: parent
        propagateComposedEvents: true
        acceptedButtons: Qt.AllButtons
        property bool is_moving: false
        property variant clickPos: "1,1"

        onPressed: {
            if (mouse.button === Qt.MidButton) {
                is_moving = true
                clickPos = Qt.point(mouse.x, mouse.y)
            } else {
                mouse.accepted = false
            }
        }
        onReleased: {
            is_moving = false
        }

        onPositionChanged: {
            if (is_moving) {
                var delta = Qt.point(mouse.x - clickPos.x, mouse.y - clickPos.y)
                var new_x = root.x + delta.x
                var new_y = root.y + delta.y
                if (new_y <= 0)
                    root.visibility = Window.Maximized
                else {
                    if (root.visibility === Window.Maximized)
                        root.visibility = Window.Windowed
                    root.x = new_x
                    root.y = new_y
                }
            }
        }
    }

    MessageDialog {
        id: sslDialog

        property var certErrors: []
        icon: StandardIcon.Warning
        standardButtons: StandardButton.No | StandardButton.Yes
        title: "Server's certificate not trusted"
        text: "Do you wish to continue?"
        detailedText: "If you wish so, you may continue with an unverified certificate. "
                      + "Accepting an unverified certificate means "
                      + "you may not be connected with the host you tried to connect to.\n"
                      + "Do you wish to override the security check and continue?"
        onYes: {
            certErrors.shift().ignoreCertificateError()
            presentError()
        }
        onNo: reject()
        onRejected: reject()

        function reject() {
            certErrors.shift().rejectCertificate()
            presentError()
        }
        function enqueue(error) {
            certErrors.push(error)
            presentError()
        }
        function presentError() {
            visible = certErrors.length > 0
        }
    }
    FileDialog {
        id: loadFileDialog
        nameFilters: ["Config files (*.conf)"]
        title: "Please choose a config file to load from"
        folder: shortcuts.home
        onAccepted: {
            config.load(loadFileDialog.fileUrl)
        }
        onRejected: {
            console.log("Canceled")
        }
    }
    FileDialog {
        id: saveFileDialog
        selectExisting: false
        nameFilters: ["Config files (*.conf)"]
        title: "Please choose a config file to save into"
        folder: shortcuts.home
        onAccepted: {
            print(loadFileDialog.fileUrl)
            config.save(saveFileDialog.fileUrl)
        }
        onRejected: {
            console.log("Canceled")
        }
    }
    MessageDialog {
        id: startOnLoadDialog
        title: "Start on load?"
        icon: StandardIcon.Question
        text: "Start all profiles after application load? "
        detailedText: "This feature currently disabled. Do you what to enable it?"
        standardButtons: StandardButton.Yes | StandardButton.No
        onYes: config.autostart = true
        onNo: config.autostart = false
    }
    MessageDialog {
        id: noUpdateDialog
        title: "Updates"
        icon: StandardIcon.Information
        text: "No updates available!"
        standardButtons: StandardButton.Ok
    }
    MessageDialog {
        id: updateDialog
        title: "Update"
        icon: StandardIcon.Question
        text: "New update available! Download and install update?"
        standardButtons: StandardButton.Yes | StandardButton.No
        onYes: {
            updaterBackend.downloadNewVersion()
            updateLoader.item.show()
        }
        onNo: console.log("disabled")
    }
    MessageDialog {
        id: resetDialog
        title: "Reset"
        icon: StandardIcon.Question
        text: "Program will be closed, all settings will be set to its defaults!\nDo you still want to reset?"
        standardButtons: StandardButton.Yes | StandardButton.No
        onYes: {
            config.resetConfig()
            backend.restart()
        }
        onNo: console.log("disabled")
    }
    MessageDialog {
        id: infoDialog
        title: "Info"
        icon: StandardIcon.Information
        text: "put text before showing this dialog!"
        standardButtons: StandardButton.Ok
    }

    MessageDialog {
        id: keepAliveDialog
        title: "Keep alive?"
        icon: StandardIcon.Question
        text: "Keep all loaded profiles alive?"
        detailedText: "This meens app. will continue monitoring profiles and if something happens(crush for example) it will restart it. Feature currently disabled. Do you what to enable it?"
        standardButtons: StandardButton.Yes | StandardButton.No
        onYes: config.keep_alive = true
        onNo: config.keep_alive = false
    }
    Window {
        id: renameDialog
        title: "Rename"
        maximumWidth: 250
        maximumHeight: 35
        minimumWidth: 250
        minimumHeight: 35
        modality: "ApplicationModal"

        onVisibleChanged: renameDialogTextInput.text = config.profiles[tabView.currentIndex].name
        TextField {
            id: renameDialogTextInput
            focus: true

            anchors.fill: parent
            anchors.leftMargin: 5
            anchors.rightMargin: 5
            anchors.topMargin: 5
            anchors.bottomMargin: 5

            Keys.onReturnPressed: {
                renameDialog.close()
            }

            onTextChanged: {
                config.profiles[tabView.currentIndex].name = text
                tabView.getTab(tabView.currentIndex).title = text
            }
        }
    }
    Window {
        id: aboutDialog
        maximumWidth: logoImg.width + 150
        maximumHeight: 125
        minimumWidth: logoImg.width + 150
        minimumHeight: 125
        modality: "ApplicationModal"
        title: "About"
        Rectangle {
            anchors.fill: parent
            anchors.leftMargin: 10
            anchors.rightMargin: 10
            anchors.topMargin: 10
            anchors.bottomMargin: 10
            Column {
                spacing: 10
                Image {
                    id: logoImg
                    source: "http://magicmess.online/ts3/mmo_logo4.png"
                }

                Text {
                    text: "<b>4GRunner</b> v" + updaterBackend.version
                          + ", by <a href=\"magicmess.online\">magicmess.online</a> Ⓒ 2018<br>"
                          + "For comments or questions, email: mmessonline@gmail.com"
                    font.pixelSize: 14
                }
            }
        }
    }
    Window {
        id: defaultWebPageDialog
        modality: "ApplicationModal"
        title: "Default web page"
        maximumWidth: 250
        maximumHeight: 40
        minimumWidth: 250
        minimumHeight: 40
        TextField {
            id: defaultWebPageDialogTextInput

            anchors.fill: parent
            anchors.leftMargin: 5
            anchors.rightMargin: 5
            anchors.topMargin: 5
            anchors.bottomMargin: 5

            Component.onCompleted: text = config.default_url
            Keys.onReturnPressed: {
                defaultWebPageDialog.close()
            }
            onTextChanged: {
                config.default_url = text
            }
        }
    }
    Window {
        id: winPosDialog
        modality: "ApplicationModal"
        color: "#b5e7a0"

        Image {
            id: winPosImg
            anchors.fill: parent
            fillMode: Image.Tile
            source: "lineage_2_background_by_vas_co.jpg"
        }

        onActiveChanged: {
            winPosDialog.x = config.profiles[tabView.currentIndex].win_pos.x
            winPosDialog.y = config.profiles[tabView.currentIndex].win_pos.y
            winPosDialog.width = config.profiles[tabView.currentIndex].win_pos.w
            winPosDialog.height = config.profiles[tabView.currentIndex].win_pos.z
        }

        Column {
            anchors.centerIn: parent
            Row {
                Text {
                    text: "Move/resize window"
                    font.bold: true
                    font.pointSize: 18
                    color: "white"
                }
            }
            Row {
                Column {
                    Row {
                        Text {
                            color: "white"
                            text: "x: "
                            font.pixelSize: 16
                        }

                        SpinBox {
                            stepSize: 1
                            maximumValue: 16384
                            value: winPosDialog.x
                            onValueChanged: winPosDialog.x = value
                        }
                    }
                    Row {
                        Text {
                            color: "white"
                            text: "y: "
                            font.pixelSize: 16
                        }

                        SpinBox {
                            stepSize: 1
                            maximumValue: 16384
                            value: winPosDialog.y
                            onValueChanged: winPosDialog.y = value
                        }
                    }
                }
                Column {
                    Row {
                        Text {
                            color: "white"
                            text: "      width: "
                            font.pixelSize: 16
                        }

                        SpinBox {
                            stepSize: 1
                            maximumValue: 16384
                            value: winPosDialog.width
                            onValueChanged: winPosDialog.width = value
                        }
                    }
                    Row {
                        Text {
                            color: "white"
                            text: "      height:"
                            font.pixelSize: 16
                        }

                        SpinBox {
                            stepSize: 1
                            maximumValue: 16384
                            value: winPosDialog.height
                            onValueChanged: winPosDialog.height = value
                        }
                    }
                }
            }
            Row {
                Text {
                    text: "Close when done"
                    font.bold: true
                    font.pointSize: 18
                    color: "white"
                }
            }
        }

        Timer {
            id: configUpdTimer
            interval: 500
            running: winPosDialog.visible
            repeat: true
            onTriggered: {
                config.profiles[tabView.currentIndex].win_pos.x = winPosDialog.x
                config.profiles[tabView.currentIndex].win_pos.y = winPosDialog.y
                config.profiles[tabView.currentIndex].win_pos.z = winPosDialog.height
                config.profiles[tabView.currentIndex].win_pos.w = winPosDialog.width
            }
        }

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.RightButton | Qt.LeftButton | Qt.MidButton
            property bool is_moving: false
            property variant clickPos: "1,1"

            onPressed: {
                if (mouse.button === Qt.MidButton) {
                    is_moving = true
                    clickPos = Qt.point(mouse.x, mouse.y)
                } else {
                    mouse.accepted = false
                }
            }
            onReleased: {
                is_moving = false
            }

            onPositionChanged: {
                if (is_moving) {
                    var delta = Qt.point(mouse.x - clickPos.x,
                                         mouse.y - clickPos.y)
                    var new_x = winPosDialog.x + delta.x
                    var new_y = winPosDialog.y + delta.y
                    if (new_y <= 0)
                        winPosDialog.visibility = Window.Maximized
                    else {
                        if (winPosDialog.visibility === Window.Maximized)
                            winPosDialog.visibility = Window.Windowed
                        winPosDialog.x = new_x
                        winPosDialog.y = new_y
                    }
                }
            }
        }
    }

    Window {
        id: autoStartDialog
        modality: "ApplicationModal"
        title: "Auto starting..."
        minimumHeight: 100
        maximumHeight: 100
        minimumWidth: 250
        maximumWidth: 250

        Text {
            id: autoStartText
            Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
            font.pixelSize: 14
            //anchors.top: parent.top
            //anchors.centerIn: parent
            text: "Waiting for profiles to load: "
        }

        Button {
            text: "Terminate"
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 5
            anchors.rightMargin: 5
            onClicked: {
                autoStartDialog.close()
            }
        }

        onVisibleChanged: {
            countDownTimer.running = visible
        }

        Timer {
            id: countDownTimer
            property var start_countdown: 4
            property var ready_to_start: false
            running: false
            interval: 1000
            repeat: true
            onTriggered: {
                if (ready_to_start === true) {
                    autoStartText.text = "Starting in " + --start_countdown + " seconds..."
                    if (start_countdown === 0) {
                        running = false
                        startGame()
                    }
                } else {
                    var ready_count = 0
                    for (var i = 0; i < config.profiles.length; ++i) {
                        if (tabView.getTab(i).item.loading === false)
                            ready_count += 1
                    }

                    autoStartText.text = "Waiting for profiles to load: "
                            + ready_count + "/" + config.profiles.length
                    if (config.profiles.length === ready_count)
                        ready_to_start = true
                }
            }
        }
    }

    Window {
        id: processNameDialog
        modality: "ApplicationModal"
        title: "Process name patern"
        maximumWidth: 250
        maximumHeight: 40
        minimumWidth: 250
        minimumHeight: 40
        TextField {
            id: processNameDialogDialogTextInput

            anchors.fill: parent
            anchors.leftMargin: 5
            anchors.rightMargin: 5
            anchors.topMargin: 5
            anchors.bottomMargin: 5

            Component.onCompleted: text = config.window_name

            Keys.onReturnPressed: {
                processNameDialog.close()
            }

            onTextChanged: {
                config.window_name = text
            }
        }
    }
}
