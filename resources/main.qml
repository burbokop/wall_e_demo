import QtQuick 2.0
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import KLib 1.0


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
    }
    Component.onCompleted: {
        codeArea.loadText(appCore.code);
    }

    Connections {
        target: appCore.executor
        function onMessage(mess, err) {
            execArea.pushMessage(mess, err);
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
            ColumnLayout {
                width: 300
                Layout.fillHeight: true
                Tile {
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
                                return "";
                            }
                        }
                    }

                    ExecArea {
                        id: execArea
                        anchors.fill: parent
                        anchors.margins: 2
                        visible: appCore.mode === AppCore.ModeExec
                    }
                }
            }
        }
    }
}
