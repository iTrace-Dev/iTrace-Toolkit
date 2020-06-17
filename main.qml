import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs 1.2
import io.qt.examples.backend 1.0


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
            Action { text: "Export Fixation Highlighting" }
        }
        Menu {
            title: qsTr("Help")
        }
        Menu {
            title: qsTr("About")
        }
    }

    // Backend Components
    BackEnd { id: buttonHandler }
    FileImporter { id: fileImportHandler }

    // Other Components
    FileDialog {
        id: databaseFileDialog
        nameFilters: ["SQLite Files (*.db;*.db3;*.sqlite;*.sqlite3)", "All Files (*.*)"]
        onAccepted: {
            fileImportHandler.fileURL = this.fileUrl
            filesLoaded.text = fileImportHandler.getFiles()
        }
    }
    FileDialog {
        id: xmlFileDialog
        nameFilters: ["iTrace XML (*.xml)", "SrcML Files (*.xml;*.srcml)", "All Files (*.*)"]
        onAccepted: {
            fileImportHandler.fileURL = this.fileUrl
            filesLoaded.text = fileImportHandler.getFiles()
        }
    }

    //Database Tab
    Rectangle {
        id: databaseTab
        width: (parent.width - margin) / 2; height: 460;
        x: margin; y: margin + menuHeight
        color: "red"

        // Loaded Files Area
        Text {
            x: margin; y: margin
            font.bold: true
            text: "Loaded Files:"
        }
        Participant {
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
        Button {
            id: button2
            x: margin; y: 30; text: "Blue"
            onClicked: buttonHandler.value = text
        }
    }

    // Fixation Tab
    Rectangle {
        id: fixationTab
        width: 300; height: 220;
        x: 330; y: 250 + menuHeight
        color: "green"
        Text { x: 5; y: 5; text: "Fixation Data Area" }
        Button {
            id: button3
            x: margin; y: 30; text: "Green"
            onClicked: buttonHandler.value = text
        }
    }
}
