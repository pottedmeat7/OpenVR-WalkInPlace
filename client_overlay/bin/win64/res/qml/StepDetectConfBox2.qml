import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

GroupBox {
    property string boxTitle: "Step Detect Threshold Conf"

    property double hmdXZ : 0.13
    property double hmdY : 0.14
    property double handWalk : 0.02
    property double handJog : 0.4
    property double handRun : 1.7

    property double thresholdStep: 0.01

    property int keyboardUIDBase: 200

    property var setHMDXZ: function(x) {}
    property var setHMDY: function(y) {}
    property var setHandWalk: function(w) {}
    property var setHandJog: function(j) {}
    property var setHandRun: function(r) {}
    property var updateValues: function() {}

    function updateGUI() {
        hmdXZInputField.text = hmdXZ.toFixed(2)
        hmdYInputField.text = hmdY.toFixed(2)
        handJogInputField.text = handJog.toFixed(2)
        handRunInputField.text = handRun.toFixed(2)
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
            columns: 6

            MyText {
                text: "Step HMD Y:"
                horizontalAlignment: Text.AlignHCenter
                Layout.preferredWidth: 240
            }

            MyText {
                text: " "
                Layout.preferredWidth: 100
            }

            MyText {
                text: "Step HMD XZ:"
                horizontalAlignment: Text.AlignHCenter
                Layout.preferredWidth: 240
            }

            MyText {
                text: " "
                Layout.preferredWidth: 100
            }            

            MyText {
                text: " "
                Layout.preferredWidth: 240
            }        

            MyText {
                text: " "
                Layout.preferredWidth: 100
            }        

        }

        GridLayout {
            columns: 8

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
                text: "0.19"
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
                Layout.preferredWidth: 100
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
                    setHMDXZ(value)
                }
            }

            MyTextField {
                id: hmdXZInputField
                text: "0.17"
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
                        setHMDXZ(val)
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
                    setHMDXZ(value)
                }
            }

            MyText {
                text: " "
                Layout.preferredWidth: 100
            }
        }

        GridLayout {
            columns: 6

            MyText {
                text: "Hand Walk Movement:"
                Layout.fillWidth: true
                font.strikeout: true
                horizontalAlignment: Text.AlignHCenter
                Layout.preferredWidth: 240
            }

            MyText {
                text: " "
                Layout.preferredWidth: 100
            }

            MyText {
                text: "Hand Jog Movement:"
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
                Layout.preferredWidth: 240
            }

            MyText {
                text: " "
                Layout.preferredWidth: 100
            }

            MyText {
                text: "Hand Run Movement:"
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
                Layout.preferredWidth: 240
            }

            MyText {
                text: " "
                Layout.preferredWidth: 100
            }
        }

        GridLayout {
            columns: 12

            MyPushButton2 {
                id: handWalkMinusButton
                Layout.preferredWidth: 40
                text: "-"
                onClicked: {
                    var value = et - thresholdStep
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
                    handWalk = value                    
                }
            }

            MyText {
                text: " "
                Layout.preferredWidth: 100
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
                Layout.preferredWidth: 100
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

            MyText {
                text: " "
                Layout.preferredWidth: 100
            }

        }
    }
}
