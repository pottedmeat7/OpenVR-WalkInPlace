import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2
import pottedmeat7.walkinplace 1.0

MyStackViewPage {
    id: graphPage
    name: "graphPage"

    property bool stepDetected : false

    property real timerInterval     : 1 / 45 * 1000
    property var stepDetects : []

    property var hmdXPoints : []
    property var hmdYPoints : []
    property var hmdZPoints : []

    property var cont1XPoints : []
    property var cont1YPoints : []
    property var cont1ZPoints : []

    property var cont2XPoints : []
    property var cont2YPoints : []
    property var cont2ZPoints : []

    property var tracker1XPoints : []
    property var tracker1YPoints : []
    property var tracker1ZPoints : []

    property var tracker2XPoints : []
    property var tracker2YPoints : []
    property var tracker2ZPoints : []

    property real hmdPlotMaxPoint : 0.6
    property real hmdPlotMinPoint : -0.6

    property real contPlotMaxPoint : 1.0
    property real contPlotMinPoint : -1.0

    property real trackerPlotMaxPoint : 0.6
    property real trackerPlotMinPoint : -0.6

    property int autoConfMode          : -1
    property real totalSampleTime      : 25000
    property real currentSampleTime    : 0
    property real totalSampleCount     : 0

    property int sampleTime : 0
    property int sampleLimit : 5000

    property real tdiff : 0

    property var startTimer: function() {
        refreshTimer.start()
    }
    
    property var stopTimer: function() {
        refreshTimer.stop()
        hmdXPoints = []
        hmdYPoints = []
        hmdZPoints = []

        cont1XPoints = []
        cont1YPoints = []
        cont1ZPoints = []

        cont2XPoints = []
        cont2YPoints = []
        cont2ZPoints = []

        tracker1XPoints = []
        tracker1YPoints = []
        tracker1ZPoints = []

        tracker2XPoints = []
        tracker2YPoints = []
        tracker2ZPoints = []

        stepDetects = []

        hmdPlotMaxPoint = 0.6
        hmdPlotMinPoint = -0.6

        contPlotMaxPoint = 0.6
        contPlotMinPoint = -0.6

        trackerPlotMaxPoint = 0.6
        trackerPlotMinPoint = -0.6
    }

    property var resetGraph: function() {
        var r = (hmdPlotMaxPoint - hmdPlotMinPoint)  
        var g = (hmdCanvas.rectHeight)
        var velRes = (((0 - hmdPlotMinPoint) * g) / r)
        while ( hmdXPoints.length < hmdCanvas.seconds*hmdCanvas.sections ) {
            hmdXPoints.push(velRes);
            hmdYPoints.push(velRes);
            hmdZPoints.push(velRes);
            stepDetects.push(velRes);
        }
        var r = (contPlotMaxPoint - contPlotMinPoint)  
        var g = (contCanvas.rectHeight)
        var velRes = (((0 - contPlotMinPoint) * g) / r)
        while ( cont1XPoints.length < contCanvas.seconds*contCanvas.sections ) {
            cont1XPoints.push(velRes);
            cont1YPoints.push(velRes);
            cont1ZPoints.push(velRes);
        }
        while ( cont2XPoints.length < contCanvas.seconds*contCanvas.sections ) {
            cont2XPoints.push(velRes);
            cont2YPoints.push(velRes);
            cont2ZPoints.push(velRes);
        }
        var r = (trackerPlotMaxPoint - trackerPlotMinPoint)  
        var g = (trackerCanvas.rectHeight)
        var velRes = (((0 - trackerPlotMinPoint) * g) / r)
        while ( tracker1XPoints.length < trackerCanvas.seconds*trackerCanvas.sections ) {
            tracker1XPoints.push(velRes);
            tracker1YPoints.push(velRes);
            tracker1ZPoints.push(velRes);
        }
        while ( tracker2XPoints.length < trackerCanvas.seconds*trackerCanvas.sections ) {
            tracker2XPoints.push(velRes);
            tracker2YPoints.push(velRes);
            tracker2ZPoints.push(velRes);
        }
    }
    

    content: Item {
        id:container

        GroupBox {
            Layout.fillWidth: true
            
            background: Rectangle {
                color: myPalette.base // "#c0c0c0" // "#277650" // "transparent"
                border.color: myPalette.base // "#c0c0c0" // "#277650"
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
                            color: myPalette.base // "#c0c0c0"
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

                    MyPushButton {
                        text: "Generate Data Model"
                        Layout.maximumWidth: 350
                        Layout.minimumWidth: 350
                        Layout.preferredWidth: 350
                        onClicked: {
                            stopTimer()
                            resetGraph()
                            autoConfigPopup.openPopup()
                            autoConfMode = 0
                            sampleTime = 0
                            currentSampleTime = 0
                        }
                    }
                }
            }
        }

        Column {
            spacing: 6 
            anchors.fill: parent
            topPadding: 80

            GridLayout {
                columns: 2
                rows: 2
                width: 1200
                height: 700

                Canvas {
                    id: hmdCanvas
                    width: 600
                    height: 300
                    antialiasing: true
                    Layout.row: 1
                    Layout.column: 1
                    Layout.columnSpan: 1
                    Layout.rowSpan: 1

                    property int rectWidth: 600
                    property int rectHeight: 300
                    property int circleRad: 15
                    property int topX : 50
                    property int topY : 0
                    property real resolution: 0.1
                    property int seconds : 5
                    property int sections : 5
                    property int period : (rectWidth/(seconds*sections)) // width at 10 mHz for 10 seconds

                    onPaint: {
                        var ctx = getContext("2d");                        
                        ctx.clearRect(0,0,hmdCanvas.width, hmdCanvas.height);
                        ctx.font = "16px sans-serif";
                        ctx.fillStyle = "#222222"
                        ctx.fillRect(0, topY, rectWidth, rectHeight);  
                        ctx.fillStyle = "#FFFFFF"
                        ctx.fillText("HMD", 650, 37);
                        ctx.fillText("X", 725, 37);
                        ctx.fillText("Y", 775, 37);
                        ctx.fillText("Z", 825, 37); 
                        ctx.fillStyle = "#DD0000"
                        ctx.fillRect(700,30,20,20);   
                        ctx.fillStyle = "#00DD00"
                        ctx.fillRect(750,30,20,20);   
                        ctx.fillStyle = "#0000DD"
                        ctx.fillRect(800,30,20,20);   
                        var yTicks = Math.ceil((hmdPlotMaxPoint-hmdPlotMinPoint)/resolution);
                        var crossedZeroLine = false
                        var valToCrossZero = hmdPlotMaxPoint
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
                            valToCrossZero -= resolution;
                        }
                        var lastHMDXPoint = rectHeight/2 + topY
                        var lastHMDYPoint = rectHeight/2 + topY
                        var lastHMDZPoint = rectHeight/2 + topY
                        var lastX = topX;
                        for(var x=topX; x<rectWidth; x+=period) {
                            if ( autoConfMode < 0 && stepDetects.length-1 > (x-topX)/period && stepDetects[Math.floor((x-topX)/period)] == 1) {
                                ctx.beginPath();  
                                if ( stepDetects[Math.floor((x-topX)/period)] == 1 ) {
                                    ctx.fillStyle = "#DDDD00";
                                }
                                if ( stepDetects[Math.floor((x-topX)/period)] == 2 ) {
                                    ctx.fillStyle = "#FFBB00";
                                }
                                if ( stepDetects[Math.floor((x-topX)/period)] == 3 ) {
                                    ctx.fillStyle = "#FF3300";
                                }
                                if ( stepDetects[Math.floor((x-topX)/period)] == 4 ) {
                                    ctx.fillStyle = "#CCCCCC";
                                }
                                ctx.lineWidth = 1;
                                ctx.globalAlpha = 0.5;
                                ctx.fillRect(lastX,lastHMDYPoint-circleRad,circleRad,circleRad);
                                ctx.stroke();
                                ctx.closePath();                      
                                ctx.globalAlpha = 1;
                            } else if ( (x-topX) % (rectWidth/period) == 0 ) {
                                //ctx.beginPath();  
                                //ctx.strokeStyle = "#CCCCCC";
                                //ctx.lineWidth = 1;
                                //ctx.moveTo(lastX,topY);
                                //ctx.lineTo(x,rectHeight+topY);
                                //ctx.stroke();
                                //ctx.closePath();
                            }
                            if ( hmdXPoints.length-1 > (x-topX)/period) {
                                ctx.beginPath();                
                                ctx.strokeStyle = "#DD0000";
                                ctx.lineWidth = 1;
                                ctx.moveTo(lastX,lastHMDXPoint);
                                var val = hmdXPoints[Math.floor((x-topX)/period)];
                                ctx.lineTo(x,val+topY);
                                lastHMDXPoint = val+topY;
                                ctx.stroke();
                                ctx.closePath();
                            }  
                            if ( hmdYPoints.length-1 > (x-topX)/period) {
                                ctx.beginPath();  
                                ctx.strokeStyle = "#00DD00";
                                ctx.lineWidth = 1;
                                ctx.moveTo(lastX,lastHMDYPoint);
                                var val = hmdYPoints[Math.floor((x-topX)/period)];
                                ctx.lineTo(x,val+topY);
                                lastHMDYPoint = val+topY;
                                ctx.stroke();
                                ctx.closePath();
                            }  
                            if ( hmdZPoints.length-1 > (x-topX)/period) {
                                ctx.beginPath();  
                                ctx.strokeStyle = "#0000DD";
                                ctx.lineWidth = 1;
                                ctx.moveTo(lastX,lastHMDZPoint);
                                var val = hmdZPoints[Math.floor((x-topX)/period)];
                                ctx.lineTo(x,val+topY);
                                lastHMDZPoint = val+topY;
                                ctx.stroke();
                                ctx.closePath();
                            }  
                            lastX = x;
                        }
                    }
                }


                Canvas {
                    id: contCanvas
                    width: 600
                    height: 700
                    antialiasing: true
                    Layout.row: 1
                    Layout.column: 2
                    Layout.columnSpan: 1
                    Layout.rowSpan: 2

                    property int rectWidth: 600
                    property int rectHeight: 700
                    property int circleRad: 15
                    property int topX : 25
                    property int topY : 0
                    property real resolution: 0.25
                    property int seconds : 5
                    property int sections : 5
                    property int period : (rectWidth/(seconds*sections)) // width at 10 mHz for 5 seconds

                    onPaint: {
                        var ctx = getContext("2d");                        
                        ctx.clearRect(0,0,contCanvas.width, contCanvas.height);
                        var yTicks = Math.ceil((contPlotMaxPoint-contPlotMinPoint)/resolution);
                        var crossedZeroLine = false
                        var valToCrossZero = contPlotMaxPoint
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
                            valToCrossZero -= resolution;
                        }
                        var lastcont1XPoint = rectHeight/2 + topY
                        var lastcont1YPoint = rectHeight/2 + topY
                        var lastcont1ZPoint = rectHeight/2 + topY
                        var lastcont2XPoint = rectHeight/2 + topY
                        var lastcont2YPoint = rectHeight/2 + topY
                        var lastcont2ZPoint = rectHeight/2 + topY
                        var lastX = topX;
                        for(var x=topX; x<rectWidth; x+=period) {
                            if ( false && autoConfMode < 0 && stepDetects.length-1 > (x-topX)/period && stepDetects[Math.floor((x-topX)/period)] >= 2) {
                                ctx.beginPath();  
                                if ( stepDetects[Math.floor((x-topX)/period)] == 1 ) {
                                    ctx.fillStyle = "#DDDD00";
                                }
                                if ( stepDetects[Math.floor((x-topX)/period)] == 2 ) {
                                    ctx.fillStyle = "#FFBB00";
                                }
                                if ( stepDetects[Math.floor((x-topX)/period)] == 3 ) {
                                    ctx.fillStyle = "#FF3300";
                                }
                                if ( stepDetects[Math.floor((x-topX)/period)] == 4 ) {
                                    ctx.fillStyle = "#CCCCCC";
                                }
                                ctx.lineWidth = 1;
                                ctx.globalAlpha = 0.5;
                                ctx.fillRect(lastX,lastcont1YPoint-circleRad,circleRad,circleRad);
                                ctx.stroke();
                                ctx.fillRect(lastX,lastcont2YPoint-circleRad,circleRad,circleRad);
                                ctx.stroke();
                                ctx.closePath();                      
                                ctx.globalAlpha = 1;
                            } else if ( (x-topX) % (rectWidth/period) == 0 ) {
                                //ctx.beginPath();  
                                //ctx.strokeStyle = "#CCCCCC";
                                //ctx.lineWidth = 1;
                                //ctx.moveTo(lastX,topY);
                                //ctx.lineTo(x,rectHeight+topY);
                                //ctx.stroke();
                                //ctx.closePath();
                            }
                            if ( cont1XPoints.length-1 > (x-topX)/period) {
                                ctx.beginPath();                
                                ctx.strokeStyle = "#DD0000";
                                ctx.lineWidth = 1;
                                ctx.moveTo(lastX,lastcont1XPoint);
                                var val = cont1XPoints[Math.floor((x-topX)/period)];
                                ctx.lineTo(x,val+topY);
                                lastcont1XPoint = val+topY;
                                ctx.stroke();
                                ctx.closePath();
                            }  
                            if ( cont1YPoints.length-1 > (x-topX)/period) {
                                ctx.beginPath();  
                                ctx.strokeStyle = "#00DD00";
                                ctx.lineWidth = 1;
                                ctx.moveTo(lastX,lastcont1YPoint);
                                var val = cont1YPoints[Math.floor((x-topX)/period)];
                                ctx.lineTo(x,val+topY);
                                lastcont1YPoint = val+topY;
                                ctx.stroke();
                                ctx.closePath();
                            }  
                            if ( cont1ZPoints.length-1 > (x-topX)/period) {
                                ctx.beginPath();  
                                ctx.strokeStyle = "#0000DD";
                                ctx.lineWidth = 1;
                                ctx.moveTo(lastX,lastcont1ZPoint);
                                var val = cont1ZPoints[Math.floor((x-topX)/period)];
                                ctx.lineTo(x,val+topY);
                                lastcont1ZPoint = val+topY;
                                ctx.stroke();
                                ctx.closePath();
                            }  
                            if ( cont2XPoints.length-1 > (x-topX)/period) {
                                ctx.beginPath();                
                                ctx.strokeStyle = "#DD00DD";
                                ctx.lineWidth = 1;
                                ctx.moveTo(lastX,lastcont2XPoint);
                                var val = cont2XPoints[Math.floor((x-topX)/period)];
                                ctx.lineTo(x,val+topY);
                                lastcont2XPoint = val+topY;
                                ctx.stroke();
                                ctx.closePath();
                            }  
                            if ( cont2YPoints.length-1 > (x-topX)/period) {
                                ctx.beginPath();  
                                ctx.strokeStyle = "#00DDDD";
                                ctx.lineWidth = 1;
                                ctx.moveTo(lastX,lastcont2YPoint);
                                var val = cont2YPoints[Math.floor((x-topX)/period)];
                                ctx.lineTo(x,val+topY);
                                lastcont2YPoint = val+topY;
                                ctx.stroke();
                                ctx.closePath();
                            }  
                            if ( cont2ZPoints.length-1 > (x-topX)/period) {
                                ctx.beginPath();  
                                ctx.strokeStyle = "#FF55FF";
                                ctx.lineWidth = 1;
                                ctx.moveTo(lastX,lastcont2ZPoint);
                                var val = cont2ZPoints[Math.floor((x-topX)/period)];
                                ctx.lineTo(x,val+topY);
                                lastcont2ZPoint = val+topY;
                                ctx.stroke();
                                ctx.closePath();
                            }  
                            lastX = x;
                        }
                    }
                }  

                Canvas {
                    id: trackerCanvas
                    width: 600
                    height: 400
                    antialiasing: true     
                    Layout.row: 2
                    Layout.column: 1
                    Layout.columnSpan: 1
                    Layout.rowSpan: 1

                    property int rectWidth: 600
                    property int rectHeight: 400
                    property int circleRad: 15
                    property int topX : 50
                    property int topY : 0
                    property real resolution: 0.1
                    property int seconds : 5
                    property int sections : 5
                    property int period : (rectWidth/(seconds*sections)) // width at 10 mHz for 10 seconds

                    onPaint: {
                        var ctx = getContext("2d");                        
                        ctx.clearRect(0,0,trackerCanvas.width, trackerCanvas.height);
                        var yTicks = Math.ceil((trackerPlotMaxPoint-trackerPlotMinPoint)/resolution);
                        var crossedZeroLine = false
                        var valToCrossZero = trackerPlotMaxPoint
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
                            valToCrossZero -= resolution;
                        }
                        var lasttracker1XPoint = rectHeight/2 + topY
                        var lasttracker1YPoint = rectHeight/2 + topY
                        var lasttracker1ZPoint = rectHeight/2 + topY
                        var lasttracker2XPoint = rectHeight/2 + topY
                        var lasttracker2YPoint = rectHeight/2 + topY
                        var lasttracker2ZPoint = rectHeight/2 + topY
                        var lastX = topX;
                        for(var x=topX; x<rectWidth; x+=period) {
                            if ( tracker1XPoints.length-1 > (x-topX)/period) {
                                ctx.beginPath();                
                                ctx.strokeStyle = "#DD0000";
                                ctx.lineWidth = 1;
                                ctx.moveTo(lastX,lasttracker1XPoint);
                                var val = tracker1XPoints[Math.floor((x-topX)/period)];
                                ctx.lineTo(x,val+topY);
                                lasttracker1XPoint = val+topY;
                                ctx.stroke();
                                ctx.closePath();
                            }  
                            if ( tracker1YPoints.length-1 > (x-topX)/period) {
                                ctx.beginPath();  
                                ctx.strokeStyle = "#00DD00";
                                ctx.lineWidth = 1;
                                ctx.moveTo(lastX,lasttracker1YPoint);
                                var val = tracker1YPoints[Math.floor((x-topX)/period)];
                                ctx.lineTo(x,val+topY);
                                lasttracker1YPoint = val+topY;
                                ctx.stroke();
                                ctx.closePath();
                            }  
                            if ( tracker1ZPoints.length-1 > (x-topX)/period) {
                                ctx.beginPath();  
                                ctx.strokeStyle = "#0000DD";
                                ctx.lineWidth = 1;
                                ctx.moveTo(lastX,lasttracker1ZPoint);
                                var val = tracker1ZPoints[Math.floor((x-topX)/period)];
                                ctx.lineTo(x,val+topY);
                                lasttracker1ZPoint = val+topY;
                                ctx.stroke();
                                ctx.closePath();
                            }  
                            if ( tracker2XPoints.length-1 > (x-topX)/period) {
                                ctx.beginPath();                
                                ctx.strokeStyle = "#DD00DD";
                                ctx.lineWidth = 1;
                                ctx.moveTo(lastX,lasttracker2XPoint);
                                var val = tracker2XPoints[Math.floor((x-topX)/period)];
                                ctx.lineTo(x,val+topY);
                                lasttracker2XPoint = val+topY;
                                ctx.stroke();
                                ctx.closePath();
                            }  
                            if ( tracker2YPoints.length-1 > (x-topX)/period) {
                                ctx.beginPath();  
                                ctx.strokeStyle = "#00DDDD";
                                ctx.lineWidth = 1;
                                ctx.moveTo(lastX,lasttracker2YPoint);
                                var val = tracker2YPoints[Math.floor((x-topX)/period)];
                                ctx.lineTo(x,val+topY);
                                lasttracker2YPoint = val+topY;
                                ctx.stroke();
                                ctx.closePath();
                            }  
                            if ( tracker2ZPoints.length-1 > (x-topX)/period) {
                                ctx.beginPath();  
                                ctx.strokeStyle = "#FF55FF";
                                ctx.lineWidth = 1;
                                ctx.moveTo(lastX,lasttracker2ZPoint);
                                var val = tracker2ZPoints[Math.floor((x-topX)/period)];
                                ctx.lineTo(x,val+topY);
                                lasttracker2ZPoint = val+topY;
                                ctx.stroke();
                                ctx.closePath();
                            }  
                            lastX = x;
                        }
                    }
                }
            }
        }

        Component.onCompleted: {
            resetGraph()
        }
    }

    Timer {
        id: refreshTimer
        interval: timerInterval
        running: false
        repeat: true
        onTriggered: {
            try {
                var velVals;
                sampleTime = sampleTime + timerInterval;
                if ( autoConfMode < 0 ) {
                    velVals = WalkInPlaceTabController.getGraphPoses();
                } else {
                    var scaleSpeed = 0
                    currentSampleTime = currentSampleTime + (timerInterval)
                    if ( currentSampleTime >= (totalSampleTime - (totalSampleTime/5.0)) ) {
                        scaleSpeed = 1
                    } else if ( currentSampleTime >= (totalSampleTime - (totalSampleTime/5.0)*2.0) ) {
                        scaleSpeed = (currentSampleTime / totalSampleTime)
                    } else if ( currentSampleTime >= (totalSampleTime - (totalSampleTime/5.0)*3.0) ) {
                        scaleSpeed = 0.5
                    } else if ( currentSampleTime >= (totalSampleTime - (totalSampleTime/5.0)*4.0) ) {
                        scaleSpeed = (currentSampleTime / totalSampleTime)
                    } // else slowest scale at walking pace is zero scale
                    velVals = WalkInPlaceTabController.lrTrainingDataSample(scaleSpeed, timerInterval);
                }
                if ( velVals.length >= 3 ) {
                    var velX = parseFloat(velVals[0]).toFixed(4);
                    var velY = parseFloat(velVals[1]).toFixed(4);
                    var velZ = parseFloat(velVals[2]).toFixed(4);            

                    velX = velX > 5 ? 5 : velX;
                    velY = velY > 5 ? 5 : velY;
                    velZ = velZ > 5 ? 5 : velZ;

                    velX = velX < -5 ? -5 : velX;
                    velY = velY < -5 ? -5 : velY;
                    velZ = velZ < -5 ? -5 : velZ;
                    
                    if ( Math.abs(velX) > hmdPlotMaxPoint ) {
                        hmdPlotMaxPoint = Math.abs(velX);
                        hmdPlotMinPoint = -hmdPlotMaxPoint;
                    }
                    if ( Math.abs(velY) > hmdPlotMaxPoint ) {
                        hmdPlotMaxPoint = Math.abs(velY);
                        hmdPlotMinPoint = -hmdPlotMaxPoint;
                    }
                    if ( Math.abs(velZ) > hmdPlotMaxPoint ) {
                        hmdPlotMaxPoint = Math.abs(velZ);
                        hmdPlotMinPoint = -hmdPlotMaxPoint;
                    }

                    if ( hmdXPoints.length >= hmdCanvas.seconds*hmdCanvas.sections ) {
                        hmdXPoints.shift();
                    }
                    if ( hmdYPoints.length >= hmdCanvas.seconds*hmdCanvas.sections ) {
                        hmdYPoints.shift();
                    }
                    if ( hmdZPoints.length >= hmdCanvas.seconds*hmdCanvas.sections ) {
                        hmdZPoints.shift();
                    }
                    var r = (hmdPlotMaxPoint - hmdPlotMinPoint)  
                    var g = (hmdCanvas.rectHeight)
                    velX = (((velX - hmdPlotMinPoint) * g) / r)
                    velY = (((velY - hmdPlotMinPoint) * g) / r)
                    velZ = (((velZ - hmdPlotMinPoint) * g) / r)
                    hmdXPoints.push(velX);
                    hmdYPoints.push(velY);
                    hmdZPoints.push(velZ);
                }
                if ( velVals.length >= 6 ) {
                    velX = parseFloat(velVals[3]).toFixed(4);
                    velY = parseFloat(velVals[4]).toFixed(4);
                    velZ = parseFloat(velVals[5]).toFixed(4); 

                    velX = velX > 10 ? 10 : velX;
                    velY = velY > 10 ? 10 : velY;
                    velZ = velZ > 10 ? 10 : velZ;

                    velX = velX < -10 ? -10 : velX;
                    velY = velY < -10 ? -10 : velY;
                    velZ = velZ < -10 ? -10 : velZ;
                    
                    if ( Math.abs(velX) > contPlotMaxPoint ) {
                        contPlotMaxPoint = Math.abs(velX);
                        contPlotMinPoint = -contPlotMaxPoint;
                    }
                    if ( Math.abs(velY) > contPlotMaxPoint ) {
                        contPlotMaxPoint = Math.abs(velY);
                        contPlotMinPoint = -contPlotMaxPoint;
                    }
                    if ( Math.abs(velZ) > contPlotMaxPoint ) {
                        contPlotMaxPoint = Math.abs(velZ);
                        contPlotMinPoint = -contPlotMaxPoint;
                    }

                    if ( cont1XPoints.length >= contCanvas.seconds*contCanvas.sections ) {
                        cont1XPoints.shift();
                    }
                    if ( cont1YPoints.length >= contCanvas.seconds*contCanvas.sections ) {
                        cont1YPoints.shift();
                    }
                    if ( cont1ZPoints.length >= contCanvas.seconds*contCanvas.sections ) {
                        cont1ZPoints.shift();
                    }

                    var r = (contPlotMaxPoint - contPlotMinPoint)  
                    var g = (contCanvas.rectHeight)
                    velX = (((velX - contPlotMinPoint) * g) / r)
                    velY = (((velY - contPlotMinPoint) * g) / r)
                    velZ = (((velZ - contPlotMinPoint) * g) / r)
                    cont1XPoints.push(velX);
                    cont1YPoints.push(velY);
                    cont1ZPoints.push(velZ);
                }
                if ( velVals.length >= 9 ) {
                    velX = parseFloat(velVals[6]).toFixed(4);
                    velY = parseFloat(velVals[7]).toFixed(4);
                    velZ = parseFloat(velVals[8]).toFixed(4);

                    velX = velX > 10 ? 10 : velX;
                    velY = velY > 10 ? 10 : velY;
                    velZ = velZ > 10 ? 10 : velZ;

                    velX = velX < -10 ? -10 : velX;
                    velY = velY < -10 ? -10 : velY;
                    velZ = velZ < -10 ? -10 : velZ;
                    
                    if ( Math.abs(velX) > contPlotMaxPoint ) {
                        contPlotMaxPoint = Math.abs(velX);
                        contPlotMinPoint = -contPlotMaxPoint;
                    }
                    if ( Math.abs(velY) > contPlotMaxPoint ) {
                        contPlotMaxPoint = Math.abs(velY);
                        contPlotMinPoint = -contPlotMaxPoint;
                    }
                    if ( Math.abs(velZ) > contPlotMaxPoint ) {
                        contPlotMaxPoint = Math.abs(velZ);
                        contPlotMinPoint = -contPlotMaxPoint;
                    }

                    if ( cont2XPoints.length >= contCanvas.seconds*contCanvas.sections ) {
                        cont2XPoints.shift();
                    }
                    if ( cont2YPoints.length >= contCanvas.seconds*contCanvas.sections ) {
                        cont2YPoints.shift();
                    }
                    if ( cont2ZPoints.length >= contCanvas.seconds*contCanvas.sections ) {
                        cont2ZPoints.shift();
                    }

                    var r = (contPlotMaxPoint - contPlotMinPoint)  
                    var g = (contCanvas.rectHeight)
                    velX = (((velX - contPlotMinPoint) * g) / r)
                    velY = (((velY - contPlotMinPoint) * g) / r)
                    velZ = (((velZ - contPlotMinPoint) * g) / r)
                    cont2XPoints.push(velX);
                    cont2YPoints.push(velY);
                    cont2ZPoints.push(velZ);
                }
                if ( velVals.length >= 12 ) {
                    velX = parseFloat(velVals[9]).toFixed(4);
                    velY = parseFloat(velVals[10]).toFixed(4);
                    velZ = parseFloat(velVals[11]).toFixed(4);            
               
                    velX = velX > 10 ? 10 : velX;
                    velY = velY > 10 ? 10 : velY;
                    velZ = velZ > 10 ? 10 : velZ;

                    velX = velX < -10 ? -10 : velX;
                    velY = velY < -10 ? -10 : velY;
                    velZ = velZ < -10 ? -10 : velZ;
                    
                    if ( Math.abs(velX) > trackerPlotMaxPoint ) {
                        trackerPlotMaxPoint = Math.abs(velX);
                        trackerPlotMinPoint = -trackerPlotMaxPoint;
                    }
                    if ( Math.abs(velY) > trackerPlotMaxPoint ) {
                        trackerPlotMaxPoint = Math.abs(velY);
                        trackerPlotMinPoint = -trackerPlotMaxPoint;
                    }
                    if ( Math.abs(velZ) > trackerPlotMaxPoint ) {
                        trackerPlotMaxPoint = Math.abs(velZ);
                        trackerPlotMinPoint = -trackerPlotMaxPoint;
                    }

                    if ( tracker1XPoints.length >= trackerCanvas.seconds*trackerCanvas.sections ) {
                        tracker1XPoints.shift();
                    }
                    if ( tracker1YPoints.length >= trackerCanvas.seconds*trackerCanvas.sections ) {
                        tracker1YPoints.shift();
                    }
                    if ( tracker1ZPoints.length >= trackerCanvas.seconds*trackerCanvas.sections ) {
                        tracker1ZPoints.shift();
                    }

                    var r = (trackerPlotMaxPoint - trackerPlotMinPoint)  
                    var g = (trackerCanvas.rectHeight)
                    velX = (((velX - trackerPlotMinPoint) * g) / r)
                    velY = (((velY - trackerPlotMinPoint) * g) / r)
                    velZ = (((velZ - trackerPlotMinPoint) * g) / r)
                    tracker1XPoints.push(velX);
                    tracker1YPoints.push(velY);
                    tracker1ZPoints.push(velZ);
                }
                if ( velVals.length >= 15 ) {
                    velX = parseFloat(velVals[12]).toFixed(4);
                    velY = parseFloat(velVals[13]).toFixed(4);
                    velZ = parseFloat(velVals[14]).toFixed(4);            
          
                    velX = velX > 10 ? 10 : velX;
                    velY = velY > 10 ? 10 : velY;
                    velZ = velZ > 10 ? 10 : velZ;

                    velX = velX < -10 ? -10 : velX;
                    velY = velY < -10 ? -10 : velY;
                    velZ = velZ < -10 ? -10 : velZ;
                    
                    if ( Math.abs(velX) > trackerPlotMaxPoint ) {
                        trackerPlotMaxPoint = Math.abs(velX);
                        trackerPlotMinPoint = -trackerPlotMaxPoint;
                    }
                    if ( Math.abs(velY) > trackerPlotMaxPoint ) {
                        trackerPlotMaxPoint = Math.abs(velY);
                        trackerPlotMinPoint = -trackerPlotMaxPoint;
                    }
                    if ( Math.abs(velZ) > trackerPlotMaxPoint ) {
                        trackerPlotMaxPoint = Math.abs(velZ);
                        trackerPlotMinPoint = -trackerPlotMaxPoint;
                    }

                    if ( tracker2XPoints.length >= trackerCanvas.seconds*trackerCanvas.sections ) {
                        tracker2XPoints.shift();
                    }
                    if ( tracker2YPoints.length >= trackerCanvas.seconds*trackerCanvas.sections ) {
                        tracker2YPoints.shift();
                    }
                    if ( tracker2ZPoints.length >= trackerCanvas.seconds*trackerCanvas.sections ) {
                        tracker2ZPoints.shift();
                    }

                    var r = (trackerPlotMaxPoint - trackerPlotMinPoint)  
                    var g = (trackerCanvas.rectHeight)
                    velX = (((velX - trackerPlotMinPoint) * g) / r)
                    velY = (((velY - trackerPlotMinPoint) * g) / r)
                    velZ = (((velZ - trackerPlotMinPoint) * g) / r)
                    tracker2XPoints.push(velX);
                    tracker2YPoints.push(velY);
                    tracker2ZPoints.push(velZ);
                }
                if ( velVals.length >= 17 ) {
                    if ( autoConfMode < 0 ) {
                        stepDetects.push(velVals[15]);

                        if ( stepDetects.length > hmdCanvas.seconds*hmdCanvas.sections ) {
                            stepDetects.shift();
                        }
                    }
                }


                if ( autoConfMode >= 0 ) {
                    if ( sampleTime >= sampleLimit ) {
                        if ( autoConfMode == 0 ) {
                            autoConfMode = 1
                            sampleTime = 0
                            autoConfigPopup.setTitle("Scaled Walking Pace Config")
                            autoConfigPopup.setTextDetail("Slowly increase your pace to jogging pace")
                            autoConfigPopup.setTimeout(3)
                            autoConfigPopup.openPopup()
                        } else if ( autoConfMode == 1) {
                            autoConfMode = 2
                            sampleTime = 0
                            autoConfigPopup.setTitle("Jogging Pace Config")
                            autoConfigPopup.setTextDetail("Start Jogging (medium paced) IN PLACE")
                            autoConfigPopup.setTimeout(3)
                            autoConfigPopup.openPopup()
                        } else if ( autoConfMode == 2 ) {
                            autoConfMode = 3
                            sampleTime = 0
                            autoConfigPopup.setTitle("Jogging Pace Config")
                            autoConfigPopup.setTextDetail("Slowly increase your pace to running pace")
                            autoConfigPopup.setTimeout(3)
                            autoConfigPopup.openPopup()
                        } else if ( autoConfMode == 3 ) {
                            autoConfMode = 4
                            sampleTime = 0
                            autoConfigPopup.setTitle("Running Pace Config")
                            autoConfigPopup.setTextDetail("Start Running (fastest pace) IN PLACE")
                            autoConfigPopup.setTimeout(3)
                            autoConfigPopup.openPopup()
                        } else if ( autoConfMode == 4 ) {
                            stopTimer()
                            resetGraph()
                            autoConfMode = -1
                            sampleTime = 0
                            autoConfigPopup.setTitle("Walking Pace Config")
                            autoConfigPopup.setTextDetail("Start Walking IN PLACE in")
                            autoConfigPopup.setTimeout(5)
                            WalkInPlaceTabController.completeLRTraining()
                            var page = mainView.pop()
                        }                       
                    }
                }

                hmdCanvas.requestPaint ();
                contCanvas.requestPaint ();
                trackerCanvas.requestPaint ();

            } catch (error) {            
            }
        }
    }

    MyTimerPopup {
        id: autoConfigPopup
        property int profileIndex: -1
        dialogTitle: "Walking Pace Config"
        dialogText:  "Begin Walking IN PLACE in:"
        dialogTO: 5
        onClosed: {
            stopPopupTimer()
            if ( autoConfMode == 0 ) {
                startTimer()
            }
            sampleTime = 0
            if (cancelClicked || autoConfMode < 0 ) {
                autoConfMode = -1
                dialogTitle = "Walking Pace Config"
                dialogText = "Begin Walking (slowest pace) IN PLACE in:"
                sampleTime = 0
                totalSampleCount = 0
            } 
        }
        function openPopup() {
            startPopupTimer()
            open()
        }
        function setTitle(s) {
            dialogTitle = s
        }
        function setTextDetail(s) {
            dialogText = s
        }
        function setTimeout(v) {
            dialogTO = v
        }
    }
}