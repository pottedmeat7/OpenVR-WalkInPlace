import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.0
import pottedmeat7.walkinplace 1.0

MyStackViewPage {
    id: disableDevicePage
    name: "disableDevicePage"
    property var initialLoaded: false

    property var startTimer: function() {
        updateInfo()
    }

    function updateInfo() {  
        hmd1.checked = WalkInPlaceTabController.getDisableHMD()
        hmd2.checked = WalkInPlaceTabController.getTrackHMDRot()
        cntrlHands1.checked = WalkInPlaceTabController.getDeviceEnabled(2,0,0)
        cntrlHands2.checked = WalkInPlaceTabController.getDeviceEnabled(2,1,0)
        cntrlHands3.checked = WalkInPlaceTabController.getDeviceEnabled(2,2,0)
        cntrlHands4.checked = WalkInPlaceTabController.getDeviceEnabled(2,3,0)
        cntrlFeet1.checked = WalkInPlaceTabController.getDeviceEnabled(2,0,1)
        cntrlFeet2.checked = WalkInPlaceTabController.getDeviceEnabled(2,1,1)
        cntrlFeet3.checked = WalkInPlaceTabController.getDeviceEnabled(2,2,1)
        cntrlFeet4.checked = WalkInPlaceTabController.getDeviceEnabled(2,3,1)
        trkrHands1.checked = WalkInPlaceTabController.getDeviceEnabled(3,0,0)
        trkrHands2.checked = WalkInPlaceTabController.getDeviceEnabled(3,1,0)
        trkrHands3.checked = WalkInPlaceTabController.getDeviceEnabled(3,2,0)
        trkrHands4.checked = WalkInPlaceTabController.getDeviceEnabled(3,3,0)
        trkrFeet1.checked = WalkInPlaceTabController.getDeviceEnabled(3,0,1)
        trkrFeet2.checked = WalkInPlaceTabController.getDeviceEnabled(3,1,1)
        trkrFeet3.checked = WalkInPlaceTabController.getDeviceEnabled(3,2,1)
        trkrFeet4.checked = WalkInPlaceTabController.getDeviceEnabled(3,3,1)
    }

    content: ColumnLayout {
        anchors.top: parent.top
        spacing: 7
        Layout.alignment: Qt.AlignHCenter

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
                    Button {
                        id: headerBackButton
                        Layout.preferredHeight: 60
                        Layout.preferredWidth: 60
                        hoverEnabled: true
                        enabled: true
                        opacity: 1.0
                        contentItem: Text {
                            color: "#ffffff"
                            text: "Back"
                            font.pointSize: 12
                        }
                        //contentItem: Image {
                        //    source: "backarrow.svg"
                        //    sourceSize.width: 60
                        //    sourceSize.height: 60
                        //    anchors.fill: parent
                        //}
                        background: Rectangle {
                            opacity: parent.down ? 1.0 : (parent.activeFocus ? 0.5 : 0.0)
                            color: myPalette.base
                            radius: 4
                            anchors.fill: parent
                        }
                        onHoveredChanged: {
                            if (hovered) {
                                forceActiveFocus()
                            } else {
                                focus = false
                            }
                        }
                        onClicked: {
                            mainView.stopTimer()
                            var page = mainView.pop()
                        }
                    }

                    MyText {
                        id: headerTitle
                        text: "OpenVR-WalkInPlace"
                        font.pointSize: 22
                    }
                }
            }
        }

        ColumnLayout {
            spacing: 7
            Layout.alignment: Qt.AlignHCenter

            GroupBox {
                Layout.minimumWidth: 1300
                Layout.alignment: Qt.AlignHCenter
                anchors.centerIn: parent

                label: MyText {
                    text: "HMD Options"
                }

                background: Rectangle {
                    color: myPalette.mid
                    border.color: myPalette.mid
                    radius: 1
                }

                ColumnLayout {
                    anchors.fill: parent

                    RowLayout {
                        spacing: 7
                        MyToggleButton {
                            id: hmd1
                            text: "Track HMD?"
                            checked: true
                            Layout.maximumWidth: 350
                            Layout.minimumWidth: 350
                            Layout.preferredWidth: 350
                            Layout.fillWidth: true
                            onCheckedChanged: {
                                WalkInPlaceTabController.setTrackHMDVel(checked)
                            }
                        }

                        MyToggleButton {
                            id: hmd2
                            text: "Track HMD Rotation?"
                            checked: true
                            Layout.maximumWidth: 350
                            Layout.minimumWidth: 350
                            Layout.preferredWidth: 350
                            Layout.fillWidth: true
                            onCheckedChanged: {
                                WalkInPlaceTabController.setTrackHMDRot(checked)
                            }
                        }
                    }
                }
            }
        }

        ColumnLayout {
            spacing: 7
            Layout.alignment: Qt.AlignHCenter

            GroupBox {
                Layout.minimumWidth: 1300
                Layout.alignment: Qt.AlignHCenter
                anchors.centerIn: parent

                label: MyText {
                    text: "Controllers for pace only"
                }

                background: Rectangle {
                    color: myPalette.mid
                    border.color: myPalette.mid
                    radius: 1
                }

                ColumnLayout {
                    anchors.fill: parent

                    RowLayout {
                        spacing: 7
                        MyToggleButton {
                            id: cntrlHands1
                            text: "1st controller"
                            checked: true
                            Layout.maximumWidth: 220
                            Layout.minimumWidth: 220
                            Layout.preferredWidth: 220
                            Layout.fillWidth: true
                            onCheckedChanged: {
                                WalkInPlaceTabController.enableDevice(2, 0, checked,0)
                            }
                        }

                        MyToggleButton {
                            id: cntrlHands2
                            text: "2nd controller"
                            checked: true
                            Layout.maximumWidth: 220
                            Layout.minimumWidth: 220
                            Layout.preferredWidth: 220
                            Layout.fillWidth: true
                            onCheckedChanged: {
                                WalkInPlaceTabController.enableDevice(2, 1,checked,0)
                            }
                        }

                        MyToggleButton {
                            id: cntrlHands3
                            text: "3rd controller"
                            checked: false
                            Layout.maximumWidth: 220
                            Layout.minimumWidth: 220
                            Layout.preferredWidth: 220
                            Layout.fillWidth: true
                            onCheckedChanged: {
                                WalkInPlaceTabController.enableDevice(2, 2,checked,0)
                            }
                        }

                        MyToggleButton {
                            id: cntrlHands4
                            text: "4th controller"
                            checked: false
                            Layout.maximumWidth: 220
                            Layout.minimumWidth: 220
                            Layout.preferredWidth: 220
                            Layout.fillWidth: true
                            onCheckedChanged: {
                                WalkInPlaceTabController.enableDevice(2, 3,checked,0)
                            }
                        }
                    }
                }
            }
        }

        ColumnLayout {
            spacing: 7
            Layout.alignment: Qt.AlignHCenter

            GroupBox {
                Layout.minimumWidth: 1300
                Layout.alignment: Qt.AlignHCenter
                anchors.centerIn: parent

                label: MyText {
                    text: "Controllers compared to model"
                }

                background: Rectangle {
                    color: myPalette.mid
                    border.color: myPalette.mid
                    radius: 1
                }

                ColumnLayout {
                    anchors.fill: parent

                    RowLayout {
                        spacing: 7
                        MyToggleButton {
                            id: cntrlFeet1
                            text: "1st controller"
                            checked: false
                            Layout.maximumWidth: 220
                            Layout.minimumWidth: 220
                            Layout.preferredWidth: 220
                            Layout.fillWidth: true
                            onCheckedChanged: {
                                WalkInPlaceTabController.enableDevice(2, 0,checked,1)
                            }
                        }

                        MyToggleButton {
                            id: cntrlFeet2
                            text: "2nd controller"
                            checked: false
                            Layout.maximumWidth: 220
                            Layout.minimumWidth: 220
                            Layout.preferredWidth: 220
                            Layout.fillWidth: true
                            onCheckedChanged: {
                                WalkInPlaceTabController.enableDevice(2, 1,checked,1)
                            }
                        }

                        MyToggleButton {
                            id: cntrlFeet3
                            text: "3rd controller"
                            checked: false
                            Layout.maximumWidth: 220
                            Layout.minimumWidth: 220
                            Layout.preferredWidth: 220
                            Layout.fillWidth: true
                            onCheckedChanged: {
                                WalkInPlaceTabController.enableDevice(2, 2,checked,1)
                            }
                        }

                        MyToggleButton {
                            id: cntrlFeet4
                            text: "4th controller"
                            checked: false
                            Layout.maximumWidth: 220
                            Layout.minimumWidth: 220
                            Layout.preferredWidth: 220
                            Layout.fillWidth: true
                            onCheckedChanged: {
                                WalkInPlaceTabController.enableDevice(2, 3,checked,1)
                            }
                        }
                    }
                }
            }
        }

        ColumnLayout {
            spacing: 7
            Layout.alignment: Qt.AlignHCenter

            GroupBox {
                Layout.minimumWidth: 1300
                Layout.alignment: Qt.AlignHCenter
                anchors.centerIn: parent

                label: MyText {
                    text: "Trackers compared to model"
                }

                background: Rectangle {
                    color: myPalette.mid
                    border.color: myPalette.mid
                    radius: 1
                }

                ColumnLayout {
                    anchors.fill: parent

                    RowLayout {
                        spacing: 7
                        MyToggleButton {
                            id: trkrFeet1
                            text: "1st tracker"
                            checked: false
                            Layout.maximumWidth: 220
                            Layout.minimumWidth: 220
                            Layout.preferredWidth: 220
                            Layout.fillWidth: true
                            onCheckedChanged: {
                                WalkInPlaceTabController.enableDevice(3,0,checked,1)
                            }
                        }

                        MyToggleButton {
                            id: trkrFeet2
                            text: "2nd tracker"
                            checked: false
                            Layout.maximumWidth: 220
                            Layout.minimumWidth: 220
                            Layout.preferredWidth: 220
                            Layout.fillWidth: true
                            onCheckedChanged: {
                                WalkInPlaceTabController.enableDevice(3,1,checked,1)
                            }
                        }

                        MyToggleButton {
                            id: trkrFeet3
                            text: "3rd tracker"
                            checked: false
                            Layout.maximumWidth: 220
                            Layout.minimumWidth: 220
                            Layout.preferredWidth: 220
                            Layout.fillWidth: true
                            onCheckedChanged: {
                                WalkInPlaceTabController.enableDevice(3,2,checked,1)
                            }
                        }

                        MyToggleButton {
                            id: trkrFeet4
                            text: "4th tracker"
                            checked: false
                            Layout.maximumWidth: 220
                            Layout.minimumWidth: 220
                            Layout.preferredWidth: 220
                            Layout.fillWidth: true
                            onCheckedChanged: {
                                WalkInPlaceTabController.enableDevice(3,3,checked,1)
                            }
                        }
                    }
                }
            }
        }

        ColumnLayout {
            spacing: 7
            Layout.alignment: Qt.AlignHCenter

            GroupBox {
                Layout.minimumWidth: 1300
                Layout.alignment: Qt.AlignHCenter
                anchors.centerIn: parent

                label: MyText {
                    text: "Trackers for pace only"
                }

                background: Rectangle {
                    color: myPalette.mid
                    border.color: myPalette.mid
                    radius: 1
                }

                ColumnLayout {
                    anchors.fill: parent

                    RowLayout {
                        spacing: 7
                        MyToggleButton {
                            id: trkrHands1
                            text: "1st tracker"
                            checked: false
                            Layout.maximumWidth: 220
                            Layout.minimumWidth: 220
                            Layout.preferredWidth: 220
                            Layout.fillWidth: true
                            onCheckedChanged: {
                                WalkInPlaceTabController.enableDevice(3,0,checked,0)
                            }
                        }

                        MyToggleButton {
                            id: trkrHands2
                            text: "2nd tracker"
                            checked: false
                            Layout.maximumWidth: 220
                            Layout.minimumWidth: 220
                            Layout.preferredWidth: 220
                            Layout.fillWidth: true
                            onCheckedChanged: {
                                WalkInPlaceTabController.enableDevice(3,1,checked,0)
                            }
                        }

                        MyToggleButton {
                            id: trkrHands3
                            text: "3rd tracker"
                            checked: false
                            Layout.maximumWidth: 220
                            Layout.minimumWidth: 220
                            Layout.preferredWidth: 220
                            Layout.fillWidth: true
                            onCheckedChanged: {
                                WalkInPlaceTabController.enableDevice(3,2,checked,0)
                            }
                        }

                        MyToggleButton {
                            id: trkrHands4
                            text: "4th tracker"
                            checked: false
                            Layout.maximumWidth: 220
                            Layout.minimumWidth: 220
                            Layout.preferredWidth: 220
                            Layout.fillWidth: true
                            onCheckedChanged: {
                                WalkInPlaceTabController.enableDevice(3,3,checked,0)
                            }
                        }
                    }
                }
            }
        }

        /*ColumnLayout {
            spacing: 7
            Layout.alignment: Qt.AlignHCenter

            GroupBox {
                Layout.minimumWidth: 1300
                Layout.alignment: Qt.AlignHCenter
                anchors.centerIn: parent

                label: MyText {
                    text: "Direction"
                }

                background: Rectangle {
                    color: myPalette.mid
                    border.color: myPalette.mid
                    radius: 1
                }

                ColumnLayout {
                    anchors.fill: parent

                    RowLayout {
                        spacing: 7
                        MyComboBox {
                            id: directionDevice 
                            currentIndex: 0
                            Layout.maximumWidth: 400
                            Layout.minimumWidth: 400
                            Layout.preferredWidth: 400
                            Layout.fillWidth: true
                            displayText: currentText
                            model: ["None, Use Game Default","1st controller", "2nd controller", "1st tracker", "2nd tracker", "3rd controller", "4th controller", "3rd tracker", "4th tracker"]
                            onCurrentIndexChanged: {
                                if (currentIndex >= 0) { 
                                   WalkInPlaceTabController.setDirectionDevice(currentIndex)                        
                                } 
                            }
                        }
                    }
                }
            }
        }*/

        Component.onCompleted: {   
            if ( !initialLoaded ) { 
                updateInfo()
            }
            initialLoaded = true
        }

    }

    function stopTimer() {

    }
}
