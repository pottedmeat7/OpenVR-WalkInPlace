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

    ColumnLayout {
        anchors.fill: parent
        Layout.alignment: Qt.AlignHCenter

        GridLayout {
            columns: 5

            Text {            
                font.pointSize: 20
                text: "Step HMD Y"
                horizontalAlignment: Text.AlignHLeft
                Layout.preferredWidth: 230
            }

            Text {            
                font.pointSize: 20
                text: "Step HMD XZ"
                horizontalAlignment: Text.AlignHCenter
                Layout.preferredWidth: 230
            }


            Text {            
                font.pointSize: 20
                text: ""
                horizontalAlignment: Text.AlignHCenter
                Layout.preferredWidth: 160
            }    
            
            Text {            
                font.pointSize: 20
                text: "Arm Swing Jog"
                horizontalAlignment: Text.AlignHCenter
                Layout.preferredWidth: 230
            }

            Text {            
                font.pointSize: 20
                text: "Arm Swing Run"
                horizontalAlignment: Text.AlignHCenter
                Layout.preferredWidth: 230
            }    

        }

        GridLayout {
            columns: 14
            Layout.alignment: Qt.AlignHCenter

            Button {
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

            Button {
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

            Button {
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

            Button {
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


            Text {
                font.pointSize: 20
                text: ""
                horizontalAlignment: Text.AlignHCenter
                Layout.preferredWidth: 160
            }    

            Button {
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

            Button {
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

            Button {
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

            Button {
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

            Text {
                font.pointSize: 20
                text: "Tracker Y"
                horizontalAlignment: Text.AlignHLeft
                Layout.preferredWidth: 230
            }

            Text {
                font.pointSize: 20
                text: "Tracker XZ"
                horizontalAlignment: Text.AlignHCenter
                Layout.preferredWidth: 230
            }

            Text {
                text: "Use Trackers?"
                horizontalAlignment: Text.AlignHLeft
                font.pointSize: 15
                Layout.preferredWidth: 160
            }

            Text {
                text: "Disable HMD?"
                horizontalAlignment: Text.AlignHLeft
                font.pointSize: 15
                Layout.preferredWidth: 230
            }

            Text {
                text: "Step Time (seconds)"
                horizontalAlignment: Text.AlignHCenter
                font.pointSize: 15
                Layout.preferredWidth: 230
            }
        }

        GridLayout {
            columns: 11

            Button {
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

            Button {
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

            Button {
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

            Button {
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

            CheckBox {
                id: useTrackersCheck
                Layout.preferredWidth: 163
                Layout.fillWidth: false
                onCheckedChanged: {
                    setUseTrackers(checked)
                }
            }  

            CheckBox {
                id: disableHMDCheck
                Layout.preferredWidth: 223
                Layout.fillWidth: false
                onCheckedChanged: {
                    setDisableHMD(checked)
                }
            }

            Button {
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

            Button {
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
