$("#navbar").load("/HTML/navbar.html");
intervalId = setInterval(getData, 1000);

const arrayMode = {0 : "Null",  1 : "Station" , 2 : "AP" , 3 : "APSTA"};
const arrayAuth = {0 : "Aucun", 1 : "WEP", 2 : "WPA_PSK",  3 : "WPA2_PSK",      4 : "WPA_WPA2_PSK", 
                   5 : "WPA2_ENTREPRISE",  6 : "WPA3_PSK", 7 : "WPA2_WPA3_PSK", 8 : "WAPI_PSK"};

function 
$.get("/getwifiattr", function(data){
    JSON.parse(data, (name, value) => {
        if(name) {
            if(name === "#modeWiFi"){
                if(arrayMode[value] === "Station"){
                    $(".apMode").hide();
                } else {
                    $(".apMode").show();
                }
                $(name).text(arrayMode[value]);
            } else if (name === "#authenWiFi") {
                $(name).text(arrayAuth[value]);
            } else {
                $(name).text(value);
            }
        }
    });
});