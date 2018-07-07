import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.2
import "." // QTBUG-34418, singletons require explicit import to load qmldir file

Rectangle {
    color: "#277650"
    width: 1200
    height: 800

    property string name : "default"

    property StackView stackView

    property string headerText: "Header Title"

    property bool headerShowBackButton: true

    property Item header: ColumnLayout {
        RowLayout {
            MyText {
                id: headerTitle
                text: headerText
                font.pointSize: 24
            }
        }

        Rectangle {
            color: "#cccccc"
            height: 1
            Layout.topMargin: 6
            Layout.fillWidth: true
        }
    }

    property Item content: Frame {
        MyText {
            text: "Content"
        }
    }

    ColumnLayout {
        id: mainLayout
        spacing: 7
        anchors.fill: parent
    }

    Component.onCompleted: {
        header.parent = mainLayout
        header.Layout.leftMargin = 40
        header.Layout.topMargin = 7
        header.Layout.rightMargin = 40
        content.parent = mainLayout
        content.Layout.fillHeight = true
        content.Layout.fillWidth = true
        content.Layout.topMargin = 2
        content.Layout.leftMargin = 40
        content.Layout.rightMargin = 40
        content.Layout.bottomMargin = 40
    }
}
