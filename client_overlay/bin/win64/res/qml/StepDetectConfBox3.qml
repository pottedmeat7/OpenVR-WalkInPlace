import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

GroupBox {
    property string boxTitle: "Game Movement Options"

    property string gameType : "touchpad (click to sprint)"

    property int keyboardUIDBase: 200

    property var setGameType: function(type) {}

    property var updateValues: function() {}

    function updateGUI() {
    }

    Layout.fillWidth: true

    label: MyText {
        leftPadding: 10
        text: parent.boxTitle
        bottomPadding: -10
    }

    background: Rectangle {
        color: "transparent"
        border.color: "#ffffff"
        radius: 8
    }

    ColumnLayout {
        anchors.fill: parent

        Rectangle {
            color: "#ffffff"
            height: 1
            Layout.fillWidth: true
            Layout.bottomMargin: 5
        }

        GridLayout {
            columns: 2 

            MyComboBox {
                id: gameTypeDialog 
                currentIndex: 0
                Layout.maximumWidth: 399
                Layout.minimumWidth: 299
                Layout.preferredWidth: 399
                Layout.fillWidth: true
                displayText: currentText
                model: ["touchpad (click to sprint)", "touchpad no click", "touchpad click only (teleport)"]
                onCurrentIndexChanged: {
                    if (currentIndex >= 0) { 
                        setGameType(currentIndex+1)
                    } 
                }
            }
        }
    }
}
