import QtQuick 2.0
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import KLib 1.0


Rectangle {
    id: localRoot
    color: "#444444"

    function pushMessage(str, err) {
        listModel.append({ message: str, error: err });
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
                        console.log(`appCore.startExecuting: ${res}`)
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
                    color: error ? "#ff0000" : "#ffffff"
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.leftMargin: 4
                    text: message
                }
            }
        }
    }
}
