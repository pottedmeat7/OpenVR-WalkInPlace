import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

Popup {
    id: myDialogPopup

    implicitHeight: parent.height
    implicitWidth: parent.width

    property string dialogTitle: ""
    property string dialogText: ""
    property int dialogTO: 3
    property int timerCount: -1
    property int dialogWidth: 600
    property int dialogHeight: 300

    property Item dialogContentItem: MyText {
        text: dialogText
        font.pointSize: 24
        horizontalAlignment: Text.AlignHCenter
        Layout.fillWidth: true
    }

    property Item dialogTOItem: MyText {
        text: ""+dialogTO
        font.pointSize: 27
        horizontalAlignment: Text.AlignHCenter
        Layout.fillWidth: true
    }

    property bool cancelClicked: false
    property bool timerUp : false

    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent

    background: Rectangle {
        color: "black"
        opacity: 0.8
     }

    contentItem: Item {
        Rectangle {
            implicitWidth: dialogWidth
            implicitHeight: dialogHeight
            anchors.centerIn: parent
            radius: 24
            color: "#0E5B36"
            border.color: "#cccccc"
            border.width: 2
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 12
                MyText {
                    Layout.leftMargin: 16
                    Layout.rightMargin: 16
                    text: dialogTitle
                }
                Rectangle {
                    color: "#cccccc"
                    height: 1
                    Layout.fillWidth: true
                }
                Item {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                }
                ColumnLayout {
                    id: dialogContent
                }
                Item {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                }
                RowLayout {
                    Layout.fillWidth: true
                    Layout.leftMargin: 24
                    Layout.rightMargin: 24
                    Layout.bottomMargin: 12
                    Item {
                        Layout.fillWidth: true
                    }
                    MyPushButton {
                        implicitWidth: 200
                        text: "Cancel"
                        onClicked: {
                            cancelClicked = true
                            myDialogPopup.close()
                        }
                    }
                    Item {
                        Layout.fillWidth: true
                    }
                }
            }
        }
    }

    Timer {
        id: refreshTimer
        interval: 1000 
        running: false
        repeat: true
        onTriggered: {
            timerCount++
            if ( timerCount >= dialogTO ) {
                myDialogPopup.close()
            }
            dialogTOItem.text = "" + (dialogTO - timerCount);
        }
    }

    function stopPopupTimer() {
        dialogTOItem.text = "" + (dialogTO);
        refreshTimer.stop()
        timerCount = 0
    }

    function startPopupTimer() {
        refreshTimer.start()
    }

    Component.onCompleted: {
        dialogContentItem.parent = dialogContent
        dialogTOItem.parent = dialogContent
    }
}
