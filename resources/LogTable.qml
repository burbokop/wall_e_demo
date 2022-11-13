import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import Km2

Item {
    id: localRoot
    property alias log: tableModel.log
    property real minColumnWidth: 100

    ColumnLayout {
        anchors.fill: parent
        spacing: 0
        HorizontalHeaderView {
            id: horizontalHeader
            syncView: tableView
            Layout.fillWidth: true
            model: tableModel.headerModel
            clip: true
        }

        TableView {
            id: tableView
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: LogTableModel { id: tableModel }
            clip: true

            //Rectangle {
            //    anchors.fill: parent
            //    color: 'black'
            //}

            rowSpacing: 1
            columnSpacing: 1

            delegate: Rectangle {
                implicitWidth: Math.max(txt.implicitWidth, localRoot.minColumnWidth) + txt.anchors.leftMargin + txt.anchors.rightMargin
                implicitHeight: txt.implicitHeight + txt.anchors.topMargin + txt.anchors.bottomMargin
                color: '#dedddd'
                Item {
                    anchors.fill: parent
                    clip: true
                    Text {
                        id: txt
                        color: textColor
                        anchors.fill: parent
                        anchors.margins: 4
                        text: display
                    }
                }

                Text {
                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.margins: 1
                    visible: hover != undefined && hover.length > 0
                    text: '*'
                }

                MouseArea {
                    id: mouseArea
                    hoverEnabled: true
                    anchors.fill: parent
                    onContainsMouseChanged: {
                        if(mouseArea.containsMouse) {
                            const pos = mouseArea.mapToItem(localRoot, mouseArea.width, 0);
                            hoverRect.x = pos.x
                            hoverRect.y = pos.y
                            hoverText.text = hover
                        }
                    }
                }
            }
        }

    }

    Rectangle {
        id: hoverRect
        implicitWidth: hoverText.implicitWidth + hoverText.anchors.leftMargin + hoverText.anchors.rightMargin
        implicitHeight: hoverText.implicitHeight + hoverText.anchors.topMargin + hoverText.anchors.bottomMargin
        radius: 2
        border.width: 1
        //color: '#bf7130'
        visible: hoverText.text.length > 0
        Text {
            anchors.margins: 4
            anchors.fill: parent
            id: hoverText
        }
    }

    ToolBarChackBox {
        id: unimportentVisibleChackBox
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.rightMargin: 5
        anchors.topMargin: horizontalHeader.height + 5

        text: 'U'
        onCheckedChanged: tableModel.unimportentVisible = unimportentVisibleChackBox.checked
    }
}
