#include "JsonwithFlash.h"
#include "cJSON.h"

/* 默认参数 */
static const PID_Config default_config = {
	.Kp = 1.0f,
	.Ki = 0.1f,
	.Kd = 0.05f,
	.setpoint = 0.0f,
	.output_limit = 100.0f,
	.timestamp = 0,
	.control_mode = 0,
	.integral_limit = 50.0f, //New parameter	新增参数默认值
	.filter_alpha = 0.1f		//New parameter 新增参数默认值
};

/* 当前参数 */
static PID_Config current_config;

/* Flash操作 */
/**
  * @brief  将数据写入Flash
  * @param  address: Flash起始地址（必须为偶数地址）
  * @param  data: 要写入的数据指针
  * @param  size: 数据大小（字节数）
  * @retval 无
  */
static void FLASH_WriteData(uint32_t address, const uint8_t* data, uint32_t size)
{
	/* 解锁Flash */
	FLASH_Unlock();

	/* 清除所有Flash标志位 */
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);

	/* 擦除指定页 */
	FLASH_Status status = FLASH_ErasePage(address);
	if (status != FLASH_COMPLETE) {
		/* 擦除失败处理 */
		return;
	}

	/* 按半字（16位）写入数据 */
	uint16_t* pData = (uint16_t*)data;
	size = (size + 1) / 2; // 转换为半字数

	for (uint32_t i = 0; i < size; i++) {
		status = FLASH_ProgramHalfWord(address + i * 2, pData[i]);
		if (status != FLASH_COMPLETE) {
			/* 写入失败处理 */
			break;
		}
	}

	/* 锁定Flash */
	FLASH_Lock();
}

/**
  * @brief  从Flash读取数据
  * @param  address: Flash起始地址
  * @param  data: 存储读取数据的缓冲区指针
  * @param  size: 要读取的数据大小（字节数）
  * @retval 无
  */
static void FLASH_ReadData(uint32_t address, uint8_t* data, uint32_t size)
{
	/* 直接内存拷贝 */
	memcpy(data, (void*)address, size);
}

/* JSON转换 */
static cJSON* Config_ToJSON(const PID_Config* config);
static void JSON_ToConfig(cJSON* root, PID_Config* config);

/* 接口实现 */
/**
  * @brief  初始化PID配置
  * @retval 无
  */
void PID_Config_Init(void)
{
	if (!PID_Config_Load()) {
		// 加载失败，使用默认参数
		current_config = default_config;
		PID_Config_Save();
	}
}

/**
  * @brief  获取当前PID配置
  * @retval 当前PID配置的指针
  */
const PID_Config* PID_Config_Get(void)
{
	return &current_config;
}

/**
  * @brief  设置新的PID配置
  * @param  new_config: 新的PID配置指针
  * @retval 无
  */
void PID_Config_Set(const PID_Config* new_config)
{
	if (new_config != NULL) {
		current_config = *new_config;
	}
}

/**
  * @brief  保存当前PID配置到Flash
  * @retval 无
  */
void PID_Config_Save(void)
{
	cJSON* json = Config_ToJSON(&current_config);
	char* json_str = cJSON_PrintUnformatted(json);

	uint32_t flag = FLASH_FLAG_VALUE;
	uint32_t json_len = strlen(json_str) + 1;

	uint8_t* buffer = (uint8_t*)malloc(sizeof(flag) + json_len);
	memcpy(buffer, &flag, sizeof(flag));
	memcpy(buffer + sizeof(flag), json_str, json_len);

	FLASH_WriteData(PID_DATA_FLASH_ADDRESS, buffer, sizeof(flag) + json_len);

	free(buffer);
	cJSON_free(json_str);
	cJSON_Delete(json);
}

/**
  * @brief  从Flash加载PID配置
  * @retval 加载成功返回1，失败返回0
  */
uint8_t PID_Config_Load(void)
{
	uint32_t flag = 0;
	FLASH_ReadData(PID_DATA_FLASH_ADDRESS, (uint8_t*)&flag, sizeof(flag));

	if (flag != FLASH_FLAG_VALUE) {
		return 0;
	}

	uint8_t buffer[FLASH_PAGE_SIZE - sizeof(flag)];
	FLASH_ReadData(PID_DATA_FLASH_ADDRESS + sizeof(flag), buffer, sizeof(buffer));

	cJSON* json = cJSON_Parse((char*)buffer);
	if (!json) {
		return 0;
	}

	JSON_ToConfig(json, &current_config);
	cJSON_Delete(json);
	return 1;
}

/**
  * @brief  获取默认PID配置
  * @retval 默认PID配置的指针
  */
const PID_Config* PID_Config_GetDefault(void)
{
	return &default_config;
}

/* 内部函数 */
/**
  * @brief  将PID配置转换为JSON对象
  * @param  config: PID配置指针
  * @retval JSON对象指针
  */
static cJSON* Config_ToJSON(const PID_Config* config)
{
	cJSON* root = cJSON_CreateObject();
	cJSON_AddNumberToObject(root, "Kp", config->Kp);
	cJSON_AddNumberToObject(root, "Ki", config->Ki);
	cJSON_AddNumberToObject(root, "Kd", config->Kd);
	cJSON_AddNumberToObject(root, "setpoint", config->setpoint);
	cJSON_AddNumberToObject(root, "output_limit", config->output_limit);
	cJSON_AddNumberToObject(root, "timestamp", config->timestamp);
	cJSON_AddNumberToObject(root, "control_mode", config->control_mode);
	cJSON_AddNumberToObject(root, "integral_limit", config->integral_limit); //New parameter 新增参数
	cJSON_AddNumberToObject(root, "filter_alpha", config->filter_alpha); //New parameter 新增参数
	return root;
}

/**
  * @brief  将JSON对象转换为PID配置
  * @param  root: JSON对象指针
  * @param  config: PID配置指针
  * @retval 无
  */
static void JSON_ToConfig(cJSON* root, PID_Config* config)
{
	cJSON* item;
	item = cJSON_GetObjectItem(root, "Kp"); if (item) config->Kp = cJSON_GetNumberValue(item);
	item = cJSON_GetObjectItem(root, "Ki"); if (item) config->Ki = cJSON_GetNumberValue(item);
	item = cJSON_GetObjectItem(root, "Kd"); if (item) config->Kd = cJSON_GetNumberValue(item);
	item = cJSON_GetObjectItem(root, "setpoint"); if (item) config->setpoint = cJSON_GetNumberValue(item);
	item = cJSON_GetObjectItem(root, "output_limit"); if (item) config->output_limit = cJSON_GetNumberValue(item);
	item = cJSON_GetObjectItem(root, "timestamp"); if (item) config->timestamp = (uint32_t)cJSON_GetNumberValue(item);
	item = cJSON_GetObjectItem(root, "control_mode"); if (item) config->control_mode = (uint8_t)cJSON_GetNumberValue(item);
	item = cJSON_GetObjectItem(root, "integral_limit"); if (item) config->integral_limit = cJSON_GetNumberValue(item); //New parameter 新增参数
	item = cJSON_GetObjectItem(root, "filter_alpha"); if (item) config->filter_alpha = cJSON_GetNumberValue(item); //New parameter 新增参数
}