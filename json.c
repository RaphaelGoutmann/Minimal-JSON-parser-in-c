#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#include "json.h"

#define is_whitespace(x)                         x == '\r' || x == '\n' || x == '\t' || x == ' '
#define remove_white_space(x)                     while(is_whitespace(*x)) x++


static int str_next_occurence(char ch, char* str)
{
    int i = 0;

    if(!ch)
        return -1;
    
    while(*str != '\0' && *str != ch)
        str++, i++;
    
    return (*str == '\0') ? -1 : i;
}

JSONObject* json_get_object(char* key, JSONObject* obj)
{
        if(!obj || !obj->list)
            return NULL;

    for(int i = 0; i < obj->size; i++)
    {
           if(obj->list[i].type == JSON_STRING)
           {
               continue;
           }
           else if(obj->list[i].type == JSON_OBJECT)
           {
                if(!strcmp(obj->list[i].key, key))
                    return obj->list[i].value->objectValue;

                JSONObject* tempJSONObject = json_get_object(key, obj->list[i].value->objectValue);
                
                if(tempJSONObject)
                    return tempJSONObject;
           }
    } 
   
   return NULL;
}

char* json_get_value(char* key, JSONObject* obj)
{
    if(!obj || !obj->list)
            return NULL;

    for(int i = 0; i < obj->size; i++)
    {
            if(obj->list[i].type == JSON_STRING)
            {
                if(!strcmp(obj->list[i].key, key))
                    return obj->list[i].value->stringValue;
            }
            else if(obj->list[i].type == JSON_OBJECT)
            {
                continue;
            }
    } 
   
   return NULL;
}


void free_json_object(JSONObject *obj) 
{
    if(obj == NULL)
        return;
    
    if(obj->list == NULL) 
    {
        free(obj);
        return;
    }
    
    for(int i = 0; i < obj->size; i++) 
    {
        if(obj->list[i].key != NULL)
            free(obj->list[i].key);

        if(obj->list[i].value != NULL) 
        {
            switch(obj->list[i].type) 
            {
                case JSON_STRING:
                    free(obj->list[i].value->stringValue);
                    break;
                case JSON_OBJECT:
                    free_json_object(obj->list[i].value->objectValue);
            }
            free(obj->list[i].value);
        }
    }
    
}


JSONObject* _parse_json(char** p_ptr)
{
    char* ptr = *p_ptr;

    JSONObject* obj = malloc(sizeof(JSONObject));
    obj->size = 0;
    obj->list = malloc(sizeof(JSONPair));

    bool opened = false;
    while(*ptr != '\0')
    {
        remove_white_space(ptr);

        if(*ptr == '{')
        {
            opened = true;
        } 
        else if(*ptr == '#')
        {
            while(*ptr != '\n' && *ptr != '\0')
                    ptr++;
        }
        else if(!opened) 
        { 
            fprintf(stderr, "json: expected '{' before any declaration\n"); 
            exit(EXIT_FAILURE); 
        }

        if(opened)
        {
            if(*ptr == '}')
            {                
                *p_ptr = ptr;
                obj->size++;
                return obj;
            } 
            else if(*ptr == '\"')
            {
                ptr++;

                int i = str_next_occurence('\"', ptr);

                if(i == -1)
                {
                    fprintf(stderr, "json: expected '\"' arround keys\n");
                    exit(EXIT_FAILURE);
                }

                obj->list[obj->size].key = malloc(sizeof(char) * (i + 1) );

                if(!obj->list[obj->size].key)
                {
                    perror("");
                    exit(EXIT_FAILURE);
                }

                memcpy(obj->list[obj->size].key, ptr, i * sizeof(char));
                obj->list[obj->size].key[i] = '\0';

                ptr += i + 1;

                remove_white_space(ptr);

                
                if(*ptr != ':')
                {
                    fprintf(stderr, "json: expected ':' after '\"' token \n");
                    exit(EXIT_FAILURE);
                }

                ptr++;
                remove_white_space(ptr);

                if(*ptr == '{')
                {
                    
                    obj->list[obj->size].value = malloc(sizeof(JSONValue));

                    if(!obj->list[obj->size].value)
                    {
                        perror("");
                        exit(EXIT_FAILURE);
                    }

                    obj->list[obj->size].type = JSON_OBJECT;
                    obj->list[obj->size].value->objectValue = _parse_json(&ptr);
                    
                }
                else if(*ptr == '"')
                {

                    ptr++;
                    i = str_next_occurence('"', ptr);
                    
                    obj->list[obj->size].type = JSON_STRING;
                    obj->list[obj->size].value = malloc(sizeof(JSONValue));

                    if(!obj->list[obj->size].value)
                    {
                        perror("");
                        exit(EXIT_FAILURE);
                    }
                    
                    obj->list[obj->size].value->stringValue = malloc(sizeof(char) * (i + 1) );


                    if(!obj->list[obj->size].value->stringValue)
                    {
                        perror("");
                        exit(EXIT_FAILURE);
                    }

                    memcpy(obj->list[obj->size].value->stringValue, ptr, i * sizeof(char));
                    obj->list[obj->size].value->stringValue[i] = '\0';

                    ptr += i;
                }
                else if(isdigit(*ptr))
                {
                    
                    obj->list[obj->size].type = JSON_STRING;
                    obj->list[obj->size].value = malloc(sizeof(JSONValue));

                    
                    if(!obj->list[obj->size].value)
                    {
                        perror("");
                        exit(EXIT_FAILURE);
                    }

                    

                    i = 0;
                    while(isdigit(*(ptr + i)))
                        i++;

                    obj->list[obj->size].value->stringValue = malloc(sizeof(char) * (i + 1) );

                    if(!obj->list[obj->size].value->stringValue)
                    {
                        perror("");
                        exit(EXIT_FAILURE);
                    }
                    
                    memcpy(obj->list[obj->size].value->stringValue, ptr, i * sizeof(char));
                    obj->list[obj->size].value->stringValue[i] = '\0';

                    ptr += (i - 1);
                }
                else 
                {
                    fprintf(stderr, "json: expected ':' or '{' after ':' token\n");
                    exit(EXIT_FAILURE);
                }
            }
            else if(*ptr == ',')
            {
                obj->size++;
                obj->list = realloc(obj->list, sizeof(JSONPair) * (obj->size + 1) );

                if(!obj->list)
                {
                    perror("");
                    exit(EXIT_FAILURE);
                }
            }
        }
        ptr++;
    }

    if(opened)
    {
        fprintf(stderr, "json: expected '}' to close root object\n");
        exit(EXIT_FAILURE);
    }

    return NULL;
}

JSONObject* parse_json(char* str)
{
    char* ptr = str;
    return (_parse_json(&ptr));
}

