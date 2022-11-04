import QtQuick 2.0
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import KLib 1.0

import Km2

Rectangle {
    id: localRoot
    color: "#444444"

    function pushMessage(mess, type, isSystem) {
        listModel.append({ message: mess, type: type, isSystem: isSystem });
    }

    signal err(string message)

    ColumnLayout {
        anchors.fill: parent

        Rectangle {
            color: "#26537C"
            Layout.fillWidth: true
            height: 30
            RowLayout {
                anchors.fill: parent
                ToolBarButton {
                    id: runButton
                    width: 100
                    height: 26
                    text: "run"
                    onClicked: {
                        const res = appCore.startExecuting();
                        if(!res.defined) {
                            runButton.emitError();
                            localRoot.err(res.left)
                        }
                    }
                }
            }
        }

        ListView {
            clip: true
            currentIndex: listModel.count - 1
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: ListModel { id: listModel }
            delegate: Item {
                width: parent.width
                implicitHeight: txt.implicitHeight
                Text {
                    id: txt
                    color: {
                        switch(type) {
                        case JitExecutor.Trace: return '#ffffff'
                        case JitExecutor.Err: return '#ff0000'
                        case JitExecutor.Warn: return '#ffff00'
                        default: return '#ffffff'
                        }
                    }
                    font.bold: isSystem
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.leftMargin: 4
                    text: message
                }
            }
        }
    }
}
