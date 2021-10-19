#include "CServerHTTP.h"

using namespace std;

const char* CServerHTTP::CWiFi::TAG = "WiFi"; //Tag WiFi utilisé pour les logs

/** @brief Constructeur de la classe CWiFi */
CServerHTTP::CWiFi::CWiFi(){
    
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    //Initialisation de netif    
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();

    //Initialisation du WiFi
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    //création de la boucle d'événements
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &WiFiEvent,
                                                        NULL,
                                                        NULL));


    //Configuration du WiFi
    wifi_config_t wifiConf = {
        {
            ESP_WIFI_SSID,
            ESP_WIFI_PASSWORD,
            strlen(ESP_WIFI_SSID),
            ESP_WIFI_CHANNEL,
            (strlen(ESP_WIFI_PASSWORD) > 0 ? WIFI_AUTH_WPA_WPA2_PSK : WIFI_AUTH_OPEN),
            0,
            ESP_WIFI_MAX_STA_CONN
        }
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifiConf));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "Mode : AP\tSSID : %s\t Password : %s",
             wifiConf.ap.ssid,  wifiConf.ap.password);


}

/** @brief Destructeur de la classe CWiFi */
CServerHTTP::CWiFi::~CWiFi(){
    ESP_ERROR_CHECK(esp_wifi_stop());
    ESP_ERROR_CHECK(esp_wifi_deinit());


}

/**
 *  @brief Gestion des événements WiFi.
 *
 *  Fonction statique appelée à chaque événement WiFi permettant de gérer les événements WiFi.
 * 
 *  @param arg : argument de la fonction
 *  @param event_base : événement enregistré sur le WiFi
 *  @param event_id : identifiant de l'événement
 *  @param event_date : données de l'événément
 */
void CServerHTTP::CWiFi::WiFiEvent(void* arg, esp_event_base_t event_base,
                                   int32_t event_id, void* event_data)
{
    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(TAG, "Station " MACSTR " join, AID=%d", MAC2STR(event->mac), event->aid);
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(TAG, "Station " MACSTR " leave, AID=%d", MAC2STR(event->mac), event->aid);
    }
}

/** @brief Constructeur de la classe CServerHTTP */
CServerHTTP::CServerHTTP()
{

}

/** @brief Destructeur de la classe CServerHTTP */
CServerHTTP::~CServerHTTP(){

}