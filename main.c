#include "STM32Headfile.h"
#include "cJSON.h"
#include "OLED.h"
#include "Flash.h"
#include "JsonwithFlash.h"

void cJSON_test();

int main(void)
{
	// 初始化系统时钟（根据实际情况配置）
	SystemInit();
	OLED_Init();

	JSON_test();

	// 从 Flash 读取 JSON
	//Read_JSON_From_Flash();

	while (1)
	{
		// 主循环
		static uint8_t count = 0;
		//OLED_ShowNum(4, 1, ++count, 5);
	}
}