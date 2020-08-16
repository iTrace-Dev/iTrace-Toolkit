import QtQuick.Window 2.15
import QtQuick 2.0
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3

Popup {
    id: mappingMenu
    property var margin: 15
    x: margin; y: parent.height/2
    width: parent.width - 2 * margin
    height: parent.height/2 - 2*margin;
    focus: true
    closePolicy: Popup.NoAutoClose | Popup.CloseOnEscape

}
