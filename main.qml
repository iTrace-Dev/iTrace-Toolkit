import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.4
import io.qt.examples.backend 1.0 // This flags as an error, but works perfectly fine


// Main iTrace_Toolkit Window
Window {
    id: main
    property var menuHeight: 40
    property var margin: 15

    property var iTraceRed: "#680314"

    visible: true
    width: 420
    height: 760 + menuHeight
    maximumHeight: height
    maximumWidth: width
    minimumHeight: height
    minimumWidth: width
    title: qsTr("iTrace Toolkit")

    Rectangle {
        x: 0; y: 0
        width: main.width;
        height: 40;
        color: "light grey"
    }

    MenuBar {
        Menu {
            title: qsTr("File")
            Action {
                text: "Exit"
                onTriggered: {
                    main.close()
                }
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
            Action {
                text: "Close Database"
                onTriggered: {
                    control.closeDatabase()
                    participantList.model.clearTasks()
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
                text: "Fixation Settings"
                onTriggered: options.open()
            }
            Action {
                text: "Map Tokens"
                onTriggered:  {
                    mappingMenu.open()
//                    control.mapTokens("C:/Users/Joshua/Desktop/iTrace/data/001/cppcheck.xml")
                }
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
            Action {
                text: "Query Fixation Data"
                onTriggered: filter.open()
            }
            Action {
                text: "Export Fixation Highlighting (WIP)"
                onTriggered: {
                    control.highlightFixations("C:/Users/Joshua/Desktop/iTrace/Output","C:/Users/Joshua/Desktop/iTrace/data/001/cppcheck.xml")
                }
            }
        }
        /*Menu {
            title: qsTr("Help")

        }
        Menu {
            title: qsTr("About")
        }*/
    }

    Controller {
        id: control
        // Signal Catchers
        onTaskAdded: participantList.model.appendTask(task);
        onOutputToScreen: outputFlick.myAppend("\n<font color=\""+color+"\">" + msg + "</font>");
        onWarning: {
            warningDialog.title = title;
            warningDialog.text = msg;
            warningDialog.open();
        }
        onDatabaseSet: {
            loadedDatabaseText.text = "Current Database: " + path;
            loadedDatabaseText.color = "green"
        }
        onDatabaseClosed: {
            loadedDatabaseText.text = "No Database Is Loaded"
            loadedDatabaseText.color = iTraceRed
            // Need to clear the participantList
        }
        onStartProgressBar: {
            loadingBar.visible = true
            loadingBar.from = start
            loadingBar.to = stop
            gif.visible = true;
        }
        onStopProgressBar: {
            loadingBar.visible = false
            loadingBar.value = 0.0
            loadingBar.indeterminate = false
            gif.visible = false;
        }
        onSetProgressBarValue: {
            loadingBar.value = val
        }
        onSetProgressBarToIndeterminate: {
            //loadingBar.visible = true
            //loadingBar.indeterminate = true
            gif.visible = true;
        }
    }

    //Database Tab
    Rectangle {
        id: databaseTab
        width: parent.width - 2 * margin
        height: parent.height - menuHeight - outputTab.height - (4 * margin)
        x: margin; y: margin + menuHeight
        border.color: iTraceRed
        border.width: 3
        //color: "red"

        Rectangle {
            id: gifRect
            x: parent.width - gif.width - margin; y: parent.height - gif.width - margin
            width: gif.width; height: gif.height
            AnimatedImage {
                id: gif
                source: "loading_128.gif"
                visible: false
            }
        }

        // Loaded Files Area
        Text {
            id: loadedDatabaseText
            x: margin; y: margin
            width: parent.width - 2 * margin
            elide: Text.ElideMiddle
            text: "No Database Is Loaded"
            color: iTraceRed
        }

        Text {
            x: margin; y: 2 * margin
            font.bold: true
            text: "Loaded Participants - Tasks:"
        }
        Participant {
            id: participantList
            x: margin; y: 3 * margin
            height: parent.height - 3 * margin
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 5
        }
    }




    // Output TextArea
    Rectangle {
        id: outputTab
        width: parent.width - (margin*2); height: 90
        x: margin; y: parent.height - height - margin
        border.color: "black"
        border.width: 3
        Flickable {
            id: outputFlick
            anchors.fill: parent
            boundsBehavior: Flickable.StopAtBounds
            ScrollBar.vertical: ScrollBar {}
            TextArea.flickable: TextArea {
                id: output
                anchors.fill: parent
                text: "Output:"//" We are <font color=\"#FDE541\">green</font> with envy"
                //color: "black"
                readOnly: true
                textFormat: Text.RichText
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
    // ProgressBar
    ProgressBar {
        id: loadingBar
        x: margin; y: parent.height - outputTab.height - 2*margin
        width: parent.width - margin*2
        value: 0.0
        visible: false
    }

    // File Dialogs
    FileDialog { // DatabaseOpen
        id: databaseOpen
        selectExisting: true
        nameFilters: ["SQLite Files (*.db3; *.db; *.sqlite; *.sqlite3)","All Files (*.*)"]
        onAccepted: {
            control.loadDatabaseFile(fileUrl)
        }
    }
    FileDialog { // DatabaseCreate
        id: databaseCreate
        selectExisting: false
        nameFilters: ["SQLite Files (*.db3; *.db; *.sqlite; *.sqlite3)","All Files (*.*)"]
        onAccepted: {
            control.saveDatabaseFile(fileUrl)
        }
    }
    FileDialog { // XMLOpen
        id: xmlOpen
        selectExisting: true
        nameFilters: ["iTrace XML (*.xml)", "SrcML Files (*.xml; *.srcml)", "All Files (*.*)"]
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

    Filter {
        id: filter
    }

    Mapping {
        id: mappingMenu
    }



}

