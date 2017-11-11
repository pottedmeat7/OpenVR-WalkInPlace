import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2
import pottedmeat7.walkinplace 1.0

MyStackViewPage {
    id: stepDetectionPage
    headerText: "Step Detection"    
    headerShowBackButton: false

    function updateInfo() {  
        stepMovementType.setGameType(WalkInPlaceTabController.getGameType())
        stepThresholdBox.setHMDXZ(WalkInPlaceTabController.getHMDXZThreshold())
        stepThresholdBox.setHMDY(WalkInPlaceTabController.getHMDYThreshold())
        stepThresholdBox.setHandWalk(WalkInPlaceTabController.getHandWalkThreshold())
        stepThresholdBox.setHandJog(WalkInPlaceTabController.getHandJogThreshold())
        stepThresholdBox.setHandRun(WalkInPlaceTabController.getHandRunThreshold())
        stepDetectionEnableToggle.checked = WalkInPlaceTabController.isStepDetectionEnabled()
        stepThresholdBox.updateGUI()
    }
    

    MyDialogOkCancelPopup {
        id: walkInPlaceDeleteProfileDialog
        property int profileIndex: -1
        dialogTitle: "Delete Profile"
        dialogText: "Do you really want to delete this profile?"
        onClosed: {
            if (okClicked) {
                WalkInPlaceTabController.deleteWalkInPlaceProfile(profileIndex)
            }
        }
    }

    MyDialogOkCancelPopup {
        id: walkInPlaceNewProfileDialog
        dialogTitle: "Create New Profile"
        dialogWidth: 600
        dialogHeight: 400
        dialogContentItem: ColumnLayout {
            RowLayout {
                Layout.topMargin: 16
                Layout.leftMargin: 16
                Layout.rightMargin: 16
                MyText {
                    text: "Name: "
                }
                MyTextField {
                    id: walkInPlaceNewProfileName
                    color: "#cccccc"
                    text: ""
                    Layout.fillWidth: true
                    font.pointSize: 20
                    function onInputEvent(input) {
                        text = input
                    }
                }
            }
        }
        onClosed: {
            if (okClicked) {
                if (walkInPlaceNewProfileName.text == "") {
                    walkInPlaceMessageDialog.showMessage("Create New Profile", "ERROR: Empty profile name.")
                } else {
                    WalkInPlaceTabController.addWalkInPlaceProfile(walkInPlaceNewProfileName.text)
                }

            }
        }
        function openPopup() {
            walkInPlaceNewProfileName.text = ""
            open()
        }
    }

    content: ColumnLayout {
        spacing: 18


        MyToggleButton {
            id: stepDetectionEnableToggle
            text: "Enable Step Detection"
            Layout.fillWidth: false
            onCheckedChanged: {
                WalkInPlaceTabController.enableStepDetection(checked)
            }
        }

        StepDetectConfBox3 {
            boxTitle: "Game Movement Type"
            id: stepMovementType
            keyboardUIDBase: 200
            setGameType: function(type) {
                WalkInPlaceTabController.setGameStepType(gameType)
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
                WalkInPlaceTabController.setHMDThreshold(xz,hmdY,xz)
                updateGUI()    
            }
            setHMDY: function(y) {
                WalkInPlaceTabController.setHMDThreshold(hmdXZ,y,hmdXZ)
                updateGUI()    
            }
            setHandWalk: function(walk) {
                updateGUI()    
            }
            setHandJog: function(jog) {
                WalkInPlaceTabController.setHandJogThreshold(jog)
                updateGUI()    
            }
            setHandRun: function(run) {
                WalkInPlaceTabController.setHandRunThreshold(run)
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

        ColumnLayout {
            Layout.bottomMargin: 6
            spacing: 18
            RowLayout {
                spacing: 18

                MyText {
                    text: "Profile:"
                }

                MyComboBox {
                    id: walkInPlaceProfileComboBox
                    Layout.maximumWidth: 799
                    Layout.minimumWidth: 799
                    Layout.preferredWidth: 799
                    Layout.fillWidth: true
                    model: [""]
                    onCurrentIndexChanged: {
                        if (currentIndex > 0) {
                            walkInPlaceApplyProfileButton.enabled = true
                            walkInPlaceDeleteProfileButton.enabled = true
                        } else {
                            walkInPlaceApplyProfileButton.enabled = false
                            walkInPlaceDeleteProfileButton.enabled = false
                        }
                    }
                }

                MyPushButton {
                    id: walkInPlaceApplyProfileButton
                    enabled: false
                    Layout.preferredWidth: 200
                    text: "Apply"
                    onClicked: {
                        if (walkInPlaceProfileComboBox.currentIndex > 0) {
                            WalkInPlaceTabController.applyWalkInPlaceProfile(walkInPlaceProfileComboBox.currentIndex - 1);
                        }
                    }
                }
            }
            RowLayout {
                spacing: 18
                Item {
                    Layout.fillWidth: true
                }
                MyPushButton {
                    id: walkInPlaceDeleteProfileButton
                    enabled: false
                    Layout.preferredWidth: 200
                    text: "Delete Profile"
                    onClicked: {
                        if (walkInPlaceProfileComboBox.currentIndex > 0) {
                            walkInPlaceDeleteProfileDialog.profileIndex = walkInPlaceProfileComboBox.currentIndex - 1
                            walkInPlaceDeleteProfileDialog.open()
                        }
                    }
                }
                MyPushButton {
                    id: walkInPlaceNewProfileButton
                    Layout.preferredWidth: 200
                    text: "New Profile"
                    onClicked: {
                        walkInPlaceNewProfileDialog.openPopup()
                    }
                }
            }
        }

        Component.onCompleted: {    
            updateInfo()
        }

        Connections {
            target: WalkInPlaceTabController
            onWalkInPlaceProfilesChanged: {
                reloadWalkInPlaceProfiles()
            }
        }

    }

    function reloadWalkInPlaceProfiles() {
        var profiles = [""]
        var profileCount = WalkInPlaceTabController.getWalkInPlaceProfileCount()
        for (var i = 0; i < profileCount; i++) {
            profiles.push(WalkInPlaceTabController.getWalkInPlaceProfileName(i))
        }
        walkInPlaceProfileComboBox.model = profiles
        walkInPlaceProfileComboBox.currentIndex = 0
    }

}

