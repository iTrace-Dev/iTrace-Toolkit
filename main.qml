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
    height: 480 + menuHeight
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
                    var file = databaseCreator.saveFile()
                    database.filePath = file
                    fileImportHandler.fileURL = file
                    filesLoaded.text = fileImportHandler.getFiles()
                }
            }
            // Open Database File
            Action {
                text: "Open Database"
                onTriggered: {
                    fileImportHandler.value = text
                    databaseFileDialog.open()
                }
            }
            // Load XML File(s) Submenu
            Menu {
                title: qsTr("Load XML")
                // Load Individual XML
                Action {
                    text: "Load XML File"
                    onTriggered: {
                        fileImportHandler.value = text;
                        xmlFileDialog.open()
                    }
                }
                // Batch Load XML From File
                Action {
                    text: "Load XML From Folder"
                }
            }

        }
        Menu {
            title: qsTr("Analyze")
        }
        Menu {
            title: qsTr("Help")
        }
        Menu {
            title: qsTr("About")
        }
    }

    Database { id: database }

    // Backend Components
    Backend { id: buttonHandler }
    FileImporter { id: fileImportHandler }
    FileCreator { id: databaseCreator }

    // Other Components
    FileDialog {
        id: databaseFileDialog
        nameFilters: ["SQLite Files (*.db;*.db3;*.sqlite;*.sqlite3)", "All Files (*.*)"]
        onAccepted: {
            //database.filePath = "file:///C:/Users/Joshu/Desktop/iTrace/TestDatabase/Test.db3"
            database.filePath = fileUrl
            fileImportHandler.fileURL = this.fileUrl
            filesLoaded.text = fileImportHandler.getFiles()

        }
    }
    FileDialog {
        id: xmlFileDialog
        nameFilters: ["iTrace XML (*.xml)", "SrcML Files (*.xml;*.srcml)", "All Files (*.*)"]
        onAccepted: {
            database.addXMLFile(fileUrl)
            fileImportHandler.fileURL = this.fileUrl
            filesLoaded.text = fileImportHandler.getFiles()
        }
    }


    //Database Tab
    Rectangle {
        id: databaseTab
        width: 300; height: 460;
        x: 10; y: 10 + menuHeight
        color: "red"
        Text { x: 5; y: 5; text: "Database and XML Import Area" }


        // Loaded Files Area
        Rectangle {
            id: fileTab
            width: parent.width - 2 * margin; height: 60
            x: margin; y: parent.height - height - margin
            color: "yellow"
            Text {
                x: 5; y: 5
                font.bold: true
                text: "Loaded Files:"
            }
            Text {
                id: filesLoaded
                x: 5; y: 20
                text: "No Files Loaded"
            }
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
}
