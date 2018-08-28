// emuPIC16.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

#include <iostream>
using std::ios;
using std::cout;
using std::endl;

#include <fstream>
using std::ifstream;

#include <string>
using std::string;
using std::stoi;

#include "PIC16F882.h"

PIC16F882::PIC16F882()
{
	/* Set default register values */
}


PIC16F882::~PIC16F882()
{
}

bool PIC16F882::LoadHexFile(const char* path)
{
	ifstream ifs(path, ios::binary);

	if (!ifs.is_open())
	{
		ifs.close();
		return false;
	}
		
	while(ifs.good())
	{
		char line[523];	// 11 chars required by file format, 255*2 data chars max, plus newline and null terminator
		uint8_t byte[255] = { 0 };

		ifs.getline(line, sizeof(line));

		uint16_t i = 1;
		uint16_t byte_num = 0;
		while ((line[i] != '\n') && (line[i] != '\r') && (line[i]))
		{
			/* Convert ascii characters to hex values */
			if (i & 0x1)
				byte[byte_num] = 0x10 * ((line[i] <= '9') ? (line[i] - '0') : (line[i] - '7'));
			else
				byte[byte_num++] += ((line[i] <= '9') ? (line[i] - '0') : (line[i] - '7'));
			i++;
		}

		/* Check for valid line */
		if (line[0] != ':')
		{
			cout << "Invalid hex file: Line does not start with ':'" << endl;
			return false;
		}
		if (byte_num != (byte[0] + 5))
		{
			cout << "Invalid hex file: Incorrect byte count" << endl;
			return false;
		}

		/* Calculate checksum */
		uint8_t chksum = 0;
		for (uint16_t i = 0; i < 4 + byte[0]; i++)
			chksum += byte[i];
		if (byte[4 + byte[0]] != (uint8_t)(~chksum + 1))
		{
			cout << "Invalid hex file: Checksum error" << endl;
			return false;
		}

		/* Determine record type */
		switch (byte[3])
		{
		case 0x00:
			/* Data */
			for (uint16_t i = 0, len = byte[0], offset = ((uint16_t)byte[1] * 0x100 + byte[2]) / 2; i < len; i += 2)
			{
				/* Put data in program memory */
				if (offset < 0x2000)
					ProgramMemory[offset + i / 2] = (uint16_t)byte[5 + i] * 0x100 + byte[4 + i];
				/* Put data in configuration memory */
				else
					ConfigMemory[offset - 0x2000 + i/2] = (uint16_t)byte[5 + i] * 0x100 + byte[4 + i];
			}
			break;
		case 0x01:
			/* End of file */
			ifs.close();
			return true;
		case 0x02:
			/* Extended segment address - unused */
			cout << "Invalid hex file: Extended segment address record type (unused)" << endl;
			return false;
		case 0x03:
			/* Start segment address - unused */
			cout << "Invalid hex file: Start segment address record type (unused)" << endl;
			return false;
		case 0x04:
			/* Extended linear address - unused */
			cout << "Invalid hex file: Extended linear address record type (unused)" << endl;
			return false;
		case 0x05:
			/* Start linear address - unused */
			cout << "Invalid hex file: Start linear address record type (unused)" << endl;
			return false;
		default:
			cout << "Invalid hex file: Unknown record type" << endl;
			return false;
		}
	}

	cout << "Invalid hex file: End of file record not found" << endl;
	ifs.close();
	return false;
}


