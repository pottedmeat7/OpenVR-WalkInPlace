import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2
import pottedmeat7.inputemulator 1.0

MyStackViewPage {
    id: stepDetectionPage
    headerText: "Step Detection"

    property int deviceIndex: -1

    function updateInfo() {  
        stepMovementType.initVars()
        stepThresholdBox.initVars()
        stepDetectionEnableToggle.checked = DeviceManipulationTabController.isStepDetectionEnabled()
        stepThresholdBox.updateGUI()
    }

    Component.onCompleted: {    
        updateInfo()
    }
    
    content: ColumnLayout {
        spacing: 18


        MyToggleButton {
            id: stepDetectionEnableToggle
            text: "Enable Step Detection"
            Layout.fillWidth: false
            onCheckedChanged: {
                DeviceManipulationTabController.enableStepDetection(checked)
            }
        }

        StepDetectConfBox3 {
            boxTitle: "Game Movement Type"
            id: stepMovementType
            keyboardUIDBase: 200
            setGameType: function(type) {
                DeviceManipulationTabController.setGameStepType(gameType)
                updateGUI()    
            }
            updateValues: function() {
                updateGUI()
            }
        }

        StepDetectConfBox2 {
            boxTitle: "Step Threshold / Accuracy"
            id: stepThresholdBox
            keyboardUIDBase: 200
            setHMDXZ: function(xz) {
                DeviceManipulationTabController.setHMDThreshold(xz,hmdY,xz)
                updateGUI()    
            }
            setHMDY: function(y) {
                DeviceManipulationTabController.setHMDThreshold(hmdXZ,y,hmdXZ)
                updateGUI()    
            }
            setHandWalk: function(walk) {
                updateGUI()    
            }
            setHandJog: function(jog) {
                DeviceManipulationTabController.setHandJogThreshold(jog)
                updateGUI()    
            }
            setHandRun: function(run) {
                DeviceManipulationTabController.setHandRunThreshold(run)
                updateGUI()    
            }
            updateValues: function() {
                updateGUI()
            }
        }

        RowLayout {
            MyPushButton {
                Layout.preferredWidth: 200
                text: "Reset"
                onClicked: {
                    stepMovementType.setGameType(1)
                    stepThresholdBox.setHMDXZ(0.13)
                    stepThresholdBox.setHMDY(0.14)
                    stepThresholdBox.setHandWalk(0.02)
                    stepThresholdBox.setHandJog(0.40)
                    stepThresholdBox.setHandRun(1.70)
                    stepMovementType.updateGUI()
                    stepThresholdBox.updateGUI()
                }
            }
        }

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        Connections {
            target: DeviceManipulationTabController
            onDeviceInfoChanged: {
            }
        }

    }

}

