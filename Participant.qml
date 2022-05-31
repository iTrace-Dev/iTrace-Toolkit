/********************************************************************************************************************************************************
* @file Participant.qml
*
* @Copyright (C) 2022 i-trace.org
*
* This file is part of iTrace Infrastructure http://www.i-trace.org/.
* iTrace Infrastructure is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
* iTrace Infrastructure is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
* You should have received a copy of the GNU General Public License along with iTrace Infrastructure. If not, see <https://www.gnu.org/licenses/>.
********************************************************************************************************************************************************/

import QtQuick 2.15
import QtQuick.Controls 2.15
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
