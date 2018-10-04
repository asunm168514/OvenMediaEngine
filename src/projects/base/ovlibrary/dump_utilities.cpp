//==============================================================================
//
//  OvenMediaEngine
//
//  Created by Hyunjun Jang
//  Copyright (c) 2018 AirenSoft. All rights reserved.
//
//==============================================================================
#include "./dump_utilities.h"
#include "./data.h"

#include "./ovlibrary_private.h"

#define __STDC_FORMAT_MACROS

#include <cinttypes>
#include <cmath>
#include <cctype>

namespace ov
{
	// Dump utilities
	//TODO: 성능 최적화 필요
	String ToHexStringWithDelimiter(const void *data, size_t length, char delimiter)
	{
		String dump;

		const auto *buffer = static_cast<const uint8_t *>(data);

		for(int index = 0; index < length; index++)
		{
			dump.AppendFormat("%02X", *buffer);

			if(index < length - 1)
			{
				dump.Append(delimiter);
			}
			buffer++;
		}

		return dump;
	}

	String ToHexStringWithDelimiter(const Data *data, char delimiter)
	{
		return ToHexStringWithDelimiter(data->GetData(), data->GetLength(), delimiter);
	}

	String ToHexString(const void *data, size_t length)
	{
		String dump;

		const auto *buffer = static_cast<const uint8_t *>(data);

		for(int index = 0; index < length; index++)
		{
			dump.AppendFormat("%02X", *buffer);
			buffer++;
		}

		return dump;
	}

	String Dump(const void *data, size_t length, const char *title, off_t offset, size_t max_bytes, const char *line_prefix) noexcept
	{
		if(offset > length)
		{
			offset = length;
		}

		String dump;
		const char *buffer = ((const char *)data + offset);
		max_bytes = std::min((length - offset), max_bytes);

		// 최대 1MB 까지 덤프 가능
		int dump_bytes = (int)std::min(max_bytes, (1024UL * 1024UL));

		if(title == nullptr)
		{
			title = "Memory dump of";
		}

		if(line_prefix == nullptr)
		{
			line_prefix = "";
		}

		if(offset > 0)
		{
			// ========== xxxxx 0x12345678 + 0xABCDEF01 (102400 / 1024000) ==========
			dump.AppendFormat("%s========== %s 0x%08X + 0x%08X (%d/%" PRIi64 " bytes) ==========", line_prefix, title, data, offset, dump_bytes, length);
		}
		else
		{
			// ========== xxxxx 0x12345678 (102400 / 1024000) ==========
			dump.AppendFormat("%s========== %s 0x%08X (%d/%" PRIi64 " bytes) ==========", line_prefix, title, data, dump_bytes, length);
		}

		if(dump_bytes == 0L)
		{
			dump.AppendFormat("\n%s(Data is empty)", line_prefix);
		}
		else
		{
			String ascii;
			int padding = (int)(log((double)dump_bytes) / log(16.0)) + 1;
			String number = String::FormatString("\n%s%%0%dX | ", line_prefix, padding);

			if(dump_bytes > 0)
			{
				dump.AppendFormat(number, 0L);
			}

			// 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F | 0123456789ABCDEF
			for(int index = 0; index < dump_bytes; index++)
			{
				if((index > 0) && (index % 16 == 0))
				{
					dump.AppendFormat("| %s", ascii.CStr());

					if(index <= (dump_bytes - 1))
					{
						dump.AppendFormat(number, index);
					}

					ascii = "";
				}

				char character = buffer[index];

				if(!isprint(character))
				{
					character = '.';
				}

				ascii.Append(character);
				dump.AppendFormat("%02X ", (unsigned char)(buffer[index]));
			}

			if(ascii.IsEmpty() == false)
			{
				if(dump_bytes % 16 > 0)
				{
					String pad_string;

					pad_string.PadRight(static_cast<size_t>((16 - (dump_bytes % 16)) * 3));

					dump.Append(pad_string);
				}

				dump.AppendFormat("| %s", ascii.CStr());
			}
		}

		return dump;
	}

	String Dump(const void *data, size_t length, size_t max_bytes) noexcept
	{
		return Dump(data, length, nullptr, 0, max_bytes, nullptr);
	}

	bool DumpToFile(FILE **file, const char *file_name, const void *data, size_t length, off_t offset, bool append) noexcept
	{
		if((file == nullptr) || (offset < 0L))
		{
			return false;
		}

		if(*file == nullptr)
		{
			*file = ::fopen(file_name, append ? "ab" : "wb");
		}

		if(*file == nullptr)
		{
			return false;
		}

		::fwrite(static_cast<const uint8_t *>(data) + offset, sizeof(uint8_t), static_cast<size_t>(length), *file);
		::fflush(*file);
	}
}