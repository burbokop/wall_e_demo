import QtQuick 2.0

Rectangle {
    id: root
    width: 26
    height: 26
    property bool checked: false
    property string text
    color: "#BF7130"

    Rectangle {
        color: root.checked ? Qt.darker("#BF7130") : "#BF7130"
        anchors.fill: parent
        anchors.margins: 4
    }
    MouseArea {
        anchors.fill: parent
        onClicked: {
            root.checked = !root.checked
        }
    }

    Text {
        color: root.checked ? "#ffffff" : "#000000"
        anchors.centerIn: parent
        text: root.text
    }
}
