import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.2
import "." // QTBUG-34418, singletons require explicit import to load qmldir file

Rectangle {
    width: 1500
    height: 800
    color: myPalette.base
    anchors.left: parent.left

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
        Layout.leftMargin : 0
        width: 1500
        height: 800
        spacing: 7
    }

    Component.onCompleted: {
        content.parent = mainLayout
        content.Layout.fillHeight = true
        content.Layout.fillWidth = true
        content.Layout.topMargin = 10
        content.Layout.leftMargin = 130
        content.Layout.rightMargin = 135
        content.Layout.bottomMargin = 40
    }
}
