import QtQuick 2.0
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

//import QtWayland.Compositor 1.3

import KLib 1.0
import Km2


Window {
    width: 1000
    height: 600
    visible: true

    id: root
    property string mode: ""

    Compiler {
        id: compiler
        code: codeArea.text
        uri: appCore.openedProjFile ? appCore.openedProjFile.fullPath : ''
        backend: appCore.backendFactory.currentBackend
    }

    Presentor {
        id: presentor
        codeDocument: codeArea.textDocument
        uri: appCore.openedProjFile ? appCore.openedProjFile.fullPath : ''
        //errors: compiler.errors
        //onPresentationCompleated: codeArea.updatePresentation()
    }


    Connections {
        target: appCore
        function onCodeLoaded(t) {
            codeArea.text = t
        }
    }



    Connections {
        target: appCore.executor
        function onMessage(mess, type, isSystem) {
            execArea.pushMessage(mess, type, isSystem);
        }
    }

    ColumnLayout {
        anchors.fill: parent

        RowLayout {
            Layout.fillWidth: true
            height: 40

            Rectangle {
                color: '#aaaa00'
                Layout.fillWidth: true
                height: 40
            }

            Rectangle {
                color: '#aaaa00'
                Layout.fillWidth: true
                height: 40
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Rectangle {
                width: 60
                Layout.fillHeight: true
                color: '#aaaa00'
                ColumnLayout {
                    anchors.fill: parent

                    ToolBarButton {
                        text: 'Tree'
                        Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
                        Layout.topMargin: 10
                        onClicked: appCore.mode = AppCore.ModeTree
                    }

                    ToolBarButton {
                        text: 'Log'
                        Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
                        Layout.topMargin: 10
                        onClicked: appCore.mode = AppCore.ModeGramLog
                    }

                    ToolBarButton {
                        text: 'Toks'
                        Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
                        onClicked: appCore.mode = AppCore.ModeTokens
                    }

                    ToolBarButton {
                        text: 'Exex'
                        Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
                        onClicked: appCore.mode = AppCore.ModeExec
                    }

                    ToolBarButton {
                        text: 'Gram'
                        Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
                        onClicked: appCore.mode = AppCore.ModeGramatic
                    }

                    ToolBarButton {
                        text: 'Asm'
                        Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
                        onClicked: appCore.mode = AppCore.ModeAsm
                    }

                    ToolBarButton {
                        text: 'Back'
                        Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
                        onClicked: backendList.collapsed = !backendList.collapsed
                    }

                    ToolBarButton {
                        text: 'clang\n  ++'
                        Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
                        onClicked: {
                            const err = appCore.makeExecutable("app_out/module");
                            if(err.length > 0) {
                                infoDialog.openWithMessage(`err: ${err}`)
                            } else {
                                infoDialog.openWithMessage(`success`)
                            }
                        }
                    }

                    ToolBarChackBox {
                        Layout.topMargin: 30
                        text: 'T'
                        Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
                        onCheckedChanged: compiler.onlyTree = checked
                    }

                    ToolBarChackBox {
                        text: 'V'
                        Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
                        onCheckedChanged: {
                            compiler.verbose = checked
                            appCore.verbose = checked
                        }
                    }

                    Item {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                    }
                }
            }
            BackendList {
                clip: true
                id: backendList
                property bool collapsed: true
                Layout.preferredWidth: backendList.collapsed ? 0 : backendList.implicitWidth
                Behavior on Layout.preferredWidth { NumberAnimation { duration: 500 } }
                Layout.fillHeight: true
                highlightedName: appCore.backendFactory.currentBackendName
                model: appCore.backendFactory.availableBackends
                onChoosed: name => appCore.backendFactory.currentBackendName = name
            }
            ColumnLayout {
                width: 300
                Layout.fillHeight: true

                RowLayout {
                    Layout.fillWidth: true
                    Layout.fillHeight: !codeAreaTile.hidden || !projViewTile.hidden
                    //Layout.minimumHeight: 20
                    Tile {
                        id: projViewTile
                        implicitWidth: projViewTile.hidden ? 20 : projView.implicitWidth + projView.anchors.margins * 2 + 40
                        //Layout.fillWidth: true
                        Layout.fillHeight: !projViewTile.hidden
                        Layout.alignment: Qt.AlignTop | Qt.AlignLeft
                        internalColor: '#ffffff'
                        ProjectView {
                            openedFile: appCore.openedProjFile
                            id: projView
                            model: appCore.projectFiles
                            anchors.fill: parent
                            anchors.margins: 2
                            onChoosed: f => appCore.loadFile(f, codeArea.text)
                        }
                    }
                    Tile {
                        id: codeAreaTile
                        Layout.fillWidth: !codeAreaTile.hidden
                        Layout.fillHeight: !codeAreaTile.hidden
                        Layout.alignment: Qt.AlignTop | Qt.AlignLeft

                        CodeArea {
                            id: codeArea
                            anchors.fill: parent
                            anchors.margins: 2
                            onTextChanged: appCore.codeEdited = true

                            property bool ctrlPressed: false
                            Keys.onPressed: event => {
                                if(!event.isAutoRepeat) {
                                    if(codeArea.ctrlPressed) {
                                        if(event.key === Qt.Key_S) {
                                            appCore.writeToCurrentFile(codeArea.text)
                                        } else if(event.key === Qt.Key_L) {
                                            codeArea.text = appCore.readFromCurrentFile()
                                            appCore.codeEdited = false;
                                        }
                                    } else if(event.key === Qt.Key_Control) {
                                        codeArea.ctrlPressed = true
                                    }
                                }
                            }
                            Keys.onReleased: event => {
                                if(event.key === Qt.Key_Control) {
                                    codeArea.ctrlPressed = false
                                }
                            }
                        }
                        Text {
                            anchors.right: parent.right
                            anchors.top: parent.top
                            anchors.rightMargin: 30
                            anchors.topMargin: 5

                            visible: appCore.codeEdited
                            font.pointSize: 14
                            text: qsTr('*')
                        }
                        Text {
                            anchors.right: parent.right
                            anchors.top: parent.top
                            anchors.rightMargin: 60
                            anchors.topMargin: 5

                            visible: codeArea.ctrlPressed
                            font.pointSize: 14
                            text: qsTr('ctrl')
                        }
                        Text {
                            anchors.right: parent.right
                            anchors.bottom: parent.bottom
                            anchors.rightMargin: 30
                            anchors.bottomMargin: 5

                            visible: presentor.astTokensReady
                            font.pointSize: 14
                            text: qsTr('A')
                        }
                    }
                }
                RowLayout {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 80
                    Tile {
                        internalColor: '#ffffff'
                        Layout.fillWidth: true
                        Layout.preferredHeight: 80
                        clip: true

                        ErrorsView {
                            anchors.fill: parent
                            model: presentor.errors
                            onClicked: err => codeArea.goToPosition(err.begin)
                            area: codeArea.textArea
                        }
                    }
                    Tile {
                        internalColor: '#ffffff'
                        Layout.fillWidth: true
                        Layout.preferredHeight: 80
                        clip: true

                        ErrorsView {
                            anchors.fill: parent
                            model: compiler.errors
                            onClicked: err => codeArea.goToPosition(err.begin)
                            area: codeArea.textArea
                        }
                    }
                }
                Tile {
                    internalColor: '#ffffff'
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    KGramTreeView {
                        tree: compiler.tree
                        id: treeWiev
                        focus: true
                        anchors.fill: parent
                        visible: appCore.mode === AppCore.ModeTree

                        ToolBarButton {
                            text: 'O'
                            anchors.top: parent.top
                            anchors.right: parent.right
                            anchors.margins: 5
                            onClicked: treeWiev.openInBrowser()
                        }
                    }

                    InfoArea {
                        visible: appCore.mode === AppCore.ModeTokens || appCore.mode === AppCore.ModeGramatic || appCore.mode === AppCore.ModeAsm
                        anchors.fill: parent
                        anchors.margins: 2
                        id: infoArea
                        text: {
                            if(appCore.mode === AppCore.ModeTokens) {
                                return compiler.tokens;
                            } else if(appCore.mode === AppCore.ModeGramatic) {
                                return compiler.gramatic;
                            } else if(appCore.mode === AppCore.ModeAsm) {
                                return compiler.asmCode;
                            } else {
                                return '';
                            }
                        }
                        subtext: {
                            if(appCore.mode === AppCore.ModeTokens) {
                                return compiler.astTokens;
                            } else if(appCore.mode === AppCore.ModeGramatic) {
                                return '';
                            } else if(appCore.mode === AppCore.ModeAsm) {
                                return '';
                            } else {
                                return '';
                            }
                        }
                    }

                    ExecArea {
                        id: execArea
                        anchors.fill: parent
                        anchors.margins: 2
                        visible: appCore.mode === AppCore.ModeExec
                        onErr: message => infoDialog.openWithMessage(message)
                    }

                    LogTable {
                        id: logTable
                        anchors.fill: parent
                        anchors.margins: 2
                        visible: appCore.mode === AppCore.ModeGramLog
                        log: compiler.log

                        Text {
                            visible: !compiler.verbose
                            anchors.centerIn: parent
                            text: 'Logs only generated if verbose flag set'
                        }
                    }
                }
            }
        }
    }

    InfoDialog {
        anchors.fill: parent
        visible: false
        id: infoDialog
    }
}
