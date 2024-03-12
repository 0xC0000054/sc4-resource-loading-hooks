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

#include "ExemplarTGILogger.h"

ExemplarTGILogger::ExemplarTGILogger(
	bool logResourceLoadErrors,
	const std::filesystem::path& logFilePath,
	bool debugLevel)
	: ExemplarResourceLoggerBase(logResourceLoadErrors, logFilePath, debugLevel)
{
}

void ExemplarTGILogger::ExemplarLoaded(
	const char* const originalFunctionName,
	const cGZPersistResourceKey& key,
	cISCResExemplar* resExemplar)
{
	LogExemplarTGI(originalFunctionName, key);
}
