import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2
import pottedmeat7.walkinplace 1.0

MyStackViewPage {
    id: dataModelPage
    name: "dataModelPage"

    property var touchPoints : []    

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

    property real canvasPlotMaxPoint : 0.6
    property real canvasPlotMinPoint : -0.6

    property var startTimer: function() {
        getModelData()
    }

    property var stopTimer: function() {
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

        touchPoints = []

        canvasPlotMaxPoint = 0.6
        canvasPlotMinPoint = -0.6
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
                        text: "Fastest Pace Touch"
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
                            } 
                        }
                    }

                    MyPushButton {
                        id: walkTouchPlusButton
                        Layout.preferredWidth: 40
                        text: "+"
                        onClicked: {
                        }
                    }

                    MyText {
                        text: " "
                        Layout.preferredWidth: 70
                    }  

                    MyPushButton {
                        id: jogTouchMinusButton
                        Layout.preferredWidth: 40
                        text: "-"
                        onClicked: {
                        }
                    }

                    MyTextField {
                        id: jogTouchInputField
                        text: "0.5"
                        keyBoardUID: 121
                        Layout.preferredWidth: 140
                        horizontalAlignment: Text.AlignHCenter
                        function onInputEvent(input) {
                            var val = parseFloat(input)
                            if (!isNaN(val)) {
                            } 
                        }
                    }

                    MyPushButton {
                        id: jogTouchPlusButton
                        Layout.preferredWidth: 40
                        text: "+"
                        onClicked: {
                        }
                    }

                    MyText {
                        text: " "
                        Layout.preferredWidth: 70
                    }  

                    MyPushButton {
                        id: runTouchMinusButton
                        Layout.preferredWidth: 40
                        text: "-"
                        onClicked: {
                        }
                    }

                    MyTextField {
                        id: runTouchInputField
                        text: "1.0"
                        keyBoardUID: 122
                        Layout.preferredWidth: 140
                        horizontalAlignment: Text.AlignHCenter
                        function onInputEvent(input) {
                            var val = parseFloat(input)
                            if (!isNaN(val)) {
                            } 
                        }
                    }

                    MyPushButton {
                        id: runTouchPlusButton
                        Layout.preferredWidth: 40
                        text: "+"
                        onClicked: {
                            var value = runTouch + thresholdStep
                            runTouch = value
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
            topPadding: 100

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
                            var lastHMDXPoint = rectHeight/2 + topY
                            var lastHMDYPoint = rectHeight/2 + topY
                            var lastHMDZPoint = rectHeight/2 + topY
                            var lastX = 50;
                            for(var x=0; x<hmdYPoints.length; x++) {
                                //ctx.beginPath();                
                                //ctx.strokeStyle = "#DD0000";
                                //ctx.lineWidth = 1;
                                //ctx.moveTo(lastX,lastHMDXPoint);
                                //var val = hmdXPoints[x];
                                //ctx.lineTo(lastX+graphXScale,val+topY);
                                //lastHMDXPoint = val+topY;
                                //ctx.stroke();
                                //ctx.closePath();
                                ctx.beginPath();  
                                ctx.strokeStyle = "#00DD00";
                                ctx.lineWidth = 1;
                                ctx.moveTo(lastX,lastHMDYPoint);
                                var val = hmdYPoints[x];
                                ctx.lineTo(lastX+graphXScale,val+topY);
                                lastHMDYPoint = val+topY;
                                ctx.stroke();
                                ctx.closePath();
                                //ctx.beginPath();  
                                //ctx.strokeStyle = "#0000DD";
                                //ctx.lineWidth = 1;
                                //ctx.moveTo(lastX,lastHMDZPoint);
                                //var val = hmdZPoints[x];
                                //ctx.lineTo(lastX+graphXScale,val+topY);
                                //lastHMDZPoint = val+topY;
                                //ctx.stroke();
                                //ctx.closePath();
                                lastX = lastX + graphXScale;
                            }
                        }
                        if ( showControllers.checked ) {
                            var lastcont1XPoint = rectHeight/2 + topY
                            var lastcont1YPoint = rectHeight/2 + topY
                            var lastcont1ZPoint = rectHeight/2 + topY
                            var lastcont2XPoint = rectHeight/2 + topY
                            var lastcont2YPoint = rectHeight/2 + topY
                            var lastcont2ZPoint = rectHeight/2 + topY
                            var lastX = 50;
                            for(var x=0; x<cont1YPoints.length; x++) {
                                //ctx.beginPath();                
                                //ctx.strokeStyle = "#DD0000";
                                //ctx.lineWidth = 1;
                                //ctx.moveTo(lastX,lastcont1XPoint);
                                //var val = cont1XPoints[x];
                                //ctx.lineTo(lastX+graphXScale,val+topY);
                                //lastcont1XPoint = val+topY;
                                //ctx.stroke();
                                //ctx.closePath();
                                ctx.beginPath();  
                                ctx.strokeStyle = "#00DD00";
                                ctx.lineWidth = 1;
                                ctx.moveTo(lastX,lastcont1YPoint);
                                var val = cont1YPoints[x];
                                ctx.lineTo(lastX+graphXScale,val+topY);
                                lastcont1YPoint = val+topY;
                                ctx.stroke();
                                ctx.closePath();
                                //ctx.beginPath();  
                                //ctx.strokeStyle = "#0000DD";
                                //ctx.lineWidth = 1;
                                //ctx.moveTo(lastX,lastcont1ZPoint);
                                //var val = cont1ZPoints[x];
                                //ctx.lineTo(lastX+graphXScale,val+topY);
                                //lastcont1ZPoint = val+topY;
                                //ctx.stroke();
                                //ctx.closePath();
                                //ctx.beginPath();                
                                //ctx.strokeStyle = "#DD00DD";
                                //ctx.lineWidth = 1;
                                //ctx.moveTo(lastX,lastcont2XPoint);
                                //var val = cont2XPoints[x];
                                //ctx.lineTo(lastX+graphXScale,val+topY);
                                //lastcont2XPoint = val+topY;
                                //ctx.stroke();
                                //ctx.closePath();
                                ctx.beginPath();  
                                ctx.strokeStyle = "#00DDDD";
                                ctx.lineWidth = 1;
                                ctx.moveTo(lastX,lastcont2YPoint);
                                var val = cont2YPoints[x];
                                ctx.lineTo(lastX+graphXScale,val+topY);
                                lastcont2YPoint = val+topY;
                                ctx.stroke();
                                ctx.closePath();
                                //ctx.beginPath();  
                                //ctx.strokeStyle = "#FF55FF";
                                //ctx.lineWidth = 1;
                                //ctx.moveTo(lastX,lastcont2ZPoint);
                                //var val = cont2ZPoints[x];
                                //ctx.lineTo(lastX+graphXScale,val+topY);
                                //lastcont2ZPoint = val+topY;
                                //ctx.stroke();
                                //ctx.closePath();
                                lastX = lastX + graphXScale;
                            }
                        }
                        if ( showTrackers.checked ) {
                            var lasttracker1XPoint = rectHeight/2 + topY
                            var lasttracker1YPoint = rectHeight/2 + topY
                            var lasttracker1ZPoint = rectHeight/2 + topY
                            var lasttracker2XPoint = rectHeight/2 + topY
                            var lasttracker2YPoint = rectHeight/2 + topY
                            var lasttracker2ZPoint = rectHeight/2 + topY
                            var lastX = 50;
                            for(var x=0; x<tracker1YPoints.length; x++) {
                                //ctx.beginPath();                
                                //ctx.strokeStyle = "#DD0000";
                                //ctx.lineWidth = 1;
                                //ctx.moveTo(lastX,lasttracker1XPoint);
                                //var val = tracker1XPoints[x];
                                //ctx.lineTo(lastX+graphXScale,val+topY);
                                //lasttracker1XPoint = val+topY;
                                //ctx.stroke();
                                //ctx.closePath();
                                ctx.beginPath();  
                                ctx.strokeStyle = "#00DD00";
                                ctx.lineWidth = 1;
                                ctx.moveTo(lastX,lasttracker1YPoint);
                                var val = tracker1YPoints[x];
                                ctx.lineTo(lastX+graphXScale,val+topY);
                                lasttracker1YPoint = val+topY;
                                ctx.stroke();
                                ctx.closePath();
                                //ctx.beginPath();  
                                //ctx.strokeStyle = "#0000DD";
                                //ctx.lineWidth = 1;
                                //ctx.moveTo(lastX,lasttracker1ZPoint);
                                //var val = tracker1ZPoints[x];
                                //ctx.lineTo(lastX+graphXScale,val+topY);
                                //lasttracker1ZPoint = val+topY;
                                //ctx.stroke();
                                //ctx.closePath();
                                //ctx.beginPath();                
                                //ctx.strokeStyle = "#DD00DD";
                                //ctx.lineWidth = 1;
                                //ctx.moveTo(lastX,lasttracker2XPoint);
                                //var val = tracker2XPoints[x];
                                //ctx.lineTo(lastX+graphXScale,val+topY);
                                //lasttracker2XPoint = val+topY;
                                //ctx.stroke();
                                //ctx.closePath();
                                ctx.beginPath();  
                                ctx.strokeStyle = "#00DDDD";
                                ctx.lineWidth = 1;
                                ctx.moveTo(lastX,lasttracker2YPoint);
                                var val = tracker2YPoints[x];
                                ctx.lineTo(lastX+graphXScale,val+topY);
                                lasttracker2YPoint = val+topY;
                                ctx.stroke();
                                ctx.closePath();
                                //ctx.beginPath();  
                                //ctx.strokeStyle = "#FF55FF";
                                //ctx.lineWidth = 1;
                                //ctx.moveTo(lastX,lasttracker2ZPoint);
                                //var val = tracker2ZPoints[x];
                                //ctx.lineTo(lastX+graphXScale,val+topY);
                                //lasttracker2ZPoint = val+topY;
                                //ctx.stroke();
                                //ctx.closePath();
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
                            var val = touchPoints[x]*-1;
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
            var offset = 1;
            while(i<velVals.length) {
                    var velX = 0 //parseFloat(velVals[i+0]).toFixed(6);
                    var velY = parseFloat(velVals[i+0]).toFixed(6);
                    var velZ = parseFloat(velVals[i+2]).toFixed(6);            

                    velX = velX > 5 ? 5 : velX;
                    velY = velY > 5 ? 5 : velY;
                    velZ = velZ > 5 ? 5 : velZ;

                    velX = velX < -5 ? -5 : velX;
                    velY = velY < -5 ? -5 : velY;
                    velZ = velZ < -5 ? -5 : velZ;
                    
                    if ( Math.abs(velX) > canvasPlotMaxPoint ) {
                        canvasPlotMaxPoint = Math.abs(velX);
                        canvasPlotMinPoint = -canvasPlotMaxPoint;
                    }
                    if ( Math.abs(velY) > canvasPlotMaxPoint ) {
                        canvasPlotMaxPoint = Math.abs(velY);
                        canvasPlotMinPoint = -canvasPlotMaxPoint;
                    }
                    if ( Math.abs(velZ) > canvasPlotMaxPoint ) {
                        canvasPlotMaxPoint = Math.abs(velZ);
                        canvasPlotMinPoint = -canvasPlotMaxPoint;
                    }

                    var r = (canvasPlotMaxPoint - canvasPlotMinPoint)  
                    var g = (modelCanvas.rectHeight)
                    velX = (((velX - canvasPlotMinPoint) * g) / r)
                    velY = (((velY - canvasPlotMinPoint) * g) / r)
                    velZ = (((velZ - canvasPlotMinPoint) * g) / r)
                    //hmdXPoints.push(velX);
                    hmdYPoints.push(velY);
                    //hmdZPoints.push(velZ);

                    velX = parseFloat(velVals[i+3]).toFixed(6);
                    velY = parseFloat(velVals[i+1]).toFixed(6);
                    //velZ = parseFloat(velVals[i+5]).toFixed(6); 

                    velX = velX > 10 ? 10 : velX;
                    velY = velY > 10 ? 10 : velY;
                    velZ = velZ > 10 ? 10 : velZ;

                    velX = velX < -10 ? -10 : velX;
                    velY = velY < -10 ? -10 : velY;
                    velZ = velZ < -10 ? -10 : velZ;
                    
                    if ( Math.abs(velX) > canvasPlotMaxPoint ) {
                        canvasPlotMaxPoint = Math.abs(velX);
                        canvasPlotMinPoint = -canvasPlotMaxPoint;
                    }
                    if ( Math.abs(velY) > canvasPlotMaxPoint ) {
                        canvasPlotMaxPoint = Math.abs(velY);
                        canvasPlotMinPoint = -canvasPlotMaxPoint;
                    }
                    if ( Math.abs(velZ) > canvasPlotMaxPoint ) {
                        canvasPlotMaxPoint = Math.abs(velZ);
                        canvasPlotMinPoint = -canvasPlotMaxPoint;
                    }

                    var r = (canvasPlotMaxPoint - canvasPlotMinPoint)  
                    var g = (modelCanvas.rectHeight)
                    velX = (((velX - canvasPlotMinPoint) * g) / r)
                    velY = (((velY - canvasPlotMinPoint) * g) / r)
                    velZ = (((velZ - canvasPlotMinPoint) * g) / r)
                    //cont1XPoints.push(velX);
                    cont1YPoints.push(velY);
                    //cont1ZPoints.push(velZ);

                    //velX = parseFloat(velVals[i+6]).toFixed(6);
                    velY = parseFloat(velVals[i+2]).toFixed(6);
                    //velZ = parseFloat(velVals[i+8]).toFixed(6);

                    velX = velX > 10 ? 10 : velX;
                    velY = velY > 10 ? 10 : velY;
                    velZ = velZ > 10 ? 10 : velZ;

                    velX = velX < -10 ? -10 : velX;
                    velY = velY < -10 ? -10 : velY;
                    velZ = velZ < -10 ? -10 : velZ;
                    
                    if ( Math.abs(velX) > canvasPlotMaxPoint ) {
                        canvasPlotMaxPoint = Math.abs(velX);
                        canvasPlotMinPoint = -canvasPlotMaxPoint;
                    }
                    if ( Math.abs(velY) > canvasPlotMaxPoint ) {
                        canvasPlotMaxPoint = Math.abs(velY);
                        canvasPlotMinPoint = -canvasPlotMaxPoint;
                    }
                    if ( Math.abs(velZ) > canvasPlotMaxPoint ) {
                        canvasPlotMaxPoint = Math.abs(velZ);
                        canvasPlotMinPoint = -canvasPlotMaxPoint;
                    }

                    var r = (canvasPlotMaxPoint - canvasPlotMinPoint)  
                    var g = (modelCanvas.rectHeight)
                    velX = (((velX - canvasPlotMinPoint) * g) / r)
                    velY = (((velY - canvasPlotMinPoint) * g) / r)
                    velZ = (((velZ - canvasPlotMinPoint) * g) / r)
                    //cont2XPoints.push(velX);
                    cont2YPoints.push(velY);
                    //cont2ZPoints.push(velZ);

                    //velX = parseFloat(velVals[i+9]).toFixed(6);
                    velY = parseFloat(velVals[i+3]).toFixed(6);
                    //velZ = parseFloat(velVals[i+11]).toFixed(6);            
               
                    velX = velX > 10 ? 10 : velX;
                    velY = velY > 10 ? 10 : velY;
                    velZ = velZ > 10 ? 10 : velZ;

                    velX = velX < -10 ? -10 : velX;
                    velY = velY < -10 ? -10 : velY;
                    velZ = velZ < -10 ? -10 : velZ;
                    
                    if ( Math.abs(velX) > canvasPlotMaxPoint ) {
                        canvasPlotMaxPoint = Math.abs(velX);
                        canvasPlotMinPoint = -canvasPlotMaxPoint;
                    }
                    if ( Math.abs(velY) > canvasPlotMaxPoint ) {
                        canvasPlotMaxPoint = Math.abs(velY);
                        canvasPlotMinPoint = -canvasPlotMaxPoint;
                    }
                    if ( Math.abs(velZ) > canvasPlotMaxPoint ) {
                        canvasPlotMaxPoint = Math.abs(velZ);
                        canvasPlotMinPoint = -canvasPlotMaxPoint;
                    }

                    var r = (canvasPlotMaxPoint - canvasPlotMinPoint)  
                    var g = (modelCanvas.rectHeight)
                    velX = (((velX - canvasPlotMinPoint) * g) / r)
                    velY = (((velY - canvasPlotMinPoint) * g) / r)
                    velZ = (((velZ - canvasPlotMinPoint) * g) / r)
                    //tracker1XPoints.push(velX);
                    tracker1YPoints.push(velY);
                    //tracker1ZPoints.push(velZ);

                    //velX = parseFloat(velVals[i+12]).toFixed(6);
                    velY = parseFloat(velVals[i+4]).toFixed(6);
                    //velZ = parseFloat(velVals[i+14]).toFixed(6);            
          
                    velX = velX > 10 ? 10 : velX;
                    velY = velY > 10 ? 10 : velY;
                    velZ = velZ > 10 ? 10 : velZ;

                    velX = velX < -10 ? -10 : velX;
                    velY = velY < -10 ? -10 : velY;
                    velZ = velZ < -10 ? -10 : velZ;
                    
                    if ( Math.abs(velX) > canvasPlotMaxPoint ) {
                        canvasPlotMaxPoint = Math.abs(velX);
                        canvasPlotMinPoint = -canvasPlotMaxPoint;
                    }
                    if ( Math.abs(velY) > canvasPlotMaxPoint ) {
                        canvasPlotMaxPoint = Math.abs(velY);
                        canvasPlotMinPoint = -canvasPlotMaxPoint;
                    }
                    if ( Math.abs(velZ) > canvasPlotMaxPoint ) {
                        canvasPlotMaxPoint = Math.abs(velZ);
                        canvasPlotMinPoint = -canvasPlotMaxPoint;
                    }

                    var r = (canvasPlotMaxPoint - canvasPlotMinPoint)  
                    var g = (modelCanvas.rectHeight)
                    velX = (((velX - canvasPlotMinPoint) * g) / r)
                    velY = (((velY - canvasPlotMinPoint) * g) / r)
                    velZ = (((velZ - canvasPlotMinPoint) * g) / r)
                    //tracker2XPoints.push(velX);
                    tracker2YPoints.push(velY);
                    //tracker2ZPoints.push(velZ);

                    var r = (canvasPlotMaxPoint - canvasPlotMinPoint)  
                    var g = (modelCanvas.rectHeight)
                    var finalPoint = (((velVals[i+5] - canvasPlotMinPoint) * g) / r)
                    touchPoints.push(finalPoint);
                    //console.info(velVals[i+5]);
                i = i + 5;//16;
            }

            modelCanvas.requestPaint ();

        } catch (error) {            
        }
    }
}