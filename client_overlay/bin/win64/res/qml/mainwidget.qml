import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0


Rectangle {
    id: root
    color: "transparent"
    width: 1200
    height: 800

    property StepDetectionPage stepDetectionPage:  StepDetectionPage {
        stackView: mainView
    }

    property StepDetectGraphPage stepDetectGraphPage:  StepDetectGraphPage {
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
	    	stepDetectionPage.setAutoConfHMDY(v)
	    }

	    function setAutoConfHMDXZ(v) {
	    	stepDetectionPage.setAutoConfHMDXZ(v)
	    }

	    function setAutoConfUseTrackers(v) {
	    	stepDetectionPage.setAutoConfUseTrackers(v)
	    }

	    function setAutoConfDisableHMD(v) {
	    	stepDetectionPage.setAutoConfDisableHMD(v)
	    }

	    function setAutoConfTRKY(v) {
	    	stepDetectionPage.setAutoConfTRKY(v)
	    }

	    function setAutoConfTRKXZ(v) {
	    	stepDetectionPage.setAutoConfTRKXZ(v)
	    }

	    function setAutoConfContMin(v) {
	    	stepDetectionPage.setAutoConfContMin(v)
	    }

	    function setAutoConfContMid(v) {
	    	stepDetectionPage.setAutoConfContMid(v)
	    }

	    function setAutoConfContMax(v) {
	    	stepDetectionPage.setAutoConfContMax(v)
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
