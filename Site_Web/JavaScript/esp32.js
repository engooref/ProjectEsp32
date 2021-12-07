$("#navbar").load("/HTML/navbar.html");
//intervalId = setInterval(getData, 1000);


$.get("/getversion", function(data){
    console.log(data);
});

function getData(){
    $.ajax({
        url: "/getsystemdata",
        datatype: 'json',
        success: function(data){
            JSON.parse(data, (name, value) => {
                if(name){
                    $("#"+ name).text(value + "%");
                    $("#"+ name + "Bar").val(value);
                }
            });
        },
        error: function(data) {
            console.log("Erreur dans la récupération des données.\nCode d'erreur: " + data["status"]);
        }
    });
}


$("#restartButton").click(function(){
    //clearInterval(intervalId);
    $("#restartPopup").css("display", "block");
    $.get("/restart");
    intervalPing = setInterval(function(){
        $.ajax({ 
            type: "HEAD",
            url: "/",
            cache: false,
            success: function(){
                $("#restartPopup").css("display", "none");
                //intervalId = setInterval(getData, 1000);
                clearInterval(intervalPing); intervalPing = 0;
            }
        })
    }, 1000);
});

$("#persistData").click(function(){$.get("/changeParam", {"state": $("#persistData").is(':checked')});});