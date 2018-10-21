import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

GroupBox {
    property string boxTitle: "Virtual Input"


    property double walkTouch : 0.35
    property double jogTouch : 0.9
    property double runTouch : 1.0
    property bool scaleTouch : false
    property bool useContDirForStraf: false
    property bool useContDirForRev: false

    property double thresholdStep: 0.01

    property int keyboardUIDBase: 120

    property var setWalkTouch: function(w) {}
    property var setJogTouch: function(j) {}
    property var setRunTouch: function(r) {}
    property var setScaleTouch: function(c) {}
    property var setUseContDirForStraf: function(val) {}
    property var setUseContDirForRev: function(val) {}
    property var updateValues: function() {}

    function updateGUI() {
        walkTouchInputField.text = walkTouch.toFixed(2)
        jogTouchInputField.text = jogTouch.toFixed(2)
        runTouchInputField.text = runTouch.toFixed(2)
        useContDirForStrafCheck.checked = useContDirForStraf
        useContDirForRevCheck.checked = useContDirForRev
    }

    Layout.fillWidth: true


    ColumnLayout {
        anchors.fill: parent
        Layout.alignment: Qt.AlignHCenter

        GridLayout {
            columns: 6

            Text {
                text: "Touch Walk Value"
                horizontalAlignment: Text.AlignHLeft
                font.pointSize: 15
                Layout.preferredWidth: 230
            }

            Text {
                text: "Touch Jog Value"
                horizontalAlignment: Text.AlignHCenter
                font.pointSize: 15
                Layout.preferredWidth: 230
            }

            Text {
                text: "Touch Run Value"
                horizontalAlignment: Text.AlignHCenter
                font.pointSize: 15
                Layout.preferredWidth: 230
            }

            Text {
                text: " "
                horizontalAlignment: Text.AlignHCenter
                Layout.preferredWidth: 230
            }
            Text {
                text: " "
                horizontalAlignment: Text.AlignHCenter
                Layout.preferredWidth: 90
            }
            Text {
                text: " "
                horizontalAlignment: Text.AlignHCenter
                Layout.preferredWidth: 90
            }

        }

        GridLayout {
            columns: 12

            Button {
                id: walkTouchMinusButton
                Layout.preferredWidth: 40
                text: "-"
                onClicked: {
                    var value = walkTouch - thresholdStep
                    if (value < -1) {
                        value = -1
                    }
                    walkTouch = value
                    setWalkTouch(value)
                }
            }

            MyTextField {
                id: walkTouchInputField
                text: "0.35"
                keyBoardUID: 120
                Layout.preferredWidth: 140
                horizontalAlignment: Text.AlignHCenter
                function onInputEvent(input) {
                    var val = parseFloat(input)
                    if (!isNaN(val)) {
                        if (val < -1) {
                            val = -1
                        } else if (val > 1.0) {
                            val = 1.0
                        }
                        walkTouch = val
                        setWalkTouch(val)
                    } 
                }
            }

            Button {
                id: walkTouchPlusButton
                Layout.preferredWidth: 40
                text: "+"
                onClicked: {
                    var value = walkTouch + thresholdStep
                    if (value > 1.0) {
                        value = 1.0
                    }
                    walkTouch = value  
                    setWalkTouch(value)                  
                }
            }

            Button {
                id: jogTouchMinusButton
                Layout.preferredWidth: 40
                text: "-"
                onClicked: {
                    var value = jogTouch - thresholdStep
                    if (value < -1) {
                        value = -1
                    }
                    jogTouch = value
                    setJogTouch(value)
                }
            }

            MyTextField {
                id: jogTouchInputField
                text: "0.9"
                keyBoardUID: 121
                Layout.preferredWidth: 140
                horizontalAlignment: Text.AlignHCenter
                function onInputEvent(input) {
                    var val = parseFloat(input)
                    if (!isNaN(val)) {
                        if (val < -1) {
                            val = -1
                        } else if (val > 1.0) {
                            val = 1.0
                        }
                        jogTouch = val
                        setJogTouch(val)
                    } 
                }
            }

            Button {
                id: jogTouchPlusButton
                Layout.preferredWidth: 40
                text: "+"
                onClicked: {
                    var value = jogTouch + thresholdStep
                    if (value > 1.0) {
                        value = 1.0
                    }
                    jogTouch = value
                    setJogTouch(value)
                }
            }

            Button {
                id: runTouchMinusButton
                Layout.preferredWidth: 40
                text: "-"
                onClicked: {
                    var value = runTouch - thresholdStep
                    if (value < -1) {
                        value = -1
                    }
                    runTouch = value
                    setRunTouch(value)
                }
            }

            MyTextField {
                id: runTouchInputField
                text: "1.0"
                keyBoardUID: 122
                Layout.preferredWidth: 140
                horizontalAlignment: Text.AlignHCenter
                function onInputEvent(input) {
                    var val = parseFloat(input)
                    if (!isNaN(val)) {
                        if (val < -1) {
                            val = -1
                        } else if (val > 1.0) {
                            val = 1.0
                        }
                        runTouch = val
                        setRunTouch(val)
                    } 
                }
            }

            Button {
                id: runTouchPlusButton
                Layout.preferredWidth: 40
                text: "+"
                onClicked: {
                    var value = runTouch + thresholdStep
                    if (value > 1.0) {
                        value = 1.0
                    }
                    runTouch = value
                    setRunTouch(value)
                }
            }

            CheckBox {
                id: enableScaleTouch
                text: "Scale Touch with Arm Swing?"
                Layout.maximumWidth: 300
                Layout.minimumWidth: 300
                Layout.preferredWidth: 300
                Layout.fillWidth: true
                onCheckedChanged: {
                    setScaleTouch(checked)
                }
            }

            Text {
                text: " "
                Layout.preferredWidth: 20
            }

            Text {
                text: " "
                Layout.preferredWidth: 20
            }

        }


        GridLayout {
            columns: 1

            Text {
                text: "Experimental Features"
                horizontalAlignment: Text.AlignHLeft
                font.pointSize: 15
                Layout.preferredWidth: 850
            }

        }

        GridLayout {
            columns: 2


            CheckBox {
                id: useContDirForStrafCheck
                font.pointSize: 15
                text: "Use Controller Direction For Straffing?"
                Layout.fillWidth: false
                Layout.preferredWidth: 550
                onCheckedChanged: {
                    setUseContDirForStraf(checked)
                }
            }

            CheckBox {
                id: useContDirForRevCheck
                text: "Use Controller Direction For Reverse?"
                font.pointSize: 15
                Layout.fillWidth: false
                Layout.preferredWidth: 550
                onCheckedChanged: {
                    setUseContDirForRev(checked)
                }
            }

        }
    }
}
