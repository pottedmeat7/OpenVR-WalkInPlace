import QtQuick 2.7
import QtQuick.Controls 2.0
import "." // QTBUG-34418, singletons require explicit import to load qmldir file

Button {
	hoverEnabled: true

	contentItem: MyText {
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        text: parent.text
        font.pointSize: 19
        color: parent.enabled ? myPalette.buttonText : "#737373" 
	}

	background: Rectangle {
        color: parent.activeFocus ? myPalette.highlight : "transparent"
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
