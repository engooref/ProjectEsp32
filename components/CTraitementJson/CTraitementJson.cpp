#include "CTraitementJson.h"

using namespace std;

const char* CTraitementJson::TAG = "JSON";

CTraitementJson::CTraitementJson() : 
    m_nameFileJson(nullptr),
    m_fileJson(NULL),
    m_json(cJSON_CreateObject())
{ 
    ESP_LOGI(TAG, "Creation de la configuration vide");
}

CTraitementJson::CTraitementJson(const char* nameFileJson) :
    m_nameFileJson((char*)malloc(strlen(nameFileJson) + 1)),
    m_fileJson(NULL),
    m_json(nullptr)
{
    sprintf(m_nameFileJson, nameFileJson);
    if(!(m_fileJson = fopen(m_nameFileJson, "r+"))) {
        ESP_LOGI(TAG, "Fichier inexistant, tentative de création du fichier");
        if(!(m_fileJson = fopen(m_nameFileJson, "w+"))){
            ESP_LOGE(TAG, "Erreur lors de la tentative de creation du fichier %s", m_nameFileJson);
            ESP_LOGE(TAG, "Code d'erreur: %d", errno);
            return; 
        }
        ESP_LOGI(TAG, "Creation du fichier reussi");
    }

    ESP_LOGI(TAG, "Chargement des données");
    fseek(m_fileJson, 0, SEEK_END);
    size_t lenghtFile = ftell(m_fileJson);
    if(lenghtFile){
        ESP_LOGI(TAG, "Longueur du fichier: %doctets", lenghtFile);        
        char* bufFile = (char*)malloc(lenghtFile);
        fseek(m_fileJson, 0, SEEK_SET);

        fread(bufFile, 1, lenghtFile, m_fileJson);
        if(!(m_json = cJSON_ParseWithLength(bufFile, lenghtFile))) {
            ESP_LOGE(TAG, "Erreur dans le fichier, verifiez les donnees");
            ESP_LOGE(TAG, "Chargement incomplet");
            return;
        }

        free(bufFile);
        ActKeyJson();
    } else {
        ESP_LOGI(TAG, "Creation de la configuration vide");
        m_json = cJSON_CreateObject();
    }
    ESP_LOGI(TAG, "Chargement reussi");
}

CTraitementJson::~CTraitementJson(){
    ESP_LOGI(TAG, "Sauvegarde des données et destruction de la classe");
    if(m_nameFileJson) { free(m_nameFileJson); m_nameFileJson = nullptr; }
    if(m_fileJson) { fclose(m_fileJson); m_fileJson = NULL; }
    if(m_json) { cJSON_Delete(m_json); m_json = nullptr; }
}

cJSON* CTraitementJson::GetItem(char* name){
    if(m_mapJsonKey.find(name) != m_mapJsonKey.end()) return cJSON_GetArrayItem(m_json, m_mapJsonKey[name]);
    else return nullptr;
}

double CTraitementJson::GetNumberToProperty(char* nameProperty){
    return cJSON_GetNumberValue(cJSON_GetArrayItem(m_json, m_mapJsonKey[nameProperty]));
}

char* CTraitementJson::GetStringToProperty(char* nameProperty){
    return cJSON_GetStringValue(cJSON_GetArrayItem(m_json, m_mapJsonKey[nameProperty]));
}

bool CTraitementJson::GetBoolToProperty(char* nameProperty){
    return cJSON_GetArrayItem(m_json, m_mapJsonKey[nameProperty])->type;
}

void CTraitementJson::AddElement(char* name, double value){
        cJSON_AddNumberToObject(m_json, name, value);
        ActKeyJson();
}

void CTraitementJson::AddElement(char* name, char* value){
    cJSON_AddStringToObject(m_json, name, value);
    ActKeyJson();
}    

void CTraitementJson::AddElement(char* name, bool value){
    cJSON_AddBoolToObject(m_json, name, value);
    ActKeyJson();
}

void CTraitementJson::DeleteElement(char* name){
    cJSON_DeleteItemFromArray(m_json, m_mapJsonKey[name]);
    ActKeyJson();
}

void CTraitementJson::ModifyElement(char* name, double newValue){
    DeleteElement(name);
    AddElement(name, newValue);
}

void CTraitementJson::ModifyElement(char* name, char* newValue){
    DeleteElement(name);
    AddElement(name, newValue);
}

void CTraitementJson::ModifyElement(char* name, bool newValue){
    DeleteElement(name);
    AddElement(name, newValue);
}

void CTraitementJson::ActKeyJson(){
    m_mapJsonKey.clear();
    size_t lenArray = cJSON_GetArraySize(m_json);
    for(int i = 0; i < lenArray; i++)
        m_mapJsonKey[cJSON_GetArrayItem(m_json, i)->string] = i;
}

void CTraitementJson::PrintPropertyAndValues(){
    for(auto itMap = m_mapJsonKey.begin(); itMap != m_mapJsonKey.end(); ++itMap){
        cJSON* item = cJSON_GetArrayItem(m_json, itMap->second);
        cout << "Propriete: " << item->string << " Valeur: ";
        if(cJSON_IsNumber(item)) cout << cJSON_GetNumberValue(item);
        else if(cJSON_IsString(item)) cout << cJSON_GetStringValue(item);
        else if(cJSON_IsBool(item)) cout << ((item->type == cJSON_True) ? "true" : "false");
        cout << endl;
    }
}