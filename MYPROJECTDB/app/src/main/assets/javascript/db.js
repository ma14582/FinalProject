/*
 ===============================================
 CALLING ANDROID
 ===============================================
 */

function callAndroid() {
    var data = AndroidFunction.getBarkFromDb();
    //var s = JSON.stringify(data);
    $('#eventList').append('<p>' + data + '</p>');
}