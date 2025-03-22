#include "Flash.h"

// 解锁 Flash
void Flash_Unlock(void) { FLASH_Unlock(); }

// 锁定 Flash
void Flash_Lock(void) { FLASH_Lock(); }

// 擦除 Flash 页
void Flash_ErasePage(uint32_t pageAddress)
{
	FLASH_Status status;
	status = FLASH_ErasePage(pageAddress);
	if (status != FLASH_COMPLETE) {
		printf("Flash erase failed\n");
	}
}

// 写入 Flash
void Flash_Write(uint32_t address, uint8_t *data, uint32_t size)
{
	FLASH_Status status;
	for (uint32_t i = 0; i < size; i += 2) {
		uint16_t value = *(uint16_t *)(data + i);
		status = FLASH_ProgramHalfWord(address + i, value);
		if (status != FLASH_COMPLETE) {
			printf("Flash write failed at address %lu\n", address + i);
			break;
		}
	}
}

// 读取 Flash
void Flash_Read(uint32_t address, uint8_t *data, uint32_t size)
{
	memcpy(data, (uint8_t *)address, size);
}