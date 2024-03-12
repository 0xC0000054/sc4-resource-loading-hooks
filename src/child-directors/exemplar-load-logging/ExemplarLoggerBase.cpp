///////////////////////////////////////////////////////////////////////////////
//
// This file is part of sc4-resource-loading-hooks, a DLL Plugin for SimCity 4
// that allows other DLLs to modify resources as the game loads them.
//
// Copyright (c) 2024 Nicholas Hayes
//
// This file is licensed under terms of the MIT License.
// See LICENSE.txt for more information.
//
///////////////////////////////////////////////////////////////////////////////

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


