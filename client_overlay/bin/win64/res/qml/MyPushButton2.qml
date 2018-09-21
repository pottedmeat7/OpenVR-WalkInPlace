import QtQuick 2.7
import QtQuick.Controls 2.0
import "." // QTBUG-34418, singletons require explicit import to load qmldir file

Button {
    hoverEnabled: true
    contentItem: MyText {
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        text: parent.text
        color: parent.enabled ? "#000000" : "#000000"
    }
    background: Rectangle {
        border.color: parent.down ? "#000000" : (parent.activeFocus ? "#111111" : "#222222")
        color: parent.down ? "#c0c0c0" : (parent.activeFocus ? "#c0c0c0" : "#c0c0c0")
        radius: 1
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
