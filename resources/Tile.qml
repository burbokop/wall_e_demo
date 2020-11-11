import QtQuick 2.0

Rectangle {
    id: root
    property color internalColor: "#E6E6E6"
    Rectangle {
        anchors.fill: parent
        anchors.margins: 2
        color: root.internalColor
    }
    color: "#444444"
}
