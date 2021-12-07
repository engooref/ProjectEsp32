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

#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>

#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_spiffs.h"
#include "esp_vfs.h"
#include "esp_http_server.h"

#include "nvs_flash.h"

#include <sys/param.h>
#include <sys/unistd.h>
#include <sys/stat.h>

#include "lwip/err.h"
#include "lwip/sys.h"


#include "macro.hpp"

#include "CTraitementJson.h"

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
        CWiFi(CServerHTTP* pServ);
        ~CWiFi();

    private:
        static const char *TAG;
        static void WiFiEvent(void* arg, esp_event_base_t event_base,
                              int32_t event_id, void* event_data);
    };

private:
    CWiFi* m_pWiFi;
    httpd_handle_t m_serverHttpd;
    CTraitementJson* m_pConfigurationJson;
public:
    CServerHTTP();
    ~CServerHTTP();

    void Run();
    esp_err_t InitSPIFFS();

    esp_err_t StartWebServer();
    void StopWebServer();

    static esp_err_t RestartHandler(httpd_req_t* req);
    static esp_err_t GetSystemDataHandler(httpd_req_t* req);
    static esp_err_t GetVersionHandler(httpd_req_t* req);
    static esp_err_t ChangeParamHandler(httpd_req_t* req);
    static esp_err_t StateHandler(httpd_req_t* req);
    static esp_err_t GetHandler(httpd_req_t* req);


private:
    static const char* TAG;
    static CServerHTTP* serverWeb;
};

#endif //CSERVERHTTP_H_