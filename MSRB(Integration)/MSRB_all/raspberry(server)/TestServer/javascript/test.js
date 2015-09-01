function callAndroid() {
    var data = AndroidFunction.getBarkFromServer();
    $.ajax({
            success: function (data) {
                if (printEvent(data) !== null) {
                    printEvent(data);
                }
            },
            error: function (error) {
                if (printError() !== null) {
                    printError();
                }
            },
            complete: function () {
                console.log("complete");
            }
        });
}

