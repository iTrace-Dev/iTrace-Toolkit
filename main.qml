import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3
import io.qt.examples.backend 1.0 // This flags as an error, but works perfectly fine


// Main iTrace_Toolkit Window
Window {
    id: main
    property var menuHeight: 40
    property var margin: 15
    visible: true
    width: 320
    height: 580 + menuHeight
    maximumHeight: height
    maximumWidth: width
    minimumHeight: height
    minimumWidth: width
    title: qsTr("iTrace Toolkit")

    MenuBar {
        Menu {
            title: qsTr("File")
            Action {
                text: "Export Token Highlighting"
            }
            Action {
                text: "Export Fixation Highlighting"
            }
        }

        // Database Menu
        Menu {
            title: qsTr("Database")
            // Create New Database File
            Action {
                text: "Create New Database"
                onTriggered: {
                    databaseCreate.open()
                }
            }
            // Open Database File
            Action {
                text: "Open Database"
                onTriggered: {
                    databaseOpen.open()
                }
            }
            // Load XML File(s) Submenu
            Menu {
                title: qsTr("Load XML")
                // Load Individual XML
                Action {
                    text: "Load XML File"
                    onTriggered: {
                        xmlOpen.open()
                    }
                }
                // Batch Load XML From File
                Action {
                    text: "Load XML From Folder"
                    onTriggered: {
                        folderOpen.open()
                    }
                }
            }

        }
        Menu {
            title: qsTr("Analyze")
            Action {
                text: "Set Fixation Settings"
                onTriggered: options.open()
            }

            Action {
                text: "Generate Fixation Data"
                onTriggered: {
                    generateFixations(options.getSettings())
                }
                function generateFixations(algorithm) {


                    control.generateFixationData(participantList.model.getModelList().getSelected(),algorithm)
                }
            }
        }
        Menu {
            title: qsTr("Help")

        }
        Menu {
            title: qsTr("About")
        }
    }

    Controller {
        id: control
        // Signal Catchers
        onTaskAdded: participantList.model.appendTask(task);
        onOutputToScreen: outputFlick.myAppend("\n" + msg);
        onWarning: {
            warningDialog.title = title;
            warningDialog.text = msg;
            warningDialog.open();
        }
    }

    //Database Tab
    Rectangle {
        id: databaseTab
        width: parent.width - 2 * margin
        height: parent.height - menuHeight - outputTab.height - (3 * margin)
        x: margin; y: margin + menuHeight
        color: "red"

        // Loaded Files Area
        Text {
            x: margin; y: margin
            font.bold: true
            text: "Loaded Participants - Tasks:"
        }
        Participant {
            id: participantList
            x: margin; y: 2 * margin
            height: parent.height - 3 * margin
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 5
        }
    }

    /*
    // Token Tab
    Rectangle {
        id: tokenTab
        width: (parent.width - 3 * margin) / 2; height: (databaseTab.height - margin) / 2
        x: databaseTab.x+databaseTab.width + margin
        y: margin + menuHeight
        color: "blue"
        Text { x: 5; y: 5; text: "Token Analysis Area" }
    }

    // Fixation Tab
    Rectangle {
        id: fixationTab
        width: (parent.width - 3 * margin) / 2; height: (databaseTab.height - margin) / 2
        x: tokenTab.x; y: tokenTab.y + tokenTab.height + margin
        color: "green"

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

        ComboBox {
            id: algSelection
            x: margin; y: margin
            width: fixationTab.width - 2 * margin
            model: ["BASIC","IDT","IVT"]
            onActivated: fixationTab.enableAlgorithm(index)
        }

        // BASIC Olsson: Window Size, Radius, Peak
        GridLayout {
            id: basicAlg
            x: margin; y: 2 * margin + algSelection.height
            height: fixationTab.height - 3 * margin - algSelection.height
            width: fixationTab.width - 2 * margin
            columns: 2
            visible: true

            Text {
                id: windowSizeLabel
                text: qsTr("Window Size: ")
            }
            TextField {
                id: windowSize
                Layout.fillWidth: true
                //Int validator requires input to be a number >1 and <MAXINT
                validator: IntValidator {bottom: 1}
                text: "4"
            }
            Text {
                id: radiusLabel
                text: qsTr("Radius: ")
            }
            TextField {
                id: radius
                Layout.fillWidth: true
                validator: IntValidator {bottom: 1}
                text: "35"
            }
            Text {
                id: peakLabel
                text: qsTr("Peak Threshold: ")
            }
            TextField {
                id: peak
                Layout.fillWidth: true
                validator: IntValidator {bottom: 1}
                text: "40"
            }

        }

        // IDT: Duration Window, Dispersion
        GridLayout {
            id: idtAlg
            x: margin; y: 2 * margin + algSelection.height
            height: fixationTab.height - 3 * margin - algSelection.height
            width: fixationTab.width - 2 * margin
            columns: 2
            visible: false

            Text {
                id: durationWindowLabel
                text: qsTr("Duration Window: ")
            }
            TextField {
                id: durationWindow
                Layout.fillWidth: true
                //Int validator requires input to be a number >1 and <MAXINT
                validator: IntValidator {bottom: 1}
                text: "10"
            }
            Text {
                id: dispersionLabel
                text: qsTr("Dispersion: ")
            }
            TextField {
                id: dispersion
                Layout.fillWidth: true
                validator: IntValidator{bottom: 1}
                text: "125"
            }
        }

        // IVT: Velocity, Duration (milliseconds)
        GridLayout {
            id: ivtAlg
            x: margin; y: 2 * margin + algSelection.height
            height: fixationTab.height - 3 * margin - algSelection.height
            width: fixationTab.width - 2 * margin
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
                text: "50"
            }
            Text {
                id: durationLabel
                text: qsTr("Duration (milliseconds): ")
            }
            TextField {
                id: duration
                Layout.fillWidth: true
                validator: IntValidator{bottom: 1}
                text: "80"
            }
        }

    }
    */

    // Output TextArea
    Rectangle {
            id: outputTab
            width: parent.width - (margin*2); height: 90
            x: margin; y: parent.height - height - margin
            border.color: "black"
            Flickable {
                id: outputFlick
                anchors.fill: parent
                boundsBehavior: Flickable.StopAtBounds
                ScrollBar.vertical: ScrollBar {}
                TextArea.flickable: TextArea {
                    id: output
                    anchors.fill: parent
                    text: "Output:"
                    color: "black"
                    readOnly: true
                }
                function currPos(){
                    return outputFlick.contentY
                }

                function setPos(pos){
                    outputFlick.contentY = pos;
                }

                function getEndPos(){
                    var ratio = 1.0 - outputFlick.visibleArea.heightRatio;
                    var endPos = outputFlick.contentHeight * ratio;
                    return endPos;
                }

                function scrollToEnd(){
                    outputFlick.contentY = getEndPos();
                }

                function myAppend(text){ //TODO: Make this put a newline
                    var pos, endPos, value;

                    value = output.text + String(text);
                    // Limit value size here

                    endPos = getEndPos();
                    pos = currPos();

                    output.text = value;

                    if(pos === endPos){
                        scrollToEnd();
                    } else {
                        setPos(pos);
                    }
                }
            }
        }

    // File Dialogs
    FileDialog { // DatabaseOpen
        id: databaseOpen
        selectExisting: true
        nameFilters: ["SQLite Files (*.db3;*.db;*.sqlite;*.sqlite3)","All Files (*.*)"]
        onAccepted: {
            control.loadDatabaseFile(fileUrl)
        }
    }
    FileDialog { // DatabaseCreate
        id: databaseCreate
        selectExisting: false
        nameFilters: ["SQLite Files (*.db3;*.db;*.sqlite;*.sqlite3)","All Files (*.*)"]
        onAccepted: {
            control.saveDatabaseFile(fileUrl)
        }
    }
    FileDialog { // XMLOpen
        id: xmlOpen
        selectExisting: true
        nameFilters: ["iTrace XML (*.xml)", "SrcML Files (*.xml;*.srcml)", "All Files (*.*)"]
        onAccepted: {
            control.importXMLFile(fileUrl)
        }
    }
    FileDialog { // FolderOpen
        id: folderOpen
        selectExisting: true
        selectFolder: true
        onAccepted: {
            control.batchAddXML(fileUrl)
        }
    }

    MessageDialog {
        id: warningDialog
        title: ""
        text: ""
        icon: StandardIcon.Warning
    }

    Options {
        id: options
    }

}

