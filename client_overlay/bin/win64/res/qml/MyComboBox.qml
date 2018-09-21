import QtQuick 2.7
import QtQuick.Controls 2.0
import "." // QTBUG-34418, singletons require explicit import to load qmldir file


ComboBox {
    id: myComboBox
    hoverEnabled: true

    background: Rectangle {
        //color: parent.pressed ? "#0E5B36" : (parent.activeFocus ? "#4B916F" : "#004021")
        border.color: parent.down ? "#000000" : (parent.activeFocus ? "#000000" : "#000000")
        color: parent.down ? "#808080" : (parent.activeFocus ? "#b0b0b0" : "#c0c0c0")
    }

    contentItem: MyText {
        leftPadding: 0
        rightPadding: parent.indicator.width + parent.spacing
        text: parent.displayText
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    delegate: ItemDelegate {
        width: myComboBox.width
        text: modelData
        hoverEnabled: true
        contentItem: MyText {
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            text: parent.text
            color: parent.enabled ? "#000000" : "#000000"
        }
        background: Rectangle {
            color: parent.pressed ? "#808080" : (parent.hovered ? "#a0a0a0" : "#b0b0b0")
        }
    }

    indicator: Canvas {
        x: parent.width - width - parent.rightPadding
        y: parent.topPadding + (parent.availableHeight - height) / 2
        width: 13
        height: 7
        contextType: "2d"

        onPaint: {
            if (!context) {
                getContext("2d")
            }
            context.reset();
            context.lineWidth = 2
            context.moveTo(1, 1);
            context.lineTo(Math.ceil(width / 2), height - 1);
            context.lineTo(width - 1, 1);
            context.strokeStyle = "#000000"
            context.stroke()
        }
    }

    onHoveredChanged: {
        if (hovered) {
            forceActiveFocus()
        }
    }

    onActivated: {
    }

    Component.onCompleted: {
        popup.background.color = "#c0c0c0"
    }
}
