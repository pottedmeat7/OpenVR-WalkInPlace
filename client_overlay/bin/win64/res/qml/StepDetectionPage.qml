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

        StepDetectConfBox1 {
            boxTitle: "Step Time"
            id: stepPhysicsBox
            keyboardUIDBase: 200
            setStepIntSec: function(sec) {
                DeviceManipulationTabController.setStepIntSec(sec)
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
                updateGUI()
            }
        }

        RowLayout {
            MyPushButton {
                Layout.preferredWidth: 200
                text: "Reset"
                onClicked: {
                    stepPhysicsBox.setStepIntSec(0.07)
                    stepThresholdBox.setHMDX(0.3)
                    stepThresholdBox.setHMDY(0.3)
                    stepThresholdBox.setHMDZ(0.3)
                    stepThresholdBox.setHandX(0.0)
                    stepThresholdBox.setHandY(0.0)
                    stepThresholdBox.setHandZ(0.0)
                    stepPhysicsBox.updateGUI()
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

