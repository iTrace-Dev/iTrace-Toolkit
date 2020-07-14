import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs 1.2
import io.qt.examples.backend 1.0 // This flags as an error, but works perfectly fine


// Main iTrace_Toolkit Window
Window {
    id: main
    property var menuHeight: 40
    property var margin: 15
    visible: true
    width: 640
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
                    database.createNewDatabase();
                    // TODO - Add update participant list
                }
            }
            // Open Database File
            Action {
                text: "Open Database"
                onTriggered: {
                    database.openDatabase()
                }
            }
            // Load XML File(s) Submenu
            Menu {
                title: qsTr("Load XML")
                // Load Individual XML
                Action {
                    text: "Load XML File"
                    onTriggered: {
                        database.importXML()
                    }
                }
                // Batch Load XML From File
                Action {
                    text: "Load XML From Folder"
                    onTriggered: {
                        database.batchAddXMLFiles()
                    }
                }
            }

        }
        Menu {
            title: qsTr("Analyze")
            Action {
                text: "Generate Fixation Data"
                onTriggered: {
                    database.generateFixations(participantList.model.getModelList().getSelected())
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

    Database {
        id: database
        onTaskAdded: participantList.model.appendTask(sessionID);
        //onOutputToScreen: output.text += "\n" + text
        onOutputToScreen: {
            output.append(text)
            outputFlick.contentY += 1
        }

    }

    //Database Tab

    // TODO - Rectangle looks weird now?
    Rectangle {
        id: databaseTab
        width: (parent.width - margin) / 2; height: 460 - margin;
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

    // Token Tab
    Rectangle {
        id: tokenTab
        width: 300; height: 220;
        x: 330; y: 10 + menuHeight
        color: "blue"
        Text { x: 5; y: 5; text: "Token Analysis Area" }
    }

    // Fixation Tab
    Rectangle {
        id: fixationTab
        width: 300; height: 220;
        x: 330; y: 250 + menuHeight
        color: "green"
        Text { x: 5; y: 5; text: "Fixation Data Area" }
    }

    // Output TextArea
    Rectangle {
        id: outputTab
        width: parent.width - (margin*2); height: 90
        x: margin; y: parent.height - height - margin
        border.color: "black"
        /*ScrollView {
            anchors.fill: parent
            TextArea {
                id: output
                anchors.fill: parent
                text: "Output:"
                color: "black"
                readOnly: true
            }
        }*/
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
        }
    }
}

