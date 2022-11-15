import QtQml 2.15
import QtQuick 2.15
import QtQuick.Controls 2.12

ListView {
    id: localRoot

    property TextArea area

    signal clicked(variant err)

    property int currentErr: -1
    delegate: Rectangle {
        color: (localRoot.currentErr === index) ? '#88888888' : '#00000000'
        id: errRect
        width: localRoot.width
        implicitHeight: txt.implicitHeight
        readonly property string substr: localRoot.area.text.substring(modelData.begin, modelData.end)
        Text {
            id: txt
            wrapMode: Text.WordWrap
            text: (localRoot.currentErr === index)
                  ? `${modelData.message}\n---------------------\nuri: ${modelData.uri}\n---------------------\n${errRect.substr}`
                  : modelData.message
            anchors.left: parent.left
            anchors.right: parent.right
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
