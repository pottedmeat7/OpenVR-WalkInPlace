import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

GroupBox {
    property string boxTitle: "Step Detect Threshold Conf"

    property double hmdXZ : 0.07
    property double hmdY : 0.09
    property int useAccuracyButton : 5
    property double handWalk : 0.02
    property double handJog : 0.4
    property double handRun : 1.7

    property double thresholdStep: 0.01

    property int keyboardUIDBase: 200

    property var setHMDXZ: function(x) {}
    property var setHMDY: function(y) {}
    property var setAccuracyButton: function(b) {}
    property var setHandJog: function(j) {}
    property var setHandRun: function(r) {}
    property var updateValues: function() {}

    function updateGUI() {
        hmdXZInputField.text = hmdXZ.toFixed(2)
        hmdYInputField.text = hmdY.toFixed(2)
        handJogInputField.text = handJog.toFixed(2)
        handRunInputField.text = handRun.toFixed(2)        
        accuracyButtonDialog.currentIndex = useAccuracyButton
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
            columns: 5

            MyText {
                text: "Step HMD Y:"
                horizontalAlignment: Text.AlignHLeft
                Layout.preferredWidth: 230
            }

            MyText {
                text: "Step HMD XZ:"
                horizontalAlignment: Text.AlignHCenter
                Layout.preferredWidth: 230
            }

            MyText {
                text: " "
                horizontalAlignment: Text.AlignHCenter
                Layout.preferredWidth: 160
            }

            MyText {
                text: "Hand Jog Vel:"
                horizontalAlignment: Text.AlignHCenter
                Layout.preferredWidth: 230
            }

            MyText {
                text: "Hand Run Vel:"
                horizontalAlignment: Text.AlignHCenter
                Layout.preferredWidth: 230
            }    
        }

        GridLayout {
            columns: 14

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
                text: "0.09"
                keyBoardUID: keyboardUIDBase
                Layout.preferredWidth: 140
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
                text: "0.07"
                keyBoardUID: keyboardUIDBase
                Layout.preferredWidth: 140
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
                Layout.preferredWidth: 160
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

        GridLayout {
            columns: 14

            MyComboBox {
                id: accuracyButtonDialog 
                currentIndex: 0
                Layout.maximumWidth: 399
                Layout.minimumWidth: 299
                Layout.preferredWidth: 399
                Layout.fillWidth: true
                displayText: currentText
                model: ["grip", "touchpad touch", "touchpad click", "trigger", "App Menu", "None"]
                onCurrentIndexChanged: {
                    if (currentIndex >= 0) { 
                        setAccuracyButton(currentIndex) 
                    } 
                }
            }
        }
    }
}
