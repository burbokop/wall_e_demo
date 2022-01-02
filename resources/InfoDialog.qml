import QtQuick 2.0
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    color: "#44666644"

    property string text

    function openWithMessage(message) {
        root.visible = true
        root.text = message
    }

    readonly property int internalMargins: 32

    Rectangle {
        width: textItem.contentWidth + root.internalMargins * 2
        height: textItem.contentHeight + button.height + root.internalMargins * 2
        anchors.centerIn: parent
        color: "#aaaa00"

        ColumnLayout {
            anchors.fill: parent
            Text {
                id: textItem
                text: root.text
            }

            ToolBarButton {
                id: button

                Layout.fillWidth: true
                text: "Close"
                onClicked: root.visible = false
            }
        }
    }
}
