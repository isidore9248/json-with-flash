#pragma once

#include "STM32Headfile.h"

void Flash_Unlock(void);
void Flash_Lock(void);
void Flash_ErasePage(uint32_t pageAddress);
void Flash_Write(uint32_t address, uint8_t *data, uint32_t size);
void Flash_Read(uint32_t address, uint8_t *data, uint32_t size);