import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2
import pottedmeat7.walkinplace 1.0

MyStackViewPage {
    id: dataModelPage
    name: "dataModelPage"

    property real timerInterval     : 1 / 4 * 1000

    property var hmdYPoints : []
    property var cntrl1YPoints : []
    property var cntrl2YPoints : []
    property var tracker1YPoints : []
    property var tracker2YPoints : []
    property var touchPoints : []   

    property var hmdSample : [0,0]
    property var cntrlSample : [0,0]
    property var trkrSample : [0,0]

    property real canvasPlotMaxPoint : 0.6
    property real canvasPlotMinPoint : -0.6

    property var thresholdStep : 0.01
    property var minTouch : 0.00
    property var midTouch : 0.5
    property var maxTouch : 1.0

    property var startTimer: function() {
        getModelData()
        if ( WalkInPlaceTabController.isWIPEnabled() ) {
            refreshTimer.start()
        }
    }

    property var stopTimer: function() {
        hmdYPoints = []
        cntrl1YPoints = []
        cntrl2YPoints = []
        tracker1YPoints = []
        tracker2YPoints = []
        touchPoints = []

        canvasPlotMaxPoint = 0.6
        canvasPlotMinPoint = -0.6
    }

    content: Item {
        id:cntrlainer

        GroupBox {
            Layout.fillWidth: true
            
            background: Rectangle {
                color: myPalette.base
                border.color: myPalette.base
                radius: 1
            }

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
                        Layout.maximumWidth: 640
                        Layout.minimumWidth: 640
                        Layout.preferredWidth: 640
                        font.pointSize: 22
                        anchors.verticalCenter: headerBackButton.verticalCenter
                        Layout.leftMargin: 30
                    }

                    MyText {
                        id: headerSpacer
                        text: " "
                        font.pointSize: 22
                        anchors.verticalCenter: headerBackButton.verticalCenter
                        Layout.maximumWidth: 120
                        Layout.minimumWidth: 120
                        Layout.preferredWidth: 120
                    }
                }

                GridLayout {
                    columns: 7

                    MyText {
                        text: " "
                        Layout.preferredWidth: 100
                    }  

                    MyText {
                        text: "Slow Pace Touch"
                        horizontalAlignment: Text.AlignHCenter
                        Layout.preferredWidth: 220
                    }

                    MyText {
                        text: " "
                        Layout.preferredWidth: 70
                    }  

                    MyText {
                        text: "Medium Pace Touch"
                        horizontalAlignment: Text.AlignHCenter
                        Layout.preferredWidth: 220
                    }

                    MyText {
                        text: " "
                        Layout.preferredWidth: 70
                    }  

                    MyText {
                        text: "Max Pace Touch"
                        horizontalAlignment: Text.AlignHCenter
                        Layout.preferredWidth: 220
                    }

                    MyText {
                        text: " "
                        Layout.preferredWidth: 70
                    }  
                }

                GridLayout {
                    columns: 16

                    MyText {
                        text: " "
                        Layout.preferredWidth: 100
                    }  

                    MyPushButton {
                        id: walkTouchMinusButton
                        Layout.preferredWidth: 40
                        text: "-"
                        onClicked: {
                            var val = minTouch - thresholdStep
                            minTouch = val
                            WalkInPlaceTabController.setMaxTouch(val)
                        }
                    }

                    MyTextField {
                        id: walkTouchInputField
                        text: "0.0"
                        keyBoardUID: 120
                        Layout.preferredWidth: 140
                        horizontalAlignment: Text.AlignHCenter
                        function onInputEvent(input) {
                            var val = parseFloat(input)
                            if (!isNaN(val)) {
                                minTouch = val
                                WalkInPlaceTabController.setMinTouch(val);
                            } 
                        }
                    }

                    MyPushButton {
                        id: walkTouchPlusButton
                        Layout.preferredWidth: 40
                        text: "+"
                        onClicked: {
                            var val = minTouch + thresholdStep
                            minTouch = val
                            WalkInPlaceTabController.setMaxTouch(val)
                        }
                    }

                    MyText {
                        text: " "
                        Layout.preferredWidth: 70
                    }  

                    MyPushButton {
                        id: midTouchMinusButton
                        Layout.preferredWidth: 40
                        text: "-"
                        onClicked: {
                            var val = midTouch - thresholdStep
                            midTouch = val
                            WalkInPlaceTabController.setMaxTouch(val);
                        }
                    }

                    MyTextField {
                        id: midTouchInputField
                        text: "0.5"
                        keyBoardUID: 121
                        Layout.preferredWidth: 140
                        horizontalAlignment: Text.AlignHCenter
                        function onInputEvent(input) {
                            var val = parseFloat(input)
                            if (!isNaN(val)) {
                                midTouch = val
                                WalkInPlaceTabController.setMidTouch(val);
                            } 
                        }
                    }

                    MyPushButton {
                        id: midTouchPlusButton
                        Layout.preferredWidth: 40
                        text: "+"
                        onClicked: {
                            var val = midTouch + thresholdStep
                            midTouch = val
                            WalkInPlaceTabController.setMaxTouch(val);
                        }
                    }

                    MyText {
                        text: " "
                        Layout.preferredWidth: 70
                    }  

                    MyPushButton {
                        id: maxTouchMinusButton
                        Layout.preferredWidth: 40
                        text: "-"
                        onClicked: {
                            var val = maxTouch - thresholdStep
                            maxTouch = val
                            WalkInPlaceTabController.setMaxTouch(val);
                        }
                    }

                    MyTextField {
                        id: maxTouchInputField
                        text: "1.0"
                        keyBoardUID: 122
                        Layout.preferredWidth: 140
                        horizontalAlignment: Text.AlignHCenter
                        function onInputEvent(input) {
                            var val = parseFloat(input)
                            if (!isNaN(val)) {
                                maxTouch = val
                                WalkInPlaceTabController.setMaxTouch(val);
                            } 
                        }
                    }

                    MyPushButton {
                        id: maxTouchPlusButton
                        Layout.preferredWidth: 40
                        text: "+"
                        onClicked: {
                            var val = maxTouch + thresholdStep
                            maxTouch = val
                            WalkInPlaceTabController.setMaxTouch(val);
                        }
                    }

                    MyText {
                        text: " "
                        Layout.preferredWidth: 70
                    }  
                }

                GridLayout {
                    columns: 6

                    MyText {
                        text: " "
                        Layout.preferredWidth: 100
                    }  

                    MyToggleButton {
                        id: showHMD
                        text: "Show HMD"
                        checked: true
                        Layout.maximumWidth: 150
                        Layout.minimumWidth: 150
                        Layout.preferredWidth: 150
                        Layout.fillWidth: true
                        onCheckedChanged: {
                            modelCanvas.requestPaint ();
                        }
                    }

                    MyText {
                        text: " "
                        Layout.preferredWidth: 70
                    }  

                    MyToggleButton {
                        id: showTrackers
                        text: "Show Trackers"
                        checked: true
                        Layout.maximumWidth: 150
                        Layout.minimumWidth: 150
                        Layout.preferredWidth: 150
                        Layout.fillWidth: true
                        onCheckedChanged: {
                            modelCanvas.requestPaint ();
                        }
                    }

                    MyText {
                        text: " "
                        Layout.preferredWidth: 70
                    }  

                    MyToggleButton {
                        id: showControllers
                        text: "Show Controllers"
                        checked: true
                        Layout.maximumWidth: 120
                        Layout.minimumWidth: 120
                        Layout.preferredWidth: 120
                        Layout.fillWidth: true
                        onCheckedChanged: {
                            modelCanvas.requestPaint ();
                        }
                    }
                }
            }
        }

        Column {
            spacing: 6 
            anchors.fill: parent
            topPadding: 130

            GridLayout {
                columns: 1
                rows: 1
                width: 1200
                height: 700

                Canvas {
                    id: modelCanvas
                    width: 1200
                    height: 600
                    antialiasing: true
                    Layout.row: 1
                    Layout.column: 1
                    Layout.columnSpan: 1
                    Layout.rowSpan: 1

                    property int rectWidth: 1200
                    property int rectHeight: 600
                    property int circleRad: 15
                    property int topX : 50
                    property int topY : 0
                    property real vResolution: 0.25

                    onPaint: {
                        var ctx = getContext("2d");                        
                        ctx.clearRect(0,0,modelCanvas.width, modelCanvas.height);
                        var yTicks = Math.ceil((canvasPlotMaxPoint-canvasPlotMinPoint)/vResolution);
                        var crossedZeroLine = false
                        var valToCrossZero = canvasPlotMaxPoint
                        var graphXScale = ((rectWidth-topX) > hmdYPoints.length ? ((rectWidth-topX)/hmdYPoints.length) : (hmdYPoints.length/(rectWidth-topX)));
                        ctx.fillStyle = "#FFFFFF"
                        for(var y=topY; y<=rectHeight+topY; y+=Math.ceil(rectHeight/yTicks)){
                            ctx.fillText(valToCrossZero.toFixed(2), 10, y);
                            ctx.beginPath();        
                            ctx.strokeStyle = "#CCCCCC";
                            ctx.lineWidth = 1;
                            if ( !crossedZeroLine ) {
                                if ( valToCrossZero <= 0 ) { 
                                    ctx.lineWidth = 3
                                    crossedZeroLine = true
                                } 
                            }
                            ctx.moveTo(topX,y);
                            ctx.lineTo(rectWidth,y);
                            ctx.stroke();
                            ctx.closePath();
                            valToCrossZero -= vResolution;
                        }
                        if ( showHMD.checked ) {
                            var lastHMDYPoint = rectHeight/2 + topY
                            var lastX = 50;
                            for(var x=0; x<hmdYPoints.length; x++) {
                                ctx.beginPath();  
                                ctx.strokeStyle = "#00DD00";
                                ctx.lineWidth = 1;
                                ctx.moveTo(lastX,lastHMDYPoint);
                                var val = hmdYPoints[x];
                                ctx.lineTo(lastX+graphXScale,val+topY);
                                lastHMDYPoint = val+topY;
                                ctx.stroke();
                                ctx.closePath();
                                lastX = lastX + graphXScale;
                            }
                        }
                        if ( showControllers.checked ) {
                            var lastcntrl1YPoint = rectHeight/2 + topY
                            var lastcntrl2YPoint = rectHeight/2 + topY
                            var lastcntrlSNYPoint = rectHeight/2 + topY
                            var lastX = 50
                            var lastSNX = 50
                            var validSNMki = cntrlSample[1]
                            for(var x=0; x<cntrl1YPoints.length; x++) {
                                ctx.beginPath();  
                                ctx.strokeStyle = "#DDDD00";
                                ctx.lineWidth = 1;
                                ctx.moveTo(lastX,lastcntrl1YPoint);
                                var val = cntrl1YPoints[x];
                                ctx.lineTo(lastX+graphXScale,val+topY);
                                lastcntrl1YPoint = val+topY;
                                ctx.stroke();
                                ctx.closePath();
                                ctx.beginPath();  
                                ctx.strokeStyle = "#00DDDD";
                                ctx.lineWidth = 1;
                                ctx.moveTo(lastX,lastcntrl2YPoint);
                                var val = cntrl2YPoints[x];
                                ctx.lineTo(lastX+graphXScale,val+topY);
                                lastcntrl2YPoint = val+topY;
                                ctx.stroke();
                                ctx.closePath();
                                if ( x == validSNMki && x < cntrlSample.length-2) {
                                    ctx.beginPath();  
                                    ctx.strokeStyle = "#DDDDDD";
                                    ctx.lineWidth = 2;
                                    ctx.moveTo(lastSNX,lastcntrlSNYPoint);
                                    var val = cntrlSample[x+2];
                                    ctx.lineTo(lastSNX+graphXScale,val+topY);
                                    lastcntrlSNYPoint = val+topY;
                                    ctx.stroke();
                                    ctx.closePath();                          
                                    lastSNX = lastSNX + graphXScale;
                                }
                                lastX = lastX + graphXScale;
                            }
                        }
                        if ( showTrackers.checked ) {
                            var lasttracker1YPoint = rectHeight/2 + topY
                            var lasttracker2YPoint = rectHeight/2 + topY
                            var lastX = 50;
                            for(var x=0; x<tracker1YPoints.length; x++) {
                                ctx.beginPath();  
                                ctx.strokeStyle = "#FFBB00";
                                ctx.lineWidth = 1;
                                ctx.moveTo(lastX,lasttracker1YPoint);
                                var val = tracker1YPoints[x];
                                ctx.lineTo(lastX+graphXScale,val+topY);
                                lasttracker1YPoint = val+topY;
                                ctx.stroke();
                                ctx.closePath();
                                ctx.beginPath();  
                                ctx.strokeStyle = "#FF7700";
                                ctx.lineWidth = 1;
                                ctx.moveTo(lastX,lasttracker2YPoint);
                                var val = tracker2YPoints[x];
                                ctx.lineTo(lastX+graphXScale,val+topY);
                                lasttracker2YPoint = val+topY;
                                ctx.stroke();
                                ctx.closePath();
                                lastX = lastX + graphXScale;
                            }
                        }
                        var lastTouchPoint = rectHeight/2 + topY;
                        var lastX = 50;
                        for(var x=0; x<touchPoints.length; x++) {
                            ctx.beginPath();                
                            ctx.strokeStyle = "#FFFFFF";
                            ctx.lineWidth = 2;
                            ctx.moveTo(lastX,lastTouchPoint);
                            var val = touchPoints[x];
                            ctx.lineTo(lastX+graphXScale,val+topY);
                            lastTouchPoint = val+topY;
                            ctx.stroke();
                            ctx.closePath();
                            lastX = lastX + graphXScale;
                        }
                    }
                }
            }
        }

        Component.onCompleted: {
        }
    }

    function getModelData() {
        try {
            var velVals = WalkInPlaceTabController.getModelData();
            var i = 0;
            while(i<velVals.length) {
                var velY = parseFloat(velVals[i]).toFixed(4);
                if ( Math.abs(velY) > canvasPlotMaxPoint ) {
                    canvasPlotMaxPoint = Math.abs(velY);
                    canvasPlotMinPoint = -canvasPlotMaxPoint;
                }
                var r = (canvasPlotMaxPoint - canvasPlotMinPoint)  
                var g = (modelCanvas.rectHeight)
                velY = (((velY - canvasPlotMinPoint) * g) / r)
                hmdYPoints.push(velY);

                velY = parseFloat(velVals[i+1]).toFixed(4);
                if ( Math.abs(velY) > canvasPlotMaxPoint ) {
                    canvasPlotMaxPoint = Math.abs(velY);
                    canvasPlotMinPoint = -canvasPlotMaxPoint;
                }
                var r = (canvasPlotMaxPoint - canvasPlotMinPoint)  
                var g = (modelCanvas.rectHeight)
                velY = (((velY - canvasPlotMinPoint) * g) / r)
                cntrl1YPoints.push(velY);

                velY = parseFloat(velVals[i+2]).toFixed(4);
                if ( Math.abs(velY) > canvasPlotMaxPoint ) {
                    canvasPlotMaxPoint = Math.abs(velY);
                    canvasPlotMinPoint = -canvasPlotMaxPoint;
                }
                var r = (canvasPlotMaxPoint - canvasPlotMinPoint)  
                var g = (modelCanvas.rectHeight)
                velY = (((velY - canvasPlotMinPoint) * g) / r)
                cntrl2YPoints.push(velY);

                velY = parseFloat(velVals[i+3]).toFixed(4);
                if ( Math.abs(velY) > canvasPlotMaxPoint ) {
                    canvasPlotMaxPoint = Math.abs(velY);
                    canvasPlotMinPoint = -canvasPlotMaxPoint;
                }
                var r = (canvasPlotMaxPoint - canvasPlotMinPoint)  
                var g = (modelCanvas.rectHeight)
                velY = (((velY - canvasPlotMinPoint) * g) / r)
                tracker1YPoints.push(velY);

                velY = parseFloat(velVals[i+4]).toFixed(4);
                if ( Math.abs(velY) > canvasPlotMaxPoint ) {
                    canvasPlotMaxPoint = Math.abs(velY);
                    canvasPlotMinPoint = -canvasPlotMaxPoint;
                }
                var r = (canvasPlotMaxPoint - canvasPlotMinPoint)  
                var g = (modelCanvas.rectHeight)
                velY = (((velY - canvasPlotMinPoint) * g) / r)
                tracker2YPoints.push(velY);

                var graphPoint = velVals[i+5] * canvasPlotMaxPoint;
                var finalPoint = (((graphPoint - canvasPlotMinPoint) * g) / r)
                touchPoints.push(-1*finalPoint)

                i = i + 6;
            }

            modelCanvas.requestPaint ();

        } catch (error) {         
            console.info(error.message)   
        }
    }

    Timer {
        id: refreshTimer
        interval: timerInterval
        running: false
        repeat: true
        onTriggered: {
            try {
                cntrlSample = WalkInPlaceTabController.getValidCNTRLSample()
                //console.info(cntrlSample);
                modelCanvas.requestPaint ();
            } catch (error) {         
                console.info(error.message)   
            }
        }
    }

}