import QtQuick.Window 2.15
import QtQuick 2.0
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2

Popup {
    id: filter
    property var margin: 15 // TODO: Make this property in a config file or something that way if we want to change it we only change it in one spot
    x: margin; y: margin
    height: parent.height - 2 * margin
    width: parent.width - 2 * margin
    modal: true
    focus: true
    closePolicy: Popup.NoAutoClose | Popup.CloseOnEscape /*| Popup.CloseOnPressOutside //*/

    /* List of properties:
     * fixation_target: file name, textbox, or dropdown list of files loaded
     * source_file_line: filter which source file lines wanted, set to 0 and -1 to disable filter
     * source_file_col: filter which source file lines wanted, set to 0 and -1 to disable filter
     * token: comma separated list of tokens to be filtered
     * left_pupil_diameter: ranges from 0 to 4
     * right_pupil_diameter: ranges from 0 to 4
     * duration: Range of duration to filter, measured in milliseconds, set to 0 and -1 to disable filter
     * save/load options to/from file
     */

    GridLayout {
        x: 0; y: 0;
        height: parent.height
        width: parent.width
        columns: 1
        GridLayout { // Fixation Target
            columns: 1
            Text {
                text: "Fixation Target (leave blank to disable): "
                font.pointSize: 10
                font.bold: true
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
            }
            TextField {
                id: fixationTargetFilter
                Layout.fillWidth: true
                placeholderText: "Comma separated list of file names to filter for"
            }
        }
        GridLayout { // Token Type
            columns: 1
            Text {
                text: "Token Type (leave blank to disable): "
                font.pointSize: 10
                font.bold: true
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
            }
            TextField {
                id: fixationTokenFilter
                Layout.fillWidth: true
                placeholderText: "Comma separated list of token names to filter for"
            }
        }
        GridLayout { // Duration
            columns: 1
            Text {
                text: "Duration (ms): "
                font.pointSize: 10
                font.bold: true
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter

            }
            GridLayout {
                columns: 4
                Text {
                    text: "Min: "
                }
                TextField {
                    id: fixationDurationFilterMin
                    Layout.fillWidth: true
                    validator: IntValidator{bottom: 0}
                    readonly property string defaultVal: "0"
                    text: defaultVal
                }
                Text {
                    text: "Max: "
                }
                TextField {
                    id: fixationDurationFilterMax
                    Layout.fillWidth: true
                    // When -1, disable filter
                    validator: IntValidator{bottom:-1}
                    readonly property string defaultVal: "-1"
                    text: defaultVal
                }

            }
        }
        GridLayout { // Source File Line
            columns: 1
            Text {
                text: "Source File Line: "
                font.pointSize: 10
                font.bold: true
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter

            }
            GridLayout {
                columns: 4
                Text {
                    text: "Min: "
                }
                TextField {
                    id: fixationLineFilterMin
                    Layout.fillWidth: true
                    validator: IntValidator{bottom: 0}
                    readonly property string defaultVal: "0"
                    text: defaultVal
                }
                Text {
                    text: "Max: "
                }
                TextField {
                    id: fixationLineFilterMax
                    Layout.fillWidth: true
                    // When -1, disable filter
                    validator: IntValidator{bottom:-1}
                    readonly property string defaultVal: "-1"
                    text: defaultVal
                }

            }
        }
        GridLayout { // Source File Column
            columns: 1
            Text {
                text: "Source File Column: "
                font.pointSize: 10
                font.bold: true
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter

            }
            GridLayout {
                columns: 4
                Text {
                    text: "Min: "
                }
                TextField {
                    id: fixationColFilterMin
                    Layout.fillWidth: true
                    validator: IntValidator{bottom: 0}
                    readonly property string defaultVal: "0"
                    text: defaultVal
                }
                Text {
                    text: "Max: "
                }
                TextField {
                    id: fixationColFilterMax
                    Layout.fillWidth: true
                    // When -1, disable filter
                    validator: IntValidator{bottom:-1}
                    readonly property string defaultVal: "-1"
                    text: defaultVal
                }

            }
        }
        GridLayout { // Pupil Diameter
            columns: 1
            id: pupilLayout
            readonly property string min: "0.00"
            readonly property string max: "3.50"
            Text {
                text: "Pupil Diameters (mm)"
                font.pointSize: 10
                font.bold: true
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
            }
            GridLayout {
                columns: 2
                GridLayout {
                    columns: 1
                    id: diameterGrid
                    Text {
                        text: "Right"
                    }
                    GridLayout {
                        columns: 3
                        TextField {
                            id: rightMin
                            text: pupilLayout.min
                            implicitWidth: 50
                            onEditingFinished: {
                                fixationRightDiameterFilter.first.value = rightMin.text
                            }
                        }
                        RangeSlider {
                            id: fixationRightDiameterFilter
                            Layout.fillWidth: true
                            from: pupilLayout.min
                            to: pupilLayout.max
                            first.value: pupilLayout.min
                            second.value: pupilLayout.max
                            stepSize: 0.001
                            snapMode: RangeSlider.SnapOnRelease

                            Connections {
                                target: fixationRightDiameterFilter.first
                                function onValueChanged() {
                                    if(linkButton.toggled) {
                                        leftMin.text = fixationRightDiameterFilter.first.value.toPrecision(3)
                                        fixationLeftDiameterFilter.first.value = leftMin.text
                                    }

                                    rightMin.text = fixationRightDiameterFilter.first.value.toPrecision(3)
                                }
                            }
                            Connections {
                                target: fixationRightDiameterFilter.second
                                function onValueChanged() {
                                    if(linkButton.toggled) {
                                        leftMax.text = fixationRightDiameterFilter.second.value.toPrecision(3)
                                        fixationLeftDiameterFilter.second.value = leftMax.text
                                    }
                                    rightMax.text = fixationRightDiameterFilter.second.value.toPrecision(3)
                                }
                            }
                        }
                        TextField {
                            id: rightMax
                            text: pupilLayout.max
                            implicitWidth: 50
                            onEditingFinished: fixationRightDiameterFilter.second.value = rightMax.text
                        }
                    }
                    Text {
                        text: "Left"
                    }
                    GridLayout {
                        columns: 3
                        TextField {
                            id: leftMin
                            text: pupilLayout.min
                            implicitWidth: 50
                            validator: DoubleValidator {
                                bottom: pupilLayout.min
                                top: pupilLayout.max
                                decimals: 3
                            }
                            onEditingFinished: fixationLeftDiameterFilter.first.value = leftMin.text
                        }
                        RangeSlider {
                            id: fixationLeftDiameterFilter
                            Layout.fillWidth: true
                            from: pupilLayout.min
                            to: pupilLayout.max
                            first.value: pupilLayout.min
                            second.value: pupilLayout.max
                            stepSize: 0.001
                            snapMode: RangeSlider.SnapOnRelease

                            Connections {
                                target: fixationLeftDiameterFilter.first
                                function onValueChanged() {
                                    if(linkButton.toggled) {
                                        rightMin.text = fixationLeftDiameterFilter.first.value.toPrecision(3)
                                        fixationRightDiameterFilter.first.value = rightMin.text
                                    }
                                    leftMin.text = fixationLeftDiameterFilter.first.value.toPrecision(3)
                                }
                            }
                            Connections {
                                target: fixationLeftDiameterFilter.second
                                function onValueChanged() {
                                    if(linkButton.toggled) {
                                        rightMax.text = fixationLeftDiameterFilter.second.value.toPrecision(3)
                                        fixationRightDiameterFilter.second.value = rightMax.text
                                    }
                                    leftMax.text = fixationLeftDiameterFilter.second.value.toPrecision(3)
                                }
                            }
                        }
                        TextField {
                            id: leftMax
                            text: pupilLayout.max
                            implicitWidth: 50
                            validator: DoubleValidator {
                                bottom: pupilLayout.min
                                top: pupilLayout.max
                                decimals: 3
                            }
                            onEditingFinished: fixationLeftDiameterFilter.second.value = leftMax.text
                        }
                    }
                }


                Button {
                    id: linkButton
                    property var toggled: true
                    implicitHeight: diameterGrid.height
                    implicitWidth: 25

                    onClicked: {
                        toggled = !toggled
                    }

                    Image {
                        source: linkButton.toggled? "chain-closed.png" : "chain-open.png"
                        anchors.fill: parent
                        anchors.margins: 4
                    }
                }
            }

        }
        Text {
            text: "Output File Type: "
            font.pointSize: 10
            font.bold: true
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter

        }
        ComboBox {
            id: exportBox
            model: [".db3",".tsv",".xml",".json"]
            implicitWidth: parent.width
        }
        Text {
            text: "Import/Export SQL Files: "
            font.pointSize: 10
            font.bold: true
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
        }
        GridLayout {
            id: sqlButtonGrid
            columns: 2
            Button {
                id: exportCurrentButton
                Layout.fillWidth: true
                text: "Export Current Settings"
                onClicked: queryFileCreate.open()
            }
            Button {
                id: importSQLButton
                Layout.fillWidth: true
                text: "Load SQL File and Filter"
                onClicked: {
                    queryFileOpen.open();
                    filter.close();
                }
            }
        }

        Item {
            // Filler element, fills the rest of the space in the layout to force the above elements to be closer to each other
            // rather than equally spaced in the entire layout space
            Layout.fillHeight: true
        }
        GridLayout {
            id: buttonGrid
            columns: 2
            Button {
                id: closeButton
                Layout.fillWidth: true
                text: "Close"
                onClicked: filter.close()
            }
            Button {
                id: filterButton
                Layout.fillWidth: true
                text: "Filter"
                onClicked: {
                    filter.close()
                    control.generateQueriedData(control.generateQuery(fixationTargetFilter.text,fixationTokenFilter.text,fixationDurationFilterMin.text,fixationDurationFilterMax.text,fixationLineFilterMin.text,fixationLineFilterMax.text,fixationColFilterMin.text,fixationColFilterMax.text,rightMin.text,rightMax.text,leftMin.text,leftMax.text),exportBox.model[exportBox.currentIndex])
                }
            }
        }
    }

    FileDialog {
        id: queryFileOpen
        selectExisting: true
        nameFilters: ["Query Files (*.sql)","All Files (*.*)"]
        onAccepted: {
            control.loadQueryFile(fileUrl,exportBox.model[exportBox.currentIndex])
        }
    }
    FileDialog {
        id: queryFileCreate
        selectExisting: false
        nameFilters: ["Query Files (*.sql)","All Files (*.*)"]
        onAccepted: {
            control.saveQueryFile(control.generateQuery(fixationTargetFilter.text,fixationTokenFilter.text,fixationDurationFilterMin.text,fixationDurationFilterMax.text,fixationLineFilterMin.text,fixationLineFilterMax.text,fixationColFilterMin.text,fixationColFilterMax.text,rightMin.text,rightMax.text,leftMin.text,leftMax.text),fileUrl);
        }
    }
}
