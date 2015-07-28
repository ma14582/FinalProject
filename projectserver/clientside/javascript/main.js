/*
 ===============================================
 SERVER CONEXION
 ===============================================
 */

// Calling the post service to insert data in database
function callInsertEvent(service, data) {
    callServicePost(service, printTest, printError, data);
}

// Calling the get service to get data from database
function callGetEvent(service) {
    callServiceGet(service, printEvent, printError);
}

// Print an error in case the failed conexion
function printError() {
    $('#eventList').append('<p>Sorry, There is a problem with the server conexion</p>');
}

function sendEventToAndroid(){
    var today = getToday();
    var time = getTime();
    var data = JSON.stringify({"petId": 1, "eventId": 1, "date": today, "time": time});
    callSendEventToAndroid('message', data);
}
/*
 ===============================================
 INSERTEVENT
 ===============================================
 */

// Getting the data about barking to be inserted in the database 
function insertBark() {
    var today = getToday();
    var time = getTime();
    var data = JSON.stringify({"petId": 1, "eventId": 1, "date": today, "time": time});
    callInsertEvent('insertEvent', data);
}

// Getting the data about movement to be inserted in the database 
function insertMovement() {
    var today = getToday();
    var time = getTime();
    var data = JSON.stringify({"petId": 1, "eventId": 2, "date": today, "time": time});
    callInsertEvent('insertEvent', data);
}

function printTest() {
    callGetEvent('getBark');
}

// Getting the date  
function getToday() {
    var today = new Date();
    var dd = today.getDate();
    var mm = today.getMonth() + 1;
    var yyyy = today.getFullYear();
    if (dd < 10) {
        dd = '0' + dd;
    }
    if (mm < 10) {
        mm = '0' + mm;
    }
    return today = yyyy + '/' + mm + '/' + dd;
}

// Getting the time 
function getTime() {
    var today = new Date();
    var hh = today.getHours();
    var mm = today.getMinutes();
    var ss = today.getSeconds();
    if (hh < 10) {
        hh = '0' + hh;
    }
    if (mm < 10) {
        mm = '0' + mm;
    }
    if (ss < 10) {
        ss = '0' + ss;
    }
    return today = hh + ':' + mm + ':' + ss;
}


/*
 ===============================================
 GETTING EVENT
 ===============================================
 */

var count = 1;
var data = {values: []};
var index = 0;
var temp = {values: []};

// Printing the event according to the result from the json sent by the server
function printEvent(result) {
    $.each(result.events, function (key, value) {
        if (value.event === "bark") {
            $('#eventList').append('<p>' + value.id + ' barking on ' + '<span class="blue">' + value.date + '</span>' + ' at ' + '<span class="blue">' + value.time + '</span>' + '</p>');
        }
        if (value.event === "movement") {
            $('#eventList').append('<p>' + value.id + ' moving on ' + '<span class="red">' + value.date + '</span>' + ' at ' + '<span class="red">' + value.time + '</span>' + '</p>');
        }
        for (var i = 0; i < result.events.length; i++) {
            if (value.date === result.events[i].date && index !== i) {
                count++;
            }
        }
        index++;
        temp.values.push({X: value.date, Y: count});
        count = 1;
    });
    // Pushing the result in the array which will be used to print the graph
    data.values.push({X: temp.values[0].X, Y: temp.values[0].Y});
    for (var i = 1; i < temp.values.length; i++) {
        if (notEqual(temp.values[i].X, data)) {
            data.values.push({X: temp.values[i].X, Y: temp.values[i].Y});
        }
    }
    loadGraphLine(data);
}

// Checking equal data in the array
function notEqual(x, data) {
    for (var i = 0; i < data.values.length; i++) {
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
        c.font = 'italic 8pt sans-serif';
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

