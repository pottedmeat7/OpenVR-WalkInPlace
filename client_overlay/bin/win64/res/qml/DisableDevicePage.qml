import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2
import pottedmeat7.walkinplace 1.0

MyStackViewPage {
    id: disableDevicePage
    name: "disableDevicePage"

    content: Item {
        id:container

        GroupBox {
            Layout.fillWidth: true            

            ColumnLayout {
                anchors.fill: parent
                width: 1200
                
                RowLayout {
                    width: 1200
                    Button {
                        id: headerBackButton
                        Layout.preferredHeight: 60
                        Layout.preferredWidth: 60
                        hoverEnabled: true
                        enabled: true
                        opacity: 1.0
                        contentItem: Image {
                            source: "backarrow.svg"
                            sourceSize.width: 60
                            sourceSize.height: 60
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

                    Text {
                        id: headerTitle
                        text: " "
                        Layout.maximumWidth: 1100
                        Layout.minimumWidth: 1100
                        Layout.preferredWidth: 1100
                        font.pointSize: 22
                        anchors.verticalCenter: headerBackButton.verticalCenter
                        Layout.leftMargin: 30
                    }
                }
            }
        }

        Column {
            spacing: 6 
            anchors.fill: parent
            topPadding: 80
            
            GroupBox {

                Layout.fillWidth: true

                ColumnLayout {
                    anchors.fill: parent
                    Layout.alignment: Qt.AlignHCenter

                    GridLayout {
                        columns: 4

                        CheckBox {
                            id: disableButtonA
                            text: "Disable?"
                            Layout.maximumWidth: 200
                            Layout.minimumWidth: 200
                            Layout.preferredWidth: 200
                            Layout.fillWidth: true
                            onCheckedChanged: {
                                //WalkInPlaceTabController.disableController(checked,0)
                            }
                        }

                        Text {
                            text: "Controller 1"
                            Layout.preferredWidth: 200
                        }  

                        CheckBox {
                            id: disableButtonB
                            text: "Disable?"
                            Layout.maximumWidth: 200
                            Layout.minimumWidth: 200
                            Layout.preferredWidth: 200
                            Layout.fillWidth: true
                            onCheckedChanged: {
                                //WalkInPlaceTabController.disableController(checked,1)
                            }
                        }
                        
                        Text {
                            text: "Controller 2"
                            Layout.preferredWidth: 200
                        }  
                    }

                    GridLayout {
                        columns: 4

                        CheckBox {
                            id: disableButtonC
                            text: "Disable?"
                            Layout.maximumWidth: 200
                            Layout.minimumWidth: 200
                            Layout.preferredWidth: 200
                            Layout.fillWidth: true
                            onCheckedChanged: {
                                //WalkInPlaceTabController.disableTracker(checked,0)
                            }
                        }

                        Text {
                            text: "Controller 1"
                            Layout.preferredWidth: 200
                        }  

                        CheckBox {
                            id: disableButtonD
                            text: "Disable?"
                            Layout.maximumWidth: 200
                            Layout.minimumWidth: 200
                            Layout.preferredWidth: 200
                            Layout.fillWidth: true
                            onCheckedChanged: {
                                //WalkInPlaceTabController.disableTracker(checked,1)
                            }
                        }
                        
                        Text {
                            text: "Controller 2"
                            Layout.preferredWidth: 200
                        }  
                    }

                    GridLayout {
                        columns: 4

                        CheckBox {
                            id: disableButtonE
                            text: "Disable?"
                            Layout.maximumWidth: 200
                            Layout.minimumWidth: 200
                            Layout.preferredWidth: 200
                            Layout.fillWidth: true
                            onCheckedChanged: {
                                //WalkInPlaceTabController.disableTracker(checked,4)
                            }
                        }

                        Text {
                            text: "Controller 1"
                            Layout.preferredWidth: 200
                        }  

                        CheckBox {
                            id: disableButtonF
                            text: "Disable?"
                            Layout.maximumWidth: 200
                            Layout.minimumWidth: 200
                            Layout.preferredWidth: 200
                            Layout.fillWidth: true
                            onCheckedChanged: {
                                //WalkInPlaceTabController.disableTracker(checked,5)
                            }
                        }
                        
                        Text {
                            text: "Controller 2"
                            Layout.preferredWidth: 200
                        }  
                    }                    
                    GridLayout {
                        columns: 4

                        CheckBox {
                            id: disableButtonG
                            text: "Disable?"
                            Layout.maximumWidth: 200
                            Layout.minimumWidth: 200
                            Layout.preferredWidth: 200
                            Layout.fillWidth: true
                            onCheckedChanged: {
                                //WalkInPlaceTabController.disableTracker(checked,6)
                            }
                        }

                        Text {
                            text: "Controller 1"
                            Layout.preferredWidth: 200
                        }  

                        CheckBox {
                            id: disableButtonH
                            text: "Disable?"
                            Layout.maximumWidth: 200
                            Layout.minimumWidth: 200
                            Layout.preferredWidth: 200
                            Layout.fillWidth: true
                            onCheckedChanged: {
                                //WalkInPlaceTabController.disableTracker(checked,7)
                            }
                        }
                        
                        Text {
                            text: "Controller 2"
                            Layout.preferredWidth: 200
                        }  
                    }
                }
            }
        }

        Component.onCompleted: {
        }
    }

}