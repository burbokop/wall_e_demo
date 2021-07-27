import QtQuick 2.0
import QtQuick.Controls 2.12


Flickable {
    id: root

    function loadText(text) {
        area.text = text;
    }

    function goToPosition(pos) {
        area.cursorPosition = pos
    }

    function textFragmentForError(err) {
        const b = appCore.errBegin(err)
        const e = appCore.errEnd(err)
        return area.text.substring(b, e)
    }

    property variant textDocument: area.textDocument

    flickableDirection: Flickable.VerticalFlick

    readonly property string text: area.text

    //Timer {
    //    interval: 500
    //    running: true
    //    repeat: true
    //    onTriggered: {
    //        area.update()
    //    }
    //}

    TextArea.flickable: TextArea {
        //textFormat: TextEdit.RichText
        id: area
        wrapMode: TextArea.Wrap
    }

    ScrollBar.vertical: ScrollBar { }
}
