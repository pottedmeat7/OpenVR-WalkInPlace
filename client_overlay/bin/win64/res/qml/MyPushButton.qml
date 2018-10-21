import QtQuick 2.7
import QtQuick.Controls 2.0
import "." // QTBUG-34418, singletons require explicit import to load qmldir file

Button {
    property bool activationSoundEnabled: true
	hoverEnabled: true
	contentItem: Text {
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        text: parent.text
        color: parent.enabled ? "#000000" : "#909090"
	}
	background: Rectangle {
        color: parent.down ? "#4B916F" : (parent.activeFocus ? "#0E5B36" : "#004021")
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
