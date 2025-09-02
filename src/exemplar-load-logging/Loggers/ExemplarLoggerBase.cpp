/*
 * This file is part of sc4-resource-loading-hooks, a DLL Plugin for SimCity 4
 * that allows other DLLs to modify resources as the game loads them.
 *
 * Copyright (C) 2024, 2025 Nicholas Hayes
 *
 * sc4-resource-loading-hooks is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * sc4-resource-loading-hooks is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with sc4-resource-loading-hooks.
 * If not, see <http://www.gnu.org/licenses/>.
 */

#include "ExemplarLoggerBase.h"
#include "cGZPersistResourceKey.h"
#include <cstdarg>

ExemplarLoggerBase::ExemplarLoggerBase(
	const std::filesystem::path& logFilePath,
	bool debugLevel)
	: outStream(),
	  isDebugLevel(debugLevel)
{

}

bool ExemplarLoggerBase::IsDebugLevel() const
{
	return isDebugLevel;
}

void ExemplarLoggerBase::LoadError(
	const char* const originalFunctionName,
	uint32_t riid)
{
	if (isDebugLevel)
	{
		WriteLineFormatted(
			__FUNCSIG__ ": Error loading exemplar, riid=0x%08X",
			riid);
	}
	else
	{
		WriteLineFormatted(
			"Error loading exemplar, riid=0x%08X",
			riid);
	}
}

void ExemplarLoggerBase::LoadError(
	const char* const originalFunctionName,
	uint32_t riid,
	const cGZPersistResourceKey& key)
{
	if (isDebugLevel)
	{
		WriteLineFormatted(
			__FUNCSIG__ ": Error loading exemplar, T=0x%08X G=0x%08X, I=0x%08X, riid=0x%08X",
			key.type,
			key.group,
			key.instance,
			riid);
	}
	else
	{
		WriteLineFormatted(
			"Error loading exemplar, T=0x%08X G=0x%08X, I=0x%08X, riid=0x%08X",
			key.type,
			key.group,
			key.instance,
			riid);
	}
}

void ExemplarLoggerBase::WriteLine(const char* const line)
{
	if (outStream)
	{
		outStream << line << std::endl;
	}
}

void ExemplarLoggerBase::WriteLineFormatted(const char* const format, ...)
{
	va_list args;
	va_start(args, format);

	va_list argsCopy;
	va_copy(argsCopy, args);

	int formattedStringLength = std::vsnprintf(nullptr, 0, format, argsCopy);

	va_end(argsCopy);

	if (formattedStringLength > 0)
	{
		size_t formattedStringLengthWithNull = static_cast<size_t>(formattedStringLength) + 1;

		constexpr size_t stackBufferSize = 1024;

		if (formattedStringLengthWithNull >= stackBufferSize)
		{
			std::unique_ptr<char[]> buffer = std::make_unique_for_overwrite<char[]>(formattedStringLengthWithNull);

			std::vsnprintf(buffer.get(), formattedStringLengthWithNull, format, args);

			WriteLine(buffer.get());
		}
		else
		{
			char buffer[stackBufferSize]{};

			std::vsnprintf(buffer, stackBufferSize, format, args);

			WriteLine(buffer);
		}
	}

	va_end(args);
}


