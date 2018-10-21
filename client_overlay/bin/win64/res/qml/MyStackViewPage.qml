import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.2
import "." // QTBUG-34418, singletons require explicit import to load qmldir file

Rectangle {
    color: "transparent" //#277650"
    width: 1200
    height: 1000

    property string name : "default"

    property StackView stackView

    property Item content: Frame {
        background: Rectangle {
            color: "transparent"
        }
        Text {
            text: "Content"
        }
    }

    ColumnLayout {
        id: mainLayout
        spacing: 12
        anchors.fill: parent
    }

    Component.onCompleted: {
        content.parent = mainLayout
        content.Layout.fillHeight = true
        content.Layout.fillWidth = true
        content.Layout.topMargin = 0
        content.Layout.leftMargin = 0
        content.Layout.rightMargin = 0
        content.Layout.bottomMargin = 0
    }
}
