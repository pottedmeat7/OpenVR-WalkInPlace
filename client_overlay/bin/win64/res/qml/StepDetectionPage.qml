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

