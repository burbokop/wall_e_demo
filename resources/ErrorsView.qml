import QtQml 2.15
import QtQuick 2.15

ListView {
    id: localRoot

    signal clicked(variant err)

    property int currentErr: -1
    delegate: Rectangle {
        color: (localRoot.currentErr === index) ? '#88888888' : '#00000000'
        id: errRect
        width: parent.width
        height: (localRoot.currentErr === index) ? 48 : 24
        Text {
            text: (localRoot.currentErr === index)
                  ? modelData.toString() + '\nat fragment: ' +  codeArea.textFragmentForError(modelData)
                  : modelData.toString()
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 4
        }
        MouseArea {
            id: errMouseArea
            anchors.fill: parent
            onClicked: {
                localRoot.clicked(modelData)
                localRoot.currentErr = index
            }
        }
    }
}
