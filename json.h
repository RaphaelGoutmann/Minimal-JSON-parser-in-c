#ifndef JSON
#define JSON

#include <string.h>
#include <stdlib.h>


struct _jsonobject;
struct _jsonpair;
union _jsonvalue;

typedef enum _jsontype
{
    JSON_OBJECT,
    JSON_STRING
}JSONType;

typedef struct _jsonobject
{
    struct _jsonpair* list;
    int size;
}JSONObject;

typedef struct _jsonpair
{
    char* key;
    enum _jsontype type;
    union _jsonvalue* value;
}JSONPair;


typedef union _jsonvalue
{
    char* stringValue;
    struct _jsonobject* objectValue;
}JSONValue;


char* json_get_value(char* key, JSONObject* obj);
JSONObject* json_get_object(char* key, JSONObject* obj);

JSONObject* parse_json(char* str);
void free_json_object(JSONObject *obj);

#endif /* JSON */