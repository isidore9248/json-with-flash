#include "JsonwithFlash.h"
#include "OLED.h"

static const uint32_t cJSON_Flash_addr = 0x08010000;

/// <summary>
/// 创建一个JSON字符串
/// </summary>
/// <param name="json_str_in">传入传出参数，指向存储JSON字符串的指针</param>
void create_json(char** json_str_in)
{
	// 创建一个JSON根对象
	cJSON* root = cJSON_CreateObject();
	*json_str_in = cJSON_Print(root);
	cJSON_Delete(root);
}

// 函数用于处理不同类型的参数
void insert_json_withroot(char** json_str_in, char* objectname, JSON_Flash_ArgType type, ...)
{
	va_list args;
	va_start(args, type);

	JSON_Flash_Value first_value;

	// 将字符串解析成 cJSON 对象
	if (*json_str_in == NULL) { return; }
	cJSON* root = cJSON_Parse(*json_str_in);

	switch (type)
	{
	case TYPE_INT:
		{
			first_value.int_value = va_arg(args, int); // 提取 int 类型
			cJSON_AddNumberToObject(root, objectname, first_value.int_value);
			break;
		}
	case TYPE_STRING:
		{
			first_value.string_value = va_arg(args, char*); // 提取 string 类型
			cJSON_AddStringToObject(root, objectname, first_value.string_value);
			break;
		}
	default:
		//printf("Unknown type!\n");
		break;
	}

	*json_str_in = cJSON_Print(root);
	cJSON_Delete(root);

	va_end(args);
}

void insert_json_withcommonobject(char ** root, char* commonobject_str_in, char* objectname, JSON_Flash_ArgType type, ...)
{
	va_list args;
	va_start(args, type);

	JSON_Flash_Value first_value;

	// 将字符串解析成 cJSON 对象
	if (commonobject_str_in == NULL || *root == NULL) { return; }
	cJSON* commonobject = cJSON_CreateObject();

	switch (type)
	{
	case TYPE_INT:
		{
			first_value.int_value = va_arg(args, int); // 提取 int 类型
			cJSON_AddNumberToObject(commonobject, objectname, first_value.int_value);
			break;
		}
	case TYPE_STRING:
		{
			first_value.string_value = va_arg(args, char*); // 提取 string 类型
			cJSON_AddStringToObject(commonobject, objectname, first_value.string_value);
			break;
		}
	default:
		//printf("Unknown type!\n");
		break;
	}

	//获取根节点的JSON对象
	cJSON* cJSON_root = cJSON_Parse(*root);
	//将JSON对象添加到根节点中
	cJSON_AddItemToObject(cJSON_root, commonobject_str_in, commonobject);

	* root = cJSON_Print(cJSON_root);

	//cJSON_Delete(commonobject),
	cJSON_Delete(cJSON_root);
	va_end(args);
}

void Write_JSON_To_Flash(char * root)
{
	//解锁 Flash
	Flash_Unlock();

	// 擦除 Flash 页
	uint32_t flashAddress = cJSON_Flash_addr; // 选择一个合适的 Flash 地址
	Flash_ErasePage(flashAddress);

	// 将 JSON 字符串写入 Flash
	Flash_Write(flashAddress, (uint8_t *)root, strlen(root) + 1);

	// 锁定 Flash
	Flash_Lock();
}

void Read_JSON_From_Flash(void)
{
	// 从 Flash 读取 JSON 字符串
	uint32_t flashAddress = cJSON_Flash_addr; // 与写入地址一致
	char json_str[256]; // 假设 JSON 字符串最大长度为 256
	Flash_Read(flashAddress, (uint8_t *)json_str, sizeof(json_str));

	// 解析 JSON 字符串
	cJSON *root = cJSON_Parse(json_str);
	if (root == NULL) { OLED_ShowString(2, 1, "Error parsing JSON"); return; }
	OLED_ShowString(2, 1, "ReadJson--ok");

	// 读取 JSON 数据
	cJSON *devicename = cJSON_GetObjectItem(root, "devicename");

	if (cJSON_IsString(devicename))
	{
		OLED_ShowString(3, 1, devicename->valuestring);
	}
	else if (cJSON_IsNumber(devicename))
	{
		OLED_ShowNum(3, 1, devicename->valueint, 4);
	}
	else
	{
		OLED_ShowString(3, 1, "JSON-read-ERROR");
	}

	//获取root对象下的嵌套对象 mpu6050
	cJSON* mpu6050 = cJSON_GetObjectItemCaseSensitive(root, "mpu6050");
	if (mpu6050)
	{
		cJSON* pitch = cJSON_GetObjectItemCaseSensitive(mpu6050, "pitch");
		if (pitch)
		{
			OLED_ShowNum(3, 8, pitch->valueint, 4);
		}
		//cJSON_Delete(pitch);
	}
	else
	{
		OLED_ShowString(1, 1, "JSON-read-ERROR");
	}
	// 释放内存
	cJSON_Delete(root);
}

void JSON_test(void)
{
	// 创建 JSON 对象
	char * root = NULL;
	create_json(&root);

	if (root == NULL) {
		OLED_ShowString(1, 1, "Error creating JSON object");
		return;
	}
	OLED_ShowString(1, 1, "CreateJson--ok");

	// 添加数据到 JSON 对象
	insert_json_withroot(&root, "devicename", TYPE_INT, 32);
	insert_json_withcommonobject(&root, "mpu6050", "pitch", TYPE_INT, 50);
	Write_JSON_To_Flash(root);
	Read_JSON_From_Flash();

	free(root);
}

//当前json文件格式内容
/**
{
    "devicename": 32,
    "mpu6050": {
        "pitch": 50
    }
}
 **/