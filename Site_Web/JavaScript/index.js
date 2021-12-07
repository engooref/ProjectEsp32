$("#navbar").load("/HTML/navbar.html");

$.ajax({
    url: "state",
    success: function(data){
        JSON.parse(data, function(name, value){
            if(value === true){
                $(name).text("Actif").css('color', 'green');
            } else if(value === false) {
                $(name).text("Inactif").css('color', 'red');
            }
        })
    },
    error: function(data) {
        alert("Erreur dans la récupération des données.\nCode d'erreur: " + data["status"]);
    }
});
