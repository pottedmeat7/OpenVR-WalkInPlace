import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

GroupBox {
    property string boxTitle: "StepDetectConf"

    property double stepAccel : 0.17
    property double stepSpeed : 0.002
    property double stepIntSec : 5.3

    property double stepAccelStep: 0.001
    property double stepSpeedStep: 0.001
    property double stepIntSecStep : 0.02

    property int keyboardUIDBase: 200

    property var setStepAcceleration: function(acc) {}
    property var setStepSpeed: function(speed) {}
    property var setStepIntSec: function(sec) {}

    property var updateValues: function() {}

    function updateGUI() {
        stepAccelInputField.text = stepAccel.toFixed(4)
        stepSpeedInputField.text = stepSpeed.toFixed(4)
        stepIntSecInputField.text = stepIntSec.toFixed(2)
    }

    function initVars(){
        setStepAcceleration(stepAccel)
        setStepSpeed(stepSpeed)
        setStepIntSec(stepIntSec)
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
                text: "Step Acceleration:"
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignRight
                Layout.rightMargin: 12
            }


            MyText {
                text: "Step Speed:"
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignRight
                Layout.rightMargin: 12
            }


            MyText {
                text: "Integration Seconds:"
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignRight
                Layout.rightMargin: 12
            }
        }

        GridLayout {
            columns: 12


            MyText {
                text: " "
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignRight
                Layout.rightMargin: 12
            }

            MyPushButton2 {
                id: accelMinusButton
                Layout.preferredWidth: 40
                text: "-"
                onClicked: {
                    var value = stepAccel - stepAccelStep
                    if (value < stepAccelStep) {
                        value = stepAccelStep
                    }
                    stepAccel = value
                    setStepAcceleration(value)
                }
            }

            MyTextField {
                id: stepAccelInputField
                text: stepAccel
                keyBoardUID: keyboardUIDBase
                Layout.preferredWidth: 140
                Layout.leftMargin: 10
                Layout.rightMargin: 10
                horizontalAlignment: Text.AlignHCenter
                function onInputEvent(input) {
                    var val = parseFloat(input)
                    if (!isNaN(val)) {
                        if (val < stepSpeedStep) {
                            val = stepSpeedStep
                        } else if (val > 10.0) {
                            val = 10
                        }
                        stepAccel = val
                        setStepAcceleration(val)
                    } 
                }
            }

            MyPushButton2 {
                id: accelPlusButton
                Layout.preferredWidth: 40
                text: "+"
                onClicked: {
                    var value = stepAccel + stepAccelStep
                    if (value > 10.0) {
                        value = 10.0
                    }
                    stepAccel = value
                    setStepAcceleration(value)
                }
            }

            MyText {
                text: " "
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignRight
                Layout.rightMargin: 12
            }
            

            MyPushButton2 {
                id: speedMinusButton
                Layout.preferredWidth: 40
                text: "-"
                onClicked: {
                    var value = stepSpeed - stepSpeedStep
                    if (value < stepSpeedStep) {
                        value = stepSpeedStep
                    }
                    stepSpeed = value
                    setStepSpeed(value)
                }
            }

            MyTextField {
                id: stepSpeedInputField
                text: stepSpeed
                keyBoardUID: keyboardUIDBase
                Layout.preferredWidth: 140
                Layout.leftMargin: 10
                Layout.rightMargin: 10
                horizontalAlignment: Text.AlignHCenter
                function onInputEvent(input) {
                    var val = parseFloat(input)
                    if (!isNaN(val)) {
                        if (val < stepSpeedStep) {
                            val = stepSpeedStep
                        } else if (val > 10.0) {
                            val = 10
                        }
                        stepSpeed = value
                        setStepSpeed(val)
                    } 
                }
            }

            MyPushButton2 {
                id: speedPlusButton
                Layout.preferredWidth: 40
                text: "+"
                onClicked: {
                    var value = stepSpeed + stepSpeedStep
                    if (value > 10.0) {
                        value = 10.0
                    }
                    stepSpeed = value
                    setStepSpeed(value)
                }
            }
            
            MyText {
                text: " "
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignRight
                Layout.rightMargin: 12
            }

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
                text: stepIntSec
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
                        } else if (val > 10.0) {
                            val = 10
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
                    if (value > 10.0) {
                        value = 10.0
                    }
                    stepIntSec = value
                    setStepIntSec(value)
                }
            }
        }
    }
}
