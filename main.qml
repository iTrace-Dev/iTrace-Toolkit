import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import io.qt.examples.backend 1.0

Window {
    property var menuHeight: 40
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

    BackEnd { id: buttonHandler }


    Rectangle {
        id: databaseTab
        width: 300; height: 460;
        x: 10; y: 10 + menuHeight
        color: "red"
        Text { x: 5; y: 5; text: "Database and XML Import Area" }
        Button {
            id: button1
            x: 15; y: 30; text: "Red"
            onClicked: buttonHandler.value = button1.text
        }
    }

    Rectangle {
        id: tokenTab
        width: 300; height: 220;
        x: 330; y: 10 + menuHeight
        color: "blue"
        Text { x: 5; y: 5; text: "Token Analysis Area" }
        Button {
            id: button2
            x: 15; y: 30; text: "Blue"
            onClicked: buttonHandler.value = button2.text
        }
    }

    Rectangle {
        id: fixationTab
        width: 300; height: 220;
        x: 330; y: 250 + menuHeight
        color: "green"
        Text { x: 5; y: 5; text: "Fixation Data Area" }
        Button {
            id: button3
            x: 15; y: 30; text: "Green"
            onClicked: buttonHandler.value = button3.text
        }
    }



}
