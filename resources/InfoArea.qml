import QtQuick 2.0
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.12


Flickable {
    id: localRoot
    clip: true
    property string text
    property string subtext

    contentWidth: localRoot.width
    contentHeight: Math.max(leftArea.implicitHeight, rightArea.implicitHeight)

    RowLayout {
        anchors.fill: parent
        TextArea {
            id: leftArea
            Layout.fillWidth: true
            Layout.fillHeight: true

            readOnly: true
            text: localRoot.text
            wrapMode: TextArea.Wrap
        }
        TextArea {
            id: rightArea
            Layout.fillWidth: true
            Layout.fillHeight: true

            visible: subtext.length > 0
            readOnly: true
            text: localRoot.subtext
            wrapMode: TextArea.Wrap
        }
    }

    ScrollBar.vertical: ScrollBar { }
}
