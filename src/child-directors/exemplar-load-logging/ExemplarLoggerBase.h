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

#pragma once
#include <filesystem>
#include <fstream>
#include <type_traits>

class cGZPersistResourceKey;
class cISCResExemplar;

enum class ExemplarLoggerOptions : uint32_t
{
	None = 0,
	LogExemplarLoading = 1 << 0,
	LogExemplarLoadingErrors = 1 << 1,
};

inline ExemplarLoggerOptions operator|(ExemplarLoggerOptions lhs, ExemplarLoggerOptions rhs)
{
	return static_cast<ExemplarLoggerOptions>(
		static_cast<std::underlying_type<ExemplarLoggerOptions>::type>(lhs) |
		static_cast<std::underlying_type<ExemplarLoggerOptions>::type>(rhs)
		);
}

inline ExemplarLoggerOptions operator&(ExemplarLoggerOptions lhs, ExemplarLoggerOptions rhs)
{
	return static_cast<ExemplarLoggerOptions>(
		static_cast<std::underlying_type<ExemplarLoggerOptions>::type>(lhs) &
		static_cast<std::underlying_type<ExemplarLoggerOptions>::type>(rhs)
		);
}

class ExemplarLoggerBase
{
public:

	ExemplarLoggerBase(const std::filesystem::path& logFilePath, bool debugLevel);

	virtual ExemplarLoggerOptions GetLoggerOptions() const = 0;

	virtual void ExemplarLoaded(
		const char* const originalFunctionName,
		const cGZPersistResourceKey& key,
		cISCResExemplar* resExemplar) = 0;

	void LoadError(
		const char* const originalFunctionName,
		uint32_t riid);

	void LoadError(
		const char* const originalFunctionName,
		uint32_t riid,
		const cGZPersistResourceKey& key);

protected:

	bool IsDebugLevel() const;

	void WriteLine(const char* const line);

	void WriteLineFormatted(const char* const format, ...);

private:

	std::ofstream outStream;
	bool isDebugLevel;
};