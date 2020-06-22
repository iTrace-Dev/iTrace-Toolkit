import QtQuick 2.15
import QtQuick.Controls 2.15
//import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3

import Participants 1.0

ListView {
    implicitHeight: 250
    implicitWidth: 250
    clip: true // Makes it so elements outside of the listview are not visible

    model: ParticipantsModel {
        list: participants
    }

    delegate: RowLayout {
        height: 20
        CheckBox {
            id: control
            checked: model.done

            indicator.height: parent.height
            indicator.width: parent.height
            indicator.x: control.leftPadding

            contentItem: Text {
                id: checkName
                text: model.description
                leftPadding: control.indicator.width + control.spacing
                verticalAlignment: Text.AlignVCenter
                font.pointSize: 10

            }

            onClicked: model.done = checked // can do function stuff here
        }
    }
}
