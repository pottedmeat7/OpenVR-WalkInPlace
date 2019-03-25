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
        anchors.bottom: parent.bottom
        spacing: 7
        Layout.fillHeight: true

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
                    spacing: 0

                    MyText {
                        id: headerTitle
                        text: "OpenVR-WalkInPlace"
                        font.pointSize: 22
                    }

                    MyText {
                        text: " "
                        Layout.preferredWidth: 165
                    }  

                    Rectangle {
                        color: "#AAAAAA"
                        height: 50
                        width: 2
                        Layout.topMargin: 5
                        Layout.bottomMargin: 10
                    }

                    MyTab {
                        Layout.preferredWidth: 300
                        Layout.preferredHeight: 60
                        text: "   Choose Tracked Devices "
                        onClicked: {
                            var res = mainView.push(disableDevicePage)
                        }
                    }

                    Rectangle {
                        color: "#AAAAAA"
                        height: 50
                        width: 2
                        Layout.topMargin: 5
                        Layout.bottomMargin: 10
                    }

                    MyTab {
                        Layout.preferredWidth: 300
                        Layout.preferredHeight: 60
                        text: "   Show Tracking Graph"
                        onClicked: {
                            var res = mainView.push(graphPage)
                            mainView.startTimer()
                        }
                    }
                }
            }
        }

        GroupBox {
            id: mainConfigBox
            anchors.top: parent.top
            anchors.topMargin: 70

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
                        text: "Enable WIP"
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
            id: dataModelColumnLYO
            anchors.top: mainConfigBox.bottom
            anchors.topMargin: 350
            spacing: 7

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
                            id: dataModelComboBox
                            Layout.maximumWidth: 310
                            Layout.minimumWidth: 310
                            Layout.preferredWidth: 310
                            Layout.fillWidth: true
                            model: [""]
                            onCurrentIndexChanged: {
                                if (currentIndex > 0) {
                                    applyDataModelButton.enabled = true
                                    deleteDataModelButton.enabled = true
                                } else {
                                    applyDataModelButton.enabled = false
                                    deleteDataModelButton.enabled = false
                                    showDataModelButton.enabled = false
                                }
                            }
                        }

                        MyPushButton {
                            id: applyDataModelButton
                            Layout.topMargin: 10
                            enabled: false
                            Layout.preferredWidth: 150
                            text: "Apply"
                            onClicked: {
                                if (dataModelComboBox.currentIndex > 0) {
                                    WalkInPlaceTabController.applyDataModel(dataModelComboBox.currentText);
                                    showDataModelButton.enabled = true
                                }
                            }
                        }

                        MyPushButton {
                            id: newDataModelButton
                            Layout.preferredWidth: 250
                            Layout.topMargin: 10
                            text: "New Data Model"
                            onClicked: {
                                newDataModelDialog.openPopup()
                            }
                        }

                        MyPushButton {
                            id: showDataModelButton
                            Layout.preferredWidth: 250
                            Layout.topMargin: 10
                            text: "Show Data Model"
                            onClicked: {
                                if (dataModelComboBox.currentIndex > 0) {
                                    var res = mainView.push(dataModelPage)
                                    mainView.startTimer()
                                }
                            }
                        }
                    }

                    RowLayout {
                        spacing: 18

                        MyPushButton {
                            id: deleteDataModelButton
                            enabled: false
                            Layout.preferredWidth: 270
                            Layout.topMargin: 10
                            text: "Delete Data Model"
                            onClicked: {
                                if (dataModelComboBox.currentIndex > 0) {
                                    deleteDataModelDialog.open()
                                }
                                showDataModelButton.enabled = false
                            }
                        }
                    }
                }
            }
        }

        ColumnLayout {
            id: profileColumnLYO
            spacing: 18
            anchors.top: dataModelColumnLYO.bottom
            anchors.topMargin: 20

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
                            id: profileComboBox
                            Layout.maximumWidth: 290
                            Layout.minimumWidth: 290
                            Layout.preferredWidth: 290
                            Layout.fillWidth: true
                            model: [""]
                            onCurrentIndexChanged: {
                                if (currentIndex > 0) {
                                    applyProfileButton.enabled = true
                                    deleteProfileButton.enabled = true
                                } else {
                                    applyProfileButton.enabled = false
                                    deleteProfileButton.enabled = false
                                }
                            }
                        }

                        MyPushButton {
                            id: applyProfileButton
                            Layout.topMargin: 10
                            enabled: false
                            Layout.preferredWidth: 150
                            text: "Apply"
                            onClicked: {
                                if (profileComboBox.currentIndex > 0) {
                                    WalkInPlaceTabController.applyProfile(profileComboBox.currentIndex - 1);
                                    updateInfo()
                                }
                            }
                        }

                        MyPushButton {
                            id: newProfileButton
                            Layout.preferredWidth: 240
                            Layout.topMargin: 10
                            text: "New Profile"
                            onClicked: {
                                newProfileDialog.openPopup()
                            }
                        }

                        MyPushButton {
                            id: deleteProfileButton
                            enabled: false
                            Layout.preferredWidth: 260
                            Layout.topMargin: 10
                            text: "Delete Profile"
                            onClicked: {
                                if (profileComboBox.currentIndex > 0) {
                                    deleteProfileDialog.profileIndex = profileComboBox.currentIndex - 1
                                    deleteProfileDialog.open()
                                }
                            }
                        }
                    }
                }
            }
        }

        Component.onCompleted: {   
            if ( !initialLoaded ) { 
                //updateInfo()
                initProfiles()
            }
            initialLoaded = true
        }

    }

    MyDialogOkCancelPopup {
        id: deleteDataModelDialog
        property int modelIndex: -1
        dialogTitle: "Delete Data Model"
        dialogText: "Do you really want to delete this data model?"
        onClosed: {
            if (okClicked) {
                WalkInPlaceTabController.deleteDataModel(dataModelComboBox.currentText)
                reloadProfiles()
            }
        }
    }

    MyDialogOkCancelPopup {
        id: newDataModelDialog
        dialogTitle: "Create New Data Model"
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
                    id: newDataModelName
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
                if (newDataModelName.text == "") {
                    //messageDialog.showMessage("Create New Profile", "ERROR: Empty profile name.")
                } else {
                    WalkInPlaceTabController.addDataModel(newDataModelName.text)
                    var res = mainView.push(graphPage)
                    mainView.startTimer()
                    mainView.startAutoConf()
                }
            }
        }
        function openPopup() {
            newProfileName.text = ""
            open()
        }
    }

    MyDialogOkCancelPopup {
        id: deleteProfileDialog
        property int profileIndex: -1
        dialogTitle: "Delete Data Model"
        dialogText: "Do you really want to delete this data model?"
        onClosed: {
            if (okClicked) {
                WalkInPlaceTabController.deleteProfile(profileIndex)
            }
        }
    }

    MyDialogOkCancelPopup {
        id: newProfileDialog
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
                    id: newProfileName
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
                if (newProfileName.text == "") {
                    //messageDialog.showMessage("Create New Profile", "ERROR: Empty profile name.")
                } else {
                    WalkInPlaceTabController.addProfile(newProfileName.text)
                }
            }
        }
        function openPopup() {
            newProfileName.text = ""
            open()
        }
    }

    function initProfiles() {
        //load data models
        var defaultFound = -1
        var dataModels = [""]
        var temp = WalkInPlaceTabController.getDataModelNames()
        for (var i = 0; i < temp.length; i++) {
            dataModels.push(temp[i].replace(".csv",""))
            if ( dataModels[i] == "default" ) {
                defaultFound = i
            }
        }
        dataModelComboBox.model = dataModels
        if ( defaultFound >= 0 ) {
            dataModelComboBox.currentIndex = defaultFound
        } else {
            dataModelComboBox.currentIndex = 0
        }
        //load profiles
        var profiles = [""]
        var profileCount = WalkInPlaceTabController.getProfileCount()
        defaultFound = -1
        for (var i = 0; i < profileCount; i++) {
            var p_name = WalkInPlaceTabController.getProfileName(i)
            if ( p_name == "default" ) {
                defaultFound = i
            }
            profiles.push(p_name)
        }
        profileComboBox.model = profiles
        profileComboBox.currentIndex = 0
        if ( defaultFound >= 0 ) {
            WalkInPlaceTabController.applyProfile(defaultFound);
            profileComboBox.currentIndex = defaultFound+1
            updateInfo()
        }
    }

    function reloadProfiles() {
        //load data models
        var defaultFound = -1
        var dataModels = [""]
        var temp = WalkInPlaceTabController.getDataModelNames()
        for (var i = 0; i < temp.length; i++) {
            dataModels.push(temp[i].replace(".csv",""))
            if ( dataModels[i] == "default" ) {
                defaultFound = i
            }
        }
        dataModelComboBox.model = dataModels
        if ( defaultFound >= 0 ) {
            dataModelComboBox.currentIndex = defaultFound
        } else {
            dataModelComboBox.currentIndex = 0
        }
        //load profiles
        var profiles = [""]
        var profileCount = WalkInPlaceTabController.getProfileCount()
        for (var i = 0; i < profileCount; i++) {
            var p_name = WalkInPlaceTabController.getProfileName(i)
            profiles.push(p_name)
        }
        profileComboBox.model = profiles
        profileComboBox.currentIndex = 0
    }

    function completeTraining() {
        WalkInPlaceTabController.completeTraining()
        reloadProfiles()
    }

}
