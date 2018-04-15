import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

GroupBox {
    property string boxTitle: "Threshold, Accuracy, Time Config"

    property double hmdXZ : 0.27
    property double hmdY : 0.17
    property double trackerY : 0.12
    property double trackerXZ : 0.27
    property int useAccuracyButton : 5
    property bool useButtonAsToggle : false
    property bool flipButtonUse : false
    property bool useTrackers : false
    property bool disableHMD : false
    property double handWalk : 0.02
    property double handJog : 0.35
    property double handRun : 1.7
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
    property var setAccuracyButton: function(b) {}
    property var setAccuracyButtonAsToggle : function(v) {}
    property var setAccuracyButtonFlip : function(v) {}
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
        accuracyButtonDialog.currentIndex = useAccuracyButton
        useButtonAsToggleCheck.checked = useButtonAsToggle
        flipButtonUseCheck.checked = flipButtonUse
        useTrackersCheck.checked = useTrackers
        disableHMDCheck.checked = disableHMD
        stepTimeInputField.text = stepTime.toFixed(2)
    }

    Layout.fillWidth: true

    background: Rectangle {
        color: "transparent"
        border.color: "#ffffff"
        radius: 8
    }

    ColumnLayout {
        anchors.fill: parent

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
                text: ""
                horizontalAlignment: Text.AlignHCenter
                Layout.preferredWidth: 160
            }    
            
            MyText {
                text: "Arm Swing Jog:"
                horizontalAlignment: Text.AlignHCenter
                Layout.preferredWidth: 230
            }

            MyText {
                text: "Arm Swing Run:"
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
                text: ""
                horizontalAlignment: Text.AlignHCenter
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
            columns: 5

            MyText {
                text: "Tracker Y:"
                horizontalAlignment: Text.AlignHLeft
                Layout.preferredWidth: 230
            }

            MyText {
                text: "Tracker XZ:"
                horizontalAlignment: Text.AlignHCenter
                Layout.preferredWidth: 230
            }

            MyText {
                text: "Use Trackers?"
                horizontalAlignment: Text.AlignHLeft
                Layout.preferredWidth: 230
            }

            MyText {
                text: "Disable HMD?"
                horizontalAlignment: Text.AlignHLeft
                Layout.preferredWidth: 230
            }

            MyText {
                text: ""
                horizontalAlignment: Text.AlignHCenter
                Layout.preferredWidth: 160
            }    
        }

        GridLayout {
            columns: 9

            MyPushButton2 {
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

            MyPushButton2 {
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

            MyPushButton2 {
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

            MyPushButton2 {
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
                Layout.preferredWidth: 230
                Layout.fillWidth: false
                onCheckedChanged: {
                    setUseTrackers(checked)
                }
            }  

            MyToggleButton {
                id: disableHMDCheck
                Layout.preferredWidth: 220
                Layout.fillWidth: false
                onCheckedChanged: {
                    setDisableHMD(checked)
                }
            }

            MyText {
                text: " "
                Layout.preferredWidth: 160
            }
        }

        GridLayout {
            columns: 5

            MyText {
                text: "Button for Movement"
                horizontalAlignment: Text.AlignHLeft
                Layout.preferredWidth: 230
            }

            MyText {
                text: " "
                horizontalAlignment: Text.AlignHCenter
                Layout.preferredWidth: 135
            }

            MyText {
                text: "Button as Toggle?"
                horizontalAlignment: Text.AlignHLeft
                font.pointSize: 15
                Layout.preferredWidth: 170
            }    

            MyText {
                text: "Flip Button Use?"
                horizontalAlignment: Text.AlignHLeft
                font.pointSize: 15
                Layout.preferredWidth: 300
            }

            MyText {
                text: "Step Time (seconds)"
                horizontalAlignment: Text.AlignHCenter
                Layout.preferredWidth: 180
            }
        }

        GridLayout {
            columns: 8

            MyComboBox {
                id: accuracyButtonDialog 
                currentIndex: 0
                Layout.maximumWidth: 360
                Layout.minimumWidth: 360
                Layout.preferredWidth: 360
                Layout.fillWidth: true
                displayText: currentText
                model: ["grip", "touchpad touch", "touchpad click", "trigger", "App Menu", "None"]
                onCurrentIndexChanged: {
                    if (currentIndex >= 0) { 
                        setAccuracyButton(currentIndex) 
                    } 
                }
            }     

            MyToggleButton {
                id: useButtonAsToggleCheck
                Layout.preferredWidth: 40
                Layout.fillWidth: false
                onCheckedChanged: {
                    setAccuracyButtonAsToggle(checked)
                }
            }

            MyText {
                text: " "
                Layout.preferredWidth: 125
            }

            MyToggleButton {
                id: flipButtonUseCheck
                Layout.preferredWidth: 40
                Layout.fillWidth: false
                onCheckedChanged: {
                    setAccuracyButtonFlip(checked)
                }
            }

            MyText {
                text: " "
                Layout.preferredWidth: 275
            }


            MyPushButton2 {
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

            MyPushButton2 {
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
