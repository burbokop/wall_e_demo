import QtQuick 2.0
import QtQuick.Controls 2.12


Flickable {
    id: root
    property string text

    TextArea.flickable: TextArea {
        readOnly: true
        text: root.text
        wrapMode: TextArea.Wrap
    }

    ScrollBar.vertical: ScrollBar { }
}
