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

#include "ExemplarResourceLoggerBase.h"
#include "ExemplarErrorLogger.h"
#include "cGZPersistResourceKey.h"
#include "cIGZPersistResource.h"
#include "cISCPropertyHolder.h"
#include "cRZAutoRefCount.h"

ExemplarResourceLoggerBase::ExemplarResourceLoggerBase(
	bool logResourceLoadErrors,
	const std::filesystem::path& logFilePath,
	bool debugLevel)
	: ExemplarLoggerBase(logFilePath, debugLevel)
{
	this->logResourceLoadErrors = logResourceLoadErrors;
}

ExemplarLoggerOptions ExemplarResourceLoggerBase::GetLoggerOptions() const
{
	ExemplarLoggerOptions options = ExemplarLoggerOptions::LogExemplarLoading;

	if (logResourceLoadErrors)
	{
		options = options | ExemplarLoggerOptions::LogExemplarLoadingErrors;
	}

	return options;
}

void ExemplarResourceLoggerBase::LogExemplarTGI(
	const char* const originalFunctionName,
	const cGZPersistResourceKey& key)
{
	if (IsDebugLevel())
	{
		WriteLineFormatted(
			"%s: T=0x%08X G=0x%08X, I=0x%08X",
			originalFunctionName,
			key.type,
			key.group,
			key.instance);
	}
	else
	{
		WriteLineFormatted(
			"T=0x%08X G=0x%08X, I=0x%08X",
			key.type,
			key.group,
			key.instance);
	}
}
