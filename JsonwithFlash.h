#pragma once

//!!!!! Ctrl+F to Search  "New parameter"
//在PID_Config(.h)结构体中添加新成员
//在default_config中设置默认值
//在Config_ToJSON(func name)中添加序列化代码
//在JSON_ToConfig(func name)中添加反序列化代码

#include "STM32Headfile.h"

/* Flash配置 */
#define FLASH_PAGE_SIZE         ((uint16_t)0x400)		// 1KB
#define PID_DATA_FLASH_ADDRESS  ((uint32_t)0x0800FC00)
#define FLASH_FLAG_VALUE        ((uint32_t)0xA5A5A5A5)	// 标志位魔数

/* 参数结构体 */
typedef struct {
	// 基础PID参数
	float Kp;
	float Ki;
	float Kd;

	// 控制参数
	float setpoint;
	float output_limit;

	// 系统参数
	uint32_t timestamp;
	uint8_t control_mode;

	// 扩展参数
	float integral_limit; //New parameter 新增积分限幅
	float filter_alpha; //New parameter 新增滤波器系数
} PID_Config;

/* 初始化接口 */
void PID_Config_Init(void);

/* 参数访问接口 */
const PID_Config* PID_Config_Get(void); //通过此函数获取当前pid参数，使用指针，修改后不需要再次获取
void PID_Config_Set(const PID_Config* new_config); //通过此函数设置新的pid参数

/* 持久化接口 */
void PID_Config_Save(void);
uint8_t PID_Config_Load(void);

/* 默认参数接口 */
const PID_Config* PID_Config_GetDefault(void);
