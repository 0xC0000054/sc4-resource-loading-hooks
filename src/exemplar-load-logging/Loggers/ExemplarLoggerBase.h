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