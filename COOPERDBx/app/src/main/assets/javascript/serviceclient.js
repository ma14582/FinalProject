/*
 ===============================================
 GET SERVICE
 ===============================================
 */

// Get service to obtain data from database according to a query
function callServiceGet(service, func, func_error) {
    $.ajax({
        url: "http://localhost:3000/" + service,
        type: 'GET',
        success: function (result) {
            if (func !== null) {
                func(result);
            }
        },
        error: function (error) {
            if (func_error !== null) {
                func_error();
            }
        },
        complete: function () {
            console.log("complete");
        }
    });
}

/*
 ===============================================
 POST SERVICE
 ===============================================
 */

// Post service to insert data in database when the event happens
function callServicePost(service, func, func_error, data) {
    $.ajax({
        url: "http://localhost:3000/" + service,
        type: 'POST',
        contentType: "application/json",
        dataType: "json",
        data: data,
        success: function (result) {
            if (func !== null) {
                func(result);
            }
        },
        error: function (error) {
            if (func_error !== null) {
                func_error();
            }
        },
        complete: function () {
            console.log("hola");
        }
    });
}


