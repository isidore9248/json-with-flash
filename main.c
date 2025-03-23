//!!!!!修改内容参考JsonwithFlash.h 


#include "STM32Headfile.h"
#include "cJSON.h"
#include "OLED.h"
#include "Flash.h"
#include "JsonwithFlash.h"



/* 主程序 */

int main(void)
{
	// 初始化系统
	SystemInit();

	OLED_Init();
	// 初始化PID配置
	PID_Config_Init();

	// 获取当前配置
	const PID_Config* config = PID_Config_Get();

	// 修改配置
	PID_Config new_config = *config;
	new_config.Kp = 2.0f;
	new_config.integral_limit = 100.0f; // 修改新增参数

	// 显示修改前前配置
	OLED_ShowNum(1, 1, (int)(config->integral_limit), 5);

	PID_Config_Set(&new_config);

	// 保存配置
	PID_Config_Save();

	int shownum = (int)(config->integral_limit);

	while (1) {
		// 主循环...
		// 显示修改后的配置
		OLED_ShowNum(2, 1, shownum, 5);
		static uint8_t count = 0;
		OLED_ShowNum(3, 1, ++count, 3);
	}
}
