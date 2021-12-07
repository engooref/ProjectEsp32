idf_component_register(SRCS "CServerHTTP.cpp"
                       INCLUDE_DIRS  "include"
                       REQUIRES nvs_flash log freertos esp_http_server spiffs CTraitementJson)

spiffs_create_partition_image(storage ../../Site_Web FLASH_IN_PROJECT)
