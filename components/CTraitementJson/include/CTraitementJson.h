#ifndef CTRAITEMENTJSON_H_
#define CTRAITEMENTJSON_H_


#include <stdio.h>
#include "esp_log.h"


#include <iostream>
#include <cJSON.h>
#include <cstring>
#include <map>

class CTraitementJson {
private:
    struct cmp_str {
        bool operator()(char const *a, char const *b) const
        {
            return std::strcmp(a, b) < 0;
        }
    };

private:
    char* m_nameFileJson;
    FILE* m_fileJson;
    cJSON* m_json;    
    std::map<char*, int, cmp_str> m_mapJsonKey;

public:

    CTraitementJson();
    CTraitementJson(const char* nameFileJson);
    CTraitementJson(cJSON* json);
    ~CTraitementJson();

    cJSON* GetItem(char* name);
    double GetNumberToProperty(char* nameProperty);
    char* GetStringToProperty(char* nameProperty);
    bool GetBoolToProperty(char* nameProperty);

    void AddElement(char* name, double value);
    void AddElement(char* name, char* value);    
    void AddElement(char* name, bool value);
    
    void DeleteElement(char* name);

    void ModifyElement(char* name, double newValue);
    void ModifyElement(char* name, char* newValue);
    void ModifyElement(char* name, bool newValue);

    void ActKeyJson();

    void PrintPropertyAndValues();

    
private:
    static const char* TAG;
};
#endif //CTRAITEMENTJSON_H_