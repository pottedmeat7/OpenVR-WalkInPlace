import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2
import pottedmeat7.walkinplace 1.0

MyMainViewPage {
    id: stepDetectionPage
    headerText: "OpenVR Walk In Place"
    headerShowBackButton: false

    function updateInfo() {  
        stepThresholdBox.setHMDXZ(WalkInPlaceTabController.getHMDXZThreshold())
        stepThresholdBox.setHMDY(WalkInPlaceTabController.getHMDYThreshold())
        stepThresholdBox.setHandJog(WalkInPlaceTabController.getHandJogThreshold())
        stepThresholdBox.setHandRun(WalkInPlaceTabController.getHandRunThreshold())
        stepThresholdBox.setAccuracyButton(WalkInPlaceTabController.getAccuracyButton())
        stepControlBox.setWalkTouch(WalkInPlaceTabController.getWalkTouch())
        stepControlBox.setJogTouch(WalkInPlaceTabController.getJogTouch())
        stepControlBox.setRunTouch(WalkInPlaceTabController.getRunTouch())
        stepControlBox.updateGUI()
        stepThresholdBox.updateGUI()
        gameTypeDialog.currentIndex = WalkInPlaceTabController.getGameType() - 1
        stepDetectionEnableToggle.checked = WalkInPlaceTabController.isStepDetectionEnabled()
    }

    content: ColumnLayout {
        spacing: 18

        GroupBox {

            Layout.fillWidth: true

            label: MyText {
                leftPadding: 10
                text: "Game Step Conf"
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
                    columns: 5

                    MyToggleButton {
                        id: stepDetectionEnableToggle
                        text: "Enable WIP"
                        Layout.fillWidth: false
                        onCheckedChanged: {
                            WalkInPlaceTabController.enableStepDetection(checked)
                        }
                    }

                    MyText {
                        text: " "
                        horizontalAlignment: Text.AlignHCenter
                        Layout.preferredWidth: 70
                    }

                    MyComboBox {
                        id: gameTypeDialog 
                        currentIndex: 0
                        Layout.maximumWidth: 399
                        Layout.minimumWidth: 299
                        Layout.preferredWidth: 399
                        Layout.fillWidth: true
                        displayText: currentText
                        model: ["touchpad (click to sprint)", "touchpad no click", "touchpad click only (teleport)", "Keyboard (WASD)", "Keyboard (Arrows)"]
                        onCurrentIndexChanged: {
                            if (currentIndex >= 0) { 
                                WalkInPlaceTabController.setGameStepType(currentIndex+1)                            
                            } 
                        }
                    }

                    MyText {
                        text: " "
                        horizontalAlignment: Text.AlignHCenter
                        Layout.preferredWidth: 70
                    }
                }
            }
        }

        StepDetectConfBox2 {
            boxTitle: "Step Threshold / Accuracy"
            id: stepThresholdBox
            keyboardUIDBase: 200
            setHMDXZ: function(xz) {
                WalkInPlaceTabController.setHMDThreshold(xz,hmdY,xz)
                hmdXZ = xz
                updateGUI()    
            }
            setHMDY: function(y) {
                WalkInPlaceTabController.setHMDThreshold(hmdXZ,y,hmdXZ)
                hmdY = y
                updateGUI()    
            }
            setHandJog: function(jog) {
                WalkInPlaceTabController.setHandJogThreshold(jog)
                handJog = jog
                updateGUI()    
            }
            setHandRun: function(run) {
                WalkInPlaceTabController.setHandRunThreshold(run)
                handRun = run
                updateGUI()    
            }
            setAccuracyButton: function(buttonId) {
                WalkInPlaceTabController.setAccuracyButton(buttonId)
                useAccuracyButton = buttonId
                updateGUI()    
            }
            updateValues: function() {
                updateGUI()
            }
        }

        StepDetectConfBox4 {
            boxTitle: "Touch Pad Configuration"
            id: stepControlBox
            keyboardUIDBase: 200
            setWalkTouch: function(val) {
                WalkInPlaceTabController.setWalkTouch(val)
                walkTouch = val
                updateGUI()
            }
            setJogTouch: function(val) {
                WalkInPlaceTabController.setJogTouch(val)
                jogTouch = val
                updateGUI()
            }
            setRunTouch: function(val) {
                WalkInPlaceTabController.setRunTouch(val)
                runTouch = val
                updateGUI()
            }
        }


        ColumnLayout {
            spacing: 18

            RowLayout {
                spacing: 18

                MyPushButton {
                    Layout.preferredWidth: 100
                    Layout.alignment: AlignHRight
                    text: "Reset"
                    onClicked: { 
                        stepThresholdBox.setHMDXZ(0.07)
                        stepThresholdBox.setHMDY(0.09)
                        stepThresholdBox.setHandWalk(0.02)
                        stepThresholdBox.setHandJog(0.40)
                        stepThresholdBox.setHandRun(1.70)
                        stepThresholdBox.setAccuracyButton(5)
                        stepControlBox.setWalkTouch(0.6)
                        stepControlBox.setJogTouch(0.9)
                        stepControlBox.setRunTouch(1)
                        stepControlBox.updateGUI()
                        stepThresholdBox.updateGUI()
                        gameTypeDialog.currentIndex = 0
                    }
                }
            }
        }

        ColumnLayout {
            spacing: 18
            RowLayout {
                spacing: 18

                MyPushButton {
                    id: walkInPlaceLoadProfiles
                    Layout.preferredWidth: 200
                    text: "Load Profiles"
                    onClicked: {
                        reloadWalkInPlaceProfiles()
                    }
                }

                MyComboBox {
                    id: walkInPlaceProfileComboBox
                    Layout.maximumWidth: 310
                    Layout.minimumWidth: 310
                    Layout.preferredWidth: 310
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
                    Layout.preferredWidth: 150
                    text: "Apply"
                    onClicked: {
                        if (walkInPlaceProfileComboBox.currentIndex > 0) {
                            WalkInPlaceTabController.applyWalkInPlaceProfile(walkInPlaceProfileComboBox.currentIndex - 1);
                            updateInfo()
                        }
                    }
                }

                MyPushButton {
                    id: walkInPlaceNewProfileButton
                    Layout.preferredWidth: 150
                    text: "New Profile"
                    onClicked: {
                        walkInPlaceNewProfileDialog.openPopup()
                    }
                }

                MyPushButton {
                    id: walkInPlaceDeleteProfileButton
                    enabled: false
                    Layout.preferredWidth: 150
                    text: "Delete Profile"
                    onClicked: {
                        if (walkInPlaceProfileComboBox.currentIndex > 0) {
                            walkInPlaceDeleteProfileDialog.profileIndex = walkInPlaceProfileComboBox.currentIndex - 1
                            walkInPlaceDeleteProfileDialog.open()
                        }
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
                updateInfo()
            }
        }

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

    function reloadWalkInPlaceProfiles() {
        var profiles = [""]
        var profileCount = WalkInPlaceTabController.getWalkInPlaceProfileCount()
        var defaultFound = -1
        for (var i = 0; i < profileCount; i++) {
            var p_name = WalkInPlaceTabController.getWalkInPlaceProfileName(i)
            if ( p_name == "default" ) {
                defaultFound = i
            }
            profiles.push(p_name)
        }
        walkInPlaceProfileComboBox.model = profiles
        walkInPlaceProfileComboBox.currentIndex = 0
        if ( defaultFound >= 0 ) {
            WalkInPlaceTabController.applyWalkInPlaceProfile(defaultFound);
            walkInPlaceProfileComboBox.currentIndex = defaultFound
            updateInfo()
        }
    }

}

