import QtQml 2.15
import QtQuick 2.0

ListView {
    id: localRoot
    property string highlightedName

    signal choosed(string name)

    property var iwlist: []
    delegate: Text {
        id: txt
        Component.onCompleted: iwlist.push(txt.implicitWidth)
        color: localRoot.highlightedName === modelData ? '#8B008B' : '#000000'
        text: modelData
        font.underline: mouseArea.containsMouse
        MouseArea {
            id: mouseArea;
            anchors.fill: parent;
            hoverEnabled: true
            onClicked: localRoot.choosed(modelData)
        }
    }
    Component.onCompleted: localRoot.implicitWidth = iwlist.reduce((a, b) => Math.max(a, b), 0)
}
