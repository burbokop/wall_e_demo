import QtQuick 2.0

Rectangle {
    id: localRoot
    clip: true

    readonly property bool hidden: hideMouseArea.tileHidden

    implicitWidth: 20
    implicitHeight: 20

    MouseArea {
        id: hideMouseArea
        property bool tileHidden: false

        x: -5
        y: -5
        width: 10
        height: 10
        cursorShape: Qt.CrossCursor
        onClicked: hideMouseArea.tileHidden = !hideMouseArea.tileHidden
    }

    property color internalColor: "#E6E6E6"
    Rectangle {
        anchors.fill: parent
        anchors.margins: 2
        color: localRoot.internalColor
    }
    color: "#444444"
}
