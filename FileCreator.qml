import QtQuick.Window 2.15
import QtQuick 2.0
import QtQuick.Controls 2.15
import io.qt.examples.backend 1.0

/*//////////////////////////////////////////////////////////////////////
CURRENTLY UNUSED - DEPRECIATED
//////////////////////////////////////////////////////////////////////*/


/* Component for creating new Database files.
TODO:
    -Add support for saving as multiple different SQLite file types
*/

Window {
    id: creator
    width: 200; height: 200
    visible: true

    FileCreator {
        id: databaseCreator
    }

    TextField {
        id: fileNameBox
        x: 10; y: 10; width: parent.height - (2*x)
        placeholderText: qsTr("Enter file name")
    }

    TextField {
        id: extBox
        x: 10; y: 30;
        width: parent.height - (2*x)
        placeholderText: qsTr("Enter file extension (no period)")
    }


    Button {
        text: "Save New Database"
        onClicked: {
            //databaseCreator.fileExtension = "." + extBox.text
            //databaseCreator.fileName = fileNameBox.text
            databaseCreator.saveFile()
            creator.close()
        }
        anchors.centerIn: parent
    }




}
