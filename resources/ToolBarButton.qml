import QtQuick 2.0

Rectangle {
    id: root
    width: 40
    height: 40
    color: timer.blick ? "#F80012" : (ma.pressed ? Qt.darker("#BF7130") : "#BF7130")

    Timer {
        id: timer
        property bool blick: false
        property int blickCount: 0
        repeat: true
        interval: 100
        onTriggered: {
            timer.blick = !timer.blick;
            timer.blickCount++;
            if(blickCount > 8) {
                timer.blickCount = 0;
                timer.blick = false;
                timer.running = false;
            }
        }
    }

    function emitError() {
        timer.running = true;
    }

    property string text: ""
    signal clicked

    Text {
        anchors.centerIn: parent
        text: root.text
    }

    MouseArea {
        id: ma
        anchors.fill: parent
        onClicked: {
            root.clicked();
        }
    }
}
