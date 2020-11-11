import QtQuick 2.0
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import KLib 1.0


Rectangle {
    color: "#444444"

    function pushMessage(str, err) {
        listModel.append({ message: str, error: err });
    }

    ColumnLayout {
        anchors.fill: parent

        Rectangle {
            color: "#26537C"
            Layout.fillWidth: true
            height: 30
            RowLayout {
                anchors.fill: parent
                ToolBarButton {
                    width: 100
                    height: 26
                    text: "run"
                    onClicked: {
                        if(!appCore.startExecuting()) {
                            emitError();
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
                height: 36
                Text {
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
