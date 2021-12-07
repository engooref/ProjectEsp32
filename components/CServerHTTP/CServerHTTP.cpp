#include "CServerHTTP.h"

using namespace std;




const char* CServerHTTP::CWiFi::TAG = "WiFi"; //Tag WiFi utilisé pour les logs
const char* CServerHTTP::TAG = "Serveur HTTP"; //Tag Serveur utilisé pour les logs
CServerHTTP* CServerHTTP::serverWeb = nullptr;

static char* get_path_from_uri(char *dest, const char *base_path, const char *uri, size_t destsize);
static esp_err_t set_content_type_from_file(httpd_req_t *req, const char *filename);


/** @brief Constructeur de la classe CWiFi */
CServerHTTP::CWiFi::CWiFi(CServerHTTP* pServ){
    
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
                                                        (void*)pServ,
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
    CServerHTTP* pServ = static_cast<CServerHTTP*>(arg);

    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(TAG, "Station " MACSTR " join, AID=%d", MAC2STR(event->mac), event->aid);
        if(!pServ->m_serverHttpd) ESP_ERROR_CHECK(pServ->StartWebServer());

    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(TAG, "Station " MACSTR " leave, AID=%d", MAC2STR(event->mac), event->aid);
        wifi_sta_list_t stationConnected;
        esp_wifi_ap_get_sta_list(&stationConnected);
        if(!stationConnected.num) pServ->StopWebServer();
    }
}

/** @brief Constructeur de la classe CServerHTTP */
CServerHTTP::CServerHTTP() :
    m_pWiFi(nullptr)
{ 
    CServerHTTP::serverWeb = this;
    ESP_ERROR_CHECK(InitSPIFFS());
    m_pWiFi = new CWiFi(this);

    m_pConfigurationJson = new CTraitementJson((BASE_PATH + "/" + NAME_FILE_CONF).c_str());
    cout << endl;
    m_pConfigurationJson->PrintPropertyAndValues();
    cout << "Modification" << endl;
    m_pConfigurationJson->ModifyElement((char*)"versionSite", (char*)"guirb");
    m_pConfigurationJson->PrintPropertyAndValues();
    cout << endl;

    Run();
}

/** @brief Destructeur de la classe CServerHTTP */
CServerHTTP::~CServerHTTP(){
    if(m_pWiFi) {delete m_pWiFi; m_pWiFi = nullptr;}
    if(m_pConfigurationJson) {delete m_pConfigurationJson; m_pConfigurationJson = nullptr;}
}

void CServerHTTP::Run(){
    for(;;);
}

esp_err_t CServerHTTP::InitSPIFFS(){
    const char* tagSpiffs = "SPIFFS";
    ESP_LOGI(tagSpiffs, "Initialise SPIFFS");

    esp_vfs_spiffs_conf_t conf = 
    {
      .base_path = BASE_PATH.c_str(),
      .partition_label = NULL,
      .max_files = 5,
      .format_if_mount_failed = true
    };

    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK) 
    {
        if (ret == ESP_FAIL) 
        {
            ESP_LOGE(tagSpiffs, "Failed to mount or format filesystem !");
        } else if (ret == ESP_ERR_NOT_FOUND) 
        {
            ESP_LOGE(tagSpiffs, "Failed to find SPIFFS partition !");
        } else 
        {
            ESP_LOGE(tagSpiffs, "Failed to initialize SPIFFS (%s) !", esp_err_to_name(ret));
        }
        return ESP_FAIL;
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(conf.partition_label, &total, &used);
    if (ret != ESP_OK) 
    {
        ESP_LOGE(tagSpiffs, "Failed to get SPIFFS partition information (%s) !", esp_err_to_name(ret));
        return ESP_FAIL;
    }

    ESP_LOGI(tagSpiffs, "Taille partition : total = %d - utilise = %d", total, used);

    return ESP_OK;
}

esp_err_t CServerHTTP::StartWebServer(){
     // initialise la configuration par défaut du serveur HTTP
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = ESP_WEB_SERVER_PORT;
    config.uri_match_fn = httpd_uri_match_wildcard;

    // pour une requête GET /*
    httpd_uri_t uri_get = {
        .uri      = "/*",
        .method   = HTTP_GET,
        .handler  = GetHandler,
        .user_ctx = NULL
    };
    
    httpd_uri_t uri_state = {
        .uri      = "/state",
        .method   = HTTP_GET,
        .handler  = StateHandler,
        .user_ctx = NULL
    };

    httpd_uri_t uri_restart = {
        .uri      = "/restart",
        .method   = HTTP_GET,
        .handler  = RestartHandler,
        .user_ctx = NULL
    };

    httpd_uri_t uri_getSystemData = {
        .uri      = "/getsystemdata",
        .method   = HTTP_GET,
        .handler  = GetSystemDataHandler,
        .user_ctx = NULL
    };

    httpd_uri_t uri_getVersion = {
        .uri      = "/getversion",
        .method   = HTTP_GET,
        .handler  = GetVersionHandler,
        .user_ctx = NULL
    };

    httpd_uri_t uri_changeParam = {
        .uri      = "/changeParam",
        .method   = HTTP_GET,
        .handler  = ChangeParamHandler,
        .user_ctx = NULL
    };
 
    esp_err_t ret = httpd_start(&m_serverHttpd, &config);

    // Démarre le serveur HTTP
    if (ret == ESP_OK) 
    {
        // enregistre les gestionnaires d'URI
        httpd_register_uri_handler(m_serverHttpd, &uri_restart);
        httpd_register_uri_handler(m_serverHttpd, &uri_getSystemData);
        httpd_register_uri_handler(m_serverHttpd, &uri_getVersion);
        httpd_register_uri_handler(m_serverHttpd, &uri_changeParam);
        httpd_register_uri_handler(m_serverHttpd, &uri_state);
        httpd_register_uri_handler(m_serverHttpd, &uri_get);
        
        //httpd_register_uri_handler(m_serverHttpd, &uri_post);
        ESP_LOGI(TAG, "Serveur Web demarrer sur le port: %d", config.server_port);
    }

    return ret;
}

void CServerHTTP::StopWebServer(){
    if(m_serverHttpd) { httpd_stop(m_serverHttpd); m_serverHttpd = NULL; } 
}
esp_err_t CServerHTTP::RestartHandler(httpd_req_t* req){
    esp_restart();
    return ESP_OK;
}

esp_err_t CServerHTTP::GetSystemDataHandler(httpd_req_t* req){
    //En construction
    return ESP_OK;
}

esp_err_t CServerHTTP::GetVersionHandler(httpd_req_t* req){
    
    return ESP_OK;
}

esp_err_t CServerHTTP::ChangeParamHandler(httpd_req_t* req){

    return ESP_OK;
}

esp_err_t CServerHTTP::StateHandler(httpd_req_t* req){
    cJSON* resp = cJSON_CreateObject();
    cJSON_AddTrueToObject(resp, "#ESP32");
    cJSON_AddBoolToObject(resp, "#WiFi", CServerHTTP::serverWeb->m_pWiFi != nullptr);
    cJSON_AddBoolToObject(resp, "#I2C" , nullptr != nullptr);
    cJSON_AddBoolToObject(resp, "#GPS" , nullptr != nullptr);
    cJSON_AddBoolToObject(resp, "#Radio", nullptr != nullptr);

    char* buf = cJSON_Print(resp);
    cJSON_Delete(resp);
    httpd_resp_send(req, buf, strlen(buf));
    return ESP_OK;
}

esp_err_t CServerHTTP::GetHandler(httpd_req_t* req){
    
    char filepath[FILE_PATH_MAX];
    FILE *fd = NULL;
    struct stat file_stat;

    ESP_LOGI(TAG, "URI : %s", req->uri);

    char *filename = get_path_from_uri(filepath, BASE_PATH.c_str(), req->uri, sizeof(filepath));
    if (!filename) 
    {
        ESP_LOGE(TAG, "Filename is too long");
        // retourne une erreur 500 (Internal Server Error)
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Filename too long");
        return ESP_FAIL;
    }


    if (filename[strlen(filename) - 1] == '/') 
    {
        sprintf(filename, "/index.html");
        sprintf(filepath, "%s/index.html", BASE_PATH.c_str());
    }

    ESP_LOGI(TAG, "filename : %s", filename);
    if (stat(filepath, &file_stat) == -1) 
    {
        if (strcmp(filename, "/index.htm") == 0) 
        {
            httpd_resp_set_status(req, "307 Temporary Redirect");
            httpd_resp_set_hdr(req, "Location", "/");
            httpd_resp_send(req, NULL, 0);
            return ESP_OK;
        }

        ESP_LOGE(TAG, "Failed to stat file : %s", filepath);
        // retourne une erreur 404 Not Found
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "File does not exist");
        return ESP_FAIL;
    }

    cout << filepath << endl;
    fd = fopen(filepath, "r");
    if (!fd) 
    {
        ESP_LOGE(TAG, "Failed to read existing file : %s", filepath);
        // retourne une erreur 500 Internal Server Error
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to read existing file");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Sending file : %s (%ld bytes)...", filename, file_stat.st_size);
    set_content_type_from_file(req, filename);

    // Retrieve the pointer to scratch buffer for temporary storage
    char*  buf;
    size_t chunksize;
    buf = (char*)malloc(BUFSIZE);

    do 
    {
        // lit une partie du fichier
        chunksize = fread(buf, 1, BUFSIZE, fd);

        if (chunksize > 0) 
        {
            // envoie le contenu du buffer
            if (httpd_resp_send_chunk(req, buf, chunksize) != ESP_OK) 
            {
                fclose(fd);
                ESP_LOGE(TAG, "File sending failed !");
                httpd_resp_sendstr_chunk(req, NULL);
                // retourne une erreur 500 Internal Server Error
                httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to send file");
               return ESP_FAIL;
           }
        }
    } while (chunksize != 0);
    
    fclose(fd);
    free(buf);
    ESP_LOGI(TAG, "File sending complete");

    httpd_resp_send_chunk(req, NULL, 0);

    return ESP_OK;
}

char* get_path_from_uri(char *dest, const char *base_path, const char *uri, size_t destsize)
{
    const size_t base_pathlen = strlen(base_path);
    size_t pathlen = strlen(uri);

    const char *quest = strchr(uri, '?');
    if (quest) 
    {
        pathlen = MIN(pathlen, quest - uri);
    }
    const char *hash = strchr(uri, '#');
    if (hash) 
    {
        pathlen = MIN(pathlen, hash - uri);
    }

    if (base_pathlen + pathlen + 1 > destsize) 
    {
        return NULL;
    }

    // construit le chemin complet (base + path)
    strcpy(dest, base_path);
    strlcpy(dest + base_pathlen, uri, pathlen + 1);

    // retourne le pointeur ver les chemin (sans la base)
    return dest + base_pathlen;
}

esp_err_t set_content_type_from_file(httpd_req_t *req, const char *filename)
{
    if (IS_FILE_EXT(filename, ".pdf")) 
    {
        return httpd_resp_set_type(req, "application/pdf");
    } 
    else if (IS_FILE_EXT(filename, ".html")) 
    {
        return httpd_resp_set_type(req, "text/html");
    } 
    else if (IS_FILE_EXT(filename, ".css")) 
    {
        return httpd_resp_set_type(req, "text/css");
    }
    else if (IS_FILE_EXT(filename, ".jpeg")) 
    {
        return httpd_resp_set_type(req, "image/jpeg");
    }
    else if (IS_FILE_EXT(filename, ".ico")) 
    {
        return httpd_resp_set_type(req, "image/x-icon");
    }
    return httpd_resp_set_type(req, "text/plain");
}
