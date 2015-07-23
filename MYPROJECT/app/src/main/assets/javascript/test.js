/*
 ===============================================
 CALLING ANDROID
 ===============================================
 */

function callAndroid(service) {
    var data = AndroidFunction.getBarkFromServer(service);
    printEvents(data);
}

/*
 ===============================================
 PRINTING RESULT
 ===============================================
 */

 var index = 0;
 var temp = {values: []};
 var data = {values: []};
 var count = 0;


// Printing the event according to the result from the json sent by the server
function printEvents(result) {

    var obj = JSON.parse(result);
    for (var i in obj.events){

        if (obj.events[i].event === "bark") {
            $('#eventList').append('<p>' + 'barking on ' + '<span class="blue">' + obj.events[i].date + '</span>' + ' at ' + '<span class="blue">' + obj.events[i].time + '</span>' + '</p>');
        }
        if (obj.events[i].event === "movement") {
            $('#eventList').append('<p>' + 'moving on ' + '<span class="red">' + obj.events[i].date + '</span>' + ' at ' + '<span class="red">' + obj.events[i].time + '</span>' + '</p>');
        }
        for (var j in obj.events) {
            if (obj.events[i].date === obj.events[j].date && index !== j) {
               count++;
            }
        }
        index++;
        temp.values.push({X: obj.events[i].date, Y: count});
        count = 0;
    }

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
 CANVAS
 ===============================================
 */

var graph;
var xPadding = 30;
var yPadding = 30;

// Loading the graph Line according to the array resulting from the json sent by the server
function loadGraphLine(data) {
    $(document).ready(function () {
        graph = $('#graph');
        var c = graph[0].getContext('2d');
        c.fillStyle = "#FFFAF0"; //numbers color white
        c.lineWidth = 2;
        c.strokeStyle = '#FF1493'; //axis color pink
        c.font = 'italic 7pt sans-serif';
        c.textAlign = "center";

        // Draw the axises
        c.beginPath();
        c.moveTo(xPadding, 0);
        c.lineTo(xPadding, graph.height() - yPadding);
        c.lineTo(graph.width(), graph.height() - yPadding);
        c.stroke();

        // Draw the X value texts
        for (var i = 0; i < data.values.length; i++) {
            c.fillText(data.values[i].X, getXPixel(i), graph.height() - yPadding + 20);
        }

        // Draw the Y value texts
        c.textAlign = "right";
        c.textBaseline = "middle";

        for (var i = 0; i < getMaxY(data); i++) {
            c.fillText(i, xPadding - 10, getYPixel(i));
        }

        c.strokeStyle = '#0000FF'; // line color blue

        // Draw the line graph
        c.beginPath();
        c.moveTo(getXPixel(0), getYPixel(data.values[0].Y));
        for (var i = 1; i < data.values.length; i++) {
            c.lineTo(getXPixel(i), getYPixel(data.values[i].Y));
        }
        c.stroke();

        // Draw the dots
        c.fillStyle = '#EEEE00'; // dot color grey

        for (var i = 0; i < data.values.length; i++) {
            c.beginPath();
            c.arc(getXPixel(i), getYPixel(data.values[i].Y), 4, 0, Math.PI * 2, true);
            c.fill();
        }
    });
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