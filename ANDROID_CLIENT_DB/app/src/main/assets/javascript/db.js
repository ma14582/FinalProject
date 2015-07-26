/*
 ===============================================
 CALLING ANDROID
 ===============================================
 */

function callAndroid() {
    var data = AndroidFunction.getBarkFromDb();
    //$('#eventList').append('<p>' + data + '</p>');
    var obj = JSON.parse(data);
    for (var i in obj){
        if (obj[i].event === "bark") {
            $('#eventList').append('<p>' + obj[i].serverId + ' A barking on ' + '<span class="blue">' + obj[i].date + '</span>' + ' at ' + '<span class="blue">' + obj[i].time + '</span>' + '</p>');
        }
        if (obj[i].event === "movement") {
            $('#eventList').append('<p>' + 'moving on ' + '<span class="red">' + obj[i].date + '</span>' + ' at ' + '<span class="red">' + obj[i].time + '</span>' + '</p>');
        }
    }
}