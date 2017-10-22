import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2
import matzman666.inputemulator 1.0

MyStackViewPage {
    id: stepDetectionPage
    headerText: "Step Detection"

    property int deviceIndex: -1

    function updateInfo() {  
        stepPhysicsBox.initVars()
        stepThresholdBox.initVars()
        stepDetectionEnableToggle.checked = DeviceManipulationTabController.isStepDetectionEnabled()
        stepPhysicsBox.updateGUI()
        stepThresholdBox.updateGUI()
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

        MyToggleButton2 {
            id: useEulerForStepToggle
            text: "Use Integration (no collisions) Vs Teleports (blinks)"
            Layout.fillWidth: false
            onCheckedChanged: {
                DeviceManipulationTabController.useEulerForStep(checked)
            }
        }

        StepDetectConfBox1 {
            boxTitle: "Step Speed/Accel"
            id: stepPhysicsBox
            keyboardUIDBase: 200
            setStepAcceleration: function(acc) {
                DeviceManipulationTabController.setStepAcceleration(acc)
                updateGUI()
            }
            setStepSpeed: function(speed) {
                DeviceManipulationTabController.setStepSpeed(speed)
                updateGUI()
            }
            setStepIntSec: function(sec) {
                DeviceManipulationTabController.setStepIntSec(sec)
                updateGUI()
            }                
            updateValues: function() {
                var hasChanged = false
                var value = DeviceManipulationTabController.getStepAcceleration()
                if (stepAccel != value) {
                    stepAccel = value
                    hasChanged = true
                }
                value = DeviceManipulationTabController.getStepSpeed()
                if (stepSpeed != value) {
                    stepSpeed = value
                    hasChanged = true
                }
                value = DeviceManipulationTabController.getStepIntSec()
                if (stepIntSec != value) {
                    stepIntSec = value
                    hasChanged = true
                }
                if (hasChanged) {
                    updateGUI()
                }
            }
        }
        StepDetectConfBox2 {
            boxTitle: "Step Threshold / Accuracy"
            id: stepThresholdBox
            keyboardUIDBase: 200
            setHMDX: function(x) {
                DeviceManipulationTabController.setHMDThreshold(x,hmdY,hmdZ)
                updateGUI()    
            }
            setHMDY: function(y) {
                DeviceManipulationTabController.setHMDThreshold(hmdX,y,hmdZ)
                updateGUI()    
            }
            setHMDZ: function(z) {
                DeviceManipulationTabController.setHMDThreshold(hmdX,hmdY,z)
                updateGUI()    
            }
            setHandX: function(x) {
                DeviceManipulationTabController.setHandThreshold(x,handY,handZ)
                updateGUI()    
            }
            setHandY: function(y) {
                DeviceManipulationTabController.setHandThreshold(handX,y,handZ)
                updateGUI()    
            }
            setHandZ: function(z) {
                DeviceManipulationTabController.setHandThreshold(handX,handY,z)
                updateGUI()    
            }
            updateValues: function() {
                var hasChanged = false
                var value = DeviceManipulationTabController.getHMDXThreshold()
                if (hmdX != value) {
                    hmdX = value
                    hasChanged = true
                }
                value = DeviceManipulationTabController.getHMDYThreshold()
                if (hmdY != value) {
                    hmdY = value
                    hasChanged = true
                }
                value = DeviceManipulationTabController.getHMDZThreshold()
                if (hmdZ != value) {
                    hmdZ = value
                    hasChanged = true
                }
                value = DeviceManipulationTabController.getHandXThreshold()
                if (handX != value) {
                    handX = value
                    hasChanged = true
                }
                value = DeviceManipulationTabController.getHandYThreshold()
                if (handY != value) {
                    handY = value
                    hasChanged = true
                }
                value = DeviceManipulationTabController.getHandZThreshold()
                if (handZ != value) {
                    handZ = value
                    hasChanged = true
                }
                if (hasChanged) {
                    updateGUI()
                }
            }
        }

        RowLayout {
            MyPushButton {
                Layout.preferredWidth: 200
                text: "Reset"
                onClicked: {
                    stepThresholdBox.setHMDX(0.3)
                    stepThresholdBox.setHMDY(0.4)
                    stepThresholdBox.setHMDZ(0.3)
                    stepThresholdBox.setHandX(0.15)
                    stepThresholdBox.setHandY(0.15)
                    stepThresholdBox.setHandZ(0.15)
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

