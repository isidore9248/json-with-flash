#pragma once

#include "STM32Headfile.h"
#include "cJSON.h"
#include "Flash.h"

// 定义一个联合体，用于存储 int 或 char*
typedef union {
	int int_value;
	char* string_value;
} JSON_Flash_Value;

// 定义一个枚举类型，用于标识参数类型,可变参数中使用
typedef enum {
	TYPE_INT,
	TYPE_STRING,
	TYPE_UNKNOWN
} JSON_Flash_ArgType;

void JSON_test(void);
void create_json(char** json_str_in);
void insert_json_withroot(char** json_str_in, char* objectname, JSON_Flash_ArgType type, ...);
void insert_json_withcommonobject(char ** root,
	char* commonobject_str_in,
	char* objectname,
	JSON_Flash_ArgType type,
	...);
void Write_JSON_To_Flash(char * root);
void Read_JSON_From_Flash(void);

void JSON_test(void);