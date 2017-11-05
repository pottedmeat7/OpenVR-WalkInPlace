import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

GroupBox {
    property string boxTitle: "StepDetectConf"

    property double stepIntSec : 0.07

    property double stepIntSecStep : 0.01

    property int keyboardUIDBase: 200

    property var setStepIntSec: function(sec) {}

    property var updateValues: function() {}

    function updateGUI() {
        stepIntSecInputField.text = stepIntSec.toFixed(2)
    }

    function initVars(){
        setStepIntSec(0.7)
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
            columns: 3

            MyText {
                text: "Step Time (in seconds):"                
                font.strikeout: true
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignLeft
                Layout.rightMargin: 12
            }
        }

        GridLayout {
            columns: 3
            
            MyPushButton2 {
                id: intSecMinusButton
                Layout.preferredWidth: 40
                text: "-"
                onClicked: {
                    var value = stepIntSec - stepIntSecStep
                    if (value < stepIntSecStep) {
                        value = stepIntSecStep
                    }
                    stepIntSec = value
                    setStepIntSec(value)
                }
            }

            MyTextField {
                id: stepIntSecInputField
                text: ""
                keyBoardUID: keyboardUIDBase
                Layout.preferredWidth: 140
                Layout.leftMargin: 10
                Layout.rightMargin: 10
                horizontalAlignment: Text.AlignHCenter
                function onInputEvent(input) {
                    var val = parseFloat(input)
                    if (!isNaN(val)) {
                        if (val < stepIntSecStep) {
                            val = stepIntSecStep
                        } else if (val > 2.0) {
                            val = 2
                        }
                        stepIntSec = value
                        setStepIntSec(val)
                    } 
                }
            }

            MyPushButton2 {
                id: intSecPlusButton
                Layout.preferredWidth: 40
                text: "+"
                onClicked: {
                    var value = stepIntSec + stepIntSecStep
                    if (value > 2.0) {
                        value = 2.0
                    }
                    stepIntSec = value
                    setStepIntSec(value)
                }
            }
        }
    }
}
