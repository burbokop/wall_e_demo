import QtQuick 2.0
import QtQuick.Controls 2.12
import Km2

Flickable {
    id: localRoot

    signal hover(int pos)

    property alias hoverDelay: hover.delay
    readonly property TextArea textArea: area

    function goToPosition(pos) {
        area.cursorPosition = pos
    }

    function updatePresentation() {
        area.append("")
    }

    property alias textDocument: area.textDocument
    property alias text: area.text

    flickableDirection: Flickable.VerticalFlick

    TextArea.flickable: TextArea {
        id: area
        wrapMode: TextArea.Wrap

        TextEditHover {
            id: hover
            textEdit: area
            onHover: pos => localRoot.hover(pos)
            delay: localRoot.hoverDelay
        }
    }

    ScrollBar.vertical: ScrollBar { }
}
