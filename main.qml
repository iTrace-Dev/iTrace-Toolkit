/********************************************************************************************************************************************************
* @file main.qml
*
* @Copyright (C) 2022 i-trace.org
*
* This file is part of iTrace Infrastructure http://www.i-trace.org/.
* iTrace Infrastructure is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
* iTrace Infrastructure is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
* You should have received a copy of the GNU General Public License along with iTrace Infrastructure. If not, see <https://www.gnu.org/licenses/>.
********************************************************************************************************************************************************/

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
    property int menuHeight: 40
    property int buttonHeight: 30;
    property int margin: 15

    property string iTraceRed: "#680314"

    visible: true
    width: 420
    height: 760 //+ menuHeight
    maximumHeight: height
    maximumWidth: width
    minimumHeight: height
    minimumWidth: width
    title: qsTr("iTrace Toolkit")

    function getBottomY(obj) {
        return obj.y + obj.height;
    }

    function getRightX(obj) {
        return obj.x + obj.width;
    }

    function equalWidth(width, num) {
        var tempN = width - margin * (num+1)
        return tempN / num;
    }

    function swapDBButtons() {
//        openDatabaseButton.enabled = !openDatabaseButton.enabled;
//        createDatabaseButton.enabled = !createDatabaseButton.enabled;

        // mapTokensButton fixationSettingsButton genFixationDataButton queryFixationButton
        folderOpenButton.enabled = true;
        //xmlButton.enabled = true;
        databaseImportButton.enabled = true;
        mapTokensButton.enabled = true;
        fixationSettingsButton.enabled = true;
        genFixationDataButton.enabled = true;
        queryFixationButton.enabled = true;
    }

//    Rectangle {
//        x: 0; y: 0
//        width: main.width;
//        height: 40;
//        color: "light grey"
//    }

//    MenuBar {
//        Menu {
//            title: qsTr("File")
//            Action {
//                text: "Exit"
//                onTriggered: {
//                    main.close()
//                }
//            }
//        }

//        // Database Menu
//        Menu {
//            title: qsTr("Database")
//            // Create New Database File
//            Action {
//                text: "Create New Database"
//                onTriggered: {
//                    databaseCreate.open()
//                }
//            }
//            // Open Database File
//            Action {
//                text: "Open Database"
//                onTriggered: {
//                    databaseOpen.open()
//                }
//            }
//            Action {
//                text: "Close Database"
//                onTriggered: {
//                    control.closeDatabase()
//                    participantList.model.clearTasks()
//                }
//            }

//            // Load XML File(s) Submenu
//            Menu {
//                title: qsTr("Load XML")
//                // Load Individual XML
//                Action {
//                    text: "Load XML File"
//                    onTriggered: {
//                        xmlOpen.open()
//                    }
//                }
//                // Batch Load XML From File
//                Action {
//                    text: "Load XML From Folder"
//                    onTriggered: {
//                        folderOpen.open()
//                    }
//                }
//            }

//        }
//        Menu {
//            title: qsTr("Analyze")
//            Action {
//                text: "Fixation Settings"
//                onTriggered: options.open()
//            }
//            Action {
//                text: "Map Tokens"
//                onTriggered:  {
//                    mappingMenu.open()
////                    control.mapTokens("C:/Users/Joshua/Desktop/iTrace/data/001/cppcheck.xml")
//                }
//            }
//            Action {
//                text: "Generate Fixation Data"
//                onTriggered: {
//                    generateFixations(options.getSettings())
//                }
//                function generateFixations(algorithm) {
//                    control.generateFixationData(participantList.model.getModelList().getSelected(),algorithm)
//                }
//            }
//            Action {
//                text: "Query Fixation Data"
//                onTriggered: filter.open()
//            }
//            Action {
//                text: "Export Fixation Highlighting (WIP)"
//                onTriggered: {
//                    control.highlightFixations("C:/Users/Joshua/Desktop/iTrace/Output","C:/Users/Joshua/Desktop/iTrace/data/001/cppcheck.xml")
//                }
//            }
//        }
//        /*Menu {
//            title: qsTr("Help")

//        }
//        Menu {
//            title: qsTr("About")
//        }*/
//    }

    Controller {
        id: control
        // Signal Catchers
        onTaskAdded: participantList.model.appendTask(task);
        onOutputToScreen: outputFlick.myAppend("<font color=\""+color+"\">" + msg + "</font>\n");
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
//            loadingBar.visible = true
//            loadingBar.from = start
//            loadingBar.to = stop
            gif.visible = true;
        }
        onStopProgressBar: {
//            loadingBar.visible = false
//            loadingBar.value = 0.0
//            loadingBar.indeterminate = false
            gif.visible = false;
        }
        onSetProgressBarValue: {
            //loadingBar.value = val
        }
        onSetProgressBarToIndeterminate: {
            //loadingBar.visible = true
            //loadingBar.indeterminate = true
            gif.visible = true;
        }
    }

    //Database Tab
    Text {
        id: dataBaseTabLabel
        text: "Database"
        x: margin; y: margin
    }

    Rectangle {
        id: databaseTab
        width: parent.width - 2 * margin
//        height: parent.height - menuHeight - outputTab.height - (4 * margin) // Old Height with menubar
        height: parent.height - outputTab.height - outputTabLabel.height - analysisTab.height - analysisTabLabel.height - (5 * margin)
        x: margin; y: getBottomY(dataBaseTabLabel) // + menuHeight
        border.color: iTraceRed
        border.width: 3
//        color: "red"

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

        Button {
            id: openDatabaseButton
            height: buttonHeight; width: equalWidth(parent.width, 2)
            x: margin; y: margin;
            enabled: true;
            text: "Open Database"
            onClicked: {
                databaseOpen.open();
            }
        }

        Button {
            id: createDatabaseButton
            height: buttonHeight; width: equalWidth(parent.width, 2)
            x: getRightX(openDatabaseButton) + margin; y: margin;
            enabled: true;
            text: "Create Database"
            onClicked: {
                databaseCreate.open();
            }
        }

        // Loaded Files Area
        Text {
            id: loadedDatabaseText
            x: margin; y: getBottomY(openDatabaseButton) + margin/2;
            width: parent.width - 2 * margin
            elide: Text.ElideMiddle
            text: "No Database Is Loaded"
            color: iTraceRed
        }

        Button {
            id: folderOpenButton
            height: buttonHeight; width: equalWidth(parent.width, 2);
            x: margin; y: getBottomY(loadedDatabaseText) + margin;
            onClicked: {
                folderOpen.open();
            }

            ToolTip.visible: hovered | down
            ToolTip.text: "Select a folder to recursively upload all Core and Plugin XML files inside it"

            enabled: false

//            Image {
//                id: folderUploadImg
//                source: "folder upload.png"
//                anchors.top: parent.top
//                anchors.bottom: parent.bottom
//                anchors.left: folderButtonText.right
//                anchors.margins: 5
//                fillMode: Image.PreserveAspectFit
//            }

            Text {
                id: folderButtonText
                anchors.verticalCenter: parent.verticalCenter
                color: parent.enabled ? "black" : "grey";
//                x: parent.width/2 - width/2 - folderUploadImg.width/2 - folderUploadImg.anchors.margins/2
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Import Data From Folder"
            }
        }

        Button {
            id: databaseImportButton
            height: buttonHeight; width: equalWidth(parent.width, 2); // equalWidth(parent.width - buttonHeight, 2); // For if close button
            x: getRightX(folderOpenButton) + margin; y: getBottomY(loadedDatabaseText) + margin;
            onClicked: {
                //xmlOpen.open();
                databaseImportOpen.open();
            }

            ToolTip.visible: hovered | down
            ToolTip.text: "Select a previous iTrace Database to import"
            //ToolTip.text: "Select individual srcML or XML files to upload"

            enabled: false

            /*Text {
                id: xmlButtonText
                anchors.verticalCenter: parent.verticalCenter
                color: parent.enabled ? "black" : "grey";
//                x: parent.width/2 - width/2 - xmlUploadImg.width/2 - xmlUploadImg.anchors.margins/2
                anchors.horizontalCenter: parent.horizontalCenter;
                text: "Import From XML"
            }*/
            Text {
                id: databaseImportButtonText
                anchors.verticalCenter: parent.verticalCenter
                color: parent.enabled ? "black" : "grey";
                //x: parent.width/2 - width/2 - xmlUploadImg.width/2 - xmlUploadImg.anchors.margins/2
                anchors.horizontalCenter: parent.horizontalCenter;
                text: "Import from Database"
            }
        }

//        Button {
//            id: closeButton
//            height: buttonHeight; width: buttonHeight;
//            x: getRightX(xmlButton) + margin/2; y: getBottomY(loadedDatabaseText) + margin/2;
//            onClicked: {
//                control.closeDatabase()
//                participantList.model.clearTasks();
//                swapDBButtons();
//            }

//            ToolTip.visible: hovered | down
//            ToolTip.text: "Close currently selected database"

//            visible: false;

//            Image {
//                id: closeImg
//                source: "cross.png";
//                anchors.fill: parent
//                anchors.margins: 5
//            }
//        }

        Text {
            id: loadedParticipantsText
            x: margin; y: getBottomY(folderOpenButton) + margin;
            font.bold: true
            text: "Loaded Participants - Tasks:"
        }
        Participant {
            id: participantList
            x: margin; y: getBottomY(loadedParticipantsText) + margin/2;
            height: parent.height - getBottomY(loadedParticipantsText) - 3 * margin / 2
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: margin
        }
    }


    Text {
        id: analysisTabLabel
        text: "Analysis"
        x: margin; y: getBottomY(databaseTab) + margin;
    }

    Rectangle {
        id: analysisTab
        x: margin; y: getBottomY(analysisTabLabel)
        width: parent.width - 2 * margin; height: 3 * margin + 2 * buttonHeight
        border.color: iTraceRed
        border.width: 3

        Button {
            id: mapTokensButton
            x: margin; y: margin;
            height: buttonHeight; width: equalWidth(parent.width, 2)
            text: "Map Tokens"

            enabled: false

            onClicked: {
                mappingMenu.open()
            }
        }

        Button {
            id: fixationSettingsButton
            x: getRightX(mapTokensButton) + margin; y: margin;
            height: buttonHeight; width: equalWidth(parent.width, 2)
            text: "Fixation Settings"

            enabled: false

            onClicked: options.open()
        }

        Button {
            id: genFixationDataButton
            x: margin; y: getBottomY(mapTokensButton) + margin;
            height: buttonHeight; width: equalWidth(parent.width, 2)
            text: "Generate Fixations"

            enabled: false

            onClicked: {
                generateFixations(options.getSettings())
            }
            function generateFixations(algorithm) {
                control.generateFixationData(participantList.model.getModelList().getSelected(),algorithm)
            }
        }

        Button {
            id: queryFixationButton
            x: getRightX(genFixationDataButton) + margin; y: getBottomY(mapTokensButton) + margin;
            height: buttonHeight; width: equalWidth(parent.width, 2)
            text: "Query Fixations"

            enabled: false

            onClicked: filter.open()
        }
    }

    Text {
        id: outputTabLabel
        text: "Output"
        x: margin; y: parent.height - outputTab.height - margin - height
    }

    // Output TextArea
    Rectangle {
        id: outputTab
        width: parent.width - (margin*2); height: 90
        x: margin; y: getBottomY(outputTabLabel)//parent.height - height - margin
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
                text: ""
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
//    ProgressBar {
//        id: loadingBar
//        x: margin; y: parent.height - outputTab.height - 2*margin
//        width: parent.width - margin*2
//        value: 0.0
//        visible: false
//    }

    // File Dialogs
    FileDialog { // DatabaseOpen
        id: databaseOpen
        selectExisting: true
        nameFilters: ["SQLite Files (*.db3; *.db; *.sqlite; *.sqlite3)","All Files (*.*)"]
        onAccepted: {
            control.closeDatabase()
            participantList.model.clearTasks()
            control.loadDatabaseFile(fileUrl)
            swapDBButtons()
        }
    }
    FileDialog { // DatabaseCreate
        id: databaseCreate
        selectExisting: false
        nameFilters: ["SQLite Files (*.db3; *.db; *.sqlite; *.sqlite3)","All Files (*.*)"]
        onAccepted: {
            control.closeDatabase()
            participantList.model.clearTasks()
            control.saveDatabaseFile(fileUrl)
            swapDBButtons()
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
    FileDialog {
        id: databaseImportOpen
        selectExisting: true
        nameFilters: ["SQLite Files (*.db3; *.db; *.sqlite; *.sqlite3)","All Files (*.*)"]
        onAccepted: {
            participantList.model.clearTasks()
            control.importDatabaseFile(fileUrl);
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

