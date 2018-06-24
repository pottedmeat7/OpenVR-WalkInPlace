import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2
import pottedmeat7.walkinplace 1.0

MyMainViewPage {
    id: stepDetectionPage
    name: "stepDetectionPage"
    headerText: "OpenVR Walk In Place"
    headerShowBackButton: false

    function updateInfo() {  
        stepThresholdBox.setHMDY(WalkInPlaceTabController.getHMDYThreshold())
        stepThresholdBox.setHMDXZ(WalkInPlaceTabController.getHMDXZThreshold())
        stepThresholdBox.setHandJog(WalkInPlaceTabController.getHandJogThreshold())
        stepThresholdBox.setHandRun(WalkInPlaceTabController.getHandRunThreshold())
        var nohmd = WalkInPlaceTabController.getDisableHMD()
        stepThresholdBox.setUseTrackers(WalkInPlaceTabController.getUseTrackers() || nohmd)
        stepThresholdBox.setDisableHMD(nohmd)
        stepThresholdBox.setTrackerY(WalkInPlaceTabController.getTrackerYThreshold())
        stepThresholdBox.setTrackerXZ(WalkInPlaceTabController.getTrackerXZThreshold())
        stepThresholdBox.setStepTime(WalkInPlaceTabController.getStepTime())
        stepThresholdBox.setAccuracyButton(WalkInPlaceTabController.getAccuracyButton())
        stepThresholdBox.setAccuracyButtonAsToggle(WalkInPlaceTabController.getAccuracyButtonIsToggle())
        stepThresholdBox.setAccuracyButtonFlip(WalkInPlaceTabController.getAccuracyButtonFlip())
        stepControlBox.setWalkTouch(WalkInPlaceTabController.getWalkTouch())
        stepControlBox.setJogTouch(WalkInPlaceTabController.getJogTouch())
        stepControlBox.setRunTouch(WalkInPlaceTabController.getRunTouch())
        stepControlBox.updateGUI()
        stepThresholdBox.updateGUI()
        gameTypeDialog.currentIndex = WalkInPlaceTabController.getGameType() - 1
        controlSelect.currentIndex = WalkInPlaceTabController.getControlSelect()
        stepDetectionEnableToggle.checked = WalkInPlaceTabController.isStepDetectionEnabled()
    }

    content: ColumnLayout {
        spacing: 18

        GroupBox {

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
                        Layout.preferredWidth: 50
                    }

                    MyComboBox {
                        id: gameTypeDialog 
                        currentIndex: 0
                        Layout.maximumWidth: 399
                        Layout.minimumWidth: 299
                        Layout.preferredWidth: 399
                        Layout.fillWidth: true
                        displayText: currentText
                        model: ["touchpad with click (Vive)", "touchpad no click (Vive)", "thumbsticks (WMR,Rift)", "touchpad (WMR, NOT WORKING)", "hold grip (Vive,WMR,Rift)", "Keyboard (WASD)", "Keyboard (Arrows)"]
                        onCurrentIndexChanged: {
                            if (currentIndex >= 0) { 
                                WalkInPlaceTabController.setGameStepType(currentIndex+1)                            
                            } 
                        }
                    }


                    MyComboBox {
                        id: controlSelect 
                        currentIndex: 0
                        Layout.maximumWidth: 399
                        Layout.minimumWidth: 299
                        Layout.preferredWidth: 399
                        Layout.fillWidth: true
                        displayText: currentText
                        model: ["1st Controller", "2nd Controller", "Either Controller"]
                        onCurrentIndexChanged: {
                            if (currentIndex >= 0) { 
                                WalkInPlaceTabController.setControlSelect(currentIndex)                        
                            } 
                        }
                    }


                }
            }
        }

        StepDetectConfBox2 {
            id: stepThresholdBox
            keyboardUIDBase: 100
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
            setUseTrackers: function(val) {
                WalkInPlaceTabController.setUseTrackers(val)
                useTrackers = val
                updateGUI()    
            }
            setDisableHMD: function(val) {
                WalkInPlaceTabController.setDisableHMD(val)
                disableHMD = val
                updateGUI()    
            }
            setTrackerXZ: function(xz) {
                trackerXZ = xz
                WalkInPlaceTabController.setTrackerThreshold(xz,trackerY,xz)
                updateGUI()    
            }
            setTrackerY: function(y) {
                trackerY = y
                WalkInPlaceTabController.setTrackerThreshold(trackerXZ,y,trackerXZ)
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
            setStepTime: function(t) {
                WalkInPlaceTabController.setStepTime(t)
                stepTime = t
                updateGUI()                 
            }
            setAccuracyButton: function(buttonId) {
                WalkInPlaceTabController.setAccuracyButton(buttonId)
                useAccuracyButton = buttonId
                updateGUI()    
            }
            setAccuracyButtonAsToggle: function(val) {
                WalkInPlaceTabController.setAccuracyButtonAsToggle(val)
                useButtonAsToggle = val
                updateGUI()    
            }
            setAccuracyButtonFlip: function(val) {
                WalkInPlaceTabController.setAccuracyButtonFlip(val)
                flipButtonUse = val
                updateGUI()    
            }
            setAccuracyButtonControlSelect: function(c) {
                WalkInPlaceTabController.setAccuracyButtonControlSelect(c)
                updateGUI()    
            }
            updateValues: function() {
                updateGUI()
            }
        }

        StepDetectConfBox4 {
            boxTitle: "Touch Pad Configuration"
            id: stepControlBox
            keyboardUIDBase: 119
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
                    Layout.preferredWidth: 180
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

        ColumnLayout {
            spacing: 18

            RowLayout {
                spacing: 18

                MyPushButton {
                    Layout.preferredWidth: 100
                    text: "Reset"
                    onClicked: { 
                        gameTypeDialog.currentIndex = 0
                        controlSelect.currentIndex = 2
                        stepThresholdBox.setHMDY(0.13)
                        stepThresholdBox.setHMDXZ(0.27)
                        stepThresholdBox.setHandJog(0.40)
                        stepThresholdBox.setHandRun(1.70)
                        stepThresholdBox.setAccuracyButton(5)
                        stepThresholdBox.useButtonAsToggle.checked = false
                        stepThresholdBox.flipButtonUse.checked = false
                        stepControlBox.setWalkTouch(0.6)
                        stepControlBox.setJogTouch(0.87)
                        stepControlBox.setRunTouch(1)
                        stepControlBox.updateGUI()
                        stepThresholdBox.updateGUI()
                    }
                }

                MyPushButton {
                    text: "Show Step Graph"
                    onClicked: {
                        WalkInPlaceTabController.setupStepGraph()
                        var res = mainView.push(stepDetectGraphPage)
                        mainView.startTimer()
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
            walkInPlaceProfileComboBox.currentIndex = defaultFound+1
            updateInfo()
        }
    }

}
