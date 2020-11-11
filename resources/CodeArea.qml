import QtQuick 2.0
import QtQuick.Controls 2.12


Flickable {
    id: root

    function loadText(text) {
        area.text = text;
    }

    property variant textDocument: area.textDocument

    flickableDirection: Flickable.VerticalFlick

    readonly property string text: area.text

    TextArea.flickable: TextArea {
        //textFormat: TextEdit.RichText
        id: area
        wrapMode: TextArea.Wrap
    }

    ScrollBar.vertical: ScrollBar { }
}
