import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

GroupBox {
    property string boxTitle: "StepDetectConf"

    property double stepAccel : 0.005
    property double stepSpeed : 0.008
    property double stepIntSec : 4.5

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
                    if (value < 0.001) {
                        value = 0.001
                    }
                    stepAccel = value
                    setStepAcceleration(value)
                }
            }

            MyTextField {
                id: stepAccelInputField
                text: "0.003"
                keyBoardUID: keyboardUIDBase
                Layout.preferredWidth: 140
                Layout.leftMargin: 10
                Layout.rightMargin: 10
                horizontalAlignment: Text.AlignHCenter
                function onInputEvent(input) {
                    var val = parseFloat(input)
                    if (!isNaN(val)) {
                        if (val < 0.0001) {
                            val = 0.0001
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
                    if (value < 0.0001) {
                        value = 0.0001
                    }
                    stepSpeed = value
                    setStepSpeed(value)
                }
            }

            MyTextField {
                id: stepSpeedInputField
                text: "0.003"
                keyBoardUID: keyboardUIDBase
                Layout.preferredWidth: 140
                Layout.leftMargin: 10
                Layout.rightMargin: 10
                horizontalAlignment: Text.AlignHCenter
                function onInputEvent(input) {
                    var val = parseFloat(input)
                    if (!isNaN(val)) {
                        if (val < 0.0001) {
                            val = 0.0001
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
                    if (value < 0.01) {
                        value = 0.01
                    }
                    stepIntSec = value
                    setStepIntSec(value)
                }
            }

            MyTextField {
                id: stepIntSecInputField
                text: "0.05"
                keyBoardUID: keyboardUIDBase
                Layout.preferredWidth: 140
                Layout.leftMargin: 10
                Layout.rightMargin: 10
                horizontalAlignment: Text.AlignHCenter
                function onInputEvent(input) {
                    var val = parseFloat(input)
                    if (!isNaN(val)) {
                        if (val < 0.01) {
                            val = 0.01
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
