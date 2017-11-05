import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

GroupBox {
    property string boxTitle: "Step Detect Threshold Conf"

    property double hmdXZ : 0.17
    property double hmdY : 0.19
    property double handWalk : 0.02
    property double handJog : 0.4
    property double handRun : 1.7

    property double thresholdStep: 0.01

    property int keyboardUIDBase: 200

    property var setHMDXZ: function(x) {}
    property var setHMDY: function(y) {}
    property var setHandJog: function(x) {}
    property var setHandRun: function(y) {}
    property var updateValues: function() {}

    function updateGUI() {
        hmdXZInputFiel.text = hmdXZ.toFixed(2)
        hmdYInputField.text = hmdY.toFixed(2)
        handJogInputField.text = handJog.toFixed(2)
        handRunInputField.text = handRun.toFixed(2)
    }

    function initVars(){
        setHMDY(0.2)
        setHMDXZ(0.3)
        setHandJog(0.0)
        setHandRun(0.0)
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
                text: "Step HMD Y:"
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignLeft
                Layout.rightMargin: 12
            }

            MyText {
                text: "Step HMD XZ:"
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignLeft
                Layout.rightMargin: 12
            }

            MyText {
                text: " "
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignRight
                Layout.rightMargin: 12
            }

        }

        GridLayout {
            columns: 12

            MyPushButton2 {
                id: hmdYMinusButton
                Layout.preferredWidth: 40
                text: "-"
                onClicked: {
                    var value = hmdY - thresholdStep
                    if (value < thresholdStep) {
                        value = thresholdStep
                    }
                    hmdY = value
                    setHMDY(value)
                }
            }

            MyTextField {
                id: hmdYInputField
                text: "0.2"
                keyBoardUID: keyboardUIDBase
                Layout.preferredWidth: 140
                Layout.leftMargin: 10
                Layout.rightMargin: 10
                horizontalAlignment: Text.AlignHCenter
                function onInputEvent(input) {
                    var val = parseFloat(input)
                    if (!isNaN(val)) {
                        if (val < thresholdStep) {
                            val = thresholdStep
                        } else if (val > 3.0) {
                            val = 3.0
                        }
                        hmdY = val
                        setHMDY(val)
                    } 
                }
            }

            MyPushButton2 {
                id: hmdYPlusButton
                Layout.preferredWidth: 40
                text: "+"
                onClicked: {
                    var value = hmdY + thresholdStep
                    if (value > 3.0) {
                        value = 3.0
                    }
                    hmdY = value
                    setHMDY(value)
                }
            }

            MyText {
                text: " "
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignRight
                Layout.rightMargin: 12
            }

            MyPushButton2 {
                id: hmdXZMinusButton
                Layout.preferredWidth: 40
                text: "-"
                onClicked: {
                    var value = hmdXZ - thresholdStep
                    if (value < thresholdStep) {
                        value = thresholdStep
                    }
                    hmdXZ = value
                    setHMDX(value)
                }
            }

            MyTextField {
                id: hmdXZInputField
                text: "0.3"
                keyBoardUID: keyboardUIDBase
                Layout.preferredWidth: 140
                Layout.leftMargin: 10
                Layout.rightMargin: 10
                horizontalAlignment: Text.AlignHCenter
                function onInputEvent(input) {
                    var val = parseFloat(input)
                    if (!isNaN(val)) {
                        if (val < thresholdStep) {
                            val = thresholdStep
                        } else if (val > 3.0) {
                            val = 3.0
                        }
                        hmdXZ = val
                        setHMDX(val)
                    } 
                }
            }

            MyPushButton2 {
                id: hmdXZPlusButton
                Layout.preferredWidth: 40
                text: "+"
                onClicked: {
                    var value = hmdXZ + thresholdStep
                    if (value > 3.0) {
                        value = 3.0
                    }
                    hmdXZ = value
                    setHMDX(value)
                }
            }

            MyText {
                text: " "
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignRight
                Layout.rightMargin: 12
            }

            MyText {
                text: " "
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignRight
                Layout.rightMargin: 12
            }

            MyText {
                text: " "
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignRight
                Layout.rightMargin: 12
            }

            MyText {
                text: " "
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignRight
                Layout.rightMargin: 12
            }
            
            MyText {
                text: " "
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignRight
                Layout.rightMargin: 12
            }
        }

        GridLayout {
            columns: 9

            MyText {
                text: "Hand Walk Movement:"
                Layout.fillWidth: true
                font.strikeout: true
                horizontalAlignment: Text.AlignLeft
                Layout.rightMargin: 12
            }

            MyText {
                text: " "
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignRight
                Layout.rightMargin: 12
            }

            MyText {
                text: " "
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignRight
                Layout.rightMargin: 12
            }

            MyText {
                text: "Hand Jog Movement:"
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignLeft
                Layout.rightMargin: 12
            }

            MyText {
                text: " "
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignRight
                Layout.rightMargin: 12
            }

            MyText {
                text: " "
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignRight
                Layout.rightMargin: 12
            }

            MyText {
                text: "Hand Run Movement:"
                horizontalAlignment: Text.AlignRight
                Layout.rightMargin: 12
            }

            MyText {
                text: " "
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignRight
                Layout.rightMargin: 12
            }
        }

        GridLayout {
            columns: 12

            MyPushButton2 {
                id: handWalkMinusButton
                Layout.preferredWidth: 40
                text: "-"
                onClicked: {
                    var value = handWalk - thresholdStep
                    if (value < thresholdStep) {
                        value = thresholdStep
                    }
                    handWalk = value
                }
            }

            MyTextField {
                id: handWalkInputField
                text: "0.02"
                keyBoardUID: keyboardUIDBase
                Layout.preferredWidth: 140
                Layout.leftMargin: 10
                Layout.rightMargin: 10
                horizontalAlignment: Text.AlignHCenter
                function onInputEvent(input) {
                    var val = parseFloat(input)
                    if (!isNaN(val)) {
                        if (val < thresholdStep) {
                            val = thresholdStep
                        } else if (val > 3.0) {
                            val = 3.0
                        }
                        handWalk = val
                    } 
                }
            }

            MyPushButton2 {
                id: handWalkPlusButton
                Layout.preferredWidth: 40
                text: "+"
                onClicked: {
                    var value = handJog + thresholdStep
                    if (value > 3.0) {
                        value = 3.0
                    }
                    handJog = value
                }
            }

            MyText {
                text: " "
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignRight
                Layout.rightMargin: 12
            }

            MyPushButton2 {
                id: handJogMinusButton
                Layout.preferredWidth: 40
                text: "-"
                onClicked: {
                    var value = handJog - thresholdStep
                    if (value < thresholdStep) {
                        value = thresholdStep
                    }
                    handJog = value
                    setHandJog(value)
                }
            }

            MyTextField {
                id: handJogInputField
                text: "0.4"
                keyBoardUID: keyboardUIDBase
                Layout.preferredWidth: 140
                Layout.leftMargin: 10
                Layout.rightMargin: 10
                horizontalAlignment: Text.AlignHCenter
                function onInputEvent(input) {
                    var val = parseFloat(input)
                    if (!isNaN(val)) {
                        if (val < thresholdStep) {
                            val = thresholdStep
                        } else if (val > 3.0) {
                            val = 3.0
                        }
                        handJog = val
                        setHandJog(val)
                    } 
                }
            }

            MyPushButton2 {
                id: handJogPlusButton
                Layout.preferredWidth: 40
                text: "+"
                onClicked: {
                    var value = handJog + thresholdStep
                    if (value > 3.0) {
                        value = 3.0
                    }
                    handJog = value
                    setHandJog(value)
                }
            }

            MyText {
                text: " "
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignRight
                Layout.rightMargin: 12
            }

            MyPushButton2 {
                id: handRunMinusButton
                Layout.preferredWidth: 40
                text: "-"
                onClicked: {
                    var value = handRun - thresholdStep
                    if (value < thresholdStep) {
                        value = thresholdStep
                    }
                    handRun = value
                    setHandRun(value)
                }
            }

            MyTextField {
                id: handRunInputField
                text: "1.7"
                keyBoardUID: keyboardUIDBase
                Layout.preferredWidth: 140
                Layout.leftMargin: 10
                Layout.rightMargin: 10
                horizontalAlignment: Text.AlignHCenter
                function onInputEvent(input) {
                    var val = parseFloat(input)
                    if (!isNaN(val)) {
                        if (val < thresholdStep) {
                            val = thresholdStep
                        } else if (val > 5.0) {
                            val = 5.0
                        }
                        handRun = val
                        setHandRun(val)
                    } 
                }
            }

            MyPushButton2 {
                id: handRunPlusButton
                Layout.preferredWidth: 40
                text: "+"
                onClicked: {
                    var value = handRun + thresholdStep
                    if (value > 5.0) {
                        value = 5.0
                    }
                    handRun = value
                    setHandRun(value)
                }
            }

        }
    }
}
