/**
 *  @file CServerHTTP.h
 *  @author Engooref
 *  @version 1.0
 *  @date 18 octobre 2021 
 *  @brief Fichier d'en-tête implémentant la classe CServerHTTP
 *
 */

#ifndef CSERVERHTTP_H_
#define CSERVERHTTP_H_

/**
 *  Inclusion des bibliothèques
 */
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#include <cstdio>
#include <cstring>

/**
 *  Déclaration des définitions pour le WiFi
 *  
 *  @def ESP_WIFI_SSID
 *  @brief SSID utilisé pour le WiFi.
 *
 *  @def ESP_WIFI_PASSWORD
 *  @brief Mot de passe utilisé pour le WiFi.
 *
 *  @def ESP_WIFI_CHANNEL
 *  @brief Channel du WiFi (uniquement pour le mode AP).
 *
 *  @def ESP_WIFI_MAX_STA_CONN
 *  @brief Nombre maximum d'appareil pouvant se connecter (uniqument pour le mode AP).
 */
#define ESP_WIFI_SSID           "ESPA6F1"
#define ESP_WIFI_PASSWORD       "TestEsp123"
#define ESP_WIFI_CHANNEL        0
#define ESP_WIFI_MAX_STA_CONN   6


/**
 * @class CServerHTTP
 * @brief Classe utilisé pour le serveur Web.
 *
 * Cette classe gère tous ce qui se passe sur le serveur web
 * que ce soit la gestion HTTP ou la gestion du WiFi (voir CWiFi)
 */
class CServerHTTP {
private:
    /**
     * @class CWiFi
     * @brief Classe utilisé pour le WiFi.
     *
     *  Cette classe gère le WiFi ainsi que les événements liés à celui-ci
     */
    class CWiFi {
    public:
        CWiFi();
        ~CWiFi();

        //Fonctions statique
        static const char *TAG;
        static void WiFiEvent(void* arg, esp_event_base_t event_base,
                              int32_t event_id, void* event_data);
    };

private:
    CWiFi m_WiFi;
public:
    CServerHTTP();
    ~CServerHTTP();
};

#endif //CSERVERHTTP_H_