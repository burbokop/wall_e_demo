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


    Connections {
        target: appCore
        function onCodeChanged() {
            codeArea.loadText(appCore.code);
        }
        function onCompilationCompleated() {
        }
        function onPresentationCompleated() {
            codeArea.updatePresentation()
        }
    }
    Component.onCompleted: {
        codeArea.loadText(appCore.code);
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
                color: "#aaaa00"
                Layout.fillWidth: true
                height: 40
            }

            Rectangle {
                color: "#aaaa00"
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
                color: "#aaaa00"
                ColumnLayout {
                    anchors.fill: parent

                    ToolBarButton {
                        text: "Tree"
                        Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
                        Layout.topMargin: 10
                        onClicked: appCore.mode = AppCore.ModeTree
                    }

                    ToolBarButton {
                        text: "Toks"
                        Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
                        onClicked: appCore.mode = AppCore.ModeTokens
                    }

                    ToolBarButton {
                        text: "Exex"
                        Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
                        onClicked: appCore.mode = AppCore.ModeExec
                    }

                    ToolBarButton {
                        text: "Gram"
                        Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
                        onClicked: appCore.mode = AppCore.ModeGramatic
                    }

                    ToolBarButton {
                        text: "Asm"
                        Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
                        onClicked: appCore.mode = AppCore.ModeAsm
                    }

                    ToolBarButton {
                        text: "Back"
                        Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
                        onClicked: backendList.collapsed = !backendList.collapsed
                    }

                    ToolBarButton {
                        text: "clang\n  ++"
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
                        text: "T"
                        Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
                        onCheckedChanged: appCore.onlyTree = checked
                    }

                    ToolBarChackBox {
                        text: "V"
                        Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
                        onCheckedChanged: appCore.verbose = checked
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
                Tile {
                    internalColor: "#ffffff"
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    CodeArea {
                        id: codeArea
                        onTextChanged: {
                            appCore.code = text;
                        }
                        onTextDocumentChanged: {
                            appCore.codeDocument = textDocument
                        }
                        anchors.fill: parent
                        anchors.margins: 2
                    }
                }
                Tile {
                    internalColor: "#ffffff"
                    id: errTile
                    Layout.fillWidth: true
                    Layout.preferredHeight: 80
                    clip: true

                    property int currentErr: -1
                    ListView {
                        anchors.fill: parent
                        model: appCore.errors
                        delegate: Rectangle {
                            color: (errTile.currentErr === index) ? "#88888888" : "#00000000"
                            id: errRect
                            width: parent.width
                            height: (errTile.currentErr === index) ? 48 : 24
                            Text {
                                text: (errTile.currentErr === index)
                                      ? appCore.errToString(modelData) + "\nat fragment: " +  codeArea.textFragmentForError(modelData)
                                      : appCore.errToString(modelData)
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.left: parent.left
                                anchors.leftMargin: 4
                            }
                            MouseArea {
                                id: errMouseArea
                                anchors.fill: parent
                                onClicked: {
                                    codeArea.goToPosition(appCore.errBegin(modelData))
                                    errTile.currentErr = index
                                }
                            }
                        }
                    }
                }
                Tile {
                    internalColor: "#ffffff"
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    KGramTreeView {
                        tree: appCore.tree
                        id: treeWiev
                        focus: true
                        anchors.fill: parent
                        visible: appCore.mode === AppCore.ModeTree
                    }

                    InfoArea {
                        visible: appCore.mode === AppCore.ModeTokens || appCore.mode === AppCore.ModeGramatic || appCore.mode === AppCore.ModeAsm
                        anchors.fill: parent
                        anchors.margins: 2
                        id: infoArea
                        text: {
                            if(appCore.mode === AppCore.ModeTokens) {
                                return appCore.tokens;
                            } else if(appCore.mode === AppCore.ModeGramatic) {
                                return appCore.gramatic;
                            } else if(appCore.mode === AppCore.ModeAsm) {
                                return appCore.asmCode;
                            } else {
                                return '';
                            }
                        }
                        subtext: {
                            if(appCore.mode === AppCore.ModeTokens) {
                                return appCore.astTokens;
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
