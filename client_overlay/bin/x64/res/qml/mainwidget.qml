import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0


Rectangle {
    SystemPalette { id: myPalette; colorGroup: SystemPalette.Active }

    id: root
    color: myPalette.base // "transparent"
    width: 1200
    height: 800

    property StepDetectionPage stepDetectionPage:  StepDetectionPage {
        stackView: mainView
    }

    property StepDetectGraphPage stepDetectGraphPage:  StepDetectGraphPage {
        stackView: mainView
    }

    property DisableDevicePage disableDevicePage:  DisableDevicePage {
        stackView: mainView
    }

    StackView {
        id: mainView
        anchors.fill: parent

        function startTimer() {
        	currentItem.startTimer()
        }
        function stopTimer() {
        	currentItem.stopTimer()
        }
        
	    function setAutoConfHMDY(v) {
	    	currentItem.setAutoConfHMDY(v)
	    }

	    function setAutoConfHMDXZ(v) {
	    	currentItem.setAutoConfHMDXZ(v)
	    }

	    function setAutoConfUseTrackers(v) {
	    	currentItem.setAutoConfUseTrackers(v)
	    }

	    function setAutoConfDisableHMD(v) {
	    	currentItem.setAutoConfDisableHMD(v)
	    }

	    function setAutoConfTRKY(v) {
	    	currentItem.setAutoConfTRKY(v)
	    }

	    function setAutoConfTRKXZ(v) {
	    	currentItem.setAutoConfTRKXZ(v)
	    }

	    function setAutoConfContMin(v) {
	    	currentItem.setAutoConfContMin(v)
	    }

	    function setAutoConfContMid(v) {
	    	currentItem.setAutoConfContMid(v)
	    }

	    function setAutoConfContMax(v) {
	    	currentItem.setAutoConfContMax(v)
	    }

		pushEnter: Transition {
			PropertyAnimation {
				property: "x"
				from: mainView.width
				to: 0
				duration: 200
			}
		}
		pushExit: Transition {
			PropertyAnimation {
				property: "x"
				from: 0
				to: -mainView.width
				duration: 200
			}
		}
		popEnter: Transition {
			PropertyAnimation {
				property: "x"
				from: -mainView.width
				to: 0
				duration: 200
			}
		}
		popExit: Transition {
			PropertyAnimation {
				property: "x"
				from: 0
				to: mainView.width
				duration: 200
			}
		}

        initialItem: stepDetectionPage
    }
}
