import QtQuick 2.0
import QtQuick.Controls 2.12


Flickable {
    id: localRoot

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
    }

    ScrollBar.vertical: ScrollBar { }
}
