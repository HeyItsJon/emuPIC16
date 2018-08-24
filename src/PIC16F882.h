#pragma once

#ifdef EMUPIC16_EXPORTS
#define EMUPIC16_API __declspec(dllexport)
#else
#define EMUPIC16_API __declspec(dllimport)
#endif

#include <cstdint>

class PIC16F882
{
public:
	EMUPIC16_API PIC16F882();
	EMUPIC16_API ~PIC16F882();

	EMUPIC16_API bool LoadHexFile(const char*);

private:
	/* 2048 bytes prog mem */
	uint16_t ProgramMemory[2048];
	uint16_t ConfigMemory[10];
	uint8_t RAM[128];
	uint8_t EEPROM[128];

};

