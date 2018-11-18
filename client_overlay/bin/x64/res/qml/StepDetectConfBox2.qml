import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

GroupBox {
    property string boxTitle: "Thresholds"

    property double hmdXZ : 0.27
    property double hmdY : 0.12
    property double trackerY : 0.10
    property double trackerXZ : 0.27
    property bool useTrackers : false
    property bool disableHMD : false
    property double handWalk : 0.02
    property double handJog : 1.1
    property double handRun : 2.1
    property double stepTime : 0.5

    property double thresholdStep : 0.01
    property double thresholdStepTime : 0.05
    property int pitchStep : 1

    property int keyboardUIDBase: 100

    property var setHMDXZ: function(x) {}
    property var setHMDY: function(y) {}
    property var setUseTrackers: function(val) {}
    property var setDisableHMD: function(val) {}
    property var setTrackerXZ: function(x) {}
    property var setTrackerY: function(y) {}
    property var setHandJog: function(j) {}
    property var setHandRun: function(r) {}
    property var setStepTime: function(t) {}
    property var updateValues: function() {}

    function updateGUI() {
        hmdYInputField.text = hmdY.toFixed(2)
        hmdXZInputField.text = hmdXZ.toFixed(2)
        trackerYInputField.text = trackerY.toFixed(2)
        trackerXZInputField.text = trackerXZ.toFixed(2)
        handJogInputField.text = handJog.toFixed(2)
        handRunInputField.text = handRun.toFixed(2)  
        useTrackersCheck.checked = useTrackers || disableHMD
        disableHMDCheck.checked = disableHMD
        stepTimeInputField.text = stepTime.toFixed(2)        
    }

    Layout.fillWidth: true

    background: Rectangle {
        color: myPalette.mid //"#c0c0c0" // "#277650"
        border.color: myPalette.mid //"#000000"
        radius: 1
    }

    ColumnLayout {
        anchors.fill: parent
        Layout.alignment: Qt.AlignHCenter

        GridLayout {
            columns: 5

            MyText {
                text: "Step HMD Y"
                horizontalAlignment: Text.AlignHLeft
                Layout.preferredWidth: 230
            }

            MyText {
                text: "Step HMD XZ"
                horizontalAlignment: Text.AlignHCenter
                Layout.preferredWidth: 230
            }


            MyText {
                text: ""
                horizontalAlignment: Text.AlignHCenter
                Layout.preferredWidth: 160
            }    
            
            MyText {
                text: "Arm Swing Jog"
                horizontalAlignment: Text.AlignHCenter
                Layout.preferredWidth: 230
            }

            MyText {
                text: "Arm Swing Run"
                horizontalAlignment: Text.AlignHCenter
                Layout.preferredWidth: 230
            }    

        }

        GridLayout {
            columns: 14
            Layout.alignment: Qt.AlignHCenter

            MyPushButton {
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
                keyBoardUID: 101
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

            MyPushButton {
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

            MyPushButton {
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
                text: "0.27"
                keyBoardUID: 102
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

            MyPushButton {
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
                text: ""
                horizontalAlignment: Text.AlignHCenter
                Layout.preferredWidth: 160
            }    

            MyPushButton {
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
                keyBoardUID: 103
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

            MyPushButton {
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

            MyPushButton {
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
                keyBoardUID: 104
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

            MyPushButton {
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
            columns: 5

            MyText {
                text: "Tracker Y"
                horizontalAlignment: Text.AlignHLeft
                Layout.preferredWidth: 230
            }

            MyText {
                text: "Tracker XZ"
                horizontalAlignment: Text.AlignHCenter
                Layout.preferredWidth: 230
            }

            MyText {
                text: "Use Trackers?"
                horizontalAlignment: Text.AlignHLeft
                Layout.preferredWidth: 160
            }

            MyText {
                text: "Disable HMD?"
                horizontalAlignment: Text.AlignHLeft
                Layout.preferredWidth: 230
            }

            MyText {
                text: "Step Time (seconds)"
                horizontalAlignment: Text.AlignHCenter
                Layout.preferredWidth: 230
            }
        }

        GridLayout {
            columns: 11

            MyPushButton {
                id: trackerYMinusButton
                Layout.preferredWidth: 40
                text: "-"
                onClicked: {
                    var value = trackerY - thresholdStep
                    if (value < thresholdStep) {
                        value = thresholdStep
                    }
                    trackerY = value
                    setTrackerY(value)
                }
            }

            MyTextField {
                id: trackerYInputField
                text: "0.09"
                keyBoardUID: 101
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
                        trackerY = val
                        setTrackerY(val)
                    } 
                }
            }

            MyPushButton {
                id: trackerYPlusButton
                Layout.preferredWidth: 40
                text: "+"
                onClicked: {
                    var value = trackerY + thresholdStep
                    if (value > 3.0) {
                        value = 3.0
                    }
                    trackerY = value
                    setTrackerY(value)
                }
            }

            MyPushButton {
                id: trackerXZMinusButton
                Layout.preferredWidth: 40
                text: "-"
                onClicked: {
                    var value = trackerXZ - thresholdStep
                    if (value < thresholdStep) {
                        value = thresholdStep
                    }
                    trackerXZ = value
                    setTrackerXZ(value)
                }
            }

            MyTextField {
                id: trackerXZInputField
                text: "0.27"
                keyBoardUID: 102
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
                        trackerXZ = val
                        setTrackerXZ(val)
                    } 
                }
            }

            MyPushButton {
                id: trackerXZPlusButton
                Layout.preferredWidth: 40
                text: "+"
                onClicked: {
                    var value = trackerXZ + thresholdStep
                    if (value > 3.0) {
                        value = 3.0
                    }
                    trackerXZ = value
                    setTrackerXZ(value)
                }
            }

            MyToggleButton {
                id: useTrackersCheck
                Layout.preferredWidth: 163
                Layout.fillWidth: false
                onCheckedChanged: {
                    setUseTrackers(checked)
                }
            }  

            MyToggleButton {
                id: disableHMDCheck
                Layout.preferredWidth: 223
                Layout.fillWidth: false
                onCheckedChanged: {
                    setDisableHMD(checked)
                }
            }

            MyPushButton {
                id: stepTimeMinusButton
                Layout.preferredWidth: 40
                text: "-"
                onClicked: {
                    var value = stepTime - thresholdStepTime
                    if (value < thresholdStepTime) {
                        value = thresholdStepTime
                    }
                    stepTime = value
                    setStepTime(value)
                }
            }

            MyTextField {
                id: stepTimeInputField
                text: "0.7"
                keyBoardUID: 105
                Layout.preferredWidth: 140
                horizontalAlignment: Text.AlignHCenter
                function onInputEvent(input) {
                    var val = parseFloat(input)
                    if (!isNaN(val)) {
                        if (val < thresholdStepTime) {
                            val = thresholdStepTime
                        } else if (val > 5.0) {
                            val = 5.0
                        }
                        stepTime = val
                        setStepTime(val)
                    } 
                }
            }

            MyPushButton {
                id: stepTimePlusButton
                Layout.preferredWidth: 40
                text: "+"
                onClicked: {
                    var value = stepTime + thresholdStepTime
                    if (value > 5.0) {
                        value = 5.0
                    }
                    stepTime = value
                    setStepTime(value)
                }
            }
        }
    }
}
