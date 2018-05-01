import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.2
import "." // QTBUG-34418, singletons require explicit import to load qmldir file

Rectangle {
    color: "#277650"
    width: 1300
    height: 1000

    property string name : "default"

    property StackView stackView

    property string headerText: "Header Title"

    property bool headerShowBackButton: true

    property Item header: ColumnLayout {
        RowLayout {
            Button {
                id: headerBackButton
                Layout.preferredHeight: 30
                Layout.preferredWidth: 30
                hoverEnabled: true
                enabled: headerShowBackButton
                opacity: headerShowBackButton ? 1.0 : 0.0
                contentItem: Image {
                    source: "backarrow.svg"
                    sourceSize.width: 30
                    sourceSize.height: 30
                    anchors.fill: parent
                }
                background: Rectangle {
                    opacity: parent.down ? 1.0 : (parent.activeFocus ? 0.5 : 0.0)
                    color: "#406288"
                    radius: 4
                    anchors.fill: parent
                }
                onHoveredChanged: {
                    if (hovered) {
                        forceActiveFocus()
                    } else {
                        focus = false
                    }
                }
                onClicked: {
                    mainView.stopTimer()
                    var page = mainView.pop()
                }
            }
            MyText {
                id: headerTitle
                text: headerText
                font.pointSize: 22
                anchors.verticalCenter: headerBackButton.verticalCenter
                Layout.leftMargin: 32
            }
        }

        Rectangle {
            color: "#cccccc"
            height: 1
            Layout.topMargin: 7
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
        spacing: 12
        anchors.fill: parent
    }

    Component.onCompleted: {
        header.parent = mainLayout
        header.Layout.leftMargin = 40
        header.Layout.topMargin = 17
        header.Layout.rightMargin = 40
        content.parent = mainLayout
        content.Layout.fillHeight = true
        content.Layout.fillWidth = true
        content.Layout.topMargin = 7
        content.Layout.leftMargin = 40
        content.Layout.rightMargin = 40
        content.Layout.bottomMargin = 10
    }
}
