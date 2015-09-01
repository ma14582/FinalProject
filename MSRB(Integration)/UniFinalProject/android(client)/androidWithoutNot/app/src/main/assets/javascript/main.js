/*
 ===============================================
 CALLING ANDROID
 ===============================================
 */
// calling an android method which gets the result depending on the connexion with the server
// A service string is passed as parameter in order to rout the query to the server
function callAndroid(service) {
    var result = "";
    result = AndroidFunction.getEventFromServerOrAndroidDb(service);
    printEvents(result);
}

/*
 ===============================================
 TEST NOTIFICATION ANDROID
 ===============================================
 */

 var newEvent = "{'events':[{'id':36,'name':'Piper','event':'miau','date':'20/02/2000','time':'16:00:00'}]}";
 var message =  "Cooper"
 function notificationUser() {
    AndroidFunction.sendNotificationUser(message, newEvent);
 }


/*
 ===============================================
 PRINTING RESULT SERVER
 ===============================================
 */

 var index = 0;
 var temp = {values: []};
 var data = {values: []};               // data used to create canvas
 var count = 0;

// Printing the event according to the result from the json sent by the server
function printEvents(result) {
    var obj = JSON.parse(result);
    for (var i in obj.events){
        if (obj.events[i].event === "bark") {
            $('#eventList').append('<p>' + 'barking on ' + '<span class="blue">' + obj.events[i].date +
             '</span>' + ' at ' + '<span class="blue">' + obj.events[i].time + '</span>' + '</p>');
        }
        if (obj.events[i].event === "movement") {
            $('#eventList').append('<p>' + 'moving on ' + '<span class="red">' + obj.events[i].date +
            '</span>' + ' at ' + '<span class="red">' + obj.events[i].time + '</span>' + '</p>');
        }
        temp = initTemp(obj, i);
    }
    initData(temp, data);
}

// Initialing temp object
function initTemp(obj, i){
    for (var j in obj.events) {
        if (obj.events[i].date === obj.events[j].date && index !== j) {
            count++;
        }
    }
    index++;
    temp.values.push({X: obj.events[i].date, Y: count});
    count = 0;
    return temp;
}

// Initialing data object used to create canvas in line graph
function initData(temp, data){
    for (var i in temp.values) {
        if (notEqual(temp.values[i].X, data)) {
            data.values.push({X: temp.values[i].X, Y: temp.values[i].Y});
        }
    }
    loadGraphLine(data);
}

// Checking equal data in the array
function notEqual(x, data) {
    for (var i in data.values) {
        if (x === data.values[i].X) {
            return false;
        }
    }
    return true;
}

/*
 ===============================================
 CANVAS (LINE GRAPH)
 ===============================================
 */

var graph;
var xPadding = 30;
var yPadding = 30;

// Loading the graph Line according to the array resulting from the json sent by the server
function loadGraphLine(data) {
    $(document).ready(function () {
        var c = initGraph(data);
        drawAxis(c);                            // Draw the axises
        drawXYValuesText(c, data);              // Draw the X value texts
        drawLine(c, data);                      // Draw the line graph
        drawDot(c, data)                        // Draw the dots
    });
}

// Initializing the graph values
function initGraph(data){
    graph = $('#graph');
    var c = graph[0].getContext('2d');
    c.fillStyle = "#FFFAF0";                    //numbers color white
    c.lineWidth = 2;
    c.strokeStyle = '#FF1493';                  //axis color pink
    c.font = 'italic 7pt sans-serif';
    c.textAlign = "center";
    return c;
}

// Drawing the axises
function drawAxis(c){
    c.beginPath();
    c.moveTo(xPadding, 0);
    c.lineTo(xPadding, graph.height() - yPadding);
    c.lineTo(graph.width(), graph.height() - yPadding);
    c.stroke();
}

// Drawing the X and Y values for the texts
function drawXYValuesText(c, data){
    for (var i = 0; i < data.values.length; i++) {
        c.fillText(data.values[i].X, getXPixel(i), graph.height() - yPadding + 20);
    }
    c.textAlign = "right";
    c.textBaseline = "middle";

    for (var i = 0; i < getMaxY(data); i++) {
        c.fillText(i, xPadding - 10, getYPixel(i));
    }
}

// Drawing the lines in the graph
function drawLine(c, data){
    c.strokeStyle = '#0000FF';              // line color blue
    c.beginPath();
    c.moveTo(getXPixel(0), getYPixel(data.values[0].Y));
    for (var i = 1; i < data.values.length; i++) {
       c.lineTo(getXPixel(i), getYPixel(data.values[i].Y));
    }
    c.stroke();
}

// Drawing the dots in the line
function drawDot(c, data){
    c.fillStyle = '#EEEE00';                // dot color yellow
    for (var i = 0; i < data.values.length; i++) {
        c.beginPath();
        c.arc(getXPixel(i), getYPixel(data.values[i].Y), 4, 0, Math.PI * 2, true);
        c.fill();
    }
}

// Getting the max Y value in our data list
function getMaxY(data) {
    var max = 0;
    for (var i = 0; i < data.values.length; i++) {
        if (data.values[i].Y > max) {
            max = data.values[i].Y;
        }
    }
    max += 10 - max % 10;
    return max;
}

// Getting the x pixel for a graph point
function getXPixel(val) {
    return ((graph.width() - xPadding) / data.values.length) * val + (xPadding * 1.5);
}

// Getting the y pixel for a graph point
function getYPixel(val) {
    return graph.height() - (((graph.height() - yPadding) / getMaxY(data)) * val) - yPadding;
}