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
#define ESP_WIFI_MAX_STA_CONN   10

/**
 *  Déclaration des définitions pour le serveur HTTP
 *  
 *  @def ESP_WEB_SERVER_PORT
 *  @brief Port du serveur HTTP
 *  
 *  @def IS_FILE_EXT(filename, ext)
 *  @brief Permet de savoir si une extension existe
 *
 *  @def FILE_PATH_MAX
 *  @brief défini la taille limite du chemins d'accès
 *
 *  @def BUFSIZE
 *  @brief Taille du buffer utilisé pour envoyer les données
 * 
 *  @def BASE_PATH
 *  @brief Racine du site web sur le module
 *
 */
#define ESP_WEB_SERVER_PORT    80
#define IS_FILE_EXT(filename, ext) (strcasecmp(&filename[strlen(filename) - sizeof(ext) + 1], ext) == 0)
#define FILE_PATH_MAX (ESP_VFS_PATH_MAX + CONFIG_SPIFFS_OBJ_NAME_LEN)
#define BUFSIZE             8192
#define BASE_PATH           string("/spiffs")
#define NAME_FILE_CONF      string("settings.json")