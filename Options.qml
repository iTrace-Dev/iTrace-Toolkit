/********************************************************************************************************************************************************
* @file Options.qml
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

Popup {
    id: popup
    property var margin: 15
    x: margin; y: margin
    width: parent.width - 2 * margin
    height: parent.height - 2 * margin
    modal: true
    focus: true
    closePolicy: Popup.NoAutoClose | Popup.CloseOnEscape /*| Popup.CloseOnPressOutside //*/

    function enableAlgorithm(index) {
        if(index === 0) { // BASIC
            basicAlg.visible = true
            idtAlg.visible = false
            ivtAlg.visible = false
        } else if (index === 1) { // IDT
            basicAlg.visible = false
            idtAlg.visible = true
            ivtAlg.visible = false
        } else if (index === 2) { // IVT
            basicAlg.visible = false
            idtAlg.visible = false
            ivtAlg.visible = true
        }
    }

    function getSettings() {
        var rtn = algSelection.model[algSelection.currentIndex]
        if(algSelection.currentIndex === 0) {
            rtn = rtn + "-" + windowSize.text + "-" + radius.text + "-" + peak.text
        } else if (algSelection.currentIndex === 1) {
            rtn = rtn + "-" + durationWindow.text + "-" + dispersion.text + "-" + maxGazeSpan.text
        } else if (algSelection.currentIndex === 2) {
            rtn = rtn + "-" + velocity.text + "-" + duration.text
        }
        return rtn
    }

    function getIndex() {
        return algSelection.currentIndex
    }

    function getAlg() {
        return algSelection.model[getIndex()]
    }

    GridLayout {
        id: algSelectionGrid
        x: margin; y: margin
        width: parent.width - 2 * margin
        columns: 2
        Text {
            id: fixationAlgorithmLabel
            text: qsTr("Fixation Filter:")
        }
        ComboBox {
            id: algSelection
            model: ["BASIC","IDT","IVT"]
            currentIndex: 1
            onActivated: enableAlgorithm(index)
        }
    }

    Rectangle {
        id: separatorLine
        x: margin / 3; y: algSelectionGrid.y + algSelectionGrid.height + margin / 3
        color: "black"
        width: parent.width - 2 * margin / 3
        height: 2
    }

    Rectangle { // Test rectangle
        x: basicAlg.x; y: basicAlg.y
        height: basicAlg.height
        width: basicAlg.width
        color: "purple"
        visible: false
    }

    // BASIC Olsson: Window Size, Radius, Peak
    GridLayout {
        id: basicAlg
        x: margin; y: separatorLine.y + separatorLine.height + margin
        height: parent.height - 4 * margin - algSelectionGrid.height - separatorLine.height - buttonGrid.height
        width: parent.width - 2 * margin
        columns: 2
        visible: false

        Layout.bottomMargin: margin

        Text {
            id: windowSizeLabel
            text: qsTr("Window Size: ")
        }
        TextField {
            id: windowSize
            Layout.fillWidth: true
            //Int validator requires input to be a number >1 and <MAXINT
            validator: IntValidator {bottom: 1}
            readonly property string defaultVal: "4"
            text: defaultVal
        }
        Text {
            id: radiusLabel
            text: qsTr("Radius: ")
        }
        TextField {
            id: radius
            Layout.fillWidth: true
            validator: IntValidator {bottom: 1}
            readonly property string defaultVal: "35"
            text: defaultVal
        }
        Text {
            id: peakLabel
            text: qsTr("Peak Threshold: ")
        }
        TextField {
            id: peak
            Layout.fillWidth: true
            validator: IntValidator {bottom: 1}
            readonly property string defaultVal: "40"
            text: defaultVal
        }
        Item {
            // Filler element, fills the rest of the space in the layout to force the above elements to be closer to each other
            // rather than equally spaced in the entire layout space
            Layout.fillHeight: true
        }

    }

    // IDT: Duration Window, Dispersion
    GridLayout {
        id: idtAlg
        x: basicAlg.x; y: basicAlg.y
        height: basicAlg.height
        width: basicAlg.width
        columns: 2
        visible: true

        Text {
            id: durationWindowLabel
            text: qsTr("Duration Window (ms): ")
        }
        TextField {
            id: durationWindow
            Layout.fillWidth: true
            //Int validator requires input to be a number >1 and <MAXINT
            validator: IntValidator {bottom: 1}
            readonly property string defaultVal: "100"
            text: defaultVal
        }
        Text {
            id: dispersionLabel
            text: qsTr("Dispersion: ")
        }
        TextField {
            id: dispersion
            Layout.fillWidth: true
            validator: IntValidator{bottom: 1}
            readonly property string defaultVal: "125"
            text: defaultVal
        }
        Text {
            id: maxGazeSpanLabel
            text: qsTr("Maximum Gaze Span (ms): ")
        }
        TextField {
            id: maxGazeSpan
            Layout.fillWidth: true
            validator: IntValidator{bottom: 1}
            readonly property string defaultVal: "1000"
            text: defaultVal
        }
        Item {
            // Filler element, fills the rest of the space in the layout to force the above elements to be closer to each other
            // rather than equally spaced in the entire layout space
            Layout.fillHeight: true
        }
    }

    // IVT: Velocity, Duration (milliseconds)
    GridLayout {
        id: ivtAlg
        x: basicAlg.x; y: basicAlg.y
        height: basicAlg.height
        width: basicAlg.width
        columns: 2
        visible: false

        Text {
            id: velocityLabel
            text: qsTr("Velocity Threshold: ")
        }
        TextField {
            id: velocity
            Layout.fillWidth: true
            //Int validator requires input to be a number >1 and <MAXINT
            validator: IntValidator {bottom: 1}
            readonly property string defaultVal: "50"
            text: defaultVal
        }
        Text {
            id: durationLabel
            text: qsTr("Duration (milliseconds): ")
        }
        TextField {
            id: duration
            Layout.fillWidth: true
            validator: IntValidator{bottom: 1}
            readonly property string defaultVal: "80"
            text: defaultVal
        }
        Item {
            // Filler element, fills the rest of the space in the layout to force the above elements to be closer to each other
            // rather than equally spaced in the entire layout space
            Layout.fillHeight: true
        }
    }

    // Buttons
    GridLayout {
        id: buttonGrid
        x: margin; y: basicAlg.y + basicAlg.height + margin
        width: basicAlg.width
        columns: 2
        Button {
            id: closeButton
            Layout.fillWidth: true
            text: "Close"
            onClicked: options.close()
        }
        Button {
            id: resetDefaultsButton
            Layout.fillWidth: true
            text: "Reset Defaults"
            onClicked: {
                //BASIC Algorithm Reset
                windowSize.text = windowSize.defaultVal
                radius.text = radius.defaultVal
                peak.text = peak.defaultVal

                //IDT Algorithm Reset
                durationWindow.text = durationWindow.defaultVal
                dispersion.text = dispersion.defaultVal
                maxGazeSpan.text = maxGazeSpan.defaultVal

                //IVT Algorithm Reset
                velocity.text = velocity.defaultVal
                duration.text = duration.defaultVal
            }
        }
    }
}
