import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2
import pottedmeat7.walkinplace 1.0

MyMainViewPage {
    id: mainConfPage
    name: "mainConfPage"
    property var initialLoaded: false

    function updateInfo() {  
        useContDirForStrafCheck.checked = WalkInPlaceTabController.getUseContDirForStraf()
        useContDirForRevCheck.checked = WalkInPlaceTabController.getUseContDirForRev()
        wipEnableToggle.checked = WalkInPlaceTabController.isWIPEnabled()
        gameTypeDialog.currentIndex = WalkInPlaceTabController.getGameType()
        hmdTypeDialog.currentIndex = WalkInPlaceTabController.getHMDType()
        buttonMode.currentIndex = WalkInPlaceTabController.getButtonEnables() ? 1 : 0
        accuracyButtonDialog.currentIndex = WalkInPlaceTabController.getDisableButton()
        buttonControlSelect.currentIndex = WalkInPlaceTabController.getButtonControlSelect()
    }

    content: ColumnLayout {
        anchors.top: parent.top
        spacing: 7

        GroupBox {
            Layout.fillWidth: true
            
            background: Rectangle {
                color: myPalette.base
                border.color: myPalette.base
                radius: 1
            }

            ColumnLayout {
                anchors.fill: parent
                Layout.alignment: Qt.AlignHCenter

                RowLayout {
                    MyText {
                        id: headerTitle
                        text: "OpenVR-WalkInPlace"
                        font.pointSize: 22
                    }
                }
            }
        }

        GroupBox {
            anchors.top: parent.top
            anchors.topMargin: 55

            Layout.fillWidth: true
            
            background: Rectangle {
                color: myPalette.mid
                border.color: myPalette.mid
                radius: 1
            }

            ColumnLayout {
                anchors.fill: parent
                Layout.alignment: Qt.AlignHCenter

                GridLayout {
                    columns: 6

                    MyToggleButton {
                        id: wipEnableToggle
                        text: "Enable Input"
                        Layout.maximumWidth: 200
                        Layout.minimumWidth: 200
                        Layout.preferredWidth: 200
                        Layout.fillWidth: true
                        onCheckedChanged: {
                            WalkInPlaceTabController.enableWIP(checked)
                        }
                    }


                    MyComboBox {
                        id: hmdTypeDialog 
                        currentIndex: 0
                        Layout.maximumWidth: 100
                        Layout.minimumWidth: 100
                        Layout.preferredWidth: 100
                        Layout.fillWidth: true
                        displayText: currentText
                        model: ["Vive/Rift", "Other"]
                        onCurrentIndexChanged: {
                            if (currentIndex >= 0) { 
                                WalkInPlaceTabController.setHMDType(currentIndex)                            
                            } 
                        }
                    }

                    MyComboBox {
                        id: gameTypeDialog 
                        currentIndex: 0
                        Layout.maximumWidth: 400
                        Layout.minimumWidth: 400
                        Layout.preferredWidth: 400
                        Layout.fillWidth: true
                        displayText: currentText
                        model: ["touchpad (click sprint)", "touchpad", "touchpad (pressed)","hold grip","Keyboard (WASD)","Keyboard (Arrows)"]
                        onCurrentIndexChanged: {
                            if (currentIndex >= 0) { 
                                WalkInPlaceTabController.setGameStepType(currentIndex)                            
                            } 
                        }
                    }
  

                    MyText {
                        text: " "
                        Layout.preferredWidth: 10
                    }  

                    MyText {
                        text: "on"
                        Layout.preferredWidth: 50
                    }

                    MyText {
                        text: "OVRWIP Controller"
                        Layout.preferredWidth: 250
                    }

                }

                GridLayout {
                    columns: 5
                    
                    MyComboBox {
                        id: buttonMode 
                        currentIndex: 0
                        Layout.maximumWidth: 305
                        Layout.minimumWidth: 305
                        Layout.preferredWidth: 305
                        Layout.fillWidth: true
                        displayText: currentText
                        model: ["Disable WIP", "Enable WIP"]
                        onCurrentIndexChanged: {
                            if (currentIndex == 0) { 
                                WalkInPlaceTabController.setButtonEnables(false)
                            }
                            else if (currentIndex == 1) { 
                                WalkInPlaceTabController.setButtonEnables(true)
                            } 
                        }
                    }

                    MyComboBox {
                        id: accuracyButtonDialog 
                        currentIndex: 0
                        Layout.maximumWidth: 401
                        Layout.minimumWidth: 401
                        Layout.preferredWidth: 401
                        Layout.fillWidth: true
                        displayText: currentText
                        model: ["by holding grip", "by holding trigger", "by touching touchpad", "only with data model"]
                        onCurrentIndexChanged: {
                            if (currentIndex >= 0) { 
                                WalkInPlaceTabController.setDisableButton(currentIndex) 
                            } 
                        }
                    }     

                    MyText {
                        text: " "
                        Layout.preferredWidth: 10
                    }  

                    MyText {
                        text: "on"
                        Layout.preferredWidth: 50
                    }

                    MyComboBox {
                        id: buttonControlSelect 
                        currentIndex: 0
                        Layout.maximumWidth: 250
                        Layout.minimumWidth: 250
                        Layout.preferredWidth: 250
                        Layout.fillWidth: true
                        displayText: currentText
                        model: ["1st Controller", "2nd Controller"]
                        onCurrentIndexChanged: {
                            if (currentIndex >= 0) { 
                               WalkInPlaceTabController.setButtonControlSelect(currentIndex)                        
                            } 
                        }
                    }
                }
            }
        }

        ColumnLayout {
            spacing: 18
            Layout.alignment: Qt.AlignHCenter

            GroupBox {

                height: 200
                Layout.fillWidth: true
                
                background: Rectangle {
                    color: myPalette.mid
                    border.color: myPalette.mid
                    radius: 1
                }

                ColumnLayout {
                    anchors.fill: parent

                    RowLayout {
                        spacing: 18

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
                                    showDataModelButton.enabled = false
                                }
                            }
                        }

                        MyPushButton {
                            id: walkInPlaceApplyProfileButton
                            Layout.topMargin: 10
                            enabled: false
                            Layout.preferredWidth: 150
                            text: "Apply"
                            onClicked: {
                                if (walkInPlaceProfileComboBox.currentIndex > 0) {
                                    WalkInPlaceTabController.applyWalkInPlaceProfile(walkInPlaceProfileComboBox.currentIndex - 1);
                                    updateInfo()
                                    showDataModelButton.enabled = true
                                }
                            }
                        }

                        MyPushButton {
                            id: walkInPlaceNewProfileButton
                            Layout.preferredWidth: 250
                            Layout.topMargin: 10
                            text: "New Data Model"
                            onClicked: {
                                walkInPlaceNewProfileDialog.openPopup()
                            }
                        }

                        MyPushButton {
                            id: showDataModelButton
                            Layout.preferredWidth: 250
                            Layout.topMargin: 10
                            text: "Show Data Model"
                            onClicked: {
                                if (walkInPlaceProfileComboBox.currentIndex > 0) {
                                    var res = mainView.push(dataModelPage)
                                    mainView.startTimer()
                                }
                            }
                        }
                    }

                    RowLayout {
                        spacing: 18

                        MyPushButton {
                            id: walkInPlaceDeleteProfileButton
                            enabled: false
                            Layout.preferredWidth: 270
                            Layout.topMargin: 10
                            text: "Delete Data Model"
                            onClicked: {
                                if (walkInPlaceProfileComboBox.currentIndex > 0) {
                                    walkInPlaceDeleteProfileDialog.profileIndex = walkInPlaceProfileComboBox.currentIndex - 1
                                    walkInPlaceDeleteProfileDialog.open()
                                }
                                showDataModelButton.enabled = false
                            }
                        }
                    }
                }
            }
        }

        Component.onCompleted: {   
            if ( !initialLoaded ) { 
                //updateInfo()
                initWalkInPlaceProfiles()
            }
            initialLoaded = true
        }

        Connections {
            target: WalkInPlaceTabController
            onWalkInPlaceProfilesChanged: {
                reloadWalkInPlaceProfiles()
            }
        }

    }

    MyDialogOkCancelPopup {
        id: walkInPlaceDeleteProfileDialog
        property int profileIndex: -1
        dialogTitle: "Delete Data Model"
        dialogText: "Do you really want to delete this data model?"
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
                    text: ""
                    Layout.fillWidth: true
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
                    var res = mainView.push(graphPage)
                    mainView.startTimer()
                }
            }
        }
        function openPopup() {
            walkInPlaceNewProfileName.text = ""
            open()
        }
    }

    function initWalkInPlaceProfiles() {
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

    function reloadWalkInPlaceProfiles() {
        var profiles = [""]
        var profileCount = WalkInPlaceTabController.getWalkInPlaceProfileCount()
        for (var i = 0; i < profileCount; i++) {
            var p_name = WalkInPlaceTabController.getWalkInPlaceProfileName(i)
            profiles.push(p_name)
        }
        walkInPlaceProfileComboBox.model = profiles
        walkInPlaceProfileComboBox.currentIndex = 0
    }

}
