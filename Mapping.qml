import QtQuick.Window 2.15
import QtQuick 2.0
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2

Popup {
    id: mappingMenu
    property var margin: 15
    x: margin; y: parent.height/4
    width: parent.width - 2 * margin
    height: parent.height/2 - 2*margin;
    modal: true
    focus: true
    padding: 0
    closePolicy: Popup.NoAutoClose | Popup.CloseOnEscape

    Button {
        id: browseButton
        y: margin
        text: "Browse for srcML  Archive"
        anchors.horizontalCenter: parent.horizontalCenter
        onClicked: {
            srcmlOpen.open()
        }
    }

    Text {
        id: pathText
        x: margin; y: browseButton.y + browseButton.height + margin
        width: parent.width - 2 * margin
        readonly property string message: "Path: "
        clip: false
        text: message
        elide: Text.ElideMiddle
        function updateSelected(path) {
            tokenButton.enabled = path !== ""
            pathText.text = pathText.message + path.replace("file:///", "")
        }
    }

    CheckBox {
        id: overwriteCheck
        anchors.horizontalCenter: parent.horizontalCenter
        y: pathText.y + pathText.height + margin
        text: "Overwrite existing data?"
    }

    Button {
        id: tokenButton
        x: margin; y: overwriteCheck.y + overwriteCheck.height + margin
        enabled: false
        text: "Identify Tokens"
        anchors.horizontalCenter: parent.horizontalCenter
        onClicked: {
            mappingMenu.close()
            control.mapTokens(srcmlOpen.fileUrl, overwriteCheck.checked)
        }
    }

    FileDialog {
        id: srcmlOpen
        selectExisting: true
        nameFilters: ["srcML Archive (*.xml; *.srcml), All Files (*.*)"]
        onAccepted: {
            pathText.updateSelected(fileUrl.toString())
        }
    }
}
