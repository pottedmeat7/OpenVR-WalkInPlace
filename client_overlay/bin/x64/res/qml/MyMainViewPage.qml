import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.2
import "." // QTBUG-34418, singletons require explicit import to load qmldir file

Rectangle {
    color: myPalette.base
    anchors.left: parent.left
    Layout.leftMargin : 150
    width: 1200
    height: 800

    property string name : "default"

    property StackView stackView

    property Item content: Frame {
        background: Rectangle {
            color: myPalette.base
        }
        MyText {
            text: "Content"
        }
    }

    ColumnLayout {
        id: mainLayout
        spacing: 7
    }

    Component.onCompleted: {
        content.parent = mainLayout
        content.Layout.fillHeight = true
        content.Layout.fillWidth = true
        content.Layout.topMargin = 10
        content.Layout.leftMargin = 40
        content.Layout.rightMargin = 40
        content.Layout.bottomMargin = 40
    }
}
