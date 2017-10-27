import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

GroupBox {
    property string boxTitle: "Step Detect Threshold Conf"

    property double hmdX : 0.3
    property double hmdY : 0.37
    property double hmdZ : 0.3
    property double handX : 0.15
    property double handY : 0.15
    property double handZ : 0.15

    property double thresholdStep: 0.01

    property int keyboardUIDBase: 200

    property var setHMDX: function(x) {}
    property var setHMDY: function(y) {}
    property var setHMDZ: function(z) {}
    property var setHandX: function(x) {}
    property var setHandY: function(y) {}
    property var setHandZ: function(z) {}
    property var updateValues: function() {}

    function updateGUI() {
        hmdXInputField.text = hmdX.toFixed(2)
        hmdYInputField.text = hmdY.toFixed(2)
        hmdZInputField.text = hmdZ.toFixed(2)
        handXInputField.text = handX.toFixed(2)
        handYInputField.text = handY.toFixed(2)
        handZInputField.text = handZ.toFixed(2)
    }

    function initVars(){
        setHMDX(hmdX)
        setHMDY(hmdY)
        setHMDZ(hmdZ)
        setHandX(handX)
        setHandY(handY)
        setHandZ(handZ)
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
                text: "Step HMD X Velo:"
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignRight
                Layout.rightMargin: 12
            }

            MyText {
                text: "Step HMD Y Velo:"
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignRight
                Layout.rightMargin: 12
            }

            MyText {
                text: "Step HMD Z Velo:"
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
                id: hmdXMinusButton
                Layout.preferredWidth: 40
                text: "-"
                onClicked: {
                    var value = hmdX - thresholdStep
                    if (value < thresholdStep) {
                        value = thresholdStep
                    }
                    hmdX = value
                    setHMDX(value)
                }
            }

            MyTextField {
                id: hmdXInputField
                text: "0.1"
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
                        hmdX = val
                        setHMDX(val)
                    } 
                }
            }

            MyPushButton2 {
                id: hmdXPlusButton
                Layout.preferredWidth: 40
                text: "+"
                onClicked: {
                    var value = hmdX + thresholdStep
                    if (value > 3.0) {
                        value = 3.0
                    }
                    hmdX = value
                    setHMDX(value)
                }
            }

            MyText {
                text: " "
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignRight
                Layout.rightMargin: 12
            }

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
                text: "0.15"
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
                id: hmdZMinusButton
                Layout.preferredWidth: 40
                text: "-"
                onClicked: {
                    var value = hmdZ - thresholdStep
                    if (value < thresholdStep) {
                        value = thresholdStep
                    }
                    hmdZ = value
                    setHMDZ(value)
                }
            }

            MyTextField {
                id: hmdZInputField
                text: "0.1"
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
                        hmdZ = val
                        setHMDZ(val)
                    } 
                }
            }

            MyPushButton2 {
                id: hmdZPlusButton
                Layout.preferredWidth: 40
                text: "+"
                onClicked: {
                    var value = hmdZ + thresholdStep
                    if (value > 3.0) {
                        value = 3.0
                    }
                    hmdZ = value
                    setHMDZ(value)
                }
            }
        }

        GridLayout {
            columns: 3

            MyText {
                text: "Step Hand X Velo:"
                font.strikeout: true
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignRight
                Layout.rightMargin: 12
            }

            MyText {
                text: "Step Hand Y Velo:"
                font.strikeout: true
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignRight
                Layout.rightMargin: 12
            }

            MyText {
                text: "Step Hand Z Velo:"
                font.strikeout: true
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
                id: handXMinusButton
                Layout.preferredWidth: 40
                text: "-"
                onClicked: {
                    var value = handX - thresholdStep
                    if (value < thresholdStep) {
                        value = thresholdStep
                    }
                    handX = value
                    setHandX(value)
                }
            }

            MyTextField {
                id: handXInputField
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
                        handX = val
                        setHandX(val)
                    } 
                }
            }

            MyPushButton2 {
                id: handXPlusButton
                Layout.preferredWidth: 40
                text: "+"
                onClicked: {
                    var value = handX + thresholdStep
                    if (value > 3.0) {
                        value = 3.0
                    }
                    handX = value
                    setHandX(value)
                }
            }

            MyText {
                text: " "
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignRight
                Layout.rightMargin: 12
            }

            MyPushButton2 {
                id: handYMinusButton
                Layout.preferredWidth: 40
                text: "-"
                onClicked: {
                    var value = handY - thresholdStep
                    if (value < thresholdStep) {
                        value = thresholdStep
                    }
                    handY = value
                    setHandY(value)
                }
            }

            MyTextField {
                id: handYInputField
                text: "0.14"
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
                        handY = val
                        setHandY(val)
                    } 
                }
            }

            MyPushButton2 {
                id: handYPlusButton
                Layout.preferredWidth: 40
                text: "+"
                onClicked: {
                    var value = handY + thresholdStep
                    if (value > 3.0) {
                        value = 3.0
                    }
                    handY = value
                    setHandY(value)
                }
            }

            MyText {
                text: " "
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignRight
                Layout.rightMargin: 12
            }

            MyPushButton2 {
                id: handZMinusButton
                Layout.preferredWidth: 40
                text: "-"
                onClicked: {
                    var value = handZ - thresholdStep
                    if (value < thresholdStep) {
                        value = thresholdStep
                    }
                    handZ = value
                    setHandZ(value)
                }
            }

            MyTextField {
                id: handZInputField
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
                        handZ = val
                        setHandZ(val)
                    } 
                }
            }

            MyPushButton2 {
                id: handZPlusButton
                Layout.preferredWidth: 40
                text: "+"
                onClicked: {
                    var value = handZ + thresholdStep
                    if (value > 3.0) {
                        value = 3.0
                    }
                    handZ = value
                    setHandZ(value)
                }
            }
        }
    }
}
