/********************************************************************************************************************************************************
* @file Mapping.qml
*
* @Copyright (C) 2022 i-trace.org
*
* This file is part of iTrace Infrastructure http://www.i-trace.org/.
* iTrace Infrastructure is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
* iTrace Infrastructure is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
* You should have received a copy of the GNU General Public License along with iTrace Infrastructure. If not, see <https://www.gnu.org/licenses/>.
********************************************************************************************************************************************************/

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
    closePolicy: Popup.NoAutoClose | Popup.CloseOnEscape | Popup.CloseOnPressOutside

    Button {
        id: browseButton
        y: margin
        text: "Browse for srcML Archive"
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
        nameFilters: ["srcML Archive (*.xml; *.srcml)", "All Files (*.*)"]
        onAccepted: {
            pathText.updateSelected(fileUrl.toString())
        }
    }
}
