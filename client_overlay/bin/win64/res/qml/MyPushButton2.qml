import QtQuick 2.7
import QtQuick.Controls 2.0
import "." // QTBUG-34418, singletons require explicit import to load qmldir file

Button {
    hoverEnabled: true
    contentItem: Text {
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        text: parent.text
        color: parent.enabled ? "#000000" : "#909090"
    }
    background: Rectangle {
        color: parent.down ? "#00391E" : (parent.activeFocus ? "#00391E" : "#0D5834")
        border.color: parent.enabled ? "#000000" : "#909090"
        radius: 8
    }
    onHoveredChanged: {
        if (hovered) {
            forceActiveFocus()
        } else {
            focus = false
        }
    }

    onClicked: {
    }
}
