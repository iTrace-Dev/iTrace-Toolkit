import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15

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


    Rectangle {
        id: databaseTab
        width: 300; height: 460;
        x: 10; y: 10 + menuHeight
        color: "red"
        Text { x: 5; y: 5; text: "Database and XML Import Area" }
    }

    Rectangle {
        id: tokenTab
        width: 300; height: 220;
        x: 330; y: 10 + menuHeight
        color: "blue"
        Text { x: 5; y: 5; text: "Token Analysis Area" }
    }

    Rectangle {
        id: fixationTab
        width: 300; height: 220;
        x: 330; y: 250 + menuHeight
        color: "green"
        Text { x: 5; y: 5; text: "Fixation Data Area" }
    }



}
