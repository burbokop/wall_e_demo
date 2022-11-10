import QtQuick 2.0
import Qt.labs.folderlistmodel
import Km2

ListView {
    id: localRoot

    signal choosed(ProjFile file)

    property ProjFile openedFile

    property var iwlist: []
    delegate: Text {
        id: txt
        Component.onCompleted: iwlist.push(txt.implicitWidth)
        text: modelData.relativePath
        font.underline: mouseArea.containsMouse
        color: localRoot.openedFile === modelData ? '#8B008B' : '#000000'
        MouseArea {
            id: mouseArea;
            anchors.fill: parent;
            hoverEnabled: true
            onClicked: localRoot.choosed(modelData)
        }
    }
    Component.onCompleted: localRoot.implicitWidth = localRoot.iwlist.reduce((a, b) => Math.max(a, b), 0)
}

