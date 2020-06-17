import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

import Participants 1.0

ListView {
    implicitHeight: 250
    implicitWidth: 250
    clip: true // Makes it so elements outside of the listview are not visible

    model: ParticipantsModel {}

    delegate: RowLayout {
        height: 15
        CheckBox {
            checked: model.done
            text: model.description

            indicator.height: parent.height // Sets size of the checkbox part of the checkbox
            indicator.width: parent.height

            onClicked: model.done = checked // can do function stuff here
        }
    }
}
