import QtQuick 2.7
import QtQuick.Controls 2.0
import "." // QTBUG-34418, singletons require explicit import to load qmldir file


CheckBox {
	hoverEnabled: true
	spacing: 12

	indicator: Rectangle {
		implicitWidth: 28
		implicitHeight: 28
		x: parent.leftPadding
		y: parent.height / 2 - height / 2
		color: parent.enabled ? (parent.down ? "#e0e0e0" : "#000000") : "#a0a0a0"
		border.width: 0
		Image {
			width: 38
			height: 38
			x: (parent.width - width) / 2
			y: (parent.height - height) / 2
			source: "image://default/check/#004021"
			sourceSize.width: width
			sourceSize.height: height
			visible: parent.parent.checked
		}
	}

	contentItem: Text {
        text: parent.text
        horizontalAlignment: Text.AlignLeft
		verticalAlignment: Text.AlignVCenter
        leftPadding: parent.indicator.width + parent.spacing
        color: parent.enabled ? "#000000" : "#909090"
	}

	background: Rectangle {
		color: "#004021"
        opacity: parent.activeFocus ? 1.0 : 0.0
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
