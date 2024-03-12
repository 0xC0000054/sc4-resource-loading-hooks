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
